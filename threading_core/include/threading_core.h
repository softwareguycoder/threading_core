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
 * @brief Pointer to a function that handles a signal.
 * @param signum Numeric code corresponding to the signal that was sent.
 */
typedef void (*LPSIGNALHANDLER)(int signum);

/**
 * @brief Pointer to a function that will be executed for a given thread.
 * @param lpThreadParameter A reference to memory containing user state.
 * May be NULL.
 * @returns Reference to a block of memory containing user state.  May be NULL.
 */
typedef void* (*LPTHREAD_START_ROUTINE)(void* lpThreadParameter);

/**
 * @brief Creates a thread to execute within the virtual address space of the
 * calling process.
 * @param lpfnThreadProc (Required.) A pointer to the application-defined
 * function to be executed by the thread.
 * @return Handle to the created thread, or INVALID_HANDLE_VALUE if an error
 * occurred.
 * @remarks The thread function specified by lpfnThreadProc will begin execution
 * immediately.
 * This function is an alias for CreateThreadEx with NULL passed for the second
 * argument.
 */
HTHREAD CreateThread(LPTHREAD_START_ROUTINE lpfnThreadProc);

/**
 * @brief Creates a new thread and returns a handle to it, or returns
 * INVALID_HANDLE_VALUE if the operating system was unable to create a new
 * thread.
 * @param lpfnThreadProc Address of a function that will serve as the thread
 * procedure.
 * @param pUserState Address of a block of memory that contains user state
 * that is to be passed
 * as an argument to the thread procedure.
 * @return Handle to the created thread, or INVALID_HANDLE_VALUE if an error
 * occurred.
 * @remarks The thread procedure begins execution immediately when this function
 * is called.
 */
HTHREAD CreateThreadEx(LPTHREAD_START_ROUTINE lpfnThreadProc,
		void* __restrict pUserState);

/**
 * @brief Destroys (deallocates) a thread handle and releases its resources to
 * the operating system.
 * @param hThread Handle to the thread you want to get rid of.
 * @return System error code.  Zero if successful.
 * @remarks Only call this function if you want a guarantee that the thread will
 * be destroyed.
 * Nominally, WaitThreadEx also releases threads once it has finished waiting
 * for them to terminate.
 */
int DestroyThread(HTHREAD hThread);

/**
 * @brief Registers a function to be called when a signal is sent to a thread.
 * @param lpfnEventHandler Address of a function of type LPSIGNALHANDLER that
 * should be called when the SIGSEGV signal is sent.
 * @remarks Sets the function to be called when the signal is processed by a
 * thread.
 * The handler should register itself over again during the call, as the last
 * statement.
 * This alias automatically maps the event handler to the SIGSEGV code.
 */
void RegisterEvent(LPSIGNALHANDLER lpfnEventHandler);

/**
 * @brief Registers a function to be called when a signal is sent to a thread.
 * @param signum Numeric value corresponding to the signal to which to respond.
 * @param lpfnEventHandler Address of a function of type LPSIGNALHANDLER that
 * should be called when the SIGSEGV signal is sent.
 * @remarks Sets the function to be called when the signal is processed by a
 * thread.
 * The handler should register itself over again during the call, as the last
 * statement.
 */
void RegisterEventEx(int signum, LPSIGNALHANDLER lpfnEventHandler);

/**
 * @brief Forcibly terminates a thread and raises a signal to it.
 * @param hThread Thread handle of the thread you wish to kill.
 * @param signum Code identifying the signal that should be sent to the thread.
 * @remarks Causes a thread to terminate and signals the thread beforehand so
 * that it has the opportunity to perform cleanup.
 */
void KillThreadEx(HTHREAD hThread, int signum);

/**
 * @brief Forcibly terminates a thread and raises a signal to it.
 * @param hThread Thread handle of the thread you wish to kill.
 * @remarks Causes a thread to terminate and signals the thread beforehand with
 * the SIGSEGV signal code.
 */
void KillThread(HTHREAD hThread);

/**
 * @brief Waits for the thread specified by hThread to terminate.
 * @param hThread Handle to the thread you want to wait for.
 * @return TRUE if the thread was launched successfully; FALSE otherwise.
 * @remarks Blocks the calling process until the thread specified by hThread
 * terminates; if the thread
 * has already terminated when this function is called, then WaitThread returns
 * immediately.
 */
int WaitThread(HTHREAD hThread);

/**
 * @brief Waits for the thread specified by hThread to terminate.
 * @param hThread Handle to the thread you want to wait for.
 * @param ppRetVal Address of memory that is to be filled with the user state
 * returned by the thread procedure.
 * @return TRUE if the thread was launched successfully; FALSE otherwise.
 * @remarks Blocks the calling process until the thread specified by hThread
 * terminates; if the thread
 * has already terminated when this function is called, then WaitThread returns
 * immediately.
 */
int WaitThreadEx(HTHREAD hThread, void** ppvRetVal);

#endif //__THREADING_CORE_H__
