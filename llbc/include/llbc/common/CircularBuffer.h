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

#ifndef __LLBC_COMMON_CIRCULAR_BUFFER_H__
#define __LLBC_COMMON_CIRCULAR_BUFFER_H__

template <typename ObjectType>
class CircularBuffer
{
public:
    CircularBuffer(const size_t capacity);
    ~CircularBuffer();

public:
    /**
    * Determines whether the buffer is full.
    * @return bool - return true if full, otherwise return false.
    */
    bool IsFull();

    /**
    * Determines whether the buffer is empty.
    * @return bool - return true if empty, otherwise return false.
    */
    bool IsEmpty();

    /**
    * Insert object to the tail.
    * @param[in] obj - The object.
    */
    void Push(const ObjectType &obj);

    /**
    * Get object from front.
    * @return ObjectType - The object.
    */
    ObjectType Pop();

private:
    const size_t _capacity;

    int _front;
    int _tail;
    bool _isFull;
    
    ObjectType *_buffers;
};

#include "llbc/common/CircularBufferImpl.h"

#endif // !__LLBC_COMMON_CIRCULAR_BUFFER_H__
