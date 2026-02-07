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
    LLBC_PrintLn("Timer heap test:");

    BaseTest<LLBC_Heap<int, std::vector<int>, std::less<int>>>(true);
    BaseTest<LLBC_Heap<int, std::vector<int>, std::greater<int>>>(false);

    PerfTest();

    LLBC_PrintLn("Press any key to continue ... ...");
    getchar();

    return 0;
}

template <typename Heap>
void TestCase_Core_Timer_Heap::BaseTest(bool isMaxHeap)
{
    // Log: begin test.
    LLBC_PrintLn("- Base test(%s):", isMaxHeap ? "Max heap" : "Min heap");

    // Define heap.
    Heap heap;

    // Define lambda: build heap.
    auto buildHeap = [&heap](size_t count) {
        LLBC_Print("  - Build heap(count: %lu):", count);
        for (size_t i = 0; i < count; ++i)
        {
            const auto elem = LLBC_Rand(1, 10000);
            LLBC_Print("%d ", elem);

            heap.push(elem);
        }

        LLBC_PrintLn("%s", "");
    };

    // Define lambda: dump heap.
    auto dumpHeap = [&heap](const char *outputPrefix) mutable {
        LLBC_Print("  - %s(size: %lu):", outputPrefix, heap.size());
        while (!heap.empty())
        {
            LLBC_Print("%d ", heap.top());
            heap.pop();
        }

        LLBC_PrintLn("%s", "");
    };

    // Build heap & dump heap.
    buildHeap(5);
    dumpHeap("After build heap");

    // Rebuild heap for test remove specific element..
    buildHeap(5);
    const auto removeIdx = LLBC_Rand(0, 5);
    LLBC_PrintLn("  - Remove specific elem: %d ...", *(heap.begin() + removeIdx));
    heap.erase(*(heap.begin() + removeIdx));
    dumpHeap("After remove");

    // Build heap for test pop.
    buildHeap(100);
    heap.pop();
    heap.pop();
    heap.pop();
    dumpHeap("After pop 3 times");

    LLBC_PrintLn("  - Base test(%s) finished", (isMaxHeap ? "Max heap" : "Min heap"));
}

void TestCase_Core_Timer_Heap::PerfTest()
{
    LLBC_PrintLn("PerfTest:");

    // Build initial heap.
    LLBC_Stopwatch sw;
    LLBC_Heap<int> heap;
    constexpr size_t heapSize = 100000;
    LLBC_PrintLn("- Build heap, size:%lu", heapSize);
    for (size_t i = 0; i < heapSize; ++i)
        heap.push(LLBC_Rand(0, static_cast<int>(heapSize * 10)));
    LLBC_PrintLn("  - Done, heap size:%lu, cost:%.3f ms", heap.size(), sw.Elapsed().GetTotalMicros() / 1000.0);

    // Test pop performance.
    sw.Restart();
    int popSum = 0;
    constexpr size_t popSize = static_cast<size_t>(heapSize * 0.5);
    LLBC_PrintLn("- Pop heap elems, pop size:%lu", popSize);
    for (size_t i = 0; i < popSize; ++i)
    {
        popSum += heap.top();
        heap.pop();
    }
    LLBC_PrintLn("  - Done, pop sum:%d, cost:%.3f ms", popSum, sw.Elapsed().GetTotalMicros() / 1000.0);

    // Regenerate popped elements.
    LLBC_PrintLn("- Regenerate popped elems...");
    for (size_t i = 0; i < popSize; ++i)
        heap.push(LLBC_Rand(0, static_cast<int>(heapSize * 10)));
    LLBC_PrintLn("  - Done, heap size:%lu", heap.size());

    // Test erase elements with different search strategies.
    size_t tailEraseSize = static_cast<size_t>(heapSize * 0.05);
    size_t tailEraseElemChoseBegin = heap.size() - tailEraseSize * 4;
    std::set<size_t, std::greater<size_t>> eraseIndexes;
    while (eraseIndexes.size() != tailEraseSize)
        eraseIndexes.emplace(LLBC_Rand(tailEraseElemChoseBegin, heap.size()));

    auto heap2 = heap;
    LLBC_PrintLn("- Front-find erase elems, tailEraseSize:%lu, heapSize:%lu",
                 tailEraseSize, heap.size());
    sw.Restart();
    for (auto eraseIdx : eraseIndexes)
        heap.erase(*(heap.begin() + eraseIdx), false);
    LLBC_PrintLn("  - Done, after erase, heapSize:%lu, cost:%.3f ms",
                 heap.size(), sw.Elapsed().GetTotalMicros() / 1000.0);

    LLBC_PrintLn("- Tail-find erase elems, tailEraseSize:%lu, heapSize:%lu",
                 tailEraseSize, heap.size());
    sw.Restart();
    for (auto eraseIdx : eraseIndexes)
        heap2.erase(*(heap.begin() + eraseIdx), true);
    LLBC_PrintLn("  - Done, after erase, heapSize:%lu, cost:%.3f ms",
                 heap2.size(), sw.Elapsed().GetTotalMicros() / 1000.0);
}