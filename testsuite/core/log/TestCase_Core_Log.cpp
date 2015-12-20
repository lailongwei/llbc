/**
 * @file    TestCase_Core_Log.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/12
 * @version 1.0
 *
 * @brief
 */

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
    if(LLBC_LoggerManagerSingleton->Initialize(mainBundle->GetBundlePath() + "/" + "Logger_Cfg.cfg") != LLBC_RTN_OK)
#else

    if(LLBC_LoggerManagerSingleton->Initialize("core/log/Logger_Cfg.cfg") != LLBC_RTN_OK)
#endif
    {
        LLBC_PrintLine("Initialize logger manager failed, err: %s", LLBC_FormatLastError());
        return -1;
    }

    // Use root logger to test.
    LLBC_DEBUG_LOG("This is a debug log message.");
    LLBC_DEBUG_LOG2("test_tag", "This is a debug log message.");

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

    LLBC_WARN_LOG("This is a warn log message.");
    LLBC_WARN_LOG2("test_tag", "This is a warn log message.");

    LLBC_ERROR_LOG("This is a error log message.");
    LLBC_ERROR_LOG2("test_tag", "This is a error log message.");

    LLBC_FATAL_LOG("This is a fatal log message.");
    LLBC_FATAL_LOG2("test_tag", "This is a fatal log message.");

    // Use test logger to test.
    LLBC_DEBUG_LOG_SPEC("test", "This is a debug log message.");
    LLBC_DEBUG_LOG_SPEC2("test", "test_tag", "This is a debug log message.");

    LLBC_INFO_LOG_SPEC("test", "This is a info log message.");
    LLBC_INFO_LOG_SPEC2("test", "test_tag", "This is a info log message.");

    LLBC_WARN_LOG_SPEC("test", "This is a warn log message.");
    LLBC_WARN_LOG_SPEC2("test", "test_tag", "This is a warn log message.");

    LLBC_ERROR_LOG_SPEC("test", "This is a error log message.");
    LLBC_ERROR_LOG_SPEC2("test", "test_tag", "This is a error log message.");

    LLBC_FATAL_LOG_SPEC("test", "This is a fatal log message.");
    LLBC_FATAL_LOG_SPEC2("test", "test_tag", "This is a fatal log message.");

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
