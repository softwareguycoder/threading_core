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
 * @remarks The thread thusly created with this function is suspended until RunThread is called.
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

	log_info("CreateThread: New thread successfully created and initialized.")

	log_info("CreateThread: Done.");

	return (HTHREAD) pNewThread;
}

