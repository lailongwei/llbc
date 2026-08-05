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

#include <chrono>
#include <thread>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/utils/Util_Debug.cpp
// @coverage-target: llbc/include/llbc/core/utils/Util_DebugInl.h

// Hex dumps are used by diagnostics and protocol troubleshooting. Exercise raw
// output, separators, line wrapping, and the empty-input fast path.
TEST(DebugUtilTest, ByteToHexFormatsBytesAndLineBreaks)
{
    const uint8 bytes[] = {0x00, 0x12, 0xab, 0xff};

    EXPECT_EQ(LLBC_Byte2Hex(bytes, sizeof(bytes)), "0012abff");
    EXPECT_EQ(LLBC_Byte2Hex(bytes, sizeof(bytes), ':'), "00:12:ab:ff:");
    EXPECT_EQ(LLBC_Byte2Hex(bytes, sizeof(bytes), '\0', 2), "0012\nabff");
    EXPECT_EQ(LLBC_Byte2Hex(bytes, sizeof(bytes), ':', 2), "00:12:\nab:ff:");
    EXPECT_TRUE(LLBC_Byte2Hex(nullptr, 0).empty());
}

// Stopwatch supports both stopped elapsed-tick instances and live measurement.
// Pausing must freeze the accumulated interval until it is explicitly resumed.
TEST(DebugUtilTest, StopwatchMeasuresAndControlsElapsedTime)
{
    LLBC_Stopwatch::InitFrequency();
    EXPECT_GT(LLBC_Stopwatch::GetFrequency(), 0u);

    LLBC_Stopwatch stopped(123u, false);
    EXPECT_FALSE(stopped.IsRunning());
    EXPECT_EQ(stopped.ElapsedTicks(), 123u);
    EXPECT_GE(stopped.Elapsed().GetTotalMicros(), 0);
    stopped.Reset();
    EXPECT_EQ(stopped.ElapsedTicks(), 0u);
    EXPECT_EQ(stopped.ElapsedNanos(), 0u);

    stopped.Resume();
    EXPECT_TRUE(stopped.IsRunning());
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    stopped.Pause();
    EXPECT_FALSE(stopped.IsRunning());
    const uint64 pausedTicks = stopped.ElapsedTicks();
    EXPECT_GT(pausedTicks, 0u);
    const uint64 pausedNanos = stopped.ElapsedNanos();
    EXPECT_GT(pausedNanos, 0u);

    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    EXPECT_EQ(stopped.ElapsedTicks(), pausedTicks);
    EXPECT_EQ(stopped.ElapsedNanos(), pausedNanos);
    EXPECT_NE(stopped.ToString().find(" ms"), static_cast<LLBC_String::size_type>(-1));

    stopped.Restart();
    EXPECT_TRUE(stopped.IsRunning());
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    EXPECT_GT(stopped.Elapsed().GetTotalMicros(), 0);
    EXPECT_GT(stopped.ElapsedNanos(), 0u);
    EXPECT_GT(stopped.ElapsedTicks(), 0u);
    stopped.Pause();
}
