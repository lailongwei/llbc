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

#include <llbc.h>
using namespace llbc;

#include <gtest/gtest.h>

#if LLBC_SUPPORT_HANDLE_CRASH

/**
 * \brief The crash test fixture.
 * 
 */
class CrashTest : public testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        EXPECT_EQ(LLBC_EnableCrashHandle(), LLBC_OK);

        // Note: Crash handler will not be called, because signal handler not inherited by child process.
        EXPECT_EQ(LLBC_SetCrashHandler("CrashHandler-llbc framework unit_test",
                                       &CrashTest::TestCrashHandler), LLBC_OK);
    };

    static void TearDownTestSuite()
    {
        LLBC_DisableCrashHandle();
    }

private:
    static void TestCrashHandler(const char *traceback, int sig)
    {
        ++_crashHandlerCallCount;
        SUCCEED() << "Crash handler called. callCount:" << _crashHandlerCallCount
                  << ", traceback:" << traceback
                  << ", sig:" << sig << std::endl;

    auto libTls = __LLBC_GetLibTls();
    if (!libTls)
    {
        FAIL() << "libTls is null";
        return;
    }

    #if LLBC_TARGET_PLATFORM_NON_WIN32 && LLBC_CFG_OS_USE_ALT_STACK_FOR_CRASH_SIGNAL
    const char *libTlsVarAddr = reinterpret_cast<char *>(&libTls);
    const size_t altStackSize = libTls->coreTls.crashSignalAltStackSize;
    const char *altStack = reinterpret_cast<const char *>(libTls->coreTls.crashSignalAltStack);
    SUCCEED() << "Crash signal alternative stack info:";
    SUCCEED() << "- crash signal alternative stack:" << altStack << "(size:" << altStackSize << ")";
    SUCCEED() << "- in alternative stack:" << (libTlsVarAddr >= altStack && libTlsVarAddr < altStack + altStackSize ? "true" : "false");
    SUCCEED() << "- stack used:" << ((altStack + altStackSize) - libTlsVarAddr);
    #else // Not support alternative stack.
    SUCCEED() << "Crash signal alternative stack info:";
    SUCCEED() << "- not support alternative stack";
    #endif // LLBC_TARGET_PLATFORM_NON_WIN32 && LLBC_CFG_OS_USE_ALT_STACK_FOR_CRASH_SIGNAL
    }

private:
    static inline int _crashHandlerCallCount = 0;
};

// Division by zero.
TEST(CrashTest, DivisionByZero)
{
    EXPECT_DEATH({
        auto result = 3 / LLBC_Str2Num<int>("0");
        std::cout << "3 / 0 = " << result << std::endl;
    }, ".*");
}

// Invalid pointer read.
TEST(CrashTest, InvalidPtrRead)
{
    EXPECT_DEATH({
        int *invalidPtr4Read = nullptr;
        std::cout << *invalidPtr4Read << std::endl;
    }, ".*");
}

// Invalid pointer write.
TEST(CrashTest, InvalidPtrWrite)
{
    EXPECT_DEATH({
        int *invalidPtr4Write = nullptr;
        *invalidPtr4Write = 3;
        std::cout << *invalidPtr4Write << std::endl;
    }, ".*");
}

#endif // LLBC_SUPPORT_HANDLE_CRASH
