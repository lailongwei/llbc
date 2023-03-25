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
#include <iomanip>

TestCase_Core_Log::TestCase_Core_Log()
{
}

TestCase_Core_Log::~TestCase_Core_Log()
{
}

int TestCase_Core_Log::Run(int argc, char *argv[])
{
    LLBC_PrintLn("core/log test:");
    LLBC_PrintLn("Current dir:%s", LLBC_Directory::CurDir().c_str());

    // Do uninited log message test
    DoUninitLogTest();

    // Initialize logger manager.
#if LLBC_TARGET_PLATFORM_IPHONE
    const LLBC_Bundle *mainBundle = LLBC_Bundle::GetMainBundle();
    if(LLBC_LoggerMgrSingleton->Initialize(mainBundle->GetBundlePath() + "/" + "LogTestCfg.cfg") != LLBC_OK)
#else

    if(LLBC_LoggerMgrSingleton->Initialize("LogTestCfg.cfg") != LLBC_OK)
#endif
    {
        LLBC_FilePrintLn(stderr, "Initialize logger manager failed, err: %s", LLBC_FormatLastError());
        LLBC_FilePrintLn(stderr, "Forgot copy LogTestCfg.cfg test config file to test dir?");
        return -1;
    }

    // Install logger hook(to root logger).
    LLBC_Logger *rootLogger = LLBC_LoggerMgrSingleton->GetRootLogger();
    rootLogger->InstallHook(LLBC_LogLevel::Debug,
                            LLBC_Delegate<void(const LLBC_LogData *)>(this, &TestCase_Core_Log::OnLogHook));

    // Use root logger to test.
    LLOG_TRACE("This is a trace log message.");
    LLOG_TRACE3("test_tag", "This is a trace log message.");
    LLOG_DEBUG("This is a debug log message.");
    LLOG_DEBUG3("test_tag", "This is a debug log message.");

    // Uninstall logger hook(from root logger).
    rootLogger->UninstallHook(LLBC_LogLevel::Debug);

#if LLBC_CFG_LOG_USING_WITH_STREAM
    LSLOG_DEBUG("Message type test, char: " <<'a' <<", bool: " <<true <<", uint8: " <<(uint8)8
        <<", sint16: " <<(sint16)-16 << ", uint16: " <<(uint16)16 <<", sint32: " <<-32
        <<", uint32: " <<(uint32)32 <<", long: " <<(long)-1 <<", ulong: " <<(llbc::ulong)1
        <<", sint64: " <<(sint64)-64 <<", uint64: " <<(uint64)64 <<", float: " <<(float)1.0
        <<", double: " <<2.0 <<", ldouble: " <<(ldouble)3.0);

    std::string stdStr = "This is a std::string";
    LLBC_String llbcStr = "This is a LLBC_String";
    LSLOG_DEBUG("std::string operator << test: " <<stdStr <<", LLBC_String operator << test: " <<llbcStr);

    LLBC_Time now = LLBC_Time::Now();
    LLBC_TimeSpan span(-30);
    LSLOG_DEBUG("Current time: " <<now <<", TimeSpan: " <<span);

    // Test precision.
    double f = 3.14159;
    LSLOG_DEBUG(std::setprecision(5) <<f);
    LSLOG_DEBUG(std::setprecision(9) <<f);
    LSLOG_DEBUG(std::setprecision(5) <<std::fixed <<f);
    LSLOG_DEBUG(std::setprecision(9) <<std::fixed <<f);
#endif // LLBC_CFG_LOG_USING_WITH_STREAM

    LLOG_INFO("This is a info log message.");
    LLOG_INFO3("test_tag", "This is a info log message.");
    LLOG_WARN("This is a warn log message.");
    LLOG_WARN3("test_tag", "This is a warn log message.");
    LLOG_ERROR("This is a error log message.");
    LLOG_ERROR3("test_tag", "This is a error log message.");
    LLOG_FATAL("This is a fatal log message.");
    LLOG_FATAL3("test_tag", "This is a fatal log message.");

    // Use 'test' logger to test.
    LLOG_TRACE2("test", "This is a trace log message.");
    LLOG_TRACE4("test", "test_tag", "This is a trace log message.");
    LLOG_DEBUG2("test", "This is a debug log message.");
    LLOG_DEBUG4("test", "test_tag", "This is a debug log message.");
    LLOG_INFO2("test", "This is a info log message.");
    LLOG_INFO4("test", "test_tag", "This is a info log message.");
    LLOG_WARN2("test", "This is a warn log message.");
    LLOG_WARN4("test", "test_tag", "This is a warn log message.");
    LLOG_ERROR2("test", "This is a error log message.");
    LLOG_ERROR4("test", "test_tag", "This is a error log message.");
    LLOG_FATAL2("test", "This is a fatal log message.");
    LLOG_FATAL4("test", "test_tag", "This is a fatal log message.");

    // Log file delete test.
    for (int i = 0; i < 5; ++i)
    {
        LLOG_DEBUG2("deltest", "This is a deltest logger message.");
        LLBC_Sleep(800);
    }

    // Peform performance test.
    const int perfTestTimes = 3;
    for (int i = 0; i < perfTestTimes; ++i)
    {
        LLBC_PrintLn("Press any key to exec performance test(times:%d):", i);
        getchar();

        LLBC_ObjectPoolStat opStat;
        LLBC_LoggerMgrSingleton->GetLogger("perftest")->GetLoggerObjectPool().Stat(opStat);
        LLBC_PrintLn("perftest logger object pool stat:\n%s", opStat.ToString().c_str());

        LLBC_CPUTime begin = LLBC_CPUTime::Current();
        const int loopLmt = 2000000;
        for (int i = 0; i < loopLmt; ++i)
            LLOG_DEBUG2("perftest", "performance test msg, msg idx:%d", i);

        LLBC_CPUTime elapsed = LLBC_CPUTime::Current() - begin;
        LLBC_PrintLn("Performance test completed, "
                       "log times:%d, cost:%s ms, per-log cost:%.3f us",
                       loopLmt,
                       elapsed.ToString().c_str(),
                       elapsed.ToNanoSeconds() / static_cast<double>(loopLmt) / 1000.0);
    }

    LLBC_PrintLn("Press any key to begin json log test");
    getchar();

    // Test json styled log.
    DoJsonLogTest();

    int jsonLogTestTimes = 30;
    for (int i = 0; i < jsonLogTestTimes; ++i)
        DoJsonLogTest();

    // Finalize logger.
    LLBC_LoggerMgrSingleton->Finalize();

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}

