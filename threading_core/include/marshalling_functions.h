// marshalling_functions.h - Interface for functions that assist in
// marshalling data across thread boundaries.
//

#ifndef __MARSHALLING_FUNCTIONS_H__
#define __MARSHALLING_FUNCTIONS_H__

/**
 * @brief Called to marshal a value from the stack to the shared heap and
 * across a thread boundary.
 * @param pvData Address of the data to be marshalled.
 * @param nBlockSize Size of the data to be marshalled.
 * @return If successful, address of the data where it has been copied to the
 * global heap.
 * @remarks It's unnecessary to marshal blocks of memory that are already
 * on the heap; so, if you have a pointer to a heap block already, just pass
 * it to the thread.  This function is meant for allowing threads to access
 * data that you already know is on the stack frame of the calling function.
 * An exception is thrown (and the calling application is killed) if the
 * operation failed.  Typcially, this is the case when there is insufficient
 * storage space on the global heap for the data block. To 'demarshal' the
 * data block, all you need to do is call free() on the pointer that this
 * function returns.
 */
void* MarshalBlock(void* pvData, int nBlockSize);

/**
 * @brief Called to marshal an integer from the stack to the heap and
 * across a thread boundary.
 * @param value Value of the integer you want to marshal.
 * @return Address, on the heap, of the integer value to send across a
 * thread boundary.
 */
int* MarshalInt(int value);

/**
 * @brief Called to demarshal an integer across a thread boundary.
 * @param pnValue Address of the heap storage containing the value to be
 * demarshalled.
 * @return Integer value that is recovered from over a thread boundary.
 * @remarks The DeMarshalInt function will free the memory block referenced by
 * ppnValue and set the pointer at the address specified by ppnValue to NULL.
 */
int DeMarshalInt(int* pnValue);

#endif //__MARSHALLING_FUNCTIONS_H__
