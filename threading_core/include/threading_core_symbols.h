// threading_core_symbols.h - Defines constants, typedefs, and other symbols
// routinely utilized in the threading_core library
//

#ifndef __THREADING_CORE_SYMBOLS_H__
#define __THREADING_CORE_SYMBOLS_H__

/**
 * @brief Error message that is written to STDERR when a thread cancellation
 * state cannot be set.
 */
#ifndef ERROR_FAILED_SET_THREAD_CANCELLATION_STATE
#define ERROR_FAILED_SET_THREAD_CANCELLATION_STATE \
  "ERROR: Failed to set thread cancellation state.\n"
#endif //ERROR_FAILED_SET_THREAD_CANCELLATION_STATE

#ifndef ERROR_FAILED_SET_THREAD_CANCELLATION_TYPE
#define ERROR_FAILED_SET_THREAD_CANCELLATION_TYPE \
  "ERROR: Failed to set thread cancellation type.\n"
#endif //ERROR_FAILED_SET_THREAD_CANCELLATION_TYPE

#endif //__THREADING_CORE_SYMBOLS_H__
