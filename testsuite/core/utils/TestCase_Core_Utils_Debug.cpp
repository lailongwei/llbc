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


#include "core/utils/TestCase_Core_Utils_Debug.h"

int TestCase_Core_Utils_Debug::Run(int argc, char *argv[])
{
    LLBC_PrintLn("core/utils/Util_Debug test:");

    // Byte2Hex test.
    char str[256];
    for(int i = 0; i < 256; ++i)
    {
        str[i] = static_cast<char>(i);
    }

    LLBC_PrintLn("Byte2Hex test:\n%s", LLBC_Byte2Hex(str, 256, 32).c_str());
    LLBC_Print("\n");

    // CPUTime test.
    LLBC_PrintLn("Test CPUTime:");
    {
        LLBC_PrintLn("- CpuFreq per second:%llu", LLBC_CPUTime::GetCPUFreqPerSecond());
        LLBC_Print("\n");
    }

    {
        LLBC_PrintLn("- LLBC_CPUTime(1000) + LLBC_CPUTime(1000) = %llu", 
                       static_cast<uint64>(LLBC_CPUTime(1000) + LLBC_CPUTime(1000)));
        LLBC_PrintLn("- LLBC_CPUTime(1000) - LLBC_CPUTime(500) = %llu", 
                       static_cast<uint64>(LLBC_CPUTime(1000) - LLBC_CPUTime(500)));
        LLBC_PrintLn("- LLBC_CPUTime(1000) - LLBC_CPUTime(2000) = %llu", 
                       static_cast<uint64>(LLBC_CPUTime(1000) - LLBC_CPUTime(2000)));

        LLBC_PrintLn("- LLBC_CPUTime(1000) += 500 = %llu", 
                       static_cast<uint64>(LLBC_CPUTime(1000) += 500));
        LLBC_PrintLn("- LLBC_CPUTime(1000) -= 500 = %llu", 
                       static_cast<uint64>(LLBC_CPUTime(1000) -= 500));
        LLBC_PrintLn("- LLBC_CPUTime(1000) -= 2000 = %llu", 
                       static_cast<uint64>(LLBC_CPUTime(1000) -= 2000));

        LLBC_PrintLn("- LLBC_CPUTime(1000) > LLBC_CPUTime(500) = %s",
                       LLBC_CPUTime(1000) > LLBC_CPUTime(500) ? "true" : "false");
        LLBC_PrintLn("- LLBC_CPUTime(1000) >= LLBC_CPUTime(500) = %s",
                       LLBC_CPUTime(1000) >= LLBC_CPUTime(500) ? "true" : "false");
        LLBC_PrintLn("- LLBC_CPUTime(1000) < LLBC_CPUTime(500) = %s",
                       LLBC_CPUTime(1000) < LLBC_CPUTime(500) ? "true" : "false");
        LLBC_PrintLn("- LLBC_CPUTime(1000) <= LLBC_CPUTime(500) = %s",
                       LLBC_CPUTime(1000) <= LLBC_CPUTime(500) ? "true" : "false");
        LLBC_PrintLn("- LLBC_CPUTime(1000) == LLBC_CPUTime(500) = %s",
                       LLBC_CPUTime(1000) == LLBC_CPUTime(500) ? "true" : "false");
        LLBC_PrintLn("- LLBC_CPUTime(1000) != LLBC_CPUTime(500) = %s",
                       LLBC_CPUTime(1000) != LLBC_CPUTime(500) ? "true" : "false");
    }

    {
        int loopTimes = 10000000;
        LLBC_PrintLn("- Test foreach empty loop %d times time cost", loopTimes);
        LLBC_CPUTime beg = LLBC_CPUTime::Current();
        for (int i = 0; i < loopTimes; ++i)
        {
        }

        LLBC_CPUTime cost = LLBC_CPUTime::Current() - beg;
        LLBC_PrintLn("- loop end, costTime(cpuCount:%llu): %s milli-seconds",
                       cost.GetCPUCount(), cost.ToString().c_str());
        LLBC_Print("\n");
    }

    {
        LLBC_PrintLn("- Test static LLBC_CPUTime::ToXXX() method:");
        LLBC_PrintLn("  - LLBC_CPUTime::ToSeconds(11111111111llu):%d", LLBC_CPUTime::ToSeconds(11111111111llu));
        LLBC_PrintLn("  - LLBC_CPUTime::ToMilliSeconds(100000000llu):%lld", LLBC_CPUTime::ToMilliSeconds(100000000llu));
        LLBC_PrintLn("  - LLBC_CPUTime::ToMicroSeconds(10000000llu):%lld", LLBC_CPUTime::ToMicroSeconds(10000000llu));
        LLBC_PrintLn("  - LLBC_CPUTime::ToNanoSeconds(1000000llu):%lld", LLBC_CPUTime::ToNanoSeconds(1000000llu));
        LLBC_Print("\n");
    }

    LLBC_Print("\n");

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}
