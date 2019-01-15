#include "stdafx.h"

#include "threading_core.h"

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
 * @param lpfnThreadProc A pointer to the application-defined function to be executed by the thread.
 * @return Handle to the created thread, or INVALID_HANDLE_VALUE if an error occurred.
 */
HTHREAD CreateThread(LPTHREAD_START_ROUTINE lpfnThreadProc) {
	log_info("In CreateThread");

	log_info(
			"CreateThread: Checking whether a valid thread procedure address has been passed.");

	if (lpfnThreadProc == NULL) {
		log_error(
				"CreateThread: Null reference supplied for 'lpfnThreadProc' parameter.  This parameter is required.");

		log_info("CreateThread: Done.");

		return INVALID_HANDLE_VALUE;
	}

	log_info("CreateThread: A valid thread procedure address has been passed.");

	log_info("CreateThread: Attempting to allocate memory for a new thread.");

	pthread_t* pNewThread = (pthread_t*) malloc(sizeof(pthread_t));
	if (NULL == pNewThread) {
		// Failed to allocate memory for a new thread.
		log_error(
				"CreateThread: Failed to allocate memory for a new thread handle.");

		log_info("CreateThread: Done.");

		return INVALID_HANDLE_VALUE;
	}

	// If we are here, then the memory allocation succeeded.

	/* NOTE: A pthread_t* and HTHREAD type are interchangeable */
	log_info("CreateThread: %d B of memory allocated.", sizeof(pthread_t));

	int nResult = pthread_create(pNewThread, NULL, lpfnThreadProc, NULL);
	if (OK != nResult) {
		log_error("CreateThread: Failed to create thread. %s",
				strerror(nResult));

		log_info("CreateThread: Done.");

		return INVALID_HANDLE_VALUE;
	}

	log_info("CreateThread: New thread successfully created and initialized.");

	log_info("CreateThread: Done.");

	return (HTHREAD) pNewThread;
}

///////////////////////////////////////////////////////////////////////////////
// WaitThread: Blocks the calling thread until the specified thread terminates.
// Does not recover any user state returned by the thread waited upon.

/**
 * @brief Waits for the thread specified by hThread to terminate.
 * @param hThread Handle to the thread you want to wait for.
 * @return TRUE if the thread was launched successfully; FALSE otherwise.
 * @remarks Blocks the calling process until the thread specified by hThread terminates; if the thread
 * has already terminated when this function is called, then WaitThread returns immediately.  This
 * function relies upon WaitThreadEx.
 */
void WaitThread(HTHREAD hThread) {

	log_info("In WaitThread");

	log_info(
			"WaitThread: Checking whether the thread handle passed references a valid thread...");

	if (INVALID_HANDLE_VALUE == hThread) {
		log_error(
				"WaitThread: The thread handle passed to this function has an invalid value.");

		log_info("WaitThread: Done.");

		return;
	}

	log_info("WaitThread: The thread handle passed has a valid value.");

	// delegate the implementation to the WaitThreadEx function by passing NULL
	// for the user state variable.
	WaitThreadEx(hThread, NULL);

	log_info("WaitThread: Done.");
}

///////////////////////////////////////////////////////////////////////////////
// WaitThreadEx: Waits for a thread to terminate and also allows the calling
// thread to have access to any user state returned from the waited-on thread.

/**
 * @brief Waits for the thread specified by hThread to terminate.
 * @param hThread Handle to the thread you want to wait for.
 * @param ppRetVal Address of memory that is to be filled with the user state returned by the thread procedure.
 * @return TRUE if the thread was launched successfully; FALSE otherwise.
 * @remarks Blocks the calling process until the thread specified by hThread terminates; if the thread
 * has already terminated when this function is called, then WaitThread returns immediately.
 */
void WaitThreadEx(HTHREAD hThread, void **ppRetVal) {
	log_info("In WaitThreadEx");

	log_info(
			"WaitThreadEx: Checking whether the thread handle passed references a valid thread...");

	if (INVALID_HANDLE_VALUE == hThread) {
		log_error(
				"WaitThreadEx: The thread handle passed to this function has an invalid value.");

		log_info("WaitThreadEx: Done.");

		return;
	}

	log_info("WaitThreadEx: The thread handle passed has a valid value.");

	log_info("WaitThreadEx: Attempting to join the specified thread...");

	int nResult = pthread_join(hThread, ppRetVal);
	if (OK != nResult) {
		log_error("WaitThreadEx: Failed to join thread %lu. %s", hThread,
				strerror(nResult));

		log_info("WaitThreadEx: Done.");

		return;
	}

	log_info("WaitThreadEx: The specified thread has terminated.")

	// Once we get here, the thread handle is completely useless, so
	// free the memory assocaited with it and invalidate the thread
	// handle.  This is necessary because thread handles are allocated
	// on the heap.

	log_info(
			"WaitThreadEx: The thread with handle %lu has terminated.  Deallocating the memory occupied by it...");

	_FreeThread(hThread);

	log_info("WaitThreadEx: The terminated thread has been deallocated.");

	log_info("WaitThreadEx: Done.");

}

