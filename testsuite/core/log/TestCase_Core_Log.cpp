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

    _logCfgFilePath = "LogTestCfg.cfg";
    const LLBC_TimeSpan logTimeOffset = LLBC_TimeSpan::FromHours(10, 30, 30);
    if(LLBC_LoggerMgrSingleton->Initialize(_logCfgFilePath, logTimeOffset) != LLBC_OK)
#endif
    {
        LLBC_FilePrintLn(stderr, "Initialize logger manager failed, err: %s", LLBC_FormatLastError());
        LLBC_FilePrintLn(stderr, "Forgot copy LogTestCfg.cfg test config file to test dir?");
        return -1;
    }

    // Defer finalize logger mgr.
    LLBC_Defer(LLBC_LoggerMgrSingleton->Finalize());

    // log output control test (config-driven, XML-only).
    // Must run AFTER LoggerMgr::Initialize because it Reloads the manager
    // from `LogTestCfg.xml` to pick up the <logControls> subtree that flat
    // `.cfg` cannot carry.
    LLBC_ErrorAndReturnIf(DoLogControlTest() != LLBC_OK, LLBC_FAILED);

    // log color filter test.
    DoLogColorFilterTest();

    // log trace enable level test.
    DoLogTraceEnableLevelTest();

    // Set log hook(to root logger).
    LLBC_Logger *rootLogger = LLBC_LoggerMgrSingleton->GetRootLogger();
    rootLogger->SetLogHook({ LLBC_LogLevel::Debug, LLBC_LogLevel::Trace },
                            LLBC_Delegate<void(const LLBC_LogData *)>(this, &TestCase_Core_Log::OnLogHook));

    // Use root logger to test.
    LLOG_TRACE("This is a trace log message.");
    LLOG_TRACE3("test_tag", "This is a trace log message.");
    LLOG_DEBUG("This is a debug log message.");
    LLOG_DEBUG3("test_tag", "This is a debug log message.");

    // Clear log hook(from root logger).
    rootLogger->SetLogHook({ LLBC_LogLevel::Debug, LLBC_LogLevel::Trace }, nullptr);

    // Sync logger multi-thread test.
    SyncLoggerMultiThreadTest();

    // Test condition macro log.
    DoConditionMacroLogTest();

#if LLBC_CFG_LOG_USING_WITH_STREAM
    LSLOG_DEBUG("Message type test, char: " <<'a' <<", bool: " <<true <<", uint8: " <<(uint8)8
        <<", sint16: " <<(sint16)-16 << ", uint16: " <<(uint16)16 <<", sint32: " <<-32
        <<", uint32: " <<(uint32)32 <<", long: " <<(long)-1 <<", ulong: " <<(LLBC_NS ulong)1
        <<", sint64: " <<(sint64)-64 <<", uint64: " <<(uint64)64 <<", float: " <<(float)1.0
        <<", double: " <<2.0 <<", ldouble: " <<(ldouble)3.0);

    std::string stdStr = "This is a std::string";
    LLBC_String llbcStr = "This is a LLBC_String";
    LSLOG_DEBUG("std::string operator<< test: " <<stdStr <<", LLBC_String operator<< test: " <<llbcStr);

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

    // Log fadvise test
    LLBC_PrintLn("Press any key to begin log advise test");
    getchar();
    
    constexpr size_t ONE_MEGABYTE = 1024 * 100;
    const LLBC_String largeStringTmp(ONE_MEGABYTE, 'A');
    for (int i = 0; i < 200 * 1024; ++i) 
    {
        LLOG_INFO2("fadvise", "idx:%d discard test:%s", i, largeStringTmp.c_str());
        LLBC_Sleep(10);
    }

    // Log trace test.
    DoLogTraceTest();

    // Log file delete test.
    for (int i = 0; i < 15; ++i)
    {
        LLOG_TRACE2("deltest", "This is a deltest logger message.");
        LLBC_Sleep(800);
    }

    // Perform log level set test.
    LLBC_PrintLn("Press any key to begin log level test");
    getchar();
    DoLogLevelSetTest();

    // Peform performance test.
    const int perfTestTimes = 3;
    LLOG_ADD_TRACE2("perftest", "id", 123456789);
    for (int i = 0; i < perfTestTimes; ++i)
    {
        LLBC_PrintLn("Press any key to exec performance test(times:%d):", i);
        getchar();

        const LLBC_String objPoolStat = 
            LLBC_LoggerMgrSingleton->GetLogger("perftest")->GetLoggerObjPool().GetStatistics();
        LLBC_PrintLn("perftest logger object pool stat:\n%s", objPoolStat.c_str());

        LLBC_Stopwatch sw;
        const int loopLmt = 2000000;
        for (int i = 0; i < loopLmt; ++i)
            LLOG_TRACE2("perftest", "performance test msg, msg idx:%d", i);

        sw.Pause();
        LLBC_PrintLn("Performance test completed, "
                       "log times:%d, cost:%s ms, per-log cost:%.3f us",
                       loopLmt,
                       sw.ToString().c_str(),
                       sw.ElapsedNanos() / static_cast<double>(loopLmt) / 1000.0);
    }

    // Test json styled log.
    DoJsonLogTest();

    // Test logger mgr reload.
    LLBC_ErrorAndReturnIf(DoLoggerMgrReloadTest() != LLBC_OK, LLBC_FAILED);

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}

