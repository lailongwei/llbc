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


#include "core/random/TestCase_Core_Random.h"

TestCase_Core_Random::TestCase_Core_Random()
{
}

TestCase_Core_Random::~TestCase_Core_Random()
{
}

int TestCase_Core_Random::Run(int argc, char *argv[])
{
    LLBC_PrintLn("core/random test:");

    // Test Rand(int end)
    LLBC_Random rand;
    Exec_RandInt_end_Test(-1, 100);
    Exec_RandInt_end_Test(-2, 100);
    Exec_RandInt_end_Test(-50, 100);
    Exec_RandInt_end_Test(-100, 100);
    Exec_RandInt_end_Test(0, 100);
    Exec_RandInt_end_Test(1, 100);
    Exec_RandInt_end_Test(2, 100);
    Exec_RandInt_end_Test(50, 100);
    Exec_RandInt_end_Test(100, 100);

    // Test Rand(int begin, int end)
    Exec_RandInt_begin_end_Test(-3, -2, 100);
    Exec_RandInt_begin_end_Test(-2, -1, 100);
    Exec_RandInt_begin_end_Test(-1, 0, 100);
    Exec_RandInt_begin_end_Test(-3, 0, 100);
    Exec_RandInt_begin_end_Test(-3, 0, 100);
    Exec_RandInt_begin_end_Test(-100, 0, 100);
    Exec_RandInt_begin_end_Test(0, 0, 100);
    Exec_RandInt_begin_end_Test(0, 1, 100);
    Exec_RandInt_begin_end_Test(1, 2, 100);
    Exec_RandInt_begin_end_Test(2, 3, 100);
    Exec_RandInt_begin_end_Test(0, 3, 100);
    Exec_RandInt_begin_end_Test(0, 100, 100);
    Exec_RandInt_begin_end_Test(-100, 100, 1000);

    // Test RandReal()
    LLBC_Print("\n");
    LLBC_PrintLn("LLBC_Random::RandReal() test:");
    for(int i = 0; i < 100; ++i)
    {
        if(i != 0 && i % 10 == 0)
            LLBC_Print("\n");

        LLBC_Print("%.6f ", rand.RandReal());
    }

    LLBC_Print("\n");
    LLBC_PrintLn("LLBC_RandReal() test:");
    for(int i = 0; i < 100; ++i)
    {
        if(i != 0 && i % 10 == 0)
            LLBC_Print("\n");

        LLBC_Print("%.6f ", LLBC_RandReal());
    }

    // Test Choice / WeightedChoice / Sample / WeightedSample.
    Exec_Choice_Test();
    Exec_Choices_Test();

    // Test shuffle.
    LLBC_Print("\n");
    LLBC_PrintLn("LLBC_Random::Shuffle() test:");
    std::vector<int> v;
    for (int i = 0; i < 100; ++i)
        v.push_back(i + 1);
    LLBC_PrintLn("- Before shuffle: %s", LLBC_Variant(v).ValueToString().c_str());

    rand.Shuffle(v.begin(), v.end());
    LLBC_PrintLn("- After shuffle: %s", LLBC_Variant(v).ValueToString().c_str());

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}

void TestCase_Core_Random::Exec_RandInt_end_Test(int end, int times)
{
    LLBC_PrintLn("LLBC_Rand(%d) test(times:%d):", end, times);
    for(int i = 0; i < times; ++i)
    {
        if(i != 0 && i % 20 == 0)
            LLBC_Print("\n");

        LLBC_Print("%3d ", LLBC_Rand(end));
    }
    LLBC_Print("\n");

    LLBC_Random rand;
    LLBC_PrintLn("LLBC_Random::Rand(%d) test(times:%d):", end, times);
    for(int i = 0; i < times; ++i)
    {
        if(i != 0 && i % 20 == 0)
            LLBC_Print("\n");

        LLBC_Print("%3d ", rand.Rand(end));
    }
    LLBC_Print("\n\n");
}

void TestCase_Core_Random::Exec_RandInt_begin_end_Test(int begin, int end, int times)
{
    LLBC_PrintLn("LLBC_Rand(%d, %d) test(times:%d):", begin, end, times);
    for(int i = 0; i < times; ++i)
    {
        if(i != 0 && i % 20 == 0)
            LLBC_Print("\n");

        LLBC_Print("%4d ", LLBC_Rand(begin, end));
    }
    LLBC_Print("\n");

    LLBC_Random rand;
    LLBC_PrintLn("LLBC_Random::Rand(%d, %d) test(times:%d):", begin, end, times);
    for(int i = 0; i < times; ++i)
    {
        if(i != 0 && i % 20 == 0)
            LLBC_Print("\n");

        LLBC_Print("%4d ", rand.Rand(begin, end));
    }
    LLBC_Print("\n\n");
}

