#ifndef __THREADING_CORE_H__
#define __THREADING_CORE_H__

#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>

#ifndef OK
#define OK		0		// Code to return to the operating system to indicate successful program termination
#endif // OK

#ifndef ERROR
#define ERROR	-1		// Code to return to the operating system to indicate an error condition
#endif // ERROR

/**
 * @brief Standardized value for a handle.
 */
#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE NULL
#endif //INVALID_HANDLE_VALUE

/**
 * @brief Handle to a process thread.
 */
typedef pthread_t* HTHREAD;

/**
 * @brief Pointer to a function that will be executed for a given thread.
 * @param lpThreadParameter A reference to memory containing user state.  May be NULL.
 * @returns Reference to a block of memory containing user state.  May be NULL.
 */
typedef void* (*LPTHREAD_START_ROUTINE)(void* lpThreadParameter);

/**
 * @brief Creates a new thread and returns a handle to it, or returns INVALID_HANDLE_VALUE if the
 * operating system was unable to create a new thread.
 * @param lpfnThreadProc Address of a function that will serve as the thread procedure.
 * @return Handle to the created thread, or INVALID_HANDLE_VALUE if an error occurred.
 * @remarks The thread procedure begins execution immediately when this function is called.
 */
HTHREAD CreateThread(LPTHREAD_START_ROUTINE lpfnThreadProc);

/**
 * @brief Waits for the thread specified by hThread to terminate.
 * @param hThread Handle to the thread you want to wait for.
 * @return TRUE if the thread was launched successfully; FALSE otherwise.
 * @remarks Blocks the calling process until the thread specified by hThread terminates; if the thread
 * has already terminated when this function is called, then WaitThread returns immediately.
 */
void WaitThread(HTHREAD hThread);

/**
 * @brief Waits for the thread specified by hThread to terminate.
 * @param hThread Handle to the thread you want to wait for.
 * @param ppRetVal Address of memory that is to be filled with the user state returned by the thread procedure.
 * @return TRUE if the thread was launched successfully; FALSE otherwise.
 * @remarks Blocks the calling process until the thread specified by hThread terminates; if the thread
 * has already terminated when this function is called, then WaitThread returns immediately.
 */
void WaitThreadEx(HTHREAD hThread, void** ppRetVal);

#endif //__THREADING_CORE_H__