void TestCase_Core_Log::DoLogLevelSetTest()
{
    auto testLogger = LLBC_LoggerMgrSingleton->GetLogger("log_level_set_test");
    LLBC_PrintLn("Log level set test, test logger name:%s, default level:%s",
                 testLogger->GetLoggerName().c_str(),
                 LLBC_LogLevel::GetLevelStr(testLogger->GetLogLevel()).c_str());

    auto testOutput = [testLogger](int appenderType, int logLevel) {
        const auto ret = testLogger->SetAppenderLogLevel(appenderType, logLevel);
        LLBC_PrintLn(">>>>>>>> logger: %s, appender:%d, level: %s, ret:%d, error:%s <<<<<<<<<",
                     testLogger->GetLoggerName().c_str(),
                     appenderType,
                     LLBC_LogLevel::GetLevelStr(testLogger->GetLogLevel()).c_str(),
                     ret,
                     ret == LLBC_OK ? "Success" : LLBC_FormatLastError());
        LLOG_TRACE2("log_level_set_test", "This is a TRACE log");
        LLOG_DEBUG2("log_level_set_test", "This is a DEBUG log");
        LLOG_INFO2("log_level_set_test", "This is a INFO log");
        LLOG_WARN2("log_level_set_test", "This is a WARN log");
        LLOG_ERROR2("log_level_set_test", "This is a ERROR log");
        LLOG_FATAL2("log_level_set_test", "This is a FATAL log");
        LLBC_PrintLn(">>>>>>>> ****************** <<<<<<<<<");
    };

    testOutput(LLBC_LogAppenderType::Console, LLBC_LogLevel::Trace);
    testOutput(LLBC_LogAppenderType::File, LLBC_LogLevel::Trace);
    testOutput(LLBC_LogAppenderType::Network, LLBC_LogLevel::Trace);

    testOutput(LLBC_LogAppenderType::Console, LLBC_LogLevel::Debug);
    testOutput(LLBC_LogAppenderType::File, LLBC_LogLevel::Debug);
    testOutput(LLBC_LogAppenderType::Network, LLBC_LogLevel::Debug);

    testOutput(LLBC_LogAppenderType::Console, LLBC_LogLevel::Info);
    testOutput(LLBC_LogAppenderType::File, LLBC_LogLevel::Info);

    testOutput(LLBC_LogAppenderType::Console, LLBC_LogLevel::Warn);
    testOutput(LLBC_LogAppenderType::File, LLBC_LogLevel::Warn);

    testOutput(LLBC_LogAppenderType::Console, LLBC_LogLevel::Error);
    testOutput(LLBC_LogAppenderType::File, LLBC_LogLevel::Error);

    testOutput(LLBC_LogAppenderType::Console, LLBC_LogLevel::Fatal);
    testOutput(LLBC_LogAppenderType::File, LLBC_LogLevel::Fatal);

    testOutput(LLBC_LogAppenderType::Console, LLBC_LogLevel::End);
    testOutput(LLBC_LogAppenderType::File, LLBC_LogLevel::End);
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
    char overeSizedLog[LLBC_CFG_LOG_FORMAT_BUF_SIZE << 1] = {};
    std::fill(overeSizedLog, overeSizedLog + sizeof(overeSizedLog), 'a');
    overeSizedLog[sizeof(overeSizedLog) - 1] = '\0';
    LLBC_LogAndDoIf(true, LLBC_LogLevel::Warn, {}, "%s", overeSizedLog);

    LLOG_TRACE("This is a uninited trace log message");
    LLOG_DEBUG("This is a uninited debug log message");
    LLOG_INFO("This is a uninited info log message");
    LLOG_WARN("This is a uninited warn log message");
    LLOG_ERROR("This is a uninited error log message");
    LLOG_FATAL("This is a uninited fatal log message");

    LLOG_DEBUG3("uninit_tag", "This is a uninited log message(has tag)");

    LJLOG_DEBUG().Add("Key1", "Key1 value").Finish("This is a uninited json log message");
    LJLOG_DEBUG3("uninit_tag").Add("Key1", "Key1 value").Finish("This is a uninited json log message");
}

void TestCase_Core_Log::SyncLoggerMultiThreadTest()
{
    LLBC_PrintLn("Sync logger multi-thread test:");

    class _TestTask : public LLBC_Task
    {
    public:
        explicit _TestTask(uint32 testTimes):_testTimes(testTimes), _nowTimes(0) {  }

    public:
        void Svc() override
        {
            sint32 nowTimes;
            sint32 logTimes = 0;
            while ((nowTimes = LLBC_AtomicFetchAndAdd(&_nowTimes, 1)) < _testTimes)
            {
                ++logTimes;
                LLOG_INFO2("sync", "[%08u] I am thread %d...", nowTimes, LLBC_GetCurrentThreadId());
            }

            LLBC_PrintLn("Test thread %d exit, nowTimes:%d, logTimes:%d", LLBC_GetCurrentThreadId(), nowTimes, logTimes);
        }

        void Cleanup() override
        {
            LLOG_INFO2("sync", "Sync logger multi-thread test finished, total test times: %d", _nowTimes);
        }

    public:
        sint32 _testTimes;
        sint32 _nowTimes;
    };

    _TestTask task(500000);
    task.Activate(20);
    task.Wait();

    LLBC_PrintLn("Sync logger multi-thread test finished");
}

void TestCase_Core_Log::DoConditionMacroLogTest()
{
    LLBC_LogAndDoIf(true, Error, {});
    LLBC_LogAndDoIf(true, Error, {}, "DoConditionMacroLogTest DoIf: Purely text");
    LLBC_LogAndDoIf(true, Error, {}, "DoConditionMacroLogTest DoIf: int:%d, float:%f, string:%s",
                    1, 3.14, "hello world");

    for(int i = 0; i < 4; i++) 
    {
        LLBC_LogAndContinueIf(i == 0, Error);
        LLBC_LogAndContinueIf(i == 1, Error, "DoConditionMacroLogTest DoContinueIf: Purely text");
        LLBC_LogAndContinueIf(i == 2, Error, "DoConditionMacroLogTest DoContinueIf: int:%d, float:%f, string:%s",
                              1, 3.14, "hello world");
    }
    for(;;)
    {
        LLBC_LogAndBreakIf(true, Error);
    }
    for(;;)
    {
        LLBC_LogAndBreakIf(true, Error, "DoConditionMacroLogTest DoBreakIf: Purely text");
    }
    for(;;) 
    {
        LLBC_LogAndBreakIf(true, Error, "DoConditionMacroLogTest DoBreakIf: int:%d, float:%f, string:%s",
                           1, 3.14, "hello world");
    }

    [](){ LLBC_LogAndReturnIf(true, Error, void()); }();
    [](){ LLBC_LogAndReturnIf(true, Error, void(), "DoConditionMacroLogTest DoReturn If: Purely text"); }();
    [](){ LLBC_LogAndReturnIf(true, Error, void(), "DoConditionMacroLogTest DoReturn If: int:%d, float:%f, string:%s", 
                                                   1, 3.14, "hello world"); }();
}

int TestCase_Core_Log::DoLoggerMgrReloadTest()
{
    LLBC_PrintLn("LoggerMgr reload test, please modify logger config file, "
                 "then press any key to continue ...");
    LLBC_PrintLn("- logger config file path:%s", _logCfgFilePath.c_str());

    // Reload.
    getchar();
    LLBC_ErrorAndReturnIf(LLBC_LoggerMgrSingleton->Reload() != LLBC_OK,
                          LLBC_FAILED,
                          "Reload logger mgr failed, err:%s",
                          LLBC_FormatLastError());

    for (auto &loggerName : {"root", "test"})
    {
        LLBC_PrintLn("After reload logger mgr, output %s logger all levers log:", loggerName);
        for (int logLevel = LLBC_LogLevel::Begin; logLevel != LLBC_LogLevel::End; ++logLevel)
        {
            LLOG(loggerName, nullptr, logLevel,
                 "This is a <%s> level log for %s logger",
                 LLBC_LogLevel::GetLevelStr(logLevel).c_str(), loggerName);
        }
        LLBC_PrintLn("All levels log has been output, please check console/file output");
    }

    return LLBC_OK;
}

