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

#include <limits>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/utils/Util_Algorithm.cpp
// @coverage-target: llbc/include/llbc/core/utils/Util_AlgorithmInl.h

// Flow descriptions are used by diagnostics and configuration. Invalid values,
// including negative integers, must map to the safe "unknown" sentinel.
TEST(AlgorithmUtilTest, FlowTypeStringConversions)
{
    EXPECT_STREQ(LLBC_FlowType::Type2Str(LLBC_FlowType::NoFlow), "no flow");
    EXPECT_STREQ(LLBC_FlowType::Type2Str(LLBC_FlowType::UnderFlow), "underflow");
    EXPECT_STREQ(LLBC_FlowType::Type2Str(LLBC_FlowType::OverFlow), "overflow");
    EXPECT_STREQ(LLBC_FlowType::Type2Str(LLBC_FlowType::Unknown), "unknown");
    EXPECT_STREQ(LLBC_FlowType::Type2Str(-1), "unknown");
    EXPECT_STREQ(LLBC_FlowType::Type2Str(99), "unknown");

    EXPECT_EQ(LLBC_FlowType::Str2Type("no flow"), LLBC_FlowType::NoFlow);
    EXPECT_EQ(LLBC_FlowType::Str2Type("underflow"), LLBC_FlowType::UnderFlow);
    EXPECT_EQ(LLBC_FlowType::Str2Type("overflow"), LLBC_FlowType::OverFlow);
    EXPECT_EQ(LLBC_FlowType::Str2Type("unknown"), LLBC_FlowType::Unknown);
    EXPECT_EQ(LLBC_FlowType::Str2Type(nullptr), LLBC_FlowType::Unknown);
}

// Overflow helpers must classify operations without evaluating an overflowing
// signed expression. Cover signed positive/negative changes and unsigned wrap.
TEST(AlgorithmUtilTest, FlowChecksCoverSignedAndUnsignedBoundaries)
{
    constexpr sint32 intMax = std::numeric_limits<sint32>::max();
    constexpr sint32 intMin = std::numeric_limits<sint32>::lowest();
    constexpr uint32 uintMax = std::numeric_limits<uint32>::max();

    EXPECT_EQ(LLBC_CheckFlowUseAdd<sint32>(0, 0), LLBC_FlowType::NoFlow);
    EXPECT_EQ(LLBC_CheckFlowUseAdd<sint32>(intMax, 1), LLBC_FlowType::OverFlow);
    EXPECT_EQ(LLBC_CheckFlowUseAdd<sint32>(intMin, -1), LLBC_FlowType::UnderFlow);
    EXPECT_EQ(LLBC_CheckFlowUseAdd<sint32>(intMax, -1), LLBC_FlowType::NoFlow);
    EXPECT_EQ(LLBC_CheckFlowUseAdd<sint32>(intMin, 1), LLBC_FlowType::NoFlow);
    EXPECT_EQ(LLBC_CheckFlowUseAdd<uint32>(uintMax, 1u), LLBC_FlowType::OverFlow);
    EXPECT_EQ(LLBC_CheckFlowUseAdd<uint32>(42u, 1u), LLBC_FlowType::NoFlow);

    EXPECT_EQ(LLBC_CheckFlowUseSub<sint32>(0, 0), LLBC_FlowType::NoFlow);
    EXPECT_EQ(LLBC_CheckFlowUseSub<sint32>(intMin, 1), LLBC_FlowType::UnderFlow);
    EXPECT_EQ(LLBC_CheckFlowUseSub<sint32>(intMax, -1), LLBC_FlowType::OverFlow);
    EXPECT_EQ(LLBC_CheckFlowUseSub<sint32>(intMin, -1), LLBC_FlowType::NoFlow);
    EXPECT_EQ(LLBC_CheckFlowUseSub<sint32>(intMax, 1), LLBC_FlowType::NoFlow);
    EXPECT_EQ(LLBC_CheckFlowUseSub<uint32>(0u, 1u), LLBC_FlowType::UnderFlow);
    EXPECT_EQ(LLBC_CheckFlowUseSub<uint32>(1u, 1u), LLBC_FlowType::NoFlow);
}

// Escaping is used to preserve delimiter-bearing values. It escapes both the
// requested characters and the escape marker itself, then restores the exact
// input through the matching unescape operation.
TEST(AlgorithmUtilTest, EscapesAndUnescapesDelimiterAndEscapeCharacters)
{
    LLBC_String escaped = "a,b\\c";
    EXPECT_EQ(LLBC_StringEscape(escaped, ",", '\\'), "a\\,b\\\\c");
    EXPECT_EQ(LLBC_StringUnEscape(escaped, '\\'), "a,b\\c");

    LLBC_String markerOnly = "a#b";
    EXPECT_EQ(LLBC_StringEscape(markerOnly, "", '#'), "a##b");
    EXPECT_EQ(LLBC_StringUnEscape(markerOnly, '#'), "a#b");

    LLBC_String unchanged = "plain";
    EXPECT_EQ(LLBC_StringEscape(unchanged, ",", '\\'), "plain");
    EXPECT_EQ(LLBC_StringUnEscape(unchanged, '\\'), "plain");

    LLBC_String empty;
    EXPECT_TRUE(LLBC_StringEscape(empty, ",", '\\').empty());

    // Signed char values are intentionally ignored by the compact 128-bit flag
    // table, which only represents the ASCII range.
    LLBC_String nonAscii(1, static_cast<char>(0xff));
    EXPECT_EQ(LLBC_StringEscape(nonAscii, nonAscii, static_cast<char>(0xfe)).size(), 1lu);

    LLBC_String terminalEscape = "end,";
    EXPECT_EQ(LLBC_StringEscape(terminalEscape, ",", '\\'), "end\\,");
}
