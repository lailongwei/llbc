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
template <typename T, class Container = std::vector<T>, class Compare = std::less<typename Container::value_type>>
class LLBC_Heap : public std::priority_queue<T, Container, Compare>
{
public:
    /**
     * \brief Get the begin and end iterator of the heap.
     */
    auto begin() const { return this->c.begin(); }
    auto end() const { return this->c.end(); }

    /**
     * \brief Remove the specified value from the heap.
     *
     * \param value - the value to remove.
     */
    void erase(const T &value)
    {
        auto it = std::find(this->c.begin(), this->c.end(), value);
        LLBC_ReturnIf(it == this->c.end(), void());

        if(it == this->c.begin())
        {
            this->pop();
            return;
        }

        this->c.erase(it);
        std::make_heap(this->c.begin(), this->c.end(), this->comp);
    }
};

__LLBC_NS_END