void TestCase_Core_Log::DoLogTraceTest()
{
    LLBC_PrintLn("Press any key to start LogTrace test...");
    getchar();

    LLBC_PrintLn("- Do LogTrace::TraceData operator bool test:");
    {
        LLBC_LogTrace::TraceKey traceKey(nullptr);
        LLBC_PrintLn("  - TraceData(nullptr) operator bool(): %s", (LLBC_LogTrace::TraceKey(nullptr) ? "true" : "false"));
        LLBC_PrintLn("  - TraceData(nullptr) operator!(): %s", (!LLBC_LogTrace::TraceKey(nullptr) ? "true" : "false"));
        LLBC_PrintLn("  - TraceData(\"0\") operator bool(): %s", (LLBC_LogTrace::TraceKey("0") ? "true" : "false"));
        LLBC_PrintLn("  - TraceData(\"0\") operator!(): %s", (!LLBC_LogTrace::TraceKey("0") ? "true" : "false"));
        LLBC_PrintLn("  - TraceData(\"\") operator bool(): %s", (LLBC_LogTrace::TraceKey("") ? "true" : "false"));
        LLBC_PrintLn("  - TraceData(\"\") operator!(): %s", (!LLBC_LogTrace::TraceKey("") ? "true" : "false"));
    }

    LLBC_PrintLn("- Do LogTrace error about test:");
    {
        // Add empty log trace content.
        auto rootLogger = LLBC_LoggerMgrSingleton->GetRootLogger();
        auto ret = rootLogger->AddLogTrace("empty_content", nullptr);
        LLBC_PrintLn("  - Add empty log trace content, return:%d, err:%s",
                     ret, LLBC_FormatLastError());

        // Add same key content limit test.
        LLBC_PrintLn("  - Same key content size limit test:");
        for (size_t i = 0; i < LLBC_CFG_CORE_LOG_TRACE_SAME_KEY_CONTENT_COUNT_LIMIT + 5; ++i)
        {
            ret = rootLogger->AddLogTrace("the_key", i);
            LLBC_PrintLn("    - Add same key content, key:the_key, times:%lu, limit:%d, ret:%d, err:%s",
                         i + 1,
                         LLBC_CFG_CORE_LOG_TRACE_SAME_KEY_CONTENT_COUNT_LIMIT,
                         ret,
                         LLBC_FormatLastError());
        }

        // Remove not exist log trace test.
        LLBC_PrintLn("  - Remove not exist log trace test(key not exist):");
        ret = rootLogger->RemoveLogTrace("not_exist_key", 10086, false);
        LLBC_PrintLn("    - Remove not exist log trace, return:%d, err:%s",
                     ret, LLBC_FormatLastError());

        LLBC_PrintLn("  - Remove not exist log trace test(content not exist):");
        ret = rootLogger->RemoveLogTrace("the_key", 10086, false);
        LLBC_PrintLn("    - Remove not exist log trace, return:%d, err:%s",
                     ret, LLBC_FormatLastError());
    }

    LLBC_PrintLn("- Do simple log trace test:");
    {
        AddLogTrace(nullptr, "the nullptr trace");
        AddLogTrace("id", 10086);
        AddLogTrace("id", 10010);
        AddLogTrace(false, true);
        AddLogTrace(false, 1.5);
        AddLogTrace(3.1415926, LLBC_LogLevel::Error);
        AddLogTrace("hello", "world");
        AddLogTrace(std::string("std::string key"), std::string("std::string content"));
        AddLogTrace(LLBC_String("LLBC_String key"), LLBC_CString("LLBC_String content"));
        AddLogTrace(LLBC_String(""), LLBC_CString("empty key content"));
    }

    LLBC_PrintLn("- Do too long key&content test:");
    {
        LLBC_String tooLongKey("key:");
        tooLongKey *= (LLBC_CFG_CORE_LOG_TRACE_KEY_LIMIT * 2);
        LLBC_String tooLongContent("content:");
        tooLongContent *= (LLBC_CFG_CORE_LOG_TRACE_CONTENT_LIMIT * 2);
        AddLogTrace(tooLongKey, tooLongContent);
    }

    LLBC_PrintLn("- Do explicit add/remove log trace test:");
    {
        auto rootLogger = LLBC_LoggerMgrSingleton->GetRootLogger();
        rootLogger->AddLogTrace("id", 8888);
        rootLogger->AddLogTrace("id", 8888);
        rootLogger->AddLogTrace("id", 8888);
        rootLogger->AddLogTrace("id", 9999);
        LLOG_INFO("After add id:8888<3times>, id:9999, id:8888 traceIimes:%lu, id:9999 traceTimes:%lu...",
                  rootLogger->GetLogTraceTimes("id", 8888), rootLogger->GetLogTraceTimes("id", 9999));

        rootLogger->RemoveLogTrace("id", 8888, false);
        rootLogger->RemoveLogTrace("id", 9999, false);
        LLOG_INFO("After remove id:8888, id:9999, id:8888 traceTimes:%lu, id:9999 traceTimes:%lu...",
                  rootLogger->GetLogTraceTimes("id", 8888), rootLogger->GetLogTraceTimes("id", 9999));

        rootLogger->AddLogTrace("id", 6666);
        rootLogger->AddLogTrace("id", 7777);
        rootLogger->RemoveLogTrace("id", 8888, true);
        LLOG_INFO("After add id:6666, id:7777, remove id:8888(setTraceTimesToZero=true), id:8888 traceTimes:%lu...",
                 rootLogger->GetLogTraceTimes("id", 8888));

        rootLogger->ClearLogTrace("id");
        LLOG_INFO("After clear log trace <id>...");

        rootLogger->AddLogTrace("id", 4444);
        rootLogger->AddLogTrace("id", 4444);
        rootLogger->AddLogTrace("id", 5555);
        rootLogger->AddLogTrace("id", 6666);
        LLOG_INFO("After add id:4444<2times>, id:5555, id:6666, "
                  "id:4444 traceTimes:%lu, id:5555 traceTimes:%lu, id:6666 traceTimes:%lu...",
                  rootLogger->GetLogTraceTimes("id", 4444),
                  rootLogger->GetLogTraceTimes("id", 5555),
                  rootLogger->GetLogTraceTimes("id", 6666));

        rootLogger->ClearAllLogTraces();
        LLOG_INFO("After clear all log traces...");
    }

    LLBC_PrintLn("- Do too long trace info test:");
    {
        LLOG_ADD_TRACE(LLBC_String("hello world") * 10, LLBC_String("hey judy") * 100);
        LLOG_ADD_TRACE(LLBC_String("Hello world") * 10, LLBC_String("hey judy") * 100);
        LLOG_ADD_TRACE(LLBC_String("HEllo world") * 10, LLBC_String("hey judy") * 100);
        LLOG_ADD_TRACE(LLBC_String("HELlo world") * 10, LLBC_String("hey judy") * 100);
        LLOG_ADD_TRACE(LLBC_String("HELLo world") * 10, LLBC_String("hey judy") * 100);
        LLOG_ADD_TRACE(LLBC_String("HELLO world") * 10, LLBC_String("hey judy") * 100);
        LLOG_ADD_TRACE(LLBC_String("WELLO world") * 10, LLBC_String("hey judy") * 100);
        LLOG_ADD_TRACE(LLBC_String("WELLO World") * 10, LLBC_String("hey judy") * 100);
        LLOG_ADD_TRACE(LLBC_String("WELLO WOrld") * 10, LLBC_String("hey judy") * 100);
        LLOG_ADD_TRACE(LLBC_String("WELLO WORld") * 10, LLBC_String("hey judy") * 100);
        LLOG_ADD_TRACE(LLBC_String("WELLO WORLd") * 10, LLBC_String("hey judy") * 100);
        LLOG_INFO("After add too long log trace info...");
    }

    LLBC_PrintLn("- Do same key content limit test:");
    {
        LLOG_ADD_TRACE("key", "content1");
        LLOG_ADD_TRACE("key", "content2");
        LLOG_ADD_TRACE("key", "content3");
        LLOG_ADD_TRACE("key", "content4");
        LLOG_ADD_TRACE("key", "content5");
        LLOG_ADD_TRACE("key", "content6");
        LLOG_ADD_TRACE("key", "content7");
        LLOG_ADD_TRACE("key", "content8");
        LLOG_INFO("After add 8 same key contents(limit:%d)...", LLBC_CFG_CORE_LOG_TRACE_SAME_KEY_CONTENT_COUNT_LIMIT);
    }

    LLBC_PrintLn("- Do complex log trace test:");
    {
        LLOG_ADD_TRACE("id", 10086);
        LLOG_ADD_TRACE("id", 10010);
        LLOG_ADD_TRACE("name", "Judy");
        LLOG_INFO("After add some log traces...");
        {
            LLOG_ADD_TRACE("id", 9527);
            LLOG_INFO("After add id:9527");

            LLOG_ADD_TRACE("id", 10086);
            LLOG_INFO("After repeatly add id:10086...");
        }

        LLOG_INFO("After remove repeatly log trace: id:10086");
    }

    LLBC_PrintLn("- Do multi-thread log trace test:");
    {
        class TestTask : public LLBC_Task
        {
        public:
            void Svc() override
            {
                for (int i = 0; i < 100; ++i)
                {
                    LLOG_ADD_TRACE2("logtrace_test", LLBC_Rand(80000, 2000000000), LLBC_Rand(50000, 500000000));
                    LLOG_INFO2("logtrace_test", "Log Trace test thread, tid:%d", LLBC_GetCurrentThreadId());
                }
            }

            void Cleanup() override {  }
        };

        auto task = new TestTask;
        task->Activate(10);
        task->Wait();
        delete task;
    }

    LLBC_PrintLn("Press any key to finish LogTrace test...");
    getchar();
}

