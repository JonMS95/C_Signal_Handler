#ifndef SIGNAL_HANDLER_API_H
#define SIGNAL_HANDLER_API_H

#ifdef __cplusplus
extern "C" {
#endif

/************************************/
/******** Include statements ********/
/************************************/

#include <stdint.h>

/************************************/

/***********************************/
/******** Define statements ********/
/***********************************/

#define C_SIGNAL_HANDLER_API __attribute__((visibility("default")))

#define SIG_HDL_ALL_SIGNALS_MASK    0xFF

#define SIG_HDL_GET_LAST_ERR_STR    SignalHandlerGetErrorString(SignalHandlerGetErrorCode())

/***********************************/

/*************************************/
/******** Function prototypes ********/
/*************************************/

C_SIGNAL_HANDLER_API int SignalHandlerAddCallback(void (*cb)(const int sig_num), const uint16_t sig_mask);
C_SIGNAL_HANDLER_API int SignalHandlerGetErrorCode(void);
C_SIGNAL_HANDLER_API const char* SignalHandlerGetErrorString(const int error_code);
C_SIGNAL_HANDLER_API void SignalHandlerResetCallbacks(void);

/*************************************/

#ifdef __cplusplus
}
#endif

#endif