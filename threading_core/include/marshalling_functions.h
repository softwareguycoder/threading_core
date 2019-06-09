// marshalling_functions.h - Interface for functions that assist in
// marshalling data across thread boundaries.
//

#ifndef __MARSHALLING_FUNCTIONS_H__
#define __MARSHALLING_FUNCTIONS_H__

/**
 * @name MarshalBlockToThread
 * @brief Called to marshal a value from the stack to the shared heap and
 * across a thread boundary.
 * @param pvData Address of the data to be marshalled.
 * @param nBlockSize Size of the data to be marshalled.  Be sure to account
 * for the size of data types as well as array element count!
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
void* MarshalBlockToThread(void* pvData, int nBlockSize);

/**
 * @name DeMarshalBlockFromThread
 * @brief Called to demarshal an arbitrary data block across a thread
 * boundary.
 * @param pvDest Address of a value that is on the stack frame to which
 * demarshalled data should be copied.
 * @param pvData Address of heap storage containing the data which is
 * to be recovered from across the thread boundary.
 * @remarks This function should be utilized by calling threads to bring data
 * back over from a child thread.
 */
void DeMarshalBlockFromThread(void* pvDest, void* pvData,
    int nDataSize);

#endif //__MARSHALLING_FUNCTIONS_H__