void TestCase_Core_Log::DoLogColorFilterTest()
{
    LLBC_PrintLn("DoLogColorFilterTest begin");
    auto rootLogger = LLBC_LoggerMgrSingleton->GetRootLogger();
    // 10086 in log color filter list, log_level: WARN
    LLBC_PrintLn("fileLogLevel: %s", LLBC_LogLevel::GetLevelStr(rootLogger->GetLogLevel()).c_str());

    LLOG_WARN("10086 in log color filter list, log_level: WARN, will output 4~6");
    LLOG_TRACE("This is a uninited trace log message 1");
    LLOG_DEBUG("This is a uninited debug log message 2");
    LLOG_INFO("This is a uninited info log message 3");
    LLOG_WARN("This is a uninited warn log message 4");     // output (4~6)
    LLOG_ERROR("This is a uninited error log message 5");
    LLOG_FATAL("This is a uninited fatal log message 6");

    LLOG_FATAL("--------------------------------------------------------------------");

    rootLogger->AddLogTrace("uin", 1088); // logTrace: uin-1088
    LLOG_WARN("Key uin:1088(not in) will output 10~12");
    LLOG_TRACE("This is a uninited trace log message 7");
    LLOG_DEBUG("This is a uninited debug log message 8");
    LLOG_INFO("This is a uninited info log message 9");
    LLOG_WARN("This is a uninited warn log message 10");    // output(10~12)
    LLOG_ERROR("This is a uninited error log message 11");
    LLOG_FATAL("This is a uninited fatal log message 12");
    
    LLOG_FATAL("--------------------------------------------------------------------");
    
    rootLogger->AddLogTrace("uin", 1086); // logTrace: uin-1088, 1086
    LLOG_WARN("Add uin:1086; [uin:1088(not in),1086(in)] will output 13~18");
    LLOG_TRACE("This is a uninited trace log message 13");  // output (13~18)
    LLOG_DEBUG("This is a uninited debug log message 14");
    LLOG_INFO("This is a uninited info log message 15");
    LLOG_WARN("This is a uninited warn log message 16");
    LLOG_ERROR("This is a uninited error log message 17");
    LLOG_FATAL("This is a uninited fatal log message 18");

    LLOG_FATAL("--------------------------------------------------------------------");

    rootLogger->RemoveLogTrace("uin", 1086, true); // logTrace: uin-1088
    rootLogger->AddLogTrace("coroid", 100001);      // logTrace: uin-1088, coroid:10001
    LLOG_WARN("Remove uin:1086 add coroid:10001; [uin:1088(not in)|coroid:100001(in)] will output 19~24");
    LLOG_TRACE("This is a uninited trace log message 19");
    LLOG_DEBUG("This is a uninited debug log message 20");
    LLOG_INFO("This is a uninited info log message 21");
    LLOG_WARN("This is a uninited warn log message 22");
    LLOG_ERROR("This is a uninited error log message 23");
    LLOG_FATAL("This is a uninited fatal log message 24");

    LLOG_FATAL("--------------------------------------------------------------------");

    rootLogger->AddLogTrace("uin", 1087); ; // logTrace: uin-1088, 1087  coroid:10001
    rootLogger->RemoveLogTrace("coroid", 100001, true); // logTrace: uin-1088, 1087
    LLOG_WARN("Remove coroid:10001 add key uin:1087; [uin:1088(not in),1087(in)] will output 25~30");
    LLOG_TRACE("This is a uninited trace log message 25");
    LLOG_DEBUG("This is a uninited debug log message 26");
    LLOG_INFO("This is a uninited info log message 27");
    LLOG_WARN("This is a uninited warn log message 28");
    LLOG_ERROR("This is a uninited error log message 29");
    LLOG_FATAL("This is a uninited fatal log message 30");

    LLOG_FATAL("--------------------------------------------------------------------");

    rootLogger->RemoveLogTrace("uin", 1088, true); // logTrace: 1087
    LLOG_WARN("Remove uin:1088; [uin:1087(in)] will output 31~36");
    LLOG_TRACE("This is a uninited trace log message 31");
    LLOG_DEBUG("This is a uninited debug log message 32");
    LLOG_INFO("This is a uninited info log message 33");
    LLOG_WARN("This is a uninited warn log message 34");
    LLOG_ERROR("This is a uninited error log message 35");
    LLOG_FATAL("This is a uninited fatal log message 36");

    LLOG_FATAL("--------------------------------------------------------------------");

    rootLogger->RemoveLogTrace("uin", 1087, true);
    LLOG_WARN("Remove 1087; [] will output 40~42");
    LLOG_TRACE("This is a uninited trace log message 37");
    LLOG_DEBUG("This is a uninited debug log message 38");
    LLOG_INFO("This is a uninited info log message 39");
    LLOG_WARN("This is a uninited warn log message 40");
    LLOG_ERROR("This is a uninited error log message 41");
    LLOG_FATAL("This is a uninited fatal log message 42");

    LLOG_FATAL("--------------------------------------------------------------------");

    rootLogger->AddColorLogTrace("NickName", "playerA");
    rootLogger->AddColorLogTrace("NickName", "playerC");

    rootLogger->AddLogTrace("NickName", "playerB");
    LLOG_WARN("add key NickName:PlayerB; ["":playerB(not in)] will output 46~48");
    LLOG_TRACE("This is a uninited trace log message 43");
    LLOG_DEBUG("This is a uninited debug log message 44");
    LLOG_INFO("This is a uninited info log message 45"); 
    LLOG_WARN("This is a uninited warn log message 46"); // output(46~48)
    LLOG_ERROR("This is a uninited error log message 47");
    LLOG_FATAL("This is a uninited fatal log message 48");

    LLOG_FATAL("--------------------------------------------------------------------");

    rootLogger->AddLogTrace("NickName", "playerA");
    LLOG_WARN("[add key NickName:PlayerA: playerB(not in), PlayerA(in)] will output 49~54");
    LLOG_TRACE("This is a uninited trace log message 49"); // output(49~54)
    LLOG_DEBUG("This is a uninited debug log message 50");
    LLOG_INFO("This is a uninited info log message 51"); 
    LLOG_WARN("This is a uninited warn log message 52"); 
    LLOG_ERROR("This is a uninited error log message 53");
    LLOG_FATAL("This is a uninited fatal log message 54");

    LLOG_FATAL("--------------------------------------------------------------------");

    rootLogger->RemoveColorLogTrace("NickName", "playerA");
    LLOG_WARN("[remove NickName:PlayerA: playerB(not in -> not in)] will output 57~60");
    LLOG_TRACE("This is a uninited trace log message 55"); 
    LLOG_DEBUG("This is a uninited debug log message 56");
    LLOG_INFO("This is a uninited info log message 57"); // output(57~60)
    LLOG_WARN("This is a uninited warn log message 58"); 
    LLOG_ERROR("This is a uninited error log message 59");
    LLOG_FATAL("This is a uninited fatal log message 60");

    LLOG_FATAL("--------------------------------------------------------------------");

    rootLogger->AddLogTrace("NickName", "playerC");
    LLOG_WARN("[add key NickName:PlayerC: playerB(not in), playerA(not in), playerC(in)] will output 61~66");
    LLOG_TRACE("This is a uninited trace log message 61");  // output(61~66)
    LLOG_DEBUG("This is a uninited debug log message 62");
    LLOG_INFO("This is a uninited info log message 63");
    LLOG_WARN("This is a uninited warn log message 64"); 
    LLOG_ERROR("This is a uninited error log message 65");
    LLOG_FATAL("This is a uninited fatal log message 66");

    LLOG_FATAL("--------------------------------------------------------------------");

    rootLogger->RemoveColorLogKey("NickName");
    LLOG_WARN("[Remove all key. NickName :playerB(not in), playerA(not in), PlayerC(in -> not in)] will output 70~72");
    LLOG_TRACE("This is a uninited trace log message 67"); 
    LLOG_DEBUG("This is a uninited debug log message 68");
    LLOG_INFO("This is a uninited info log message 69"); 
    LLOG_WARN("This is a uninited warn log message 70"); // output(70~72)
    LLOG_ERROR("This is a uninited error log message 71");
    LLOG_FATAL("This is a uninited fatal log message 72");

    LLOG_FATAL("--------------------------------------------------------------------");

    rootLogger->ClearAllLogTraces();
    rootLogger->AddLogTrace("TestReload", 12345); // logTrace: TestReload-12345
    LLOG_TRACE("This is a uninited trace log message reload_1");
    LLOG_DEBUG("This is a uninited debug log message reload_2");
    LLOG_INFO("This is a uninited info log message reload_3");
    LLOG_WARN("This is a uninited warn log message reload_4");     // output (4~6)
    LLOG_ERROR("This is a uninited error log message reload_5");
    LLOG_FATAL("This is a uninited fatal log message reload_6");
    
    LLOG_FATAL("--------------------------------------------------------------------");
    // test reload.
    LLBC_PrintLn("Please try update logger config file, and reload...!");
    getchar();
    LLBC_LoggerMgrSingleton->Reload();

    LLOG_TRACE("This is a uninited trace log message reload_7"); // output (7~12)
    LLOG_DEBUG("This is a uninited debug log message reload_8");
    LLOG_INFO("This is a uninited info log message reload_9");
    LLOG_WARN("This is a uninited warn log message reload_10");     
    LLOG_ERROR("This is a uninited error log message reload_11");
    LLOG_FATAL("This is a uninited fatal log message reload_12");
}

