// marshalling_functions.c - Implementations of the functions defined in
// marshalling_functions.h
//

#include "stdafx.h"

#include "marshalling_functions.h"
#include "threading_core.h"
#include "threading_core_symbols.h"

///////////////////////////////////////////////////////////////////////////////
// Internal-use-only functions


///////////////////////////////////////////////////////////////////////////////
// Publicly-exposed functions

///////////////////////////////////////////////////////////////////////////////
// MarshalBlock function

// NOTE: DOUBLE-CHECK that this function's argument is on the stack frame
// PRIOR to using this function!  If you give this function a heap block, the
// heap block provided will simply be duplicated, which might not be what you
// want.

void* MarshalBlock(void* pvData, int nBlockSize) {
  // OKAY, so we have the address of some data, and the data block is
  // supposedly on the stack frame of the caller (which obviously, the address
  // of has been placed on OUR stack frame just now by the compiler).
  //
  // So we allocate nBlockSize bytes of memory on the heap, and then
  // we copy the value pointed to by pvData into that memory and then return
  // the address of the newly-allocated and initialized memory block. I had
  // thought about doing a realloc here instead of malloc; however, I do not
  // want to try to free stack storage accidentally, nor do I want to mess
  // with the memory the caller provided me an address to.
  //

  if (pvData == NULL) {
    ThrowMarshalingException(ERROR_FAILED_TO_MARSHAL_BLOCK);
  }

  if (nBlockSize <= 0) {
    ThrowArgumentOutOfRangeException(ERROR_BLOCK_SIZE_INVALID);
  }

  void* pvResult = malloc(nBlockSize);
  if (pvResult == NULL) {
    ThrowMarshalingException(ERROR_FAILED_TO_MARSHAL_BLOCK);
  }

  /* Transfer the data values from the source block to the
   * newly-allocated block.  Use memmove just in case. */
  memmove(pvResult, pvData, nBlockSize);

  return pvResult;
}

///////////////////////////////////////////////////////////////////////////////
// MarshalInt function

int* MarshalInt(int value) {
  void* pvMarshalledBlock = MarshalBlock(&value, 1*sizeof(int));
  if ( pvMarshalledBlock == NULL) {
    return NULL;
  }
  int* pnResult = (int*)pvMarshalledBlock;
  return pnResult;
}

///////////////////////////////////////////////////////////////////////////////
// DeMarshalInt function

int DeMarshalInt(int* pnValue) {
  if (pnValue == NULL) {
    ThrowMarshalingException(ERROR_FAILED_TO_DEMARSHAL_INT);
  }

  int nResult = 0;
  nResult = *pnValue; /* Retrieve the value from the heap */
  FreeBuffer((void**) &pnValue);  /* release the heap memory referenced */
  return nResult; /* return the value via the stack */
}
