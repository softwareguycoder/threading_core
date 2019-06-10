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

void* MarshalBlockToThread(void* pvData, int nBlockSize) {
  // OKAY, so we have the address of some data, and the data block is
  // supposedly on the stack frame of the caller (which obviously, the address
  // of has been placed on OUR stack frame just now by the compiler).
  //
  // So we allocate nBlockSize bytes of memory on the heap, and then
  // we copy the contents of the block pointed to by pvData into that memory
  // and then return the address of the newly-allocated and initialized memory
  // block. I had thought about doing a realloc here instead of malloc;
  // however, I do not want to try to free stack storage accidentally, nor do
  // I want to mess with the memory the caller provided me an address to.
  //

  if (pvData == NULL) {
    ThrowArgumentException("pvData");
  }

  if (nBlockSize <= 0) {
    ThrowArgumentOutOfRangeException("nBlockSize");
  }

  void* pvResult = malloc(nBlockSize);
  if (pvResult == NULL) {
    ThrowMarshalingException(ERROR_FAILED_TO_MARSHAL_BLOCK);
  }

  /* Transfer the data values from the source block to the
   * newly-allocated block.  Use memmove just in case. */
  if (memmove(pvResult, pvData, nBlockSize) == NULL) {
    ThrowMarshalingException(ERROR_FAILED_TO_MARSHAL_BLOCK);
  }

  return pvResult;
}

///////////////////////////////////////////////////////////////////////////////
// DeMarshalBlockFromThread function

void DeMarshalBlockFromThread(void* pvDest, void* pvData,
    int nDataSize) {
  if (pvDest == NULL) {
    ThrowArgumentException("pvDest");
  }

  if (pvData == NULL) {
    ThrowArgumentException("pvData");
  }

  if (nDataSize <= 0) {
    ThrowArgumentOutOfRangeException("nDataSize");
  }

  /* Copy the data from the heap location (which we assume is referenced
   * by pvData) to the location referenced by pvDest (which we assume is
   * on the local stack frame of the calling function.)  We use memmove
   * to try and account for overlaps. */
  if (memmove(pvDest, pvData, nDataSize) == NULL) {
    ThrowMarshalingException(ERROR_FAILED_TO_DEMARSHAL_BLOCK);
  }

  /* Call free on the pvData pointer since we assume it's on the heap...
   * but now, we no longer need access to that memory. NOTE: Since
   * pvData was an address that was passed by value to this function,
   * any points that the caller has to the data will be useless after
   * this function is done executing. Callers should take care to set
   * any externally passed value for pvData to NULL in their code after
   * this function returns. */
  free(pvData);
}