void TestCase_Core_Log::DoLogTraceEnableLevelTest()
{
    LLBC_PrintLn("DoLogTraceEnableLevelTest begin");

    constexpr const char *testLoggerName = "logtrace_enable_level";

    auto logTraceEnablerLogger = LLBC_LoggerMgrSingleton->GetLogger(testLoggerName);
    LLBC_PrintLn("Log trace enable level test, test logger name:%s, default level:%s",
                 logTraceEnablerLogger->GetLoggerName().c_str(),
                 LLBC_LogLevel::GetLevelStr(logTraceEnablerLogger->GetLogLevel()).c_str());

    LLBC_PrintLn("fileLogLevel: %s", LLBC_LogLevel::GetLevelStr(logTraceEnablerLogger->GetLogLevel()).c_str());

    auto ret = logTraceEnablerLogger->AddLogTrace("LOG_TRACE_ENABLE_LEVEL_TEST", 12345678);
    LLBC_PrintLn("  - Add test log trace content, return:%d, err:%s",
                 ret, LLBC_FormatLastError());

    LLOG_TRACE2(testLoggerName, "test log trace enable level in trace.");
    LLOG_DEBUG2(testLoggerName, "test log trace enable level in debug.");
    LLOG_INFO2(testLoggerName, "test log trace enable level in info.");
    LLOG_WARN2(testLoggerName, "test log trace enable level in warn.");
    LLOG_ERROR2(testLoggerName, "test log trace enable level in error.");
    LLOG_FATAL2(testLoggerName, "test log trace enable level in FATAL.");

    LLOG_FATAL("--------------------------------------------------------------------");
}