void TestCase_Core_Random::Exec_Choice_Test()
{
    LLBC_Print("\n");
    LLBC_PrintLn("LLBC_Random::Choice() / WeightedChoice() test(vector range:[1, 100]):");

    LLBC_Random rand;

    // std::vector container - uniform Choice.
    std::vector<int> v;
    for (int i = 0; i < 100; ++i)
        v.push_back(i + 1);

    for (int i = 0; i < 10; ++i)
    {
        auto it = rand.Choice(v.begin(), v.end());
        LLBC_PrintLn("  Choice(vector) result: %d", it != v.end() ? *it : -1);
    }

    // std::list container - uniform Choice.
    std::list<int> lst{11, 22, 33, 44, 55};
    {
        auto it = rand.Choice(lst.begin(), lst.end());
        LLBC_PrintLn("  Choice(list) result: %d", it != lst.end() ? *it : -1);
    }

    // std::array container - uniform Choice.
    std::array<int, 5> arr{100, 200, 300, 400, 500};
    {
        auto it = rand.Choice(arr.begin(), arr.end());
        LLBC_PrintLn("  Choice(array) result: %d", it != arr.end() ? *it : -1);
    }

    // WeightedChoice with vector weights (only the third element has positive weight).
    {
        std::vector<int> elements{10, 20, 30};
        std::vector<int> weights{0, 0, 1};
        auto it = rand.WeightedChoice(elements.begin(), elements.end(),
                                      weights.begin(), weights.end());
        LLBC_PrintLn("  WeightedChoice(vector, weights{0,0,1}) result: %d (expect 30)",
                     it != elements.end() ? *it : -1);
    }

    // WeightedChoice with list weights (only the second element has positive weight).
    {
        std::list<int> elements{7, 8, 9};
        std::list<int> weights{0, 1, 0};
        auto it = rand.WeightedChoice(elements.begin(), elements.end(),
                                      weights.begin(), weights.end());
        LLBC_PrintLn("  WeightedChoice(list, weights{0,1,0}) result: %d (expect 8)",
                     it != elements.end() ? *it : -1);
    }

    // WeightedChoice on std::map by projecting .second to a weight vector.
    // Only key=300 has positive weight, expect it->first == 300.
    {
        std::map<int, int> m{{100, 0}, {200, 0}, {300, 1}};
        std::vector<int> ws;
        ws.reserve(m.size());
        for (auto &kv : m) ws.push_back(kv.second);

        auto it = rand.WeightedChoice(m.begin(), m.end(),
                                      ws.begin(),  ws.end());
        LLBC_PrintLn("  WeightedChoice(map, projected weights{0,0,1}) result: %d (expect 300)",
                     it != m.end() ? it->first : -1);
    }

    // Choice on empty container: expect end iterator.
    {
        std::vector<int> empty;
        auto it = rand.Choice(empty.begin(), empty.end());
        LLBC_PrintLn("  Choice(empty vector) result: %s (expect end)",
                     it != empty.end() ? "non-end" : "end");
    }

    // WeightedChoice with all-zero weights: expect end iterator.
    {
        std::vector<int> elements{1, 2, 3};
        std::vector<int> weights{0, 0, 0};
        auto it = rand.WeightedChoice(elements.begin(), elements.end(),
                                      weights.begin(), weights.end());
        LLBC_PrintLn("  WeightedChoice(vector, weights{0,0,0}) result: %s (expect end)",
                     it != elements.end() ? "non-end" : "end");
    }
}

