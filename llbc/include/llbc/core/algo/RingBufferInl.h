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

__LLBC_NS_BEGIN

template <typename ElemType>
LLBC_FORCE_INLINE LLBC_RingBuffer<ElemType>::LLBC_RingBuffer(size_t cap)
: _elems(LLBC_Malloc(ElemType, sizeof(ElemType) * cap))
, _capacity(cap)
, _front(0)
, _tail(0)
, _full(false)
{
}

template <typename ElemType>
LLBC_FORCE_INLINE LLBC_RingBuffer<ElemType>::~LLBC_RingBuffer()
{
    while (!IsEmpty())
    {
        _elems[_front].~ElemType();
        if (UNLIKELY(++_front == _capacity))
            _front = 0;

        if (UNLIKELY(_full))
            _full = false;
    }

    free(_elems);
}

template <typename ElemType>
LLBC_FORCE_INLINE void LLBC_RingBuffer<ElemType>::Push(const ElemType &elem)
{
    if (UNLIKELY(_full))
        ReCapacity(_capacity << 1);

    new (&_elems[_tail]) ElemType(elem);
    if (UNLIKELY(++_tail == _capacity))
        _tail = 0;

    _full = (_tail == _front);
}

template <typename ElemType>
LLBC_FORCE_INLINE ElemType LLBC_RingBuffer<ElemType>::Pop()
{
#if LLBC_DEBUG
    ASSERT(!IsEmpty() && "Try pop a empty LLBC_RingBuffer!");
#endif

    // TODO: Use cxx11 rvalue ref construct.
    ElemType elem = _elems[_front];
    _elems[_front].~ElemType();

    if (UNLIKELY(++_front == _capacity))
        _front = 0;

    if (UNLIKELY(_full))
        _full = false;

    return elem;
}

template <typename ElemType>
LLBC_FORCE_INLINE ElemType &LLBC_RingBuffer<ElemType>::Front()
{
    return _elems[_front];
}

template <typename ElemType>
LLBC_FORCE_INLINE const ElemType &LLBC_RingBuffer<ElemType>::Front() const
{
    return _elems[_front];
}

template <typename ElemType>
LLBC_FORCE_INLINE ElemType &LLBC_RingBuffer<ElemType>::Tail()
{
    return _elems[_tail];
}

template <typename ElemType>
LLBC_FORCE_INLINE const ElemType &LLBC_RingBuffer<ElemType>::Tail() const
{
    return _elems[_tail];
}

template <typename ElemType>
LLBC_FORCE_INLINE size_t LLBC_RingBuffer<ElemType>::GetSize() const
{
    if (_tail > _front)
        return _tail - _front;
    else if (_tail == _front)
        return _full ? _capacity : 0;
    else
        return _capacity - _front + _tail;
}

template <typename ElemType>
LLBC_FORCE_INLINE size_t LLBC_RingBuffer<ElemType>::GetCapacity() const
{
    return _capacity;
}

template <typename ElemType>
LLBC_FORCE_INLINE void LLBC_RingBuffer<ElemType>::ReCapacity(size_t newCap)
{
    if (UNLIKELY(newCap <= _capacity))
        return;

    size_t tail = 0;
    ElemType *newElems = LLBC_Malloc(ElemType, sizeof(ElemType) * newCap);
    while (!IsEmpty())
        new (&newElems[tail++]) ElemType(Pop());

    free(_elems);
    _elems = newElems;

    _front = 0;
    _tail = tail;
    _capacity = newCap;
}

template <typename ElemType>
LLBC_FORCE_INLINE bool LLBC_RingBuffer<ElemType>::IsFull() const
{
    return _full;
}

template <typename ElemType>
LLBC_FORCE_INLINE bool LLBC_RingBuffer<ElemType>::IsEmpty() const
{
    return !_full && _tail == _front;
}

template <typename ElemType>
LLBC_FORCE_INLINE void LLBC_RingBuffer<ElemType>::Clear()
{
    if (_front < _tail)
    {

        for (; _front != _tail; ++_front)
            _elems[_front].~ElemType();
    }
    else
    {
        if (IsEmpty())
            return;

        for (; _front != _capacity; ++_front)
            _elems[_front].~ElemType();

        _front = 0;
        for (; _front != _tail; ++_front)
            _elems[_front].~ElemType();

        _full = false;
    }
}

__LLBC_NS_END
