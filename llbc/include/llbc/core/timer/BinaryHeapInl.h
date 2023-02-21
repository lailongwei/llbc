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

template <typename T, typename Comp>
LLBC_BinaryHeap<T, Comp>::LLBC_BinaryHeap(size_t capacity)
{
    _size = 0;
    _elems.resize(capacity > 1 ? capacity : 1);
}

template <typename T, typename Comp>
LLBC_BinaryHeap<T, Comp>::LLBC_BinaryHeap(const typename LLBC_BinaryHeap<T, Comp>::Container &elems)
{
    _size = elems.size();
    _elems.resize(elems.size() + 1);

    for (size_t i = 0; i < _size; ++i)
        _elems[i + 1] = elems[i];

    this->BuildHeap();
}

template <typename T, typename Comp>
LLBC_BinaryHeap<T, Comp>::LLBC_BinaryHeap(const LLBC_BinaryHeap<T, Comp> &other)
{
    this->_size = other._size;
    this->_elems.insert(this->_elems.begin(), other._elems.begin(), other._elems.end());
}

template <typename T, typename Comp>
inline bool LLBC_BinaryHeap<T, Comp>::IsEmpty() const
{
    return _size == 0;
}

template <typename T, typename Comp>
int LLBC_BinaryHeap<T, Comp>::FindTop(T &elem) const
{
    if (this->IsEmpty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    elem = _elems[1];
    return LLBC_OK;
}

template <typename T, typename Comp>
void LLBC_BinaryHeap<T, Comp>::Insert(const T &elem)
{
    size_t size = _elems.size();
    if (_size == size - 1)
        _elems.resize(size * 2);

    size_t pos = ++_size;
    size_t parentPos = pos / 2;
    for (; pos > 1 && _comp(elem, _elems[parentPos]); pos = parentPos, parentPos /= 2)
        _elems[pos] = _elems[parentPos];
    
    _elems[pos] = elem;
}

template <typename T, typename Comp>
int LLBC_BinaryHeap<T, Comp>::DeleteTop()
{
    if (this->IsEmpty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    if (_size != 1)
        _elems[1] = _elems[_size];

    _elems[_size--] = T();

    this->PercolateDown(1);

    return LLBC_OK;
}

template <typename T, typename Comp>
int LLBC_BinaryHeap<T, Comp>::DeleteTop(T &elem)
{
    if (this->IsEmpty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    elem = _elems[1];

    if (_size != 1)
        _elems[1] = _elems[_size];

    _elems[_size--] = T();

    this->PercolateDown(1);

    return LLBC_OK;
}

template <typename T, typename Comp>
int LLBC_BinaryHeap<T, Comp>::DeleteElem(size_t index)
{
    if (this->IsEmpty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    if (index <= 0 || index > _size)
    {
        LLBC_SetLastError(LLBC_ERROR_RANGE);
        return LLBC_FAILED;
    }

    for (size_t i = index; i < _size; ++i)
        _elems[i] = _elems[i + 1];

    _size -= 1;

    this->BuildHeap();

    return LLBC_OK;
}

template<typename T, typename Comp>
int LLBC_BinaryHeap<T, Comp>::DeleteElem(const T &elem)
{
    for (size_t i = 1; i <= _size; ++i)
    {
        if (_elems[i] == elem)
            return this->DeleteElem(i);
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    return LLBC_FAILED;
}

template <typename T, typename Comp>
int LLBC_BinaryHeap<T, Comp>::DeleteElem(size_t index, T &elem)
{
    if (this->IsEmpty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    if (index <= 0 || index > _size)
    {
        LLBC_SetLastError(LLBC_ERROR_RANGE);
        return LLBC_FAILED;
    }

    elem = _elems[index];
    for (size_t i = index; i < _size; ++i)
        _elems[i] = _elems[i + 1];

    _size -= 1;

    this->BuildHeap();

    return LLBC_OK;
}

template <typename T, typename Comp>
void LLBC_BinaryHeap<T, Comp>::MakeEmpty()
{
    _size = 0;
    _elems.resize(1);
}

template <typename T, typename Comp>
size_t LLBC_BinaryHeap<T, Comp>::GetSize() const
{
    return _size;
}

template <typename T, typename Comp>
const typename LLBC_BinaryHeap<T, Comp>::Container &LLBC_BinaryHeap<T, Comp>::GetData() const
{
    return _elems;
}

template <typename T, typename Comp>
typename LLBC_BinaryHeap<T, Comp>::_This &LLBC_BinaryHeap<
    T, Comp>::operator =(const typename LLBC_BinaryHeap<T, Comp>::_This &right)
{
    this->MakeEmpty();

    this->_size = right._size;
    this->_elems.assign(right.begin(), right.end());

    return *this;
}

template <typename T, typename Comp>
inline LLBC_BinaryHeap<T, Comp>::operator bool() const
{
    return !this->IsEmpty();
}

template <typename T, typename Comp>
inline bool LLBC_BinaryHeap<T, Comp>::operator !() const
{
    return this->IsEmpty();
}

template <typename T, typename Comp>
void LLBC_BinaryHeap<T, Comp>::BuildHeap()
{
    for (size_t i = _size / 2; i > 0; --i)
        this->PercolateDown(static_cast<int>(i));
}

template <typename T, typename Comp>
void LLBC_BinaryHeap<T, Comp>::PercolateDown(int index)
{
    int child;
    T elem = _elems[index];
    for (; index * 2 <= static_cast<int>(_size); index = child)
    {
        child = index * 2;
        if (child != static_cast<int>(_size) && _comp(_elems[child + 1], _elems[child]))
            ++child;

        if (_comp(_elems[child], elem))
            _elems[index] = _elems[child];
        else
            break;
    }

    _elems[index] = elem;
}

__LLBC_NS_END
