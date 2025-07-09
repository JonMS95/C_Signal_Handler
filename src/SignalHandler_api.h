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

/// @brief Mask to indicate that the given callback is meant to catch every possible signal (to be used alongside SignalHandlerAddCallback).
#define SIG_HDL_ALL_SIGNALS_MASK    0xFF

/// @brief Macro to make error string retrieval easier.
#define SIG_HDL_GET_LAST_ERR_STR    SignalHandlerGetErrorString(SignalHandlerGetErrorCode())

/***********************************/

/*************************************/
/******** Function prototypes ********/
/*************************************/

/// @brief Adds a given callback to the list of callbacks to be executed on catched signals.
/// @param cb Tragte callback to be executed.
/// @param sig_mask Mask telling on which signals should the callback in question be executed.
/// @return 0 if succeeded, < 0 otherwise.
C_SIGNAL_HANDLER_API int SignalHandlerAddCallback(void (*cb)(const int sig_num), const uint16_t sig_mask);

/// @brief Gets library's latest stored error code.
/// @return Error code.
C_SIGNAL_HANDLER_API int SignalHandlerGetErrorCode(void);

/// @brief Retrieves error-code-associated string.
/// @param error_code Error code.
/// @return Error describing string if any, "Out of boundaries error code" otherwise.
C_SIGNAL_HANDLER_API const char* SignalHandlerGetErrorString(const int error_code);

/// @brief Erases every priorly stored callback. 
C_SIGNAL_HANDLER_API void SignalHandlerResetCallbacks(void);

/*************************************/

#ifdef __cplusplus
}
#endif

#endif