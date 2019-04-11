#include "stdafx.h"

#include "threading_core.h"

///////////////////////////////////////////////////////////////////////////////
// _FreeThread: Internal function for freeing malloc'd thread handles.  This
// function is not exposed in the header file for this library, as it is
// meant for internal use only.

void _FreeThread(HTHREAD hThread) {
	if (INVALID_HANDLE_VALUE == hThread) {
		// If we have an invalid handle (i.e., NULL pointer),
		// then there is nothing to do.
		return;
	}

	// The HMUTEX handle type is just a typedef of pthread_mutex_t*
	// However, to work with the pthread functions, we need to view it
	// as such.
	pthread_t* pThread = (pthread_t*) hThread;

	free(pThread);
	pThread = NULL;

	hThread = INVALID_HANDLE_VALUE;
}

///////////////////////////////////////////////////////////////////////////////
// CreateThread function: Requests the operating system to create a new thread
// in the current process.  If successful, returns a handle to the new thread.

HTHREAD CreateThread(LPTHREAD_START_ROUTINE lpfnThreadProc) {
	return CreateThreadEx(lpfnThreadProc, NULL /* pUserState */);
}

///////////////////////////////////////////////////////////////////////////////
// CreateThread: Requests the operating system to create a new thread in the
// current process.  If successful, returns a handle to the new thread.

HTHREAD CreateThreadEx(LPTHREAD_START_ROUTINE lpfnThreadProc,
		void* __restrict pUserState) {

	/* NOTE: We can't have a thread without a thread procedure function!
	 * If nothing has been passed for the lpfnThreadProc parameter then that
	 * is a fatal error. */

	if (NULL == lpfnThreadProc) {
		// Nothing to do if no thread proc passed
		return INVALID_HANDLE_VALUE;
	}

	pthread_t* pNewThread = (pthread_t*) malloc(sizeof(pthread_t));
	if (NULL == pNewThread) {
		// Failed to allocate memory
		return INVALID_HANDLE_VALUE;
	}

	/* NOTE: A pthread_t* and HTHREAD type are interchangeable */
	int nResult = pthread_create(pNewThread, NULL, lpfnThreadProc, pUserState);
	if (OK != nResult) {
		return INVALID_HANDLE_VALUE;
	}

	return (HTHREAD) pNewThread;
}

///////////////////////////////////////////////////////////////////////////////
// DestroyThread function - Releases resources consumed by the specified thread
// back to the operating system.

int DestroyThread(HTHREAD hThread) {
	int nResult = OK;

	if (INVALID_HANDLE_VALUE == hThread) {
		return nResult; /* Nothing to do if thread handle is already
		 	 	 	 	 	 an invalid value */
	}

	/* Release the memory consumed by the thread handle */
	_FreeThread(hThread);

	/* Even though we explicitly called _FreeThread above, let's explicitly
	 * set the thread handle to INVALID_HANDLE_VALUE just to be on the safe
	 * side. */
	hThread = INVALID_HANDLE_VALUE;

	return nResult;
}

void KillThread(HTHREAD hThread) {
	LogDebug("In KillThread");

	LogInfo("KillThread: Calling KillThreadEx with SIGSEGV for signum...");

	KillThreadEx(hThread, SIGSEGV);

	LogInfo("KillThread: Successfully called KillThreadEx.");

	LogDebug("KillThread: Done.");
}

///////////////////////////////////////////////////////////////////////////////
// KillThreadEx function - like KillThread, but lets you customize the signal
// that is sent to a semaphore implemented in the thread to indicate to the
// thread that its lifetime is at an end

void KillThreadEx(HTHREAD hThread, int signum) {
	LogDebug("In KillThreadEx");

	LogInfo("KillThreadEx: Checking whether thread handle passed is valid...");

	if (INVALID_HANDLE_VALUE == hThread) {
		LogError("KillThreadEx: Invalid thread handle passed.  Stopping.");

		LogDebug("KillThreadEx: Done.");

		return;
	}

	LogInfo("KillThreadEx: Valid thread handle passed.");

	LogDebug("KillThreadEx: signum = %d", signum);

	if (signum <= 0) {
		return;	// Invalid value for signum
	}

	LogInfo("KillThreadEx: Attempting to signal the thread...");

	int retval = pthread_kill((pthread_t) (*hThread), signum);

	sleep(1); 	// force a context switch

	LogDebug("KillThreadEx: pthread_kill retval = %d", retval);

	if (OK != retval) {
		LogError("KillThreadEx: Failed to signal thread.");

		LogDebug("KillThreadEx: Done.");

		perror("KillThreadEx");

		exit(ERROR);
	}

	LogInfo("KillThreadEx: Thread signaled successfully.");

	LogDebug("KillThreadEx: Done.");
}

