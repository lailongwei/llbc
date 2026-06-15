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

    // Test Choice / Choices.
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
    LLBC_PrintLn("LLBC_Random::Choice() test(vector range:[1, 100]):");

    LLBC_Random rand;

    // std::vector container.
    std::vector<int> v;
    for (int i = 0; i < 100; ++i)
        v.push_back(i + 1);

    for (int i = 0; i < 10; ++i)
    {
        auto picked = rand.Choice(v);
        LLBC_PrintLn("  Choice(vector) result: %d", picked.has_value() ? picked->get() : -1);
    }

    // std::list container.
    std::list<int> lst{11, 22, 33, 44, 55};
    {
        auto picked = rand.Choice(lst);
        LLBC_PrintLn("  Choice(list) result: %d", picked.has_value() ? picked->get() : -1);
    }

    // std::array container.
    std::array<int, 5> arr{100, 200, 300, 400, 500};
    {
        auto picked = rand.Choice(arr);
        LLBC_PrintLn("  Choice(array) result: %d", picked.has_value() ? picked->get() : -1);
    }

    // Test Choice with vector weights (only the third element has positive weight).
    {
        std::vector<int> elements{10, 20, 30};
        std::vector<int> weights{0, 0, 1};
        auto picked = rand.Choice(elements, &weights);
        LLBC_PrintLn("  Choice(vector, vec weights{0,0,1}) result: %d (expect 30)",
                     picked.has_value() ? picked->get() : -1);
    }

    // Test Choice with list weights (only the second element has positive weight).
    {
        std::list<int> elements{7, 8, 9};
        std::list<int> weights{0, 1, 0};
        auto picked = rand.Choice(elements, &weights);
        LLBC_PrintLn("  Choice(list, list weights{0,1,0}) result: %d (expect 8)",
                     picked.has_value() ? picked->get() : -1);
    }

    // Test Choice with std::map (use pair.second as implicit weights).
    // Only key=300 has positive weight, expect picked.first == 300.
    {
        std::map<int, int> m{{100, 0}, {200, 0}, {300, 1}};
        auto picked = rand.Choice(m);
        LLBC_PrintLn("  Choice(map, .second as weights{0,0,1}) result: %d (expect 300)",
                     picked.has_value() ? picked->get().first : -1);
    }
}

void TestCase_Core_Random::Exec_Choices_Test()
{
    LLBC_Print("\n");
    LLBC_PrintLn("LLBC_Random::Choices() test(vector range:[1, 100]):");

    LLBC_Random rand;

    // std::vector container.
    std::vector<int> v;
    for (int i = 0; i < 100; ++i)
        v.push_back(i + 1);

    {
        auto picked = rand.Choices(v, 5);
        LLBC_Print("  Choices(vector, k=5) result: ");
        for (const auto &value : picked)
            LLBC_Print("%d ", value.get());
        LLBC_Print("\n");
    }

    // std::list container.
    {
        std::list<int> lst{11, 22, 33, 44, 55, 66, 77};
        auto picked = rand.Choices(lst, 3);
        LLBC_Print("  Choices(list, k=3) result: ");
        for (const auto &value : picked)
            LLBC_Print("%d ", value.get());
        LLBC_Print("\n");
    }

    // std::array container.
    {
        std::array<int, 6> arr{100, 200, 300, 400, 500, 600};
        auto picked = rand.Choices(arr, 3);
        LLBC_Print("  Choices(array, k=3) result: ");
        for (const auto &value : picked)
            LLBC_Print("%d ", value.get());
        LLBC_Print("\n");
    }

    // Test Choices with vector weights.
    {
        std::vector<int> elements{1, 2, 3};
        std::vector<int> weights{30, 50, 20};
        auto picked = rand.Choices(elements, 2, &weights);
        LLBC_Print("  Choices(vector, k=2, vec weights{30,50,20}) result: ");
        for (const auto &value : picked)
            LLBC_Print("%d ", value.get());
        LLBC_Print("\n");
    }

    // Test Choices with list weights.
    {
        std::list<int> elements{10, 20, 30, 40};
        std::list<int> weights{1, 2, 3, 4};
        auto picked = rand.Choices(elements, 2, &weights);
        LLBC_Print("  Choices(list, k=2, list weights{1,2,3,4}) result: ");
        for (const auto &value : picked)
            LLBC_Print("%d ", value.get());
        LLBC_Print("\n");
    }

    // Test Choices with std::map (use pair.second as implicit weights).
    {
        std::map<int, int> m{{1, 10}, {2, 20}, {3, 30}, {4, 40}};
        auto picked = rand.Choices(m, 2);
        LLBC_Print("  Choices(map, k=2, .second as weights{10,20,30,40}) result: ");
        for (const auto &value : picked)
            LLBC_Print("(%d,%d) ", value.get().first, value.get().second);
        LLBC_Print("\n");
    }

    // Test Choices with std::unordered_map (use pair.second as implicit weights).
    {
        std::unordered_map<int, int> um{{11, 1}, {22, 2}, {33, 3}};
        auto picked = rand.Choices(um, 2);
        LLBC_Print("  Choices(unordered_map, k=2, .second as weights{1,2,3}) result: ");
        for (const auto &value : picked)
            LLBC_Print("(%d,%d) ", value.get().first, value.get().second);
        LLBC_Print("\n");
    }
}