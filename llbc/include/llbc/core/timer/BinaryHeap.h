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
 * \brief The binary heap template class encapsulation.
 */
template <typename T, typename Comp = std::less<T> >
class LLBC_BinaryHeap
{
    typedef LLBC_BinaryHeap<T, Comp> _This;

public:
    typedef std::vector<T> Container;

public:
    explicit LLBC_BinaryHeap(size_t capacity = 256);
    explicit LLBC_BinaryHeap(const Container &elems);

    LLBC_BinaryHeap(const LLBC_BinaryHeap &other);

public:
    /**
     * Check heap empty or not.
     * @return bool - return true is empty, otherwise return false.
     */
    bool empty() const;

    /**
     * Get top element, if is empty, wlll raise abort signal.
     * @return const T & - the top element reference.
     */
    const T &top() const;

    /**
     * Get top element.
     * @param[out] elem - top element.
     * @return int - return 0 not empty, otherwise return -1.
     */
    int top(T &elem) const;

    /**
     * Insert element.
     */
    void insert(const T &elem);

    /**
     * Delete top element.
     * @return void
     */
    void pop();

    /**
     * Delete top element, and return deleted element copy.
     * @param[out] elem - deleted element.
     * @return void
     */
    void pop(T &elem);

    /**
     * Delete specific index's heap element.
     * @param[in] index - heap index.
     * @return int - return 0 if success, otherwise return -1.
     */
    int erase(size_t index);

    /**
     * Delete specific element in heap.
     * @param[in] elem - element.
     * @return int - return 0 if success, otherwise return -1.
     */
    int erase(const T &elem);

    /**
     * Delete specific index's heap element, and return deleted element copy.
     * @param[in] index - heap index.
     * @return int - return 0 if success, otherwise return -1.
     */
    int erase(size_t index, T &elem);

    /**
     * Clear heap.
     */
    void clear();

    /**
     * Get heap size.
     * @return size_t - heap size.
     */
    size_t size() const;

public:
    /**
     * = Operate function.
     */
    _This &operator=(const _This &right);

    /**
     * bool Operate function.
     */
    operator bool() const;

    /**
     * ! Operate function.
     */
    bool operator!() const;

private:
    /**
     * Build heap.
     */
    void BuildHeap();

    /**
     * Percolate heap element.
     * @param[in] index - index.
     */
    void PercolateDown(int index);

private:
    Comp _comp;

    size_t _size;
    Container _elems;
};

__LLBC_NS_END

#include "llbc/core/timer/BinaryHeapInl.h"