void TestCase_Core_Random::Exec_Choices_Test()
{
    LLBC_Print("\n");
    LLBC_PrintLn("LLBC_Random::Sample() / WeightedSample() test(vector range:[1, 100]):");

    LLBC_Random rand;

    // std::vector container - uniform Sample (random-access path).
    std::vector<int> v;
    for (int i = 0; i < 100; ++i)
        v.push_back(i + 1);

    {
        std::vector<int> picked;
        rand.Sample(v.begin(), v.end(), std::back_inserter(picked), 5);
        LLBC_Print("  Sample(vector, n=5) result: ");
        for (auto x : picked)
            LLBC_Print("%d ", x);
        LLBC_Print("\n");
    }

    // std::list container - uniform Sample (forward path / reservoir).
    {
        std::list<int> lst{11, 22, 33, 44, 55, 66, 77};
        std::vector<int> picked;
        rand.Sample(lst.begin(), lst.end(), std::back_inserter(picked), 3);
        LLBC_Print("  Sample(list, n=3) result: ");
        for (auto x : picked)
            LLBC_Print("%d ", x);
        LLBC_Print("\n");
    }

    // std::array container - uniform Sample.
    {
        std::array<int, 6> arr{100, 200, 300, 400, 500, 600};
        std::vector<int> picked;
        rand.Sample(arr.begin(), arr.end(), std::back_inserter(picked), 3);
        LLBC_Print("  Sample(array, n=3) result: ");
        for (auto x : picked)
            LLBC_Print("%d ", x);
        LLBC_Print("\n");
    }

    // WeightedSample with vector weights.
    {
        std::vector<int> elements{1, 2, 3};
        std::vector<int> weights{30, 50, 20};
        std::vector<int> picked;
        rand.WeightedSample(elements.begin(), elements.end(),
                            weights.begin(),  weights.end(),
                            std::back_inserter(picked), 2);
        LLBC_Print("  WeightedSample(vector, n=2, weights{30,50,20}) result: ");
        for (auto x : picked)
            LLBC_Print("%d ", x);
        LLBC_Print("\n");
    }

    // WeightedSample with list weights.
    {
        std::list<int> elements{10, 20, 30, 40};
        std::list<int> weights{1, 2, 3, 4};
        std::vector<int> picked;
        rand.WeightedSample(elements.begin(), elements.end(),
                            weights.begin(),  weights.end(),
                            std::back_inserter(picked), 2);
        LLBC_Print("  WeightedSample(list, n=2, weights{1,2,3,4}) result: ");
        for (auto x : picked)
            LLBC_Print("%d ", x);
        LLBC_Print("\n");
    }

    // WeightedSample on std::map by projecting .second to a weight vector.
    {
        std::map<int, int> m{{1, 10}, {2, 20}, {3, 30}, {4, 40}};
        std::vector<int> ws;
        ws.reserve(m.size());
        for (auto &kv : m) ws.push_back(kv.second);

        std::vector<std::pair<int, int>> picked;
        rand.WeightedSample(m.begin(), m.end(),
                            ws.begin(), ws.end(),
                            std::back_inserter(picked), 2);
        LLBC_Print("  WeightedSample(map, n=2, projected weights{10,20,30,40}) result: ");
        for (auto &kv : picked)
            LLBC_Print("(%d,%d) ", kv.first, kv.second);
        LLBC_Print("\n");
    }

    // WeightedSample on std::unordered_map by projecting .second to a weight vector.
    {
        std::unordered_map<int, int> um{{11, 1}, {22, 2}, {33, 3}};
        std::vector<int> ws;
        ws.reserve(um.size());
        for (auto &kv : um) ws.push_back(kv.second);

        std::vector<std::pair<int, int>> picked;
        rand.WeightedSample(um.begin(), um.end(),
                            ws.begin(), ws.end(),
                            std::back_inserter(picked), 2);
        LLBC_Print("  WeightedSample(unordered_map, n=2, projected weights{1,2,3}) result: ");
        for (auto &kv : picked)
            LLBC_Print("(%d,%d) ", kv.first, kv.second);
        LLBC_Print("\n");
    }

    // Sample on empty container: expect 0 elements written.
    {
        std::vector<int> empty;
        std::vector<int> picked;
        rand.Sample(empty.begin(), empty.end(), std::back_inserter(picked), 3);
        LLBC_PrintLn("  Sample(empty vector, n=3) size: %zu (expect 0)", picked.size());
    }

    // Sample with n == 0: expect 0 elements written.
    {
        std::vector<int> picked;
        rand.Sample(v.begin(), v.end(), std::back_inserter(picked), 0);
        LLBC_PrintLn("  Sample(vector, n=0) size: %zu (expect 0)", picked.size());
    }

    // Sample with n > size: expect size clamped to container size.
    {
        std::vector<int> small{1, 2, 3};
        std::vector<int> picked;
        rand.Sample(small.begin(), small.end(), std::back_inserter(picked), 10);
        LLBC_Print("  Sample(vector{1,2,3}, n=10) size: %zu (expect 3), result.: ",
                   picked.size());
        for (auto x : picked)
            LLBC_Print("%d ", x);
        LLBC_Print("\n");
    }
}