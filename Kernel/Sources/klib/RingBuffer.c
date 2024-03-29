//
//  RingBuffer.c
//  kernel
//
//  Created by Dietmar Planitzer on 5/30/21.
//  Copyright © 2021 Dietmar Planitzer. All rights reserved.
//

#include "RingBuffer.h"


// Initializes the ring buffer to empty. 'capacity' is the buffer capacity in bytes.
// This value is rounded up to the next power of 2.
errno_t RingBuffer_Init(RingBuffer* _Nonnull pBuffer, size_t capacity)
{
    pBuffer->capacity = Int_NextPowerOf2(capacity);
    pBuffer->readIdx = 0;
    pBuffer->writeIdx = 0;
    return kalloc(pBuffer->capacity * sizeof(char), (void**) &pBuffer->data);
}

// Frees the ring buffer. This frees the ring buffer storage but not the elements
// stored inside the buffer.
void RingBuffer_Deinit(RingBuffer* _Nonnull pBuffer)
{
    kfree(pBuffer->data);
    pBuffer->data = NULL;
    pBuffer->capacity = 0;
    pBuffer->readIdx = 0;
    pBuffer->writeIdx = 0;
}

// Puts a single byte into the ring buffer.  Returns 0 if the buffer is empty and
// no byte has been copied out.
size_t RingBuffer_PutByte(RingBuffer* _Nonnull pBuffer, char byte)
{
    if (RingBuffer_ReadableCount(pBuffer) < pBuffer->capacity) {
        pBuffer->data[RingBuffer_MaskIndex(pBuffer, pBuffer->writeIdx++)] = byte;
        return 1;
    } else {
        return 0;
    }
}

// Puts a sequence of bytes into the ring buffer by copying them. Returns the
// number of bytes that have been successfully copied into the buffer.
size_t RingBuffer_PutBytes(RingBuffer* _Nonnull pBuffer, const void* _Nonnull pBytes, size_t count)
{
    const int avail = RingBuffer_WritableCount(pBuffer);
    
    if (count == 0 || avail == 0) {
        return 0;
    }
    
    const int nBytesToCopy = __min(avail, count);
    for (int i = 0; i < nBytesToCopy; i++) {
        pBuffer->data[RingBuffer_MaskIndex(pBuffer, pBuffer->writeIdx + i)] = ((const char*)pBytes)[i];
    }
    pBuffer->writeIdx += nBytesToCopy;
    
    return nBytesToCopy;
}

// Gets a single byte from the ring buffer. Returns 0 if the buffer is empty and
// no byte has been copied out.
size_t RingBuffer_GetByte(RingBuffer* _Nonnull pBuffer, char* _Nonnull pByte)
{
    if (!RingBuffer_IsEmpty(pBuffer)) {
        *pByte = pBuffer->data[RingBuffer_MaskIndex(pBuffer, pBuffer->readIdx++)];
        return 1;
    } else {
        return 0;
    }
}

// Gets a sequence of bytes from the ring buffer. The bytes are copied. Returns
// 0 if the buffer is empty. Returns the number of bytes that have been copied
// to 'pBytes'. 0 is returned if nothing has been copied because 'count' is 0
// or the ring buffer is empty.
size_t RingBuffer_GetBytes(RingBuffer* _Nonnull pBuffer, void* _Nonnull pBytes, size_t count)
{
    const int avail = RingBuffer_ReadableCount(pBuffer);
    
    if (count == 0 || avail == 0) {
        return 0;
    }
    
    const int nBytesToCopy = __min(avail, count);
    for (int i = 0; i < nBytesToCopy; i++) {
        ((char*)pBytes)[i] = pBuffer->data[RingBuffer_MaskIndex(pBuffer, pBuffer->readIdx + i)];
    }
    pBuffer->readIdx += nBytesToCopy;
    
    return nBytesToCopy;
}
