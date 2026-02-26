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
 * \brief The heap template class encapsulation.
 */
template <typename T,
          typename Container = std::vector<T>,
          typename Compare = std::less<typename Container::value_type>>
class LLBC_Heap : public std::priority_queue<T, Container, Compare>
{
public:
    LLBC_Heap(): std::priority_queue<T, Container, Compare>() {}
    LLBC_Heap(const Container &cont): std::priority_queue<T, Container, Compare>(Compare(), cont) {}

public:
    auto begin() -> decltype(this->c.begin()) { return this->c.begin(); }
    auto end() -> decltype(this->c.end()) { return this->c.end(); }
    /**
     * \brief Get the begin and end iterator of the heap.
     */
    auto begin() const -> decltype(this->c.begin()) { return this->c.begin(); }
    auto end() const -> decltype(this->c.end()) { return this->c.end(); }

    /**
     * Remove the specified value from the heap.
     * @param[in] value - the value to remove.
     * @param[in] reverse_find - if true, search from the end of the container.
     */
    void erase(const T &value, bool reverse_find = false)
    {
        typename Container::iterator it;

        // Find the target value in the container.
        auto &c = this->c;
        if (reverse_find)
        {
            const auto rit = std::find(c.rbegin(), c.rend(), value);
            LLBC_ReturnIf(rit == c.rend(), void());

            it = --(rit.base());
        }
        else
        {
            it = std::find(c.begin(), c.end(), value);
            LLBC_ReturnIf(it == c.end(), void());
        }

        // If it is the first element, call pop() and return.
        const size_t size = c.size();
        const size_t index = it - c.begin();
        if(index == 0)
        {
            this->pop();
            return;
        }

        // If it is the last element, erase it directly and return.
        if (index == size - 1)
        {
            c.erase(it);
            return;
        }

        // Move the last element to the current position.
        *it = std::move(c[size - 1]);
        c.pop_back();

        // Restore heap property by percolating down/percolating up.
        if (/*index > 0 && */this->comp(c.data()[((index - 1) >> 1)], *it))
            percolate_up(index);
        else
            percolate_down(index);
    }

private:
    /**
     * Percolate up operation to restore heap property.
     * @param[in] index - the index of the element to percolate up.
     */
    LLBC_FORCE_INLINE void percolate_up(size_t index)
    {
        auto c_data = this->c.data();
        const auto &comp = this->comp;

        size_t parent;
        T elem = std::move(c_data[index]);
        for (; index > 0; index = parent)
        {
            parent = (index - 1) >> 1;
            if (comp(c_data[parent], elem))
                c_data[index] = std::move(c_data[parent]);
            else
                break;
        }

        c_data[index] = std::move(elem);
    }

    /**
     * Percolate down operation to restore heap property.
     * @param[in] index - the index of the element to percolate down.
     */
    LLBC_FORCE_INLINE void percolate_down(size_t index)
    {
        auto c_data = this->c.data();
        const auto &comp = this->comp;

        size_t child;
        T elem = std::move(c_data[index]);
        const size_t size = this->c.size();
        for (; (child = (index << 1) + 1) < size; index = child)
        {
            if (child != (size - 1) && comp(c_data[child], c_data[child + 1]))
                ++child;

            if (comp(elem, c_data[child]))
                c_data[index] = std::move(c_data[child]);
            else
                break;
        }

        c_data[index] = std::move(elem);
    }
};

__LLBC_NS_END

