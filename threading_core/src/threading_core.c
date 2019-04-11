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

///////////////////////////////////////////////////////////////////////////////
// KillThread function - Signals any semaphore set up on the thread indicated
// with the SIGSEGV signal to alert the thread that it's time to cleanup/
// terminate. Does not release the resoruces for the thread, but, depending on
// what the thread does in response to the signal, resources used by it may
// be cleaned up, depending on the implementation.

void KillThread(HTHREAD hThread) {
	KillThreadEx(hThread, SIGSEGV);
}

///////////////////////////////////////////////////////////////////////////////
// KillThreadEx function - like KillThread, but lets you customize the signal
// that is sent to a semaphore implemented in the thread to indicate to the
// thread that its lifetime is at an end

void KillThreadEx(HTHREAD hThread, int signum) {
	if (INVALID_HANDLE_VALUE == hThread) {
		return;	// Invalid thread handle specified; nothing to do.
	}

	if (signum <= 0) {
		return;	// Invalid value for signum; nothing to do.
	}

	int retval = pthread_kill((pthread_t) (*hThread), signum);

	sleep(1); 	// force a context switch to let the thread do its thing

	if (OK != retval) {
		// Failed to kill and/or signal the thread

		perror("KillThreadEx");

		exit(ERROR);
	}
}

///////////////////////////////////////////////////////////////////////////////
// RegisterEvent function - Registers the specified semaphore function to be
// called when the SIGSEGV signal is raised on a thread.

void RegisterEvent(LPSIGNALHANDLER lpfnEventHandler) {
	/* This function is just an alias for the RegisterEventEx function
	 * with a default signum value of SIGSEGV passed. */
	RegisterEventEx(SIGSEGV, lpfnEventHandler);
}

///////////////////////////////////////////////////////////////////////////////
// RegisterEventEx function - Registers a semaphore function to be called
// when the signal indicated by signum is signalled.

void RegisterEventEx(int signum, LPSIGNALHANDLER lpfnEventHandler) {
	if (NULL == lpfnEventHandler) {
		return;	// No event handler function handler specified; nothing to do.
	}

	if (SIG_ERR == signal(signum, lpfnEventHandler)) {
		// Failed to register event handler
		perror("RegisterEventEx");

		exit(ERROR);
	}
}

///////////////////////////////////////////////////////////////////////////////
// WaitThread: Blocks the calling thread until the specified thread terminates.
// Does not recover any user state returned by the thread waited upon.

int WaitThread(HTHREAD hThread) {
	// delegate the implementation to the WaitThreadEx function by passing NULL
	// for the user state variable.
	return WaitThreadEx(hThread, NULL);
}

///////////////////////////////////////////////////////////////////////////////
// WaitThreadEx: Waits for a thread to terminate and also allows the calling
// thread to have access to any user state returned from the waited-on thread.

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

