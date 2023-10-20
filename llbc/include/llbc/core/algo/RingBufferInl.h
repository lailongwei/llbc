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

template <typename T>
LLBC_FORCE_INLINE LLBC_RingBuffer<T>::LLBC_RingBuffer(size_t cap)
: _elems(LLBC_Malloc(T, sizeof(T) * cap))
, _capacity(cap)

, _front(0)
, _tail(0)
, _full(false)
{
}

template <typename T>
LLBC_FORCE_INLINE LLBC_RingBuffer<T>::~LLBC_RingBuffer()
{
    Clear();
    free(_elems);
}

template <typename T>
LLBC_FORCE_INLINE void LLBC_RingBuffer<T>::Push(T &&elem)
{
    if (UNLIKELY(_full))
        ReCapacity(_capacity << 1);

    new (&_elems[_tail]) T(std::move(elem));
    if (UNLIKELY(++_tail == _capacity))
        _tail = 0;

    _full = (_tail == _front);
}

template <typename T>
LLBC_FORCE_INLINE void LLBC_RingBuffer<T>::Push(const T &elem)
{
    if (UNLIKELY(_full))
        ReCapacity(_capacity << 1);

    new (&_elems[_tail]) T(elem);
    if (UNLIKELY(++_tail == _capacity))
        _tail = 0;

    _full = (_tail == _front);
}

template <typename T>
LLBC_FORCE_INLINE T LLBC_RingBuffer<T>::Pop()
{
#if LLBC_DEBUG
    ASSERT(!IsEmpty() && "Try pop a empty LLBC_RingBuffer!");
#endif

    T elem(std::move(_elems[_front]));
    _elems[_front].~T();

    if (UNLIKELY(++_front == _capacity))
        _front = 0;

    _full = false;

    return elem;
}

template <typename T>
LLBC_FORCE_INLINE T &LLBC_RingBuffer<T>::Front()
{
    return _elems[_front];
}

template <typename T>
LLBC_FORCE_INLINE const T &LLBC_RingBuffer<T>::Front() const
{
    return const_cast<LLBC_RingBuffer *>(this)->Front();
}

template <typename T>
LLBC_FORCE_INLINE T &LLBC_RingBuffer<T>::Tail()
{
    return _tail > 0 ? _elems[_tail - 1] : _elems[_capacity - 1];
}

template <typename T>
LLBC_FORCE_INLINE const T &LLBC_RingBuffer<T>::Tail() const
{
    return const_cast<LLBC_RingBuffer *>(this)->Tail();
}

template <typename T>
LLBC_FORCE_INLINE size_t LLBC_RingBuffer<T>::GetSize() const
{
    if (_tail > _front)
        return _tail - _front;
    else if (_tail == _front)
        return _full ? _capacity : 0;
    else
        return _capacity - _front + _tail;
}

template <typename T>
LLBC_FORCE_INLINE size_t LLBC_RingBuffer<T>::GetCapacity() const
{
    return _capacity;
}

template <typename T>
LLBC_FORCE_INLINE void LLBC_RingBuffer<T>::ReCapacity(size_t newCap)
{
    if (UNLIKELY(newCap <= _capacity))
        return;

    size_t tail = 0;
    T *newElems = LLBC_Malloc(T, sizeof(T) * newCap);
    while (!IsEmpty())
        new (&newElems[tail++]) T(Pop());

    free(_elems);
    _elems = newElems;

    _front = 0;
    _tail = tail;
    _capacity = newCap;
}

template <typename T>
LLBC_FORCE_INLINE bool LLBC_RingBuffer<T>::IsFull() const
{
    return _full;
}

template <typename T>
LLBC_FORCE_INLINE bool LLBC_RingBuffer<T>::IsEmpty() const
{
    return _tail == _front && !_full;
}

template <typename T>
LLBC_FORCE_INLINE void LLBC_RingBuffer<T>::Clear()
{
    if (_front < _tail)
    {

        for (; _front != _tail; ++_front)
            _elems[_front].~T();
    }
    else if (_front != _tail || _full)
    {
        for (; _front != _capacity; ++_front)
            _elems[_front].~T();

        _front = 0;
        for (; _front != _tail; ++_front)
            _elems[_front].~T();

        _full = false;
    }
}

__LLBC_NS_END
