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

    // - RDTSCP support flag.
    #if LLBC_SUPPORT_RDTSC
    LLBC_PrintLn("- __LLBC_supportedRdtscp: %s", __LLBC_supportedRdtscp ? "true" : "false");
    #else
    LLBC_PrintLn("- __LLBC_supportedRdtscp: false");
    #endif

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

        sw.Pause();
        LLBC_PrintLn("- Pause watcher and sleep 1000ms...");
        LLBC_Sleep(1000);
        LLBC_PrintLn("- Done, watcher: %s", sw.ToString().c_str());

        sw.Reset();
        LLBC_PrintLn("- After reset watcher: %s", sw.ToString().c_str());

        LLBC_Sleep(618);
        LLBC_PrintLn("- After sleep 618ms, watcher: %s", sw.ToString().c_str());

        sw.Resume();
        LLBC_Sleep(618);
        LLBC_PrintLn("- After Resume & sleep 618ms, watcher: %s", sw.ToString().c_str());

        sw.Restart();
        LLBC_Sleep(618);
        sw.Restart();
        LLBC_Sleep(618);
        LLBC_PrintLn("- After Restart & sleep 618ms(2 times), watcher: %s", sw.ToString().c_str());
    }

    // - Construct from ticks test.
    {
        LLBC_PrintLn("Construct from ticks test:");
        const auto beginTick = LLBC_RdTsc();
        LLBC_Sleep(618);
        const auto elapsedTick = LLBC_RdTsc() - beginTick;

        LLBC_Stopwatch sw(elapsedTick);
        LLBC_PrintLn("- After sleep 618ms, watcher: %s", sw.ToString().c_str());
    }

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    // - Func trace test.
    LLBC_Print("\n");
    {
        // Initialize logger manager.
#if LLBC_TARGET_PLATFORM_IPHONE
        const LLBC_Bundle *mainBundle = LLBC_Bundle::GetMainBundle();
        if (LLBC_LoggerMgrSingleton->Initialize(mainBundle->GetBundlePath() + "/" + "LogTestCfg.cfg") != LLBC_OK)
#else
        const LLBC_CString logCfgFilePath = "LogTestCfg.cfg";
        const LLBC_TimeSpan logTimeOffset = LLBC_TimeSpan::FromHours(10, 30, 30);
        if (LLBC_LoggerMgrSingleton->Initialize(logCfgFilePath, logTimeOffset) != LLBC_OK)
#endif
        {
            LLBC_FilePrintLn(stderr, "Initialize logger manager failed, err: %s", LLBC_FormatLastError());
            LLBC_FilePrintLn(stderr, "Forgot copy LogTestCfg.cfg test config file to test dir?");
            return -1;
        }

        // Defer finalize logger mgr.
        LLBC_Defer(LLBC_LoggerMgrSingleton->Finalize());
        LLOG_TRACE("test func trace start.");

        // case 1. test func trace with new operator and not delete.
        {
            LLBC_FUNC_TRACE_EX("TestAllocationScope", true);
            constexpr int num = 10 * 1024 * 1024; // 40MB
            int *data = new int[num];
            std::fill(data, data + num, 1);
            
            //delete[] data;
        }

        // case 2. test func trace with no other operation.
        {
            LLBC_FUNC_TRACE_EX("TestNoOperation", true);
            LLBC_FUNC_TRACE_EX(123, false);
        }

        // case 3. test func trace by use LLBC_FUNC_TRACE.
        {
            LLBC_FUNC_TRACE(true);
        }

        // case 4. test func trace with other logger.
        {
            LLBC_String uniqInfo("FuncTraceLoggerTest");
            LLBC_FuncTracer funcTraceTest(__FILE__, __LINE__, __FUNCTION__, false, uniqInfo, "logtrace_test");
        }

        // case 5. test tag value type.
        {
            // 5.1. value type (int)
            LLBC_FUNC_TRACE_EX(12345, false);

            // 5.2. test negative value type (sint64)
            sint64 negativeId = -999999;
            LLBC_FUNC_TRACE_EX(negativeId, false);

            // 5.3. test bool value type
            LLBC_FUNC_TRACE_EX(true, false);

            // 5.4. test uint32 / uint64
            uint32 u32Val = 4294967295U; 
            LLBC_FUNC_TRACE_EX(u32Val, false);

            // 5.5. test uint0 value type
            LLBC_FUNC_TRACE_EX(0U, false);

            // 5.6. test const string value type
            LLBC_FUNC_TRACE_EX("Static_String_Tag", false);

            // 5.7. test std::string value type
            std::string stdStr = "Dynamic_Std_String";
            LLBC_FUNC_TRACE_EX(stdStr, false);

            // 5.8. test LLBC_CString value type
            LLBC_CString llbcStr = "LLBC_Specific_String";
            LLBC_FUNC_TRACE_EX(llbcStr, false);
        }
        LLOG_TRACE("test func trace end.");
    }

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}
