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
    LLBC_PrintLine("Current dir:%s", LLBC_Directory::CurDir().c_str());

    // Do uninited log message test
    DoUninitLogTest();

    // Initialize logger manager.
#if LLBC_TARGET_PLATFORM_IPHONE
    const LLBC_Bundle *mainBundle = LLBC_Bundle::GetMainBundle();
    if(LLBC_LoggerManagerSingleton->Initialize(mainBundle->GetBundlePath() + "/" + "Logger_Cfg.cfg") != LLBC_OK)
#else

    LLBC_String a = LLBC_Directory::AbsPath("Logger_Cfg.cfg");
    if(LLBC_LoggerManagerSingleton->Initialize("Logger_Cfg.cfg") != LLBC_OK)
#endif
    {
        LLBC_FilePrintLine(stderr, "Initialize logger manager failed, err: %s", LLBC_FormatLastError());
        LLBC_FilePrintLine(stderr, "Forgot copy Logger_Cfg.cfg test config file to CWD?");
        return -1;
    }

    // Install logger hook(to root logger).
    LLBC_Logger *rootLogger = LLBC_LoggerManagerSingleton->GetRootLogger();
    rootLogger->InstallHook(LLBC_LogLevel::Debug, LLBC_Delegate<void(const LLBC_LogData *)>(this, &TestCase_Core_Log::OnLogHook));

    // Use root logger to test.
    LLBC_DEBUG_LOG("This is a debug log message.");
    LLBC_DEBUG_LOG2("test_tag", "This is a debug log message.");

    LLog.d("This is a debug log message(from LLog.d())");
    LLog.d2("test_tag", "This is a debug log message(from LLog.d2())");

    // Uninstall logger hook(from root logger).
    rootLogger->UninstallHook(LLBC_LogLevel::Debug);

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
    LLog.i("This is a info log message(from LLog.i()).");
    LLog.i2("test_tag", "This is a info log message(from LLog.i2()).");
    LLog.i2<TestCase_Core_Log>("This is a info log message(from LLog.i2<>()).");

    LLBC_WARN_LOG("This is a warn log message.");
    LLBC_WARN_LOG2("test_tag", "This is a warn log message.");
    LLog.w("This is a warn log message(from LLog.w()).");
    LLog.w2("test_tag", "This is a warn log message(from LLog.w2()).");
    LLog.w2<TestCase_Core_Log>("This is a warn log message(from LLog.w2<>()).");

    LLBC_ERROR_LOG("This is a error log message.");
    LLBC_ERROR_LOG2("test_tag", "This is a error log message.");
    LLog.e("This is a error log message(from LLog.e()).");
    LLog.e2("test_tag", "This is a error log message(from LLog.e2()).");
    LLog.e2<TestCase_Core_Log>("This is a error log message(from LLog.e2<>()).");

    LLBC_FATAL_LOG("This is a fatal log message.");
    LLBC_FATAL_LOG2("test_tag", "This is a fatal log message.");
    LLog.f("This is a fatal log message(from LLog.f()).");
    LLog.f2("test_tag", "This is a fatal log message(from LLog.f2()).");
    LLog.f2<TestCase_Core_Log>("This is a fatal log message(from LLog.f2<>()).");

    // Use test logger to test.
    LLBC_DEBUG_LOG_SPEC("test", "This is a debug log message.");
    LLBC_DEBUG_LOG_SPEC2("test", "test_tag", "This is a debug log message.");
    LLog.d3("test", "This is a debug log message(from LLog.d3()).");
    LLog.d4("test", "test_tag", "This is a debug log message(from LLog.d4()).");
    LLog.d4<TestCase_Core_Log>("test", "This is a debug log message(from LLog.d4<>()).");

    LLBC_INFO_LOG_SPEC("test", "This is a info log message.");
    LLBC_INFO_LOG_SPEC2("test", "test_tag", "This is a info log message.");
    LLog.i3("test", "This is a info log message(from LLog.i3()).");
    LLog.i4("test", "test_tag", "This is a info log message(from LLog.i4()).");
    LLog.i4<TestCase_Core_Log>("test", "This is a info log message(from LLog.i4<>()).");

    LLBC_WARN_LOG_SPEC("test", "This is a warn log message.");
    LLBC_WARN_LOG_SPEC2("test", "test_tag", "This is a warn log message.");
    LLog.w3("test", "This is a warn log message(from LLog.w3()).");
    LLog.w4("test", "test_tag", "This is a warn log message(from LLog.w4()).");
    LLog.w4<TestCase_Core_Log>("test", "This is a warn log message(from LLog.w4<>()).");

    LLBC_ERROR_LOG_SPEC("test", "This is a error log message.");
    LLBC_ERROR_LOG_SPEC2("test", "test_tag", "This is a error log message.");
    LLog.e3("test", "This is a error log message(from LLog.e3()).");
    LLog.e4("test", "test_tag", "This is a error log message(from LLog.e4()).");
    LLog.e4<TestCase_Core_Log>("test", "This is a error log message(from LLog.e4<>()).");

    LLBC_FATAL_LOG_SPEC("test", "This is a fatal log message.");
    LLBC_FATAL_LOG_SPEC2("test", "test_tag", "This is a fatal log message.");
    LLog.f3("test", "This is a fatal log message(from LLog.f3()).");
    LLog.f4("test", "test_tag", "This is a fatal log message(from LLog.f4()).");
    LLog.f4<TestCase_Core_Log>("test", "This is a fatal log message(from LLog.f4<>()).");

    // Log file delete test.
    for (int i = 0; i < 20; ++i)
    {
        LLBC_DEBUG_LOG_SPEC("deltest", "This is a deltest logger message.");
        LLBC_ThreadManager::Sleep(1000);
    }

    // Peform performance test.
    LLBC_PrintLine("Perform preformance test:");
    LLBC_CPUTime begin = LLBC_CPUTime::Current();
    const int loopLmt = 500000;
    for (int i = 0; i < loopLmt; ++i)
        LLBC_DEBUG_LOG_SPEC("perftest", "performance test msg");

    LLBC_CPUTime elapsed = LLBC_CPUTime::Current() - begin;
    LLBC_PrintLine("Performance test completed, "
        "log size:%d, elapsed time: %s", loopLmt, elapsed.ToString().c_str());

    LLBC_PrintLine("Press any key to begin json log test");
    getchar();

    // test json styled log
    DoJsonLogTest();

    int jsonLogTestTimes = 100;
    for (int i = 0; i < jsonLogTestTimes; ++i)
        DoJsonLogTest();

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    // Finalize logger manager.
    LLBC_LoggerManagerSingleton->Finalize();

    return 0;
}

