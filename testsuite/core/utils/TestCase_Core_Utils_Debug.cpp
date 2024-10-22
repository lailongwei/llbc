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

    LLBC_PrintLn("Byte2Hex test:\n%s", LLBC_Byte2Hex(str, 256, ' ', 16).c_str());
    LLBC_Print("\n");

    // Stopwatch test.
    LLBC_PrintLn("Test Stopwatch:");

    // - Output basic information.
    {
        LLBC_PrintLn("- IsHighResolution: %d", LLBC_Stopwatch::IsHighResolution());
        LLBC_PrintLn("- Frequency: %llu", LLBC_Stopwatch::GetFrequency());
        LLBC_Print("\n");
    }

    // - Stopwatch core function test.
    {
        LLBC_Stopwatch sw;
        LLBC_Sleep(1000);
        LLBC_PrintLn("- After sleep 1000ms: watcher: %s", sw.ToString().c_str());
        LLBC_Sleep(500);
        LLBC_PrintLn("- After sleep 500ms: watcher: %s", sw.ToString().c_str());
        LLBC_PrintLn("  - Watch detail info, ElapsedNanos:%llu, ElapsedTicks:%llu",
                     sw.ElapsedNanos(), sw.ElapsedTicks());

        sw.Stop();
        LLBC_PrintLn("- Stop watcher and sleep 1000ms...");
        LLBC_Sleep(1000);
        LLBC_PrintLn("- Done, watcher: %s", sw.ToString().c_str());

        sw.Reset();
        LLBC_PrintLn("- After reset watcher: %s", sw.ToString().c_str());

        LLBC_Sleep(618);
        LLBC_PrintLn("- After sleep 618ms, watcher: %s", sw.ToString().c_str());

        sw.Start();
        LLBC_Sleep(618);
        LLBC_PrintLn("- After Start & sleep 618ms, watcher: %s", sw.ToString().c_str());
    }

    LLBC_Print("\n");

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}
