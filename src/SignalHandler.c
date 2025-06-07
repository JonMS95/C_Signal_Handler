/************************************/
/******** Include statements ********/
/************************************/

#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include "SeverityLog_api.h"
#include "MutexGuard_api.h"

/************************************/

/***********************************/
/******** Define statements ********/
/***********************************/

#define C_SIGNAL_HANDLER_ALIGNED __attribute__((aligned(sizeof(size_t))))

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
    "Null callback"                     ,
    "Could not allocate callback matrix",
    "Out of boundaries error code"      ,
};

/*************************************/

/**** Private function prototypes ****/

__attribute__((constructor)) static void SignalHandlerLoad(void);
__attribute__((destructor)) static void SignalHandlerUnload(void);
static int SignalHandlerCbArrayAddSlot(void);

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
}

__attribute__((destructor)) static void SignalHandlerUnload(void)
{
    MTX_GRD_DESTROY(&sig_cb_mat_mtx);
    free(cb_sig_array);
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
    if(!cb)
    {
        sig_hdl_errno = SIG_HDL_ERR_NULL_CB;
        return -1;
    }

    if(SignalHandlerCbArrayAddSlot() < 0)
        return -2;

    cb_sig_array[cb_sig_array_size - 1].cb          = cb;
    cb_sig_array[cb_sig_array_size - 1].sig_mask    = sig_mask;

    return 0;
}

/*************************************/