template <typename _KeyTy, typename _ContentTy>
void TestCase_Core_Log::AddLogTrace(const _KeyTy &key, const _ContentTy &content)
{
    const auto keyStr = LLBC_Variant(key).ToString();
    const auto contentStr = LLBC_Variant(content).ToString();

    LLOG_INFO("Before add %s:%s", keyStr.c_str(), contentStr.c_str());
    LLOG_ADD_TRACE(key, content)
    LLOG_INFO("After add %s:%s", keyStr.c_str(), contentStr.c_str());
}

void TestCase_Core_Log::OnLogHook(const LLBC_LogData *logData)
{
    LLBC_PrintLn("Log hook, loggerName: %s, level: %s, message: %s",
                   logData->logger->GetLoggerName().c_str(),
                   LLBC_LogLevel::GetLevelStr(logData->level).c_str(),
                   logData->msg);
}

// ----------------------------------------------------------------------------
// Log output control items test (Mute / SetLevel) — config-driven only.
// ----------------------------------------------------------------------------
// The log control list is a startup/reload-only configuration surface; there
// is no public runtime API to mutate it after initialization. Coverage here
// is therefore limited to the two ingestion paths that actually exist:
//
//   1) Config-driven items (logcontrol_full)
//        - file+line exact / wildcard (line==0) / half-open range / multi-seg
//        - func name single / multi-value (OR within one item)
//        - threadId multi-value (parsing / structural assertions only)
//        - SetLevel: rewrite level to a higher value
//        - single-appender scope restriction
//        - declaration order is preserved; control item count matches config
//   2) LoggerMgr::Reload re-installs the config-driven control list.
// ----------------------------------------------------------------------------

bool TestCase_Core_Log::EmitAndCheckSuppressed(LLBC_Logger *logger,
                                               int level,
                                               const char *file,
                                               int line,
                                               const char *func,
                                               const char *msg,
                                               size_t expectedHits)
{
    // Emit one fake-source log record and check whether the control filter
    // dropped (Mute action) it on `expectedHits` appender(s). The suppressed
    // count is incremented exactly once per (record, appender) pair that hits
    // a Mute action.
    const uint64 before = logger->GetLogControlSuppressedCount();
    logger->Output(level, nullptr, file, line, func, "%s", msg);
    return logger->GetLogControlSuppressedCount() == before + expectedHits;
}