void TestCase_Core_Log::DoJsonLogTest()
{
    LLBC_Logger *rootLogger = LLBC_LoggerManagerSingleton->GetRootLogger();
    LLog.jd().Add("testKey", "testValue->jd.1").Finish("");
    LLog.jd().Add("testKey", "testValue->jd.2").Finish("%s", "Finish Test");
    LLog.jd().Add("testKey", "testValue->jd.3").Finish("%s%d", "Finish Test", 2);

    LLog.jd2("testTag").Add("testKey", "testValue->jd2.1").Finish("");
    LLog.jd2("testTag").Add("testKey", "testValue->jd2.2").Finish("%s", "Finish Test");
    LLog.jd2("testTag").Add("testKey", "testValue->jd2.3").Finish("%s%d", "Finish Test", 2);

    LLog.jd2<TestCase_Core_Log>().Add("testKey", LLBC_Variant()).Finish("");
    LLog.jd2<TestCase_Core_Log>().Add("testKey", "testValue->jd2<>.1").Finish("");
    LLog.jd2<TestCase_Core_Log>().Add("testKey", "testValue->jd2<>.2").Finish("%s", "Finish Test");
    LLog.jd2<TestCase_Core_Log>().Add("testKey", "testValue->jd2<>.3").Finish("%s%d", "Finish Test", 2);

    LLog.jd3(nullptr).Add("testKey", "testValue->Log.jd3(1).").Finish("");
    LLog.jd3("").Add("testKey", "testValue->jd3(2).").Finish("");
    LLog.jd3("test").Add("testKey", "testValue->jd3(3).").Finish("");
    LLog.jd3("test").Add("testKey", "testValue->jd3(4).").Finish("%s", "Finish Test");
    LLog.jd3("test").Add("testKey", "testValue->jd3(5).").Finish("%s%d", "Finish Test", 2);

    LLog.jd4<TestCase_Core_Log>(nullptr).Add("testKey", "testValue->jd4<>.1").Finish("");
    LLog.jd4<TestCase_Core_Log>("").Add("testKey", "testValue->jd4<>.2").Finish("");
    LLog.jd4<TestCase_Core_Log>("test").Add("testKey", "testValue->jd4<>.3").Finish("%s", "Finish Test");
    LLog.jd4<TestCase_Core_Log>("test").Add("testKey", "testValue->jd4<>.4").Finish("%s%d", "Finish Test", 2);

    LLog.ji().Add("testKey", "testValue->ji.1").Finish("");
    LLog.ji().Add("testKey", "testValue->ji.2").Finish("%s", "Finish Test");
    LLog.ji().Add("testKey", "testValue->ji.3").Finish("%s%d", "Finish Test", 2);

    LLog.ji2("testTag").Add("testKey", "testValue->ji2.1").Finish("");
    LLog.ji2("testTag").Add("testKey", "testValue->ji2.2").Finish("%s", "Finish Test");
    LLog.ji2("testTag").Add("testKey", "testValue->ji2.3").Finish("%s%d", "Finish Test", 2);

    LLog.ji2<TestCase_Core_Log>().Add("testKey", "testValue->ji2<>.1").Finish("");
    LLog.ji2<TestCase_Core_Log>().Add("testKey", "testValue->ji2<>.2").Finish("%s", "Finish Test");
    LLog.ji2<TestCase_Core_Log>().Add("testKey", "testValue->ji2<>.3").Finish("%s%d", "Finish Test", 2);

    LLog.ji3(nullptr).Add("testKey", "testValue->Log.ji3().1").Finish("");
    LLog.ji3("").Add("testKey", "testValue->ji3().2").Finish("");
    LLog.ji3("test").Add("testKey", "testValue->ji3().3").Finish("");
    LLog.ji3("test").Add("testKey", "testValue->ji3().4").Finish("%s", "Finish Test");
    LLog.ji3("test").Add("testKey", "testValue->ji3().5").Finish("%s%d", "Finish Test", 2);

    LLog.ji4<TestCase_Core_Log>(nullptr).Add("testKey", "testValue->ji4<>1").Finish("");
    LLog.ji4<TestCase_Core_Log>("").Add("testKey", "testValue->ji4<>2").Finish("");
    LLog.ji4<TestCase_Core_Log>("test").Add("testKey", "testValue->ji4<>3").Finish("%s", "Finish Test");
    LLog.ji4<TestCase_Core_Log>("test").Add("testKey", "testValue->ji4<>4").Finish("%s%d", "Finish Test", 2);

    LLog.jw().Add("testKey", "testValue->jw.1").Finish("");
    LLog.jw().Add("testKey", "testValue->jw.2").Finish("%s", "Finish Test");
    LLog.jw().Add("testKey", "testValue->jw.3").Finish("%s%d", "Finish Test", 2);

    LLog.jw2("testTag").Add("testKey", "testValue->jw2.1").Finish("");
    LLog.jw2("testTag").Add("testKey", "testValue->jw2.2").Finish("%s", "Finish Test");
    LLog.jw2("testTag").Add("testKey", "testValue->jw2.3").Finish("%s%d", "Finish Test", 2);

    LLog.jw2<TestCase_Core_Log>().Add("testKey", "testValue->jw2<>.1").Finish("");
    LLog.jw2<TestCase_Core_Log>().Add("testKey", "testValue->jw2<>.2").Finish("%s", "Finish Test");
    LLog.jw2<TestCase_Core_Log>().Add("testKey", "testValue->jw2<>.3").Finish("%s%d", "Finish Test", 2);

    LLog.jw3(nullptr).Add("testKey", "testValue->Log.jw3().1").Finish("");
    LLog.jw3("").Add("testKey", "testValue->jw3().2").Finish("");
    LLog.jw3("test").Add("testKey", "testValue->jw3().3").Finish("");
    LLog.jw3("test").Add("testKey", "testValue->jw3().4").Finish("%s", "Finish Test");
    LLog.jw3("test").Add("testKey", "testValue->jw3().5").Finish("%s%d", "Finish Test", 2);

    LLog.jw4<TestCase_Core_Log>(nullptr).Add("testKey", "testValue->jw4<>1").Finish("");
    LLog.jw4<TestCase_Core_Log>("").Add("testKey", "testValue->jw4<>2").Finish("");
    LLog.jw4<TestCase_Core_Log>("test").Add("testKey", "testValue->jw4<>3").Finish("%s", "Finish Test");
    LLog.jw4<TestCase_Core_Log>("test").Add("testKey", "testValue->jw4<>4").Finish("%s%d", "Finish Test", 2);

    LLog.je().Add("testKey", "testValue->je.1").Finish("");
    LLog.je().Add("testKey", "testValue->je.2").Finish("%s", "Finish Test");
    LLog.je().Add("testKey", "testValue->je.3").Finish("%s%d", "Finish Test", 2);

    LLog.je2("testTag").Add("testKey", "testValue->je2.1").Finish("");
    LLog.je2("testTag").Add("testKey", "testValue->je2.2").Finish("%s", "Finish Test");
    LLog.je2("testTag").Add("testKey", "testValue->je2.3").Finish("%s%d", "Finish Test", 2);

    LLog.je2<TestCase_Core_Log>().Add("testKey", "testValue->je2<>.1").Finish("");
    LLog.je2<TestCase_Core_Log>().Add("testKey", "testValue->je2<>.2").Finish("%s", "Finish Test");
    LLog.je2<TestCase_Core_Log>().Add("testKey", "testValue->je2<>.3").Finish("%s%d", "Finish Test", 2);

    LLog.je3(nullptr).Add("testKey", "testValue->Log.je3().1").Finish("");
    LLog.je3("").Add("testKey", "testValue->je3().2").Finish("");
    LLog.je3("test").Add("testKey", "testValue->je3().3").Finish("");
    LLog.je3("test").Add("testKey", "testValue->je3().4").Finish("%s", "Finish Test");
    LLog.je3("test").Add("testKey", "testValue->je3().5").Finish("%s%d", "Finish Test", 2);

    LLog.je4<TestCase_Core_Log>(nullptr).Add("testKey", "testValue->je4<>1").Finish("");
    LLog.je4<TestCase_Core_Log>("").Add("testKey", "testValue->je4<>2").Finish("");
    LLog.je4<TestCase_Core_Log>("test").Add("testKey", "testValue->je4<>3").Finish("%s", "Finish Test");
    LLog.je4<TestCase_Core_Log>("test").Add("testKey", "testValue->je4<>4").Finish("%s%d", "Finish Test", 2);

    LLog.jf().Add("testKey", "testValue->jf.1").Finish("");
    LLog.jf().Add("testKey", "testValue->jf.2").Finish("%s", "Finish Test");
    LLog.jf().Add("testKey", "testValue->jf.3").Finish("%s%d", "Finish Test", 2);

    LLog.jf2("testTag").Add("testKey", "testValue->jf2.1").Finish("");
    LLog.jf2("testTag").Add("testKey", "testValue->jf2.2").Finish("%s", "Finish Test");
    LLog.jf2("testTag").Add("testKey", "testValue->jf2.3").Finish("%s%d", "Finish Test", 2);

    LLog.jf2<TestCase_Core_Log>().Add("testKey", "testValue->jf2<>.1").Finish("");
    LLog.jf2<TestCase_Core_Log>().Add("testKey", "testValue->jf2<>.2").Finish("%s", "Finish Test");
    LLog.jf2<TestCase_Core_Log>().Add("testKey", "testValue->jf2<>.3").Finish("%s%d", "Finish Test", 2);

    LLog.jf3(nullptr).Add("testKey", "testValue->Log.jf3().1").Finish("");
    LLog.jf3("").Add("testKey", "testValue->jf3().2").Finish("");
    LLog.jf3("test").Add("testKey", "testValue->jf3().3").Finish("");
    LLog.jf3("test").Add("testKey", "testValue->jf3().4").Finish("%s", "Finish Test");
    LLog.jf3("test").Add("testKey", "testValue->jf3().5").Finish("%s%d", "Finish Test", 2);

    LLog.jf4<TestCase_Core_Log>(nullptr).Add("testKey", "testValue->jf4<>1").Finish("");
    LLog.jf4<TestCase_Core_Log>("").Add("testKey", "testValue->jf4<>2").Finish("");
    LLog.jf4<TestCase_Core_Log>("test").Add("testKey", "testValue->jf4<>3").Finish("%s", "Finish Test");
    LLog.jf4<TestCase_Core_Log>("test").Add("testKey", "testValue->jf4<>4").Finish("%s%d", "Finish Test", 2);
}

void TestCase_Core_Log::DoUninitLogTest()
{
    LLog.d("This is a uninited log message");
    LLog.jd().Add("Key1", "Key1 value").Finish("This is a uninited json log message");
}

void TestCase_Core_Log::OnLogHook(const LLBC_LogData *logData)
{
    LLBC_PrintLine("Log hook, loggerName: %s, level: %s",
                   logData->loggerName, LLBC_LogLevel::GetLevelDesc(logData->level).c_str());
}

