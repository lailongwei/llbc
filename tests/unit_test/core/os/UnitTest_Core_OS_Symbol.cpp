// The MIT License (MIT)
//
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

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/os/OS_Symbol.cpp

// Symbol capture is used by diagnostics and crash reports. Restrict the test to
// the non-Windows implementation, where initialization is idempotent and stack
// capture only observes the current thread without installing signal handlers.
TEST(SymbolOsTest, InitializesCapturesAndCleansUpCurrentThreadSymbols)
{
#if LLBC_CFG_OS_IMPL_SYMBOL && LLBC_TARGET_PLATFORM_NON_WIN32
    EXPECT_EQ(LLBC_InitSymbol(), LLBC_OK);

    const LLBC_String trace = LLBC_CaptureStackBackTrace(0, 8);
    EXPECT_FALSE(trace.empty());
    EXPECT_NE(trace.find("#"), static_cast<LLBC_String::size_type>(-1));

    EXPECT_FALSE(LLBC_CaptureStackBackTrace().empty());
    EXPECT_FALSE(
        LLBC_CaptureStackBackTrace(1, LLBC_CFG_OS_SYMBOL_MAX_CAPTURE_FRAMES * 2).empty());
    EXPECT_TRUE(
        LLBC_CaptureStackBackTrace(LLBC_CFG_OS_SYMBOL_MAX_CAPTURE_FRAMES, 8).empty());
    EXPECT_EQ(LLBC_CleanupSymbol(), LLBC_OK);
#else
    GTEST_SKIP() << "symbol implementation is platform-specific";
#endif
}
