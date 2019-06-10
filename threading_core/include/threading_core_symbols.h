// threading_core_symbols.h - Defines constants, typedefs, and other symbols
// routinely utilized in the threading_core library
//

#ifndef __THREADING_CORE_SYMBOLS_H__
#define __THREADING_CORE_SYMBOLS_H__

/**
 * @name ERROR_FAILED_TO_MARSHAL_BLOCK
 * @brief Error message displayed when the operation of marshaling a block
 * of data across the thread boundary has failed.
 */
#ifndef ERROR_FAILED_TO_MARSHAL_BLOCK
#define ERROR_FAILED_TO_MARSHAL_BLOCK \
  "Failed to marshal memory block onto the heap\n" \
  "\t(did you pass a NULL pointer to MarshalBlock?).\n"
#endif //ERROR_FAILED_TO_MARSHAL_BLOCK

/**
 * @name ERROR_FAILED_TO_DEMARSHAL_BLOCK
 * @brief Error message displayed to the user when a demarshaling operation
 * failed.
 */
#ifndef ERROR_FAILED_TO_DEMARSHAL_BLOCK
#define ERROR_FAILED_TO_DEMARSHAL_BLOCK \
  "Failed to demarshal data block from across thread boundary.\n"
#endif //ERROR_FAILED_TO_DEMARSHAL_BLOCK

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
