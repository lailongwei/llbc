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


#include "core/log/TestCase_Core_Log.h"

TestCase_Core_Log::TestCase_Core_Log()
{
}

TestCase_Core_Log::~TestCase_Core_Log()
{
}

int TestCase_Core_Log::Run(int argc, char *argv[])
{
    LLBC_PrintLine("core/log test:");

#if LLBC_TARGET_PLATFORM_IPHONE
    const LLBC_Bundle *mainBundle = LLBC_Bundle::GetMainBundle();
    if(LLBC_LoggerManagerSingleton->Initialize(mainBundle->GetBundlePath() + "/" + "Logger_Cfg.cfg") != LLBC_OK)
#else

    if(LLBC_LoggerManagerSingleton->Initialize("Logger_Cfg.cfg") != LLBC_OK)
#endif
    {
        LLBC_FilePrintLine(stderr, "Initialize logger manager failed, err: %s", LLBC_FormatLastError());
        LLBC_FilePrintLine(stderr, "Forgot copy Logger_Cfg.cfg test config file to CWD?");
        return -1;
    }

    // Use root logger to test.
    LLBC_DEBUG_LOG("This is a debug log message.");
    LLBC_DEBUG_LOG2("test_tag", "This is a debug log message.");

    Log.d("This is a debug log message(from Log.d())");
    Log.d2("test_tag", "This is a debug log message(from Log.d2())");

#if LLBC_CFG_LOG_USING_WITH_STREAM
    LLBC_DEBUG_LOG("Message type test, char: " <<'a' <<", bool: " <<true <<", uint8: " <<(uint8)8
        <<", sint16: " <<(sint16)-16 << ", uint16: " <<(uint16)16 <<", sint32: " <<-32
        <<", uint32: " <<(uint32)32 <<", long: " <<(long)-1 <<", ulong: " <<(llbc::ulong)1
        <<", sint64: " <<(sint64)-64 <<", uint64: " <<(uint64)64 <<", float: " <<(float)1.0
        <<", double: " <<2.0 <<", ldouble: " <<(ldouble)3.0);

    std::string stdStr = "This is a std::string";
    LLBC_String llbcStr = "This is a LLBC_String";
    LLBC_DEBUG_LOG("std::string operator << test: " <<stdStr <<", LLBC_String operator << test: " <<llbcStr);

    LLBC_Time now;
    LLBC_TimeSpan span(-30);
    LLBC_DEBUG_LOG("Current time: " <<now <<", TimeSpan: " <<span);

    // Test precision.
    double f = 3.14159;
    LLBC_DEBUG_LOG(std::setprecision(5) <<f);
    LLBC_DEBUG_LOG(std::setprecision(9) <<f);
    LLBC_DEBUG_LOG(std::setprecision(5) <<std::fixed <<f);
    LLBC_DEBUG_LOG(std::setprecision(9) <<std::fixed <<f);
#endif // LLBC_CFG_LOG_USING_WITH_STREAM`

    LLBC_INFO_LOG("This is a info log message.");
    LLBC_INFO_LOG2("test_tag", "This is a info log message.");
    Log.i("This is a info log message(from Log.i()).");
    Log.i2("test_tag", "This is a info log message(from Log.i2()).");
    Log.i2<TestCase_Core_Log>("This is a info log message(from Log.i2()).");

    LLBC_WARN_LOG("This is a warn log message.");
    LLBC_WARN_LOG2("test_tag", "This is a warn log message.");
    Log.w("This is a warn log message(from Log.w()).");
    Log.w2("test_tag", "This is a warn log message(from Log.w2()).");
    Log.w2<TestCase_Core_Log>("This is a warn log message(from Log.w2()).");

    LLBC_ERROR_LOG("This is a error log message.");
    LLBC_ERROR_LOG2("test_tag", "This is a error log message.");
    Log.e("This is a error log message(from Log.e()).");
    Log.e2("test_tag", "This is a error log message(from Log.e2()).");
    Log.e2<TestCase_Core_Log>("This is a error log message(from Log.e2()).");

    LLBC_FATAL_LOG("This is a fatal log message.");
    LLBC_FATAL_LOG2("test_tag", "This is a fatal log message.");
    Log.f("This is a fatal log message(from Log.f()).");
    Log.f2("test_tag", "This is a fatal log message(from Log.f2()).");
    Log.f2<TestCase_Core_Log>("This is a fatal log message(from Log.f2()).");

    // Use test logger to test.
    LLBC_DEBUG_LOG_SPEC("test", "This is a debug log message.");
    LLBC_DEBUG_LOG_SPEC2("test", "test_tag", "This is a debug log message.");
    Log.d3("test", "This is a debug log message(from Log.d3()).");
    Log.d4("test", "test_tag", "This is a debug log message(from Log.d4()).");

    LLBC_INFO_LOG_SPEC("test", "This is a info log message.");
    LLBC_INFO_LOG_SPEC2("test", "test_tag", "This is a info log message.");
    Log.i3("test", "This is a info log message(from Log.i3()).");
    Log.i4("test", "test_tag", "This is a info log message(from Log.i4()).");

    LLBC_WARN_LOG_SPEC("test", "This is a warn log message.");
    LLBC_WARN_LOG_SPEC2("test", "test_tag", "This is a warn log message.");
    Log.w3("test", "This is a warn log message(from Log.w3()).");
    Log.w4("test", "test_tag", "This is a warn log message(from Log.w4()).");

    LLBC_ERROR_LOG_SPEC("test", "This is a error log message.");
    LLBC_ERROR_LOG_SPEC2("test", "test_tag", "This is a error log message.");
    Log.e3("test", "This is a error log message(from Log.e3()).");
    Log.e4("test", "test_tag", "This is a error log message(from Log.e4()).");

    LLBC_FATAL_LOG_SPEC("test", "This is a fatal log message.");
    LLBC_FATAL_LOG_SPEC2("test", "test_tag", "This is a fatal log message.");
    Log.f3("test", "This is a fatal log message(from Log.f3()).");
    Log.f4("test", "test_tag", "This is a fatal log message(from Log.f4()).");

    // Log file delete test.
    for (int i = 0; i < 20; i++)
    {
        LLBC_DEBUG_LOG_SPEC("deltest", "This is a deltest logger message.");
        LLBC_ThreadManager::Sleep(1000);
    }

    // Peform performance test.
    LLBC_PrintLine("Perform preformance test:");
    LLBC_CPUTime begin = LLBC_CPUTime::Current();
    const int loopLmt = 500000;
    for (int i = 0; i < loopLmt; i++)
        LLBC_DEBUG_LOG_SPEC("perftest", "performance test msg");
    LLBC_LoggerManagerSingleton->Finalize();

    LLBC_CPUTime elapsed = LLBC_CPUTime::Current() - begin;
    LLBC_PrintLine("Performance test completed, "
        "log size:%d, elapsed time: %s", loopLmt, elapsed.ToString().c_str());

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
