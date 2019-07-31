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

#ifdef __LLBC_COMMON_CIRCULAR_BUFFER_H__

template <typename ObjectType>
inline CircularBuffer<ObjectType>::CircularBuffer(const size_t capacity)
: _capacity(capacity)

, _front(0)
, _tail(0)
, _isFull(false)

, _buffers(new ObjectType[capacity])
{
}

template <typename ObjectType>
inline CircularBuffer<ObjectType>::~CircularBuffer()
{
    LLBC_Deletes(_buffers);
}

template <typename ObjectType>
inline bool CircularBuffer<ObjectType>::IsFull()
{
    return _isFull;
}

template <typename ObjectType>
inline bool CircularBuffer<ObjectType>::IsEmpty()
{
    return !_isFull && _tail == _front;
}

template <typename ObjectType>
inline void CircularBuffer<ObjectType>::Push(const ObjectType &obj)
{
    _buffers[_tail] = obj;
    if (++_tail == _capacity)
        _tail = 0;

    _isFull = (_tail == _front);
}

template <typename ObjectType>
inline ObjectType CircularBuffer<ObjectType>::Pop()
{
    ObjectType &obj = _buffers[_front];

    if (++_front == _capacity)
        _front = 0;

    if (_isFull)
        _isFull = false;

    return obj;
}

#endif // !__LLBC_COMMON_CIRCULAR_BUFFER_H__