///////////////////////////////////////////////////////////////////////////////
// RegisterEvent function - Registers the specified semaphore function to be
// called when the SIGSEGV signal is raised on a thread.

void RegisterEvent(LPSIGNALHANDLER lpfnEventHandler) {
	LogDebug("In RegisterEvent");

	LogInfo("RegisterEvent: Calling RegisterEventEx with SIGSEGV specified...");

	RegisterEventEx(SIGSEGV, lpfnEventHandler);

	LogInfo("RegisterEvent: Finished call to RegisterEventEx");

	LogDebug("RegisterEvent: Done.");
}

///////////////////////////////////////////////////////////////////////////////
// RegisterEventEx function - Registers a semaphore function to be called
// when the signal indicated by signum is signalled.

void RegisterEventEx(int signum, LPSIGNALHANDLER lpfnEventHandler) {
	LogDebug("In RegisterEventEx");

	LogInfo(
			"RegisterEventEx: Checking whether lpfnEventHandler delegate is null...");

	if (NULL == lpfnEventHandler) {
		LogError(
				"RegisterEventEx: Address of an event handler routine must be supplied.");

		LogDebug("RegisterEventEx: Done.");

		exit(ERROR);
	}

	LogInfo("RegisterEventEx: Event handler address passed is valid.");

	LogInfo("RegisterEventEx: Attempting to register the event handler...");

	if (SIG_ERR == signal(signum, lpfnEventHandler)) {
		LogError("RegisterEventEx: Failed to register event handler.");

		perror("RegisterEventEx");

		exit(ERROR);
	}

	LogInfo("RegisterEventEx: Event handler registered successfully.");

	LogDebug("RegisterEventEx: Done.");
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
	LogInfo("In WaitThreadEx");

	int nResult = ERROR;

	LogInfo(
			"WaitThreadEx: Checking whether the thread handle passed references a valid thread...");

	if (INVALID_HANDLE_VALUE == hThread) {
		LogError(
				"WaitThreadEx: The thread handle passed to this function has an invalid value.");

		LogInfo("WaitThreadEx: Result = %d", nResult);

		LogInfo("WaitThreadEx: Done.");

		return nResult;
	}

	LogInfo("WaitThreadEx: The thread handle passed has a valid value.");

	pthread_t *pThread = (pthread_t*) hThread;

	// pthread_join wants us to dereference the HTHREAD
	if (pThread == NULL) {
		LogError(
				"WaitThreadEx: The thread handle passed to this function has an invalid value.");

		LogInfo("WaitThreadEx: Result = %d", nResult);

		LogInfo("WaitThreadEx: Done.");

		return nResult;
	}

	pthread_t nThreadID = *pThread;

	LogInfo("WaitThreadEx: Attempting to join the specified thread...");

	nResult = pthread_join(nThreadID, ppRetVal);
	if (OK != nResult) {
		LogError("WaitThreadEx: Failed to join thread at address %x. %s",
				hThread, strerror(nResult));

		LogInfo("WaitThreadEx: Result = %d", nResult);

		LogInfo("WaitThreadEx: Done.");

		return nResult;
	}

	LogInfo("WaitThreadEx: The specified thread has terminated.");

	// Once we get here, the thread handle is completely useless, so
	// free the memory assocaited with it and invalidate the thread
	// handle.  This is necessary because thread handles are allocated
	// on the heap.

	LogInfo(
			"WaitThreadEx: The thread with handle at the memory address %0x has terminated.");

	LogInfo("WaitThreadEx: Deallocating the memory occupied by it...");

	_FreeThread(hThread);

	LogInfo("WaitThreadEx: The terminated thread has been deallocated.");

	LogInfo("WaitThreadEx: Result = %d", nResult);

	LogInfo("WaitThreadEx: Done.");

	return nResult;
}

