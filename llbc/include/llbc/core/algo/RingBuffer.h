// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The basic ring buffer data structure implement.
 */
template <typename ElemType>
class LLBC_RingBuffer
{
public:
    /**
     * Constructor.
     * @param[in] cap - default capacity.
     */
    explicit LLBC_RingBuffer(size_t cap = LLBC_CFG_CORE_ALGO_RING_BUFFER_DEFAULT_CAP);
    ~LLBC_RingBuffer();

public:
    /**
     * Push element to ring buffer.
     * Note: If ring buffer reach to limit, will auto recapacity.
     * @param[in] elem - the element.
     */
    void Push(const ElemType &elem);

    /**
     * Pop element from ring buffer.
     * Warning: Do not pop empty ring buffer, the behavior is not defined!
     * @return ElemType - the element.
     */
    ElemType Pop();

    /**
     * Get front element.
     * @return ElemType & - the front element reference.
     */
    ElemType &Front();
    /**
     * Get front element.
     * @return const ElemType & - the front element const reference.
     */
    const ElemType &Front() const;

    /**
     * Get front element.
     * @return ElemType & - the front element reference.
     */
    ElemType &Tail();
    /**
     * Get front element.
     * @return const ElemType & - the front element const reference.
     */
    const ElemType &Tail() const;

    /**
     * Get ring buffer size(used capacity).
     * @return size_t - the buffer size.
     */
    size_t GetSize() const;

    /**
     * Get ring buffer capacity.
     * @return size_t - the ring bufer capacity.
     */
    size_t GetCapacity() const;

    /**
     * Adjust ring buffer capacity.
     * @param[in] newCap - the new capacity.
     */
    void ReCapacity(size_t newCap);

public:
    /**
     * Check ring buffer is full or not.
     * @return bool - return true if full, otherwise return false.
     */
    bool IsFull() const;

    /**
     * Check ring buffer is empty or not.
     * @return bool - return true if empty, otherwise return false.
     */
    bool IsEmpty() const;

    /**
     * Clear ring buffer.
     */
    void Clear();

    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_RingBuffer);

private:
    ElemType *_elems;
    size_t _capacity;

    size_t _front;
    size_t _tail;
    bool _full;
};

__LLBC_NS_END

#include "llbc/core/algo/RingBufferInl.h"


