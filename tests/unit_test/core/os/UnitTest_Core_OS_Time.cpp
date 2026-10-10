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

#include <chrono>
#include <thread>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/os/OS_Time.cpp

// The OS time adapter initializes local timezone state, exposes wall-clock
// timeval data, and provides a monotonic tick counter for scheduling.
TEST(OsTimeTest, InitializesTimezoneAndReportsWallAndMonotonicTime)
{
    LLBC_TZSet();
    const int timezone = LLBC_GetTimezone();
    EXPECT_GE(timezone, -24 * 60 * 60);
    EXPECT_LE(timezone, 24 * 60 * 60);

    timeval wallTime {};
    ASSERT_EQ(LLBC_GetTimeOfDay(&wallTime, nullptr), LLBC_OK);
    EXPECT_GT(wallTime.tv_sec, 0);
    EXPECT_GE(wallTime.tv_usec, 0);
    EXPECT_LT(wallTime.tv_usec, 1000000);

    const uint64 before = LLBC_GetTickCount();
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    EXPECT_GE(LLBC_GetTickCount(), before);

    // This is a no-op on platforms without RDTSC, but initializes capability
    // flags where a processor counter is supported.
    LLBC_InitTSCSupportFlags();
}