void TestCase_Core_Log::DoJsonLogTest()
{
    // Test LJLOG_TRACE macros.
    LJLOG_TRACE().Add("testKey", "testValue->LJLOG_TRACE().1").Finish("");
    LJLOG_TRACE().Add("testKey", "testValue->LJLOG_TRACE().2").Finish("%s", "Finish Test");
    LJLOG_TRACE().Add("testKey", "testValue->LJLOG_TRACE().3").Finish("%s%d", "Finish Test", 2);

    LJLOG_TRACE2(nullptr).Add("testKey", "testValue->LJLOG_TRACE3().1").Finish("");
    LJLOG_TRACE2("").Add("testKey", "testValue->LJLOG_TRACE3().2").Finish("");
    LJLOG_TRACE2("test").Add("testKey", "testValue->LJLOG_TRACE3().3").Finish("");
    LJLOG_TRACE2("test").Add("testKey", "testValue->LJLOG_TRACE4().4").Finish("%s", "Finish Test");
    LJLOG_TRACE2("test").Add("testKey", "testValue->LJLOG_TRACE4().5").Finish("%s%d", "Finish Test", 2);

    LJLOG_TRACE3("testTag").Add("testKey", "testValue->LJLOG_TRACE2().1").Finish("");
    LJLOG_TRACE3("testTag").Add("testKey", "testValue->LJLOG_TRACE2().2").Finish("%s", "Finish Test");
    LJLOG_TRACE3("testTag").Add("testKey", "testValue->LJLOG_TRACE2().3").Finish("%s%d", "Finish Test", 2);

    LJLOG_TRACE4(nullptr, "testTraceTag4_1").Add("testKey", "LJLOG_TRACE4().1").Finish("");
    LJLOG_TRACE4("", "testTraceTag4_2").Add("testKey", "testValue->LJLOG_TRACE4().2").Finish("");
    LJLOG_TRACE4("test", "testTraceTag4_3").Add("testKey", "testValue->LJLOG_TRACE4().3").Finish("");
    LJLOG_TRACE4("test", "testTraceTag4_4").Add("testKey", "testValue->LJLOG_TRACE4().4").Finish("%s", "Finish Test");
    LJLOG_TRACE4("test", "testTraceTag4_5").Add("testKey", "testValue->LJLOG_TRACE4().5").Finish("%s%d", "Finish Test", 2);

    // Test LJLOG_DEBUG macros.
    LJLOG_DEBUG().Add("testKey", "testValue->LJLOG_DEBUG().1").Finish("");
    LJLOG_DEBUG().Add("testKey", "testValue->LJLOG_DEBUG().2").Finish("%s", "Finish Test");
    LJLOG_DEBUG().Add("testKey", "testValue->LJLOG_DEBUG().3").Finish("%s%d", "Finish Test", 2);

    LJLOG_DEBUG2(nullptr).Add("testKey", "testValue->LJLOG_DEBUG3().1").Finish("");
    LJLOG_DEBUG2("").Add("testKey", "testValue->LJLOG_DEBUG3().2").Finish("");
    LJLOG_DEBUG2("test").Add("testKey", "testValue->LJLOG_DEBUG3().3").Finish("");
    LJLOG_DEBUG2("test").Add("testKey", "testValue->LJLOG_DEBUG4().4").Finish("%s", "Finish Test");
    LJLOG_DEBUG2("test").Add("testKey", "testValue->LJLOG_DEBUG4().5").Finish("%s%d", "Finish Test", 2);

    LJLOG_DEBUG3("testTag").Add("testKey", "testValue->LJLOG_DEBUG2().1").Finish("");
    LJLOG_DEBUG3("testTag").Add("testKey", "testValue->LJLOG_DEBUG2().2").Finish("%s", "Finish Test");
    LJLOG_DEBUG3("testTag").Add("testKey", "testValue->LJLOG_DEBUG2().3").Finish("%s%d", "Finish Test", 2);

    LJLOG_DEBUG4(nullptr, "testDbgTag4_1").Add("testKey", "LJLOG_DEBUG4().1").Finish("");
    LJLOG_DEBUG4("", "testDbgTag4_2").Add("testKey", "testValue->LJLOG_DEBUG4().2").Finish("");
    LJLOG_DEBUG4("test", "testDbgTag4_3").Add("testKey", "testValue->LJLOG_DEBUG4().3").Finish("");
    LJLOG_DEBUG4("test", "testDbgTag4_4").Add("testKey", "testValue->LJLOG_DEBUG4().4").Finish("%s", "Finish Test");
    LJLOG_DEBUG4("test", "testDbgTag4_5").Add("testKey", "testValue->LJLOG_DEBUG4().5").Finish("%s%d", "Finish Test", 2);

    // Test LJLOG_INFO macros.
    LJLOG_INFO().Add("testKey", "testValue->LJLOG_INFO().1").Finish("");
    LJLOG_INFO().Add("testKey", "testValue->LJLOG_INFO().2").Finish("%s", "Finish Test");
    LJLOG_INFO().Add("testKey", "testValue->LJLOG_INFO().3").Finish("%s%d", "Finish Test", 2);

    LJLOG_INFO2(nullptr).Add("testKey", "testValue->LJLOG_INFO3().1").Finish("");
    LJLOG_INFO2("").Add("testKey", "testValue->LJLOG_INFO3().2").Finish("");
    LJLOG_INFO2("test").Add("testKey", "testValue->LJLOG_INFO3().3").Finish("");
    LJLOG_INFO2("test").Add("testKey", "testValue->LJLOG_INFO3().4").Finish("%s", "Finish Test");
    LJLOG_INFO2("test").Add("testKey", "testValue->LJLOG_INFO3().5").Finish("%s%d", "Finish Test", 2);

    LJLOG_INFO3("testTag").Add("testKey", "testValue->LJLOG_INFO2().1").Finish("");
    LJLOG_INFO3("testTag").Add("testKey", "testValue->LJLOG_INFO2().2").Finish("%s", "Finish Test");
    LJLOG_INFO3("testTag").Add("testKey", "testValue->LJLOG_INFO2().3").Finish("%s%d", "Finish Test", 2);

    LJLOG_INFO4(nullptr, "testInfoTag4_1").Add("testKey", "testValue->LJLOG_INFO4().1").Finish("");
    LJLOG_INFO4("", "testInfoTag4_2").Add("testKey", "testValue->LJLOG_INFO4().2").Finish("");
    LJLOG_INFO4("test", "testInfoTag4_3").Add("testKey", "testValue->LJLOG_INFO4().3").Finish("%s", "Finish Test");
    LJLOG_INFO4("test", "testInfoTag4_4").Add("testKey", "testValue->LJLOG_INFO4().4").Finish("%s%d", "Finish Test", 2);

    // Test LJLOG_WARN macros.
    LJLOG_WARN().Add("testKey", "testValue->LJLOG_WARN().1").Finish("");
    LJLOG_WARN().Add("testKey", "testValue->LJLOG_WARN().2").Finish("%s", "Finish Test");
    LJLOG_WARN().Add("testKey", "testValue->LJLOG_WARN().3").Finish("%s%d", "Finish Test", 2);

    LJLOG_WARN2(nullptr).Add("testKey", "testValue->LJLOG_WARN3().1").Finish("");
    LJLOG_WARN2("").Add("testKey", "testValue->LJLOG_WARN3().2").Finish("");
    LJLOG_WARN2("test").Add("testKey", "testValue->LJLOG_WARN3().3").Finish("");
    LJLOG_WARN2("test").Add("testKey", "testValue->LJLOG_WARN3().4").Finish("%s", "Finish Test");
    LJLOG_WARN2("test").Add("testKey", "testValue->LJLOG_WARN3().5").Finish("%s%d", "Finish Test", 2);

    LJLOG_WARN3("testTag").Add("testKey", "testValue->LJLOG_WARN2().1").Finish("");
    LJLOG_WARN3("testTag").Add("testKey", "testValue->LJLOG_WARN2().2").Finish("%s", "Finish Test");
    LJLOG_WARN3("testTag").Add("testKey", "testValue->LJLOG_WARN2().3").Finish("%s%d", "Finish Test", 2);

    LJLOG_WARN4(nullptr, "testWarnTag4_1").Add("testKey", "testValue->LJLOG_WARN4().1").Finish("");
    LJLOG_WARN4("", "testWranTag4_2").Add("testKey", "testValue->LJLOG_WARN4().2").Finish("");
    LJLOG_WARN4("test", "testWarnTag4_3").Add("testKey", "testValue->LJLOG_WARN4().3").Finish("%s", "Finish Test");
    LJLOG_WARN4("test", "testWarnTag4_4").Add("testKey", "testValue->LJLOG_WARN4().4").Finish("%s%d", "Finish Test", 2);

    // Test LJLOG_ERROR macros.
    LJLOG_ERROR().Add("testKey", "testValue->LJLOG_ERROR().1").Finish("");
    LJLOG_ERROR().Add("testKey", "testValue->LJLOG_ERROR().2").Finish("%s", "Finish Test");
    LJLOG_ERROR().Add("testKey", "testValue->LJLOG_ERROR().3").Finish("%s%d", "Finish Test", 2);

    LJLOG_ERROR2(nullptr).Add("testKey", "testValue->LJLOG_ERROR3().1").Finish("");
    LJLOG_ERROR2("").Add("testKey", "testValue->LJLOG_ERROR3().2").Finish("");
    LJLOG_ERROR2("test").Add("testKey", "testValue->LJLOG_ERROR3().3").Finish("");
    LJLOG_ERROR2("test").Add("testKey", "testValue->LJLOG_ERROR3().4").Finish("%s", "Finish Test");
    LJLOG_ERROR2("test").Add("testKey", "testValue->LJLOG_ERROR3().5").Finish("%s%d", "Finish Test", 2);

    LJLOG_ERROR3("testTag").Add("testKey", "testValue->LJLOG_ERROR2().1").Finish("");
    LJLOG_ERROR3("testTag").Add("testKey", "testValue->LJLOG_ERROR2().2").Finish("%s", "Finish Test");
    LJLOG_ERROR3("testTag").Add("testKey", "testValue->LJLOG_ERROR2().3").Finish("%s%d", "Finish Test", 2);

    LJLOG_ERROR4(nullptr, "testTag").Add("testKey", "testValue->LJLOG_ERROR4().1").Finish("");
    LJLOG_ERROR4("", "testTag").Add("testKey", "testValue->LJLOG_ERROR4().2").Finish("");
    LJLOG_ERROR4("test", "testTag").Add("testKey", "testValue->LJLOG_ERROR4().3").Finish("%s", "Finish Test");
    LJLOG_ERROR4("test", "testTag").Add("testKey", "testValue->LJLOG_ERROR4().4").Finish("%s%d", "Finish Test", 2);

    // Test LJLOG_FATAL macros.
    LJLOG_FATAL().Add("testKey", "testValue->LJLOG_FATAL().1").Finish("");
    LJLOG_FATAL().Add("testKey", "testValue->LJLOG_FATAL().2").Finish("%s", "Finish Test");
    LJLOG_FATAL().Add("testKey", "testValue->LJLOG_FATAL().3").Finish("%s%d", "Finish Test", 2);

    LJLOG_FATAL2(nullptr).Add("testKey", "testValue->LJLOG_FATAL3().1").Finish("");
    LJLOG_FATAL2("").Add("testKey", "testValue->LJLOG_FATAL3().2").Finish("");
    LJLOG_FATAL2("test").Add("testKey", "testValue->LJLOG_FATAL3().3").Finish("");
    LJLOG_FATAL2("test").Add("testKey", "testValue->LJLOG_FATAL().4").Finish("%s", "Finish Test");
    LJLOG_FATAL2("test").Add("testKey", "testValue->LJLOG_FATAL().5").Finish("%s%d", "Finish Test", 2);

    LJLOG_FATAL3("testTag").Add("testKey", "testValue->LJLOG_FATAL2().1").Finish("");
    LJLOG_FATAL3("testTag").Add("testKey", "testValue->LJLOG_FATAL2().2").Finish("%s", "Finish Test");
    LJLOG_FATAL3("testTag").Add("testKey", "testValue->LJLOG_FATAL2().3").Finish("%s%d", "Finish Test", 2);

    LJLOG_FATAL4(nullptr, "testTag").Add("testKey", "testValue->LJLOG_FATAL4().1").Finish("");
    LJLOG_FATAL4("", "testTag").Add("testKey", "testValue->LJLOG_FATAL4().2").Finish("");
    LJLOG_FATAL4("test", "testTag").Add("testKey", "testValue->LJLOG_FATAL4().3").Finish("%s", "Finish Test");
    LJLOG_FATAL4("test", "testTag").Add("testKey", "testValue->LJLOG_FATAL4().4").Finish("%s%d", "Finish Test", 2);
}

void TestCase_Core_Log::DoUninitLogTest()
{
    LLOG_DEBUG("This is a uninited log message");
    LLOG_DEBUG3("uninit_tag", "This is a uninited log message");
    LJLOG_DEBUG().Add("Key1", "Key1 value").Finish("This is a uninited json log message");
    LJLOG_DEBUG3("uninit_tag").Add("Key1", "Key1 value").Finish("This is a uninited json log message");
}

void TestCase_Core_Log::OnLogHook(const LLBC_LogData *logData)
{
    LLBC_PrintLn("Log hook, loggerName: %s, level: %s",
                   logData->logger->GetLoggerName().c_str(),
                   LLBC_LogLevel::GetLevelStr(logData->level).c_str());
}

