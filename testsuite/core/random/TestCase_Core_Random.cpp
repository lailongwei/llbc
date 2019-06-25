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
    LLBC_PrintLine("core/random test:");

    // Test RandInt.
    LLBC_Random rand;
    LLBC_PrintLine("LLBC_Random::Rand(0, 100) test:");
    for(int i = 0; i < 100; ++i)
    {
        if(i != 0 && i % 20 == 0)
            LLBC_PrintLine("");

        LLBC_Print("%02d ", rand.Rand(0, 100));
    }

    LLBC_PrintLine("");
    LLBC_PrintLine("LLBC_RandInt(-100, 100) test:");
    for (int i = 0; i < 100; ++i)
    {
        if (i != 0 && i % 20 == 0)
            LLBC_PrintLine("");

        LLBC_Print("%3d ", LLBC_RandInt(-100, 100));
    }

    // Test RandReal.
    LLBC_PrintLine("");
    LLBC_PrintLine("LLBC_Random::RandReal() test:");
    for(int i = 0; i < 100; ++i)
    {
        if(i != 0 && i % 10 == 0)
            LLBC_PrintLine("");

        LLBC_Print("%.6f ", rand.RandReal());
    }

    LLBC_PrintLine("");
    LLBC_PrintLine("LLBC_RandReal() test:");
    for(int i = 0; i < 100; ++i)
    {
        if(i != 0 && i % 10 == 0)
            LLBC_PrintLine("");

        LLBC_Print("%.6f ", LLBC_RandReal());
    }

    // Test Choice.
    LLBC_PrintLine("");
    LLBC_PrintLine("LLBC_Random::Choice() test(vector range:[1, 100]):");
    std::vector<int> v;
    for (int i = 0; i < 100; ++i)
        v.push_back(i + 1);

    for (int i = 0; i < 10; ++i)
        LLBC_PrintLine("  Choice result: %d", *rand.Choice(v.begin(), v.end()));

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
