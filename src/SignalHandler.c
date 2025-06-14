/************************************/
/******** Include statements ********/
/************************************/

#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include "SeverityLog_api.h"
#include "MutexGuard_api.h"
#include "SignalHandler_api.h"

/************************************/

/***********************************/
/******** Define statements ********/
/***********************************/

#define C_SIGNAL_HANDLER_ALIGNED __attribute__((aligned(sizeof(size_t))))

#define SIGNAL_HANDLER_EXECUTION_MSG    "Executing signal handlers."

/**** Private type definitions *****/

// The type defined below is a callback-signal mask pair.
// Each pair contains the handler function + a number referencing the signal to attend in each bit.
typedef struct C_SIGNAL_HANDLER_ALIGNED
{
    void (*cb)(const int sig_num);
    uint16_t sig_mask;
} CB_SIGNAL_PAIR;

typedef enum
{
    SIG_HDL_ERR_NULL_CB                             = 1000  ,
    SIG_HDL_ERR_ALLOCATE_CB_MAT                             ,
    SIG_HDL_ERR_LOCK_SIG_HDL_MTX                            ,
    SIG_HDL_ERR_EMPTY_SIGNAL_MASK                           ,
    SIG_HDL_ERR_UNK_SIG                                     ,
    SIG_HDL_ERR_OUT_OF_BOUNDARIES_ERR                       ,

    SIG_HDL_ERR_MIN = SIG_HDL_ERR_NULL_CB                   ,
    SIG_HDL_ERR_MAX = SIG_HDL_ERR_OUT_OF_BOUNDARIES_ERR     ,
} SIG_HDL_ERR_CODE;

/***********************************/

/***********************************/
/******** Private variables ********/
/***********************************/

static CB_SIGNAL_PAIR* cb_sig_array     = NULL;
static size_t cb_sig_array_size         = 0;
static MTX_GRD sig_cb_mat_mtx           = {0};
static __thread int sig_hdl_errno       = 0;

static const char* error_str_table[SIG_HDL_ERR_MAX - SIG_HDL_ERR_MIN + 1] =
{
    "Null callback"                         ,
    "Could not allocate callback matrix"    ,
    "Could not lock signal handler mutex"   ,
    "Empty signal mask"                     ,
    "Unknown signal"                        ,
    "Out of boundaries error code"          ,
};

static int signals_to_handle[] =
{
    SIGHUP  ,   // Terminal closed or controlling process died
    SIGINT  ,   // Interrupt from keyboard (Ctrl+C)
    SIGQUIT ,   // Quit from keyboard (Ctrl+\) — creates core dump
    SIGILL  ,   // Illegal instruction
    SIGTRAP ,   // Trap (usually for debugging breakpoints)
    SIGABRT ,   // Abnormal termination (abort called)
    SIGBUS  ,   // Bus error (misaligned or inaccessible memory)
    SIGFPE  ,   // Floating-point exception (e.g., divide by zero)
    SIGSEGV ,   // Segmentation fault (invalid memory access)
    SIGPIPE ,   // Write to pipe/socket with no reader
    SIGALRM ,   // Timer expired (from alarm() or timer APIs)
    SIGTERM ,   // Termination request (e.g., kill)
    SIGSYS  ,   // Bad system call
};


/*************************************/

/**** Private function prototypes ****/

__attribute__((constructor)) static void SignalHandlerLoad(void);
__attribute__((destructor)) static void SignalHandlerUnload(void);
static void SignalHandlerSetupSigHdl(void);
static int SignalHandlerSearchSignalAux(const void *a, const void *b);
static void SignalHandlerExecuteCallbacks(const int signal_number);
static int SignalHandlerCbArrayAddSlot();

/*************************************/

/*************************************/
/******* Function definitions ********/
/*************************************/

__attribute__((constructor)) static void SignalHandlerLoad(void)
{
    MTX_GRD_ATTR_INIT_SC(   &sig_cb_mat_mtx         ,
                            PTHREAD_MUTEX_ERRORCHECK,
                            PTHREAD_PRIO_INHERIT    ,
                            PTHREAD_PROCESS_PRIVATE ,
                            p_sig_cb_mat_mtx_attr   );

    MTX_GRD_INIT(&sig_cb_mat_mtx);

    SignalHandlerSetupSigHdl();
}

__attribute__((destructor)) static void SignalHandlerUnload(void)
{
    MTX_GRD_DESTROY(&sig_cb_mat_mtx);
    free(cb_sig_array);
}

static void SignalHandlerSetupSigHdl(void)
{
    struct sigaction sa;
    sa.sa_handler = SignalHandlerExecuteCallbacks;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    for(int i = 0; i < (sizeof(signals_to_handle) / sizeof(signals_to_handle[0])); i++)
        sigaction(signals_to_handle[i], &sa, NULL);
}

static int SignalHandlerSearchSignalAux(const void *a, const void *b)
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;

    return ( (int_a > int_b) - (int_a < int_b));
}

static void SignalHandlerExecuteCallbacks(const int signal_number)
{
    SVRTY_LOG_DBG(SIGNAL_HANDLER_EXECUTION_MSG);

    int search_arr_size = (sizeof(signals_to_handle) / sizeof(signals_to_handle[0]));
    int search_arr_elem_size = sizeof(signals_to_handle[0]);

    int* p_found = (int*)bsearch(   &signal_number              ,
                                    signals_to_handle           ,
                                    search_arr_size             ,
                                    search_arr_elem_size        ,
                                    SignalHandlerSearchSignalAux);

    if(!p_found)
    {
        sig_hdl_errno = SIG_HDL_ERR_UNK_SIG;
        exit(EXIT_FAILURE);
    }

    int sig_idx = *p_found;

    for(int i = (cb_sig_array_size - 1); i >= 0; i--)
    {
        if(cb_sig_array[i].sig_mask & (1 << sig_idx))
        {
            if(!cb_sig_array[i].cb)
            {
                sig_hdl_errno = SIG_HDL_ERR_NULL_CB;
                continue;
            }

            cb_sig_array[i].cb(signal_number);
        }
    }

    exit(EXIT_FAILURE);
}

static int SignalHandlerCbArrayAddSlot()
{
    if(!cb_sig_array)
        cb_sig_array = (CB_SIGNAL_PAIR*)calloc(1, sizeof(CB_SIGNAL_PAIR));
    else
        cb_sig_array = (CB_SIGNAL_PAIR*)realloc(cb_sig_array, sizeof(CB_SIGNAL_PAIR));
    
    // If cb_sig_array == NULL still, then exit.
    if(!cb_sig_array)
    {
        sig_hdl_errno = SIG_HDL_ERR_ALLOCATE_CB_MAT;
        return -1;
    }
    
    ++cb_sig_array_size;

    return 0;
}

int SignalHandlerAddCallback(void (*cb)(const int sig_num), const uint16_t sig_mask)
{
    MTX_GRD_LOCK_SC(&sig_cb_mat_mtx, p_sig_cb_mat_mtx);

    if(!p_sig_cb_mat_mtx)
    {
        sig_hdl_errno = SIG_HDL_ERR_LOCK_SIG_HDL_MTX;
        return -1;
    }

    if(!cb)
    {
        sig_hdl_errno = SIG_HDL_ERR_NULL_CB;
        return -2;
    }

    if(!sig_mask)
    {
        sig_hdl_errno = SIG_HDL_ERR_EMPTY_SIGNAL_MASK;
        return -3;
    }

    if(SignalHandlerCbArrayAddSlot() < 0)
        return -4;

    cb_sig_array[cb_sig_array_size - 1].cb          = cb;
    cb_sig_array[cb_sig_array_size - 1].sig_mask    = sig_mask;

    return 0;
}

int SignalHandlerGetErrorCode(void)
{
    return sig_hdl_errno;
}

const char* SignalHandlerGetErrorString(const int error_code)
{
    if( (error_code < SIG_HDL_ERR_MIN) || (error_code > SIG_HDL_ERR_MAX) )
        return error_str_table[SIG_HDL_ERR_MAX - SIG_HDL_ERR_MIN];

    return error_str_table[error_code - SIG_HDL_ERR_MIN];
}

/*************************************/
