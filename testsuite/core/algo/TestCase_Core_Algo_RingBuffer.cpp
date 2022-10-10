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

#include "core/algo/TestCase_Core_Algo_RingBuffer.h"

TestCase_Core_Algo_RingBuffer::TestCase_Core_Algo_RingBuffer()
{
}

TestCase_Core_Algo_RingBuffer::~TestCase_Core_Algo_RingBuffer()
{
}

int TestCase_Core_Algo_RingBuffer::Run(int argc, char *argv[])
{
    std::cout << "core/algo/RingBuffer test:" << std::endl;

    DoBasicTest();
    DoPerfTest();

    std::cout << "Press any key to continue..." << std::endl;
    getchar();

    return 0;
}

void TestCase_Core_Algo_RingBuffer::DoBasicTest()
{
    std::cout << "RingBuffer Basic test:" << std::endl;
    {
        LLBC_RingBuffer<int> rb;
        std::cout << "- default ringbuffer capacity:" << rb.GetCapacity() << std::endl;
        std::cout << "- is empty: " << rb.IsEmpty() << ", is full: " << rb.IsFull() << std::endl;

        std::cout << "- push some elems..." << std::endl;
        for (size_t i = 0; i != rb.GetCapacity() / 2; ++i)
        {
            std::cout << "  - push: " << i << std::endl;
            rb.Push(static_cast<int>(i));
        }
        std::cout << "- after push, is empty: " << rb.IsEmpty() << ", is full: " << rb.IsFull() << std::endl;

        std::cout << "- pop some elems...";
        while (!rb.IsEmpty())
            std::cout << "  - pop: " << rb.Pop() << std::endl;
        std::cout << "- after pop, is empty: " << rb.IsEmpty() << ", is full: " << rb.IsFull() << std::endl;

        int oldCap = static_cast<int>(rb.GetCapacity());
        std::cout << "- auto recapacity test:" << std::endl;
        for (int i = 0; i < oldCap * 2; ++i)
        {
            std::cout << "  - push: " << i;
            rb.Push(i);
            std::cout << ", after push, capacity: " << rb.GetCapacity() << std::endl;
        }
        std::cout << "- after push elems, capacity:" << rb.GetCapacity() << ", is empty: " << rb.IsEmpty() << ", is full: " << rb.IsFull() << std::endl;
    }

    std::cout << "- random push/pop test: " << std::endl;
    {
        LLBC_RingBuffer<int> rb;
        LLBC_Random rand(static_cast<int>(LLBC_Time::NowTimestampInSecs()));
        for (int i = 0; i <100; ++i)
        {
            int pushTimes = rand.Rand(2, 1000);
            int popTimes = MAX(1, pushTimes - rand.Rand(1, 500));
            for (int j = 0; j < pushTimes; ++j)
                rb.Push(j);
            for (int j = 0; j < popTimes; ++j)
                rb.Pop();
        }

        std::cout << "- random push/pop test finished, please use dbg tools to check RingBuffer status" << std::endl;
    }

    std::cout << "get size test:" << std::endl;
    {
        LLBC_RingBuffer<int> rb(5);
        rb.Push(1);
        rb.Push(2);
        rb.Push(3);
        rb.Pop();
        std::cout << "After push(1)->push(2)->push(3)->pop(), size: " << rb.GetSize() << std::endl;

        rb.Clear();
        rb.Push(3);
        rb.Push(4);
        rb.Push(5);
        std::cout << "After push(3)->push(4)->push(5), size: " << rb.GetSize() << std::endl;

        rb.Clear();
        rb.Push(10);
        rb.Push(20);
        rb.Push(30);
        rb.Push(40);
        rb.Push(50);
        std::cout << "After push(10/20/30/40/50), size: " << rb.GetSize() << ", empty?: " << rb.IsEmpty() << ", full?: " << rb.IsFull() << std::endl;

        rb.Clear();
        std::cout <<"After clear, size: " << rb.GetSize() << ", empty?: " << rb.IsEmpty() << ", full?: " << rb.IsFull() << std::endl;
    }
}

void TestCase_Core_Algo_RingBuffer::DoPerfTest()
{
    std::cout << "Performance test:" << std::endl;

#if LLBC_DEBUG
    const int testTimes = 10000;
#else
    const int testTimes = 1000000;
#endif

    LLBC_RingBuffer<int> rb(512);
    sint64 begTestTime = LLBC_GetMicroSeconds();
    for (int i = 0; i < testTimes; ++i)
    {
        for (int j = 0; j < 512; ++j)
            rb.Push(i);
        for (int j = 0; j < 512; ++j)
            rb.Pop();
    }
    sint64 usedTime = LLBC_GetMicroSeconds() - begTestTime;
    std::cout << "- test finished, used time(micro-seconds): " << usedTime << std::endl;
}

