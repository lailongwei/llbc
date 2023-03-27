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

    // Test Choice.
    LLBC_Print("\n");
    LLBC_PrintLn("LLBC_Random::Choice() test(vector range:[1, 100]):");
    std::vector<int> v;
    for (int i = 0; i < 100; ++i)
        v.push_back(i + 1);

    for (int i = 0; i < 10; ++i)
        LLBC_PrintLn("  Choice result: %d", *rand.Choice(v.begin(), v.end()));

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
