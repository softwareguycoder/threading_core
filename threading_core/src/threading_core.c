#include "stdafx.h"

#include "threading_core.h"

void RegisterEventEx(int signum, LPSIGNALHANDLER lpfnEventHandler) {
	log_debug("In RegisterEventEx");

	log_info(
			"RegisterEventEx: Checking whether lpfnEventHandler delegate is null...");

	if (NULL == lpfnEventHandler) {
		log_error(
				"RegisterEventEx: Address of an event handler routine must be supplied.");

		log_debug("RegisterEventEx: Done.");

		exit(ERROR);
	}

	log_info("RegisterEventEx: Event handler address passed is valid.");

	log_info("RegisterEventEx: Attempting to register the event handler...");

	if (SIG_ERR == signal(signum, lpfnEventHandler)) {
		log_error("RegisterEventEx: Failed to register event handler.");

		perror("RegisterEventEx");

		exit(ERROR);
	}

	log_info("RegisterEventEx: Event handler registered successfully.");

	log_debug("RegisterEventEx: Done.");
}

void RegisterEvent(LPSIGNALHANDLER lpfnEventHandler) {
	log_debug("In RegisterEvent");

	log_info(
			"RegisterEvent: Calling RegisterEventEx with SIGSEGV specified...");

	RegisterEventEx(SIGSEGV, lpfnEventHandler);

	log_info("RegisterEvent: Finished call to RegisterEventEx");

	log_debug("RegisterEvent: Done.");
}

///////////////////////////////////////////////////////////////////////////////
// _FreeThread: Internal function for freeing malloc'd thread handles.  This
// function is not exposed in the header file for this library, as it is
// meant for internal use only.

/**
 * @brief Internal (i.e., will not be put in the mutex.h header file) method
 * for freeing malloc'd thread handles.
 * @param hThread The handle to be freed.
 */
void _FreeThread(HTHREAD hThread) {
	log_info("In _FreeThread");

	log_info(
			"_FreeThread: Checking whether the thread handle passed is valid...");

	if (INVALID_HANDLE_VALUE == hThread) {
		log_warning(
				"_FreeThread: The thread handle passed has an invalid value; assuming it's already been deallocated.");

		log_info("_FreeThread: Done.");

		// If we have an invalid handle (i.e., NULL pointer), then there is nothing to do.
		return;
	}

	log_info(
			"_FreeThread: The thread handle passed is valid.  Freeing the memory...");

	// The HMUTEX handle type is just a typedef of pthread_mutex_t*
	// However, to work with the pthread functions, we need to view it
	// as such.
	pthread_t* pThread = (pthread_t*) hThread;

	free(pThread);
	pThread = NULL;
	hThread = INVALID_HANDLE_VALUE;

	log_info(
			"_FreeThread: The memory occupied by the thread handle passed has been freed.");

	log_info("_FreeThread: Done.");
}

///////////////////////////////////////////////////////////////////////////////
// CreateThread: Requests the operating system to create a new thread in the
// current process.  If successful, returns a handle to the new thread.

/**
 * @brief Creates a thread to execute within the virtual address space of the calling process.
 * @param lpfnThreadProc (Required.) A pointer to the application-defined function to be executed by the thread.
 * @return Handle to the created thread, or INVALID_HANDLE_VALUE if an error occurred.
 * @remarks The thread function specified by lpfnThreadProc will begin execution immediately.
 * This function is an alias for CreateThreadEx with NULL passed for the second argument.
 */
HTHREAD CreateThread(LPTHREAD_START_ROUTINE lpfnThreadProc) {
	return CreateThreadEx(lpfnThreadProc, NULL /* pUserState */);
}

///////////////////////////////////////////////////////////////////////////////
// CreateThread: Requests the operating system to create a new thread in the
// current process.  If successful, returns a handle to the new thread.

/**
 * @brief Creates a new thread and returns a handle to it, or returns INVALID_HANDLE_VALUE if the
 * operating system was unable to create a new thread.
 * @param lpfnThreadProc Address of a function that will serve as the thread procedure.
 * @param pUserState Address of a block of memory that contains user state that is to be passed
 * as an argument to the thread procedure.  You may pass NULL for this parameter.
 * @return Handle to the created thread, or INVALID_HANDLE_VALUE if an error occurred.
 * @remarks The thread procedure begins execution immediately when this function is called.
 */
HTHREAD CreateThreadEx(LPTHREAD_START_ROUTINE lpfnThreadProc,
		void* __restrict pUserState) {
	log_info("In CreateThreadEx");

	/* NOTE: We can't have a thread without a thread procedure function!  If nothing has been passed
	 * for the lpfnThreadProc parameter then that is a fatal error. */

	log_info(
			"CreateThreadEx: Checking whether a valid thread procedure address has been passed.");

	if (NULL == lpfnThreadProc) {
		log_error(
				"CreateThreadEx: Null reference supplied for 'lpfnThreadProc' parameter.  This parameter is required.");

		log_info("CreateThreadEx: Done.");

		return INVALID_HANDLE_VALUE;
	}

	log_info(
			"CreateThreadEx: A valid thread procedure address has been passed.");

	log_info("CreateThreadEx: Attempting to allocate memory for a new thread.");

	pthread_t* pNewThread = (pthread_t*) malloc(sizeof(pthread_t));
	if (NULL == pNewThread) {
		// Failed to allocate memory for a new thread.
		log_error(
				"CreateThreadEx: Failed to allocate memory for a new thread handle.");

		log_info("CreateThreadEx: Done.");

		return INVALID_HANDLE_VALUE;
	}

	// If we are here, then the memory allocation succeeded.

	/* NOTE: A pthread_t* and HTHREAD type are interchangeable */
	log_info("CreateThreadEx: %d B of memory allocated.", sizeof(pthread_t));

	int nResult = pthread_create(pNewThread, NULL, lpfnThreadProc, pUserState);
	if (OK != nResult) {
		log_error("CreateThreadEx: Failed to create thread. %s",
				strerror(nResult));

		log_info("CreateThreadEx: Done.");

		return INVALID_HANDLE_VALUE;
	}

	log_info(
			"CreateThreadEx: New thread successfully created and initialized.");

	log_info("CreateThreadEx: Done.");

	return (HTHREAD) pNewThread;
}
///////////////////////////////////////////////////////////////////////////////
// WaitThread: Blocks the calling thread until the specified thread terminates.
// Does not recover any user state returned by the thread waited upon.

/**
 * @brief Waits for the thread specified by hThread to terminate.
 * @param hThread Handle to the thread you want to wait for.
 * @return Return code from the pthread_join function.
 * @remarks Blocks the calling thread until the thread specified by hThread
 * terminates; if the thread has already terminated when this function is called,
 * then WaitThread returns immediately.  This function relies upon WaitThreadEx.
 */
int WaitThread(HTHREAD hThread) {
	// delegate the implementation to the WaitThreadEx function by passing NULL
	// for the user state variable.
	return WaitThreadEx(hThread, NULL);
}

///////////////////////////////////////////////////////////////////////////////
// WaitThreadEx: Waits for a thread to terminate and also allows the calling
// thread to have access to any user state returned from the waited-on thread.

/**
 * @brief Waits for the thread specified by hThread to terminate.
 * @param hThread Handle to the thread you want to wait for.
 * @param ppRetVal Address of memory that is to be filled with the user state
 * returned by the thread procedure. NULL can also be specified if you don't care
 * to receive the returned user state.
 * @return TRUE if the thread was launched successfully; FALSE otherwise.
 * @remarks Blocks the calling thread until the thread specified by hThread terminates;
 * if the thread has already terminated when this function is called, then WaitThread
 * returns immediately.
 */
int WaitThreadEx(HTHREAD hThread, void **ppRetVal) {
	log_info("In WaitThreadEx");

	int nResult = ERROR;

	log_info(
			"WaitThreadEx: Checking whether the thread handle passed references a valid thread...");

	if (INVALID_HANDLE_VALUE == hThread) {
		log_error(
				"WaitThreadEx: The thread handle passed to this function has an invalid value.");

		log_info("WaitThreadEx: Result = %d", nResult);

		log_info("WaitThreadEx: Done.");

		return nResult;
	}

	log_info("WaitThreadEx: The thread handle passed has a valid value.");

	pthread_t *pThread = (pthread_t*) hThread;

	// pthread_join wants us to dereference the HTHREAD
	if (pThread == NULL) {
		log_error(
				"WaitThreadEx: The thread handle passed to this function has an invalid value.");

		log_info("WaitThreadEx: Result = %d", nResult);

		log_info("WaitThreadEx: Done.");

		return nResult;
	}

	pthread_t nThreadID = *pThread;

	log_info("WaitThreadEx: Attempting to join the specified thread...");

	nResult = pthread_join(nThreadID, ppRetVal);
	if (OK != nResult) {
		log_error("WaitThreadEx: Failed to join thread at address %x. %s",
				hThread, strerror(nResult));

		log_info("WaitThreadEx: Result = %d", nResult);

		log_info("WaitThreadEx: Done.");

		return nResult;
	}

	log_info("WaitThreadEx: The specified thread has terminated.");

	// Once we get here, the thread handle is completely useless, so
	// free the memory assocaited with it and invalidate the thread
	// handle.  This is necessary because thread handles are allocated
	// on the heap.

	log_info(
			"WaitThreadEx: The thread with handle at the memory address %0x has terminated.");

	log_info("WaitThreadEx: Deallocating the memory occupied by it...");

	_FreeThread(hThread);

	log_info("WaitThreadEx: The terminated thread has been deallocated.");

	log_info("WaitThreadEx: Result = %d", nResult);

	log_info("WaitThreadEx: Done.");

	return nResult;

}

/**
 * @brief Destroys (deallocates) a thread handle and releases its resources to the operating system.
 * @param hThread Handle to the thread you want to get rid of.
 * @return System error code.  Zero if successful.
 * @remarks Only call this function if you want a guarantee that the thread will be destroyed.
 * Nominally, WaitThreadEx also releases threads once it has finished waiting for them to
 * terminate.
 */
int DestroyThread(HTHREAD hThread) {
	log_info("In DestroyThread");

	int nResult = OK;

	log_info(
			"DestroyThread: Checking whether the handle passed to us has already been invalidated...");

	if (INVALID_HANDLE_VALUE == hThread) {
		log_warning(
				"DestroyThread: The thread handle passed to us has already been invalidated.  Nothing more to do.");

		log_info("DestroyThread: Result = %d", nResult);

		log_info("DestroyThread: Done.");

		return nResult; /* Nothing to do if thread handle is already an invalid value */
	}

	log_info(
			"DestroyThread: The thread handle passed to us has not been invalidated yet.");

	log_info(
			"DestroyThread: Calling _FreeThread to release the system resources used by the thread...");

	_FreeThread(hThread);

	log_info("DestroyThread: Finished calling _FreeThread.");

	log_info(
			"DestroyThread: Setting the thread handle to an invalid value to guarantee it is released...");

	/* Even though we explicitly called _FreeThread above, let's explicitly set the
	 * thread handle to INVALID_HANDLE_VALUE just to be on the safe side.
	 */
	hThread = INVALID_HANDLE_VALUE;

	log_info(
			"DestroyThread: The thread handle passed to us has been invalidated.");

	log_info("DestroyThread: Result = %d", nResult);

	log_info("DestroyThread: Done");

	return nResult;
}

