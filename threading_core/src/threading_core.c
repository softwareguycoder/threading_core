#include "stdafx.h"

#include "threading_core.h"
#include "threading_core_symbols.h"

typedef struct sigaction SIGACTION, *LPSIGACTION;

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
// CancelThread function

void CancelThread(HTHREAD hThread) {
  if (INVALID_HANDLE_VALUE == hThread) {
    return;
  }

  pthread_cancel(*((pthread_t*) hThread));
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

    exit(EXIT_FAILURE);
  }
}

///////////////////////////////////////////////////////////////////////////////
// RegisterEvent function - Registers the specified semaphore function to be
// called when the SIGSEGV signal is raised on a thread.

BOOL RegisterEvent(LPSIGNALHANDLER lpfnEventHandler) {
  /* This function is just an alias for the RegisterEventEx function
   * with a default signum value of SIGSEGV passed. */
  return RegisterEventEx(SIGSEGV, lpfnEventHandler);
}

///////////////////////////////////////////////////////////////////////////////
// RegisterEventEx function - Registers a semaphore function to be called
// when the signal indicated by signum is signalled.

BOOL RegisterEventEx(int nSignal, LPSIGNALHANDLER lpfnEventHandler) {
  BOOL bResult = FALSE;
  if (nSignal <= 0) { /* all signal codes are positive integers */
    return bResult;
  }

  // If lpfnEventHandler is NULL, then there is nothing to do.
  if (lpfnEventHandler == NULL) {
    return bResult;
  }

  SIGACTION sigAction;
  memset(&sigAction, 0, sizeof(SIGACTION));

  sigAction.sa_handler = lpfnEventHandler;
  sigemptyset(&sigAction.sa_mask);
  sigAction.sa_flags = 0;

  return OK == sigaction(nSignal, &sigAction, NULL);
}

///////////////////////////////////////////////////////////////////////////////
// SetThreadCancelState function

int SetThreadCancelState(int nState) {
  errno = OK;

  if (nState != PTHREAD_CANCEL_ENABLE
      && nState != PTHREAD_CANCEL_DISABLE) {
    return ERROR;
  }

  int nOldState = OK;
  if (OK != pthread_setcancelstate(nState, &nOldState)) {
    fprintf(stderr, ERROR_FAILED_SET_THREAD_CANCELLATION_STATE);
    perror("SetThreadCancelState");
    exit(EXIT_FAILURE);
  }

  return nOldState;
}

///////////////////////////////////////////////////////////////////////////////
// SetThreadCancelType function

int SetThreadCancelType(int nType) {
  errno = OK;

  if (nType != PTHREAD_CANCEL_DEFERRED
      && nType != PTHREAD_CANCEL_ASYNCHRONOUS) {
    errno = EINVAL;
    return ERROR;
  }

  int nOldType = OK;
  if (OK != pthread_setcanceltype(nType, &nOldType)) {
    fprintf(stderr, ERROR_FAILED_SET_THREAD_CANCELLATION_TYPE);
    perror("SetThreadCancelState");
    exit(EXIT_FAILURE);
  }

  return nOldType;
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

int WaitThreadEx(HTHREAD hThread, void **ppvRetVal) {
  int nResult = ERROR;

  if (INVALID_HANDLE_VALUE == hThread) {
    return nResult;	// Invalid thread handle passed; nothing to do.
  }

  pthread_t *pThread = (pthread_t*) hThread;

  // pthread_join wants us to dereference the HTHREAD; we can't do this
  // if pThread is NULL to begin with
  if (pThread == NULL) {
    return nResult;
  }

  // get the pthread_t referenced by the handle
  pthread_t nThreadID = *pThread;

  nResult = pthread_join(nThreadID, ppvRetVal);
  if (OK != nResult) {
    // Failed to join the specified thread.
    return nResult;
  }

  // Once we get here, the thread handle is completely useless, so
  // free the memory assocaited with it and invalidate the thread
  // handle.  This is necessary because thread handles are allocated
  // on the heap.
  _FreeThread(hThread);

  // Pass the result of pthread_join to the caller
  return nResult;
}

///////////////////////////////////////////////////////////////////////////////