int TestCase_Core_Log::DoLogControlTest()
{
    LLBC_PrintLn("DoLogControlTest (log control items) begin");

    // The logger manager was initialized with `LogTestCfg.cfg`, which can only
    // carry flat properties; `logControls` is structurally nested and is
    // therefore XML-only. We Reload to `LogTestCfg.xml` here so the
    // logcontrol_full logger (already declared with its basic output
    // properties in `.cfg`) picks up its <logControls> subtree.
    // See doc/log_control.md, sections 3 and 6.
    if (LLBC_LoggerMgrSingleton->Reload("LogTestCfg.xml") != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr,
                         "Reload LogTestCfg.xml for log control test failed, err:%s",
                         LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    // ------------------------------------------------------------------
    // 1) Config-driven items: logcontrol_full
    //    Configured (declaration order):
    //      [0] file=mute_fileA.cpp line==100        -> mute
    //      [1] file=mute_fileA.cpp line==200        -> mute
    //      [2] file=mute_fileB.cpp (any line)       -> mute
    //      [3] func=MutedFuncOne                    -> mute (all appenders)
    //      [4] func=MutedFuncTwo, scope=[console]   -> mute
    //      [5] file=mute_fileC.cpp                  -> setlevel warn
    //      [6] file=mute_fileD.cpp                  -> setlevel error
    //      [7] file=mute_fileE.cpp [300,305)        -> mute  (single-range)
    //      [8] file=mute_fileF.cpp [400,410)        -> mute  (single-range)
    //      [9] file=mute_fileG.cpp 50, 60-65, 70    -> mute  (multi-segment)
    //     [10] file=mute_fileH.cpp 100-103, 200     -> mute  (multi-segment)
    //     [11] func=[MutedFuncMA, MutedFuncMB]      -> mute  (func array multi-value)
    //     [12] threadId=[1, 2147483640]             -> mute  (threadId array multi-value)
    //    Only `console` appender is enabled, so each muted record contributes
    //    +1 to GetLogControlSuppressedCount().
    // ------------------------------------------------------------------
    auto fullLogger = LLBC_LoggerMgrSingleton->GetLogger("logcontrol_full");
    LLBC_ErrorAndReturnIf(fullLogger == nullptr,
                          LLBC_FAILED, "get logcontrol_full failed");
    fullLogger->ResetLogControlSuppressedCount();

    {
        std::vector<LLBC_LogControlItem> items;
        fullLogger->GetLogControls(items);
        LLBC_PrintLn("[full] parsed control item count: %zu", items.size());
        LLBC_ErrorAndReturnIf(items.size() != 13,
                              LLBC_FAILED, "[full] expect 13 control items");
        // Spot-check declaration order.
        LLBC_ErrorAndReturnIf(!items[0].file.enabled || items[0].file.file != "mute_fileA.cpp" ||
                              items[0].file.lineRanges.size() != 1 ||
                              items[0].file.lineRanges[0].first != 100 ||
                              items[0].file.lineRanges[0].second != 101 ||
                              items[0].action != LLBC_LogControlAction::Mute,
                              LLBC_FAILED, "[full] item[0] mismatch");
        LLBC_ErrorAndReturnIf(!items[4].func.enabled || items[4].func.values.size() != 1 ||
                              items[4].func.values[0] != "MutedFuncTwo" ||
                              items[4].appenderTypes.size() != 1 ||
                              items[4].appenderTypes[0] != LLBC_LogAppenderType::Console,
                              LLBC_FAILED, "[full] item[4] mismatch");
        LLBC_ErrorAndReturnIf(items[5].action != LLBC_LogControlAction::SetLevel ||
                              items[5].newLevel != LLBC_LogLevel::Warn,
                              LLBC_FAILED, "[full] item[5] mismatch");
        // Range items: half-open [begin, end), single-segment.
        LLBC_ErrorAndReturnIf(!items[7].file.enabled || items[7].file.file != "mute_fileE.cpp" ||
                              items[7].file.lineRanges.size() != 1 ||
                              items[7].file.lineRanges[0].first != 300 ||
                              items[7].file.lineRanges[0].second != 305 ||
                              items[7].action != LLBC_LogControlAction::Mute,
                              LLBC_FAILED, "[full] item[7] (string range) mismatch");
        LLBC_ErrorAndReturnIf(!items[8].file.enabled || items[8].file.file != "mute_fileF.cpp" ||
                              items[8].file.lineRanges.size() != 1 ||
                              items[8].file.lineRanges[0].first != 400 ||
                              items[8].file.lineRanges[0].second != 410 ||
                              items[8].action != LLBC_LogControlAction::Mute,
                              LLBC_FAILED, "[full] item[8] (object range) mismatch");
        // Multi-segment items: G (string) "50, 60-65, 70" -> [50,51),[60,65),[70,71);
        //                      H (object lines) "100-103, 200" -> [100,103),[200,201).
        LLBC_ErrorAndReturnIf(!items[9].file.enabled || items[9].file.file != "mute_fileG.cpp" ||
                              items[9].file.lineRanges.size() != 3 ||
                              items[9].file.lineRanges[0] != std::make_pair(50, 51) ||
                              items[9].file.lineRanges[1] != std::make_pair(60, 65) ||
                              items[9].file.lineRanges[2] != std::make_pair(70, 71),
                              LLBC_FAILED, "[full] item[9] (string multi-seg) mismatch");
        LLBC_ErrorAndReturnIf(!items[10].file.enabled || items[10].file.file != "mute_fileH.cpp" ||
                              items[10].file.lineRanges.size() != 2 ||
                              items[10].file.lineRanges[0] != std::make_pair(100, 103) ||
                              items[10].file.lineRanges[1] != std::make_pair(200, 201),
                              LLBC_FAILED, "[full] item[10] (object multi-seg) mismatch");
        // Multi-value func / threadId: arrays in JSON.
        LLBC_ErrorAndReturnIf(!items[11].func.enabled ||
                              items[11].func.values.size() != 2 ||
                              items[11].func.values[0] != "MutedFuncMA" ||
                              items[11].func.values[1] != "MutedFuncMB",
                              LLBC_FAILED, "[full] item[11] (func array) mismatch");
        LLBC_ErrorAndReturnIf(!items[12].threadId.enabled ||
                              items[12].threadId.values.size() != 2 ||
                              items[12].threadId.values[0] != static_cast<LLBC_ThreadId>(1) ||
                              items[12].threadId.values[1] != static_cast<LLBC_ThreadId>(2147483640),
                              LLBC_FAILED, "[full] item[12] (threadId array) mismatch");
    }

    // 1.a) file+line exact: A:100 muted, A:101 pass, A:200 muted.
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileA.cpp", 100, "AnyFunc", "A100 muted", 1),
        LLBC_FAILED, "[full.line] A:100 not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileA.cpp", 101, "AnyFunc", "A101 pass", 0),
        LLBC_FAILED, "[full.line] A:101 wrongly muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileA.cpp", 200, "AnyFunc", "A200 muted", 1),
        LLBC_FAILED, "[full.line] A:200 not muted");

    // 1.b) file wildcard: B:any-line muted.
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileB.cpp", 1, "AnyFunc", "B:1 muted", 1),
        LLBC_FAILED, "[full.line] B:1 not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileB.cpp", 99999, "AnyFunc", "B:99999 muted", 1),
        LLBC_FAILED, "[full.line] B:99999 not muted");

    // 1.c) func: MutedFuncOne / MutedFuncTwo muted (the latter via console-only
    //      scope; since only console appender exists, both should land).
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "any.cpp", 1, "MutedFuncOne", "func one muted", 1),
        LLBC_FAILED, "[full.func] MutedFuncOne not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "any.cpp", 1, "MutedFuncTwo", "func two muted", 1),
        LLBC_FAILED, "[full.func] MutedFuncTwo not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "any.cpp", 1, "OtherFunc", "OtherFunc pass", 0),
        LLBC_FAILED, "[full.func] OtherFunc wrongly muted");

    // 1.d) SetLevel: file=mute_fileC.cpp -> warn rewrite. The record is NOT
    //      muted by the control filter (so suppressedCount unchanged), but
    //      its level becomes WARN before reaching the appender. We verify
    //      the "no mute" side here (positive observation of level rewrite is
    //      covered later in (3) by chaining with another mute item).
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileC.cpp", 1, "Foo", "C info -> warn", 0),
        LLBC_FAILED, "[full.setlevel] C info wrongly muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Error,
                                "mute_fileD.cpp", 1, "Foo", "D error -> error", 0),
        LLBC_FAILED, "[full.setlevel] D error wrongly muted");

    // 1.f) file+line RANGE: half-open [begin, end). E:[300,305), F:[400,410).
    //      Endpoints: begin included, end excluded (consistent with llbc's
    //      other range APIs).
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileE.cpp", 299, "AnyFunc", "E:299 pass (below begin)", 0),
        LLBC_FAILED, "[full.range] E:299 wrongly muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileE.cpp", 300, "AnyFunc", "E:300 muted (begin included)", 1),
        LLBC_FAILED, "[full.range] E:300 not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileE.cpp", 302, "AnyFunc", "E:302 muted (mid)", 1),
        LLBC_FAILED, "[full.range] E:302 not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileE.cpp", 304, "AnyFunc", "E:304 muted (end-1, included)", 1),
        LLBC_FAILED, "[full.range] E:304 not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileE.cpp", 305, "AnyFunc", "E:305 pass (end excluded)", 0),
        LLBC_FAILED, "[full.range] E:305 wrongly muted (end should be excluded)");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileF.cpp", 405, "AnyFunc", "F:405 muted (object range mid)", 1),
        LLBC_FAILED, "[full.range] F:405 not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileF.cpp", 410, "AnyFunc", "F:410 pass (object range end excluded)", 0),
        LLBC_FAILED, "[full.range] F:410 wrongly muted (end should be excluded)");

    // 1.g) file+line MULTI-SEGMENT.
    //   G "50, 60-65, 70" -> hits 50 / 60..64 / 70; misses 49,51..59,65,69,71
    //   H "100-103, 200"  -> hits 100..102 / 200;  misses 99,103,199,201
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileG.cpp", 49, "AnyFunc", "G:49 pass", 0),
        LLBC_FAILED, "[full.multiseg] G:49 wrongly muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileG.cpp", 50, "AnyFunc", "G:50 muted (single)", 1),
        LLBC_FAILED, "[full.multiseg] G:50 not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileG.cpp", 51, "AnyFunc", "G:51 pass (gap)", 0),
        LLBC_FAILED, "[full.multiseg] G:51 wrongly muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileG.cpp", 60, "AnyFunc", "G:60 muted (range begin)", 1),
        LLBC_FAILED, "[full.multiseg] G:60 not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileG.cpp", 64, "AnyFunc", "G:64 muted (range end-1)", 1),
        LLBC_FAILED, "[full.multiseg] G:64 not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileG.cpp", 65, "AnyFunc", "G:65 pass (range end excluded)", 0),
        LLBC_FAILED, "[full.multiseg] G:65 wrongly muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileG.cpp", 70, "AnyFunc", "G:70 muted (last single)", 1),
        LLBC_FAILED, "[full.multiseg] G:70 not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileG.cpp", 71, "AnyFunc", "G:71 pass (after last)", 0),
        LLBC_FAILED, "[full.multiseg] G:71 wrongly muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileH.cpp", 102, "AnyFunc", "H:102 muted (object range mid)", 1),
        LLBC_FAILED, "[full.multiseg] H:102 not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileH.cpp", 103, "AnyFunc", "H:103 pass (range end excluded)", 0),
        LLBC_FAILED, "[full.multiseg] H:103 wrongly muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileH.cpp", 200, "AnyFunc", "H:200 muted (single in lines)", 1),
        LLBC_FAILED, "[full.multiseg] H:200 not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "mute_fileH.cpp", 201, "AnyFunc", "H:201 pass", 0),
        LLBC_FAILED, "[full.multiseg] H:201 wrongly muted");

    // 1.h) func MULTI-VALUE (config-driven, JSON array): MutedFuncMA / MutedFuncMB
    //      both muted, MutedFuncMC pass. Threadid multi-value (item[12]) is not
    //      exercised here since neither tid in the array equals the current
    //      thread's id (synthetic values), so it has no effect on these emits.
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "any.cpp", 1, "MutedFuncMA", "func MA muted", 1),
        LLBC_FAILED, "[full.multifunc] MutedFuncMA not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "any.cpp", 1, "MutedFuncMB", "func MB muted", 1),
        LLBC_FAILED, "[full.multifunc] MutedFuncMB not muted");
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "any.cpp", 1, "MutedFuncMC", "func MC pass", 0),
        LLBC_FAILED, "[full.multifunc] MutedFuncMC wrongly muted");

    // 1.e) un-registered file should always pass.
    LLBC_ErrorAndReturnIf(
        !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                "no_such_file.cpp", 100, "AnyFunc", "unrelated pass", 0),
        LLBC_FAILED, "[full.misc] unrelated file wrongly muted");

    LLBC_PrintLn("[full] suppressed count after pass: %llu",
                 (unsigned long long)fullLogger->GetLogControlSuppressedCount());

    // ------------------------------------------------------------------
    // 2) LoggerMgr::Reload propagates logControls into the live logger.
    // We reload the XML config (note: .cfg cannot carry logControls in the
    // new design, so we must reload from the same XML used at test entry)
    // and verify that:
    //   - the logger's logControls list is repopulated to a non-zero size,
    //   - and the canonical config-driven rule (file=mute_fileA.cpp:100)
    //     becomes effective again.
    // ------------------------------------------------------------------
    {
        LLBC_ErrorAndReturnIf(
            LLBC_LoggerMgrSingleton->Reload("LogTestCfg.xml") != LLBC_OK,
            LLBC_FAILED,
            "[reload] LoggerMgr::Reload failed: %s",
            LLBC_FormatLastError());

        // Re-acquire the logger pointer after reload just to be safe (the
        // logger identity is preserved by design, but we don't want the test
        // to depend on that beyond the interface).
        fullLogger = LLBC_LoggerMgrSingleton->GetLogger("logcontrol_full");
        LLBC_ErrorAndReturnIf(fullLogger == nullptr,
                              LLBC_FAILED,
                              "[reload] get logcontrol_full failed after reload");

        LLBC_ErrorAndReturnIf(
            fullLogger->GetLogControlCount() == 0,
            LLBC_FAILED,
            "[reload] logControls not repopulated after reload");

        fullLogger->ResetLogControlSuppressedCount();

        // Config-driven canonical rule re-armed.
        LLBC_ErrorAndReturnIf(
            !EmitAndCheckSuppressed(fullLogger, LLBC_LogLevel::Info,
                                    "mute_fileA.cpp", 100, "AnyFunc",
                                    "post-reload A:100 muted", 1),
            LLBC_FAILED,
            "[reload] canonical rule not effective after reload");
    }

    LLBC_PrintLn("DoLogControlTest done. final suppressed count of full logger: %llu",
                 (unsigned long long)fullLogger->GetLogControlSuppressedCount());

    return LLBC_OK;
}
