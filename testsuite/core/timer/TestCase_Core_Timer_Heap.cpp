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

#include "core/timer/TestCase_Core_Timer_Heap.h"


int TestCase_Core_Timer_Heap::Run(int argc, char** argv)
{
    std::cout << "core/time/timer test:" << std::endl;

    MaxHeapTest();
    std::cout << std::endl;

    MinHeapTest();
    std::cout << std::endl;

    std::cout << "Press any key to continue ... ..." << std::endl;
    getchar();

    return 0;
}

void TestCase_Core_Timer_Heap::MaxHeapTest()
{
    std::cout << "LLBC_Heap max heap test:" << std::endl;

    std::vector<int> data = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    // Max heap
    LLBC_Heap<int> heap;
    for(auto d : data)
    {
        heap.push(d);
    }
    while(!heap.empty())
    {
        std::cout << "max heap top: " << heap.top() << std::endl;
        heap.pop();
    }
    std::cout << std::endl;

    for(auto d : data)
    {
        heap.push(d);
    }
    heap.erase(5);
    while(!heap.empty())
    {
        std::cout << "max heap top: " << heap.top() << std::endl;
        heap.pop();
    }

    heap.pop();
    heap.pop();
    heap.pop();
}

void TestCase_Core_Timer_Heap::MinHeapTest()
{
    std::cout << "LLBC_Heap min heap test:" << std::endl;

    std::vector<int> data = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

    // Min heap
    LLBC_Heap<int, std::vector<int>, std::greater<int>> heap;
    for(auto d : data)
    {
        heap.push(d);
    }
    while(!heap.empty())
    {
        std::cout << "min heap top: " << heap.top() << std::endl;
        heap.pop();
    }
    std::cout << std::endl;

    for(auto d : data)
    {
        heap.push(d);
    }
    heap.erase(5);
    while(!heap.empty())
    {
        std::cout << "min heap top: " << heap.top() << std::endl;
        heap.pop();
    }

    heap.pop();
    heap.pop();
    heap.pop();
}
