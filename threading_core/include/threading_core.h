#ifndef __THREADING_CORE_H__
#define __THREADING_CORE_H__

#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>

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

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE NULL
#endif //INVALID_HANDLE_VALUE

/**
 * @brief Creates a new thread and returns a handle to it, or returns INVALID_HANDLE_VALUE if the
 * operating system was unable to create a new thread.
 * @param lpfnThreadProc Address of a function that will serve as the thread procedure.
 * @return Handle to the created thread, or INVALID_HANDLE_VALUE if an error occurred.
 */
HTHREAD CreateThread(LPTHREAD_START_ROUTINE lpfnThreadProc);

#endif //__THREADING_CORE_H__
