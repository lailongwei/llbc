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

#include <sstream>
#include <string>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/time/TimeSpan.cpp
// @coverage-target: llbc/include/llbc/core/time/TimeSpanInl.h

namespace
{

constexpr sint64 SpanMicros(int days,
                            int hours = 0,
                            int minutes = 0,
                            int seconds = 0,
                            int millis = 0,
                            int micros = 0)
{
    return static_cast<sint64>(days) * LLBC_TimeConst::numOfMicrosPerDay +
           static_cast<sint64>(hours) * LLBC_TimeConst::numOfMicrosPerHour +
           static_cast<sint64>(minutes) * LLBC_TimeConst::numOfMicrosPerMinute +
           static_cast<sint64>(seconds) * LLBC_TimeConst::numOfMicrosPerSecond +
           static_cast<sint64>(millis) * LLBC_TimeConst::numOfMicrosPerMillisecond +
           micros;
}

} // namespace

// Time spans represent a signed duration in microseconds. The convenience builders
// must preserve every component and expose both component and total accessors.
TEST(TimeSpanTest, FactoriesComponentsAndConstants)
{
    constexpr auto span = LLBC_TimeSpan::FromDays(1, 2, 3, 4, 5, 6);
    static_assert(span == LLBC_TimeSpan::FromMicros(SpanMicros(1, 2, 3, 4, 5, 6)));

    EXPECT_EQ(span.GetDays(), 1);
    EXPECT_EQ(span.GetHours(), 2);
    EXPECT_EQ(span.GetMinutes(), 3);
    EXPECT_EQ(span.GetSeconds(), 4);
    EXPECT_EQ(span.GetMillis(), 5);
    EXPECT_EQ(span.GetMicros(), 6);
    EXPECT_EQ(span.GetTotalDays(), 1);
    EXPECT_EQ(span.GetTotalHours(), 26);
    EXPECT_EQ(span.GetTotalMinutes(), 1563);
    EXPECT_EQ(span.GetTotalSeconds(), 93784);
    EXPECT_EQ(span.GetTotalMillis(), 93784005);
    EXPECT_EQ(span.GetTotalMicros(), SpanMicros(1, 2, 3, 4, 5, 6));
    EXPECT_EQ(span.ToString(), "01 02:03:04.005006");

    EXPECT_EQ(LLBC_TimeSpan::FromMicros(123).GetTotalMicros(), 123);
    EXPECT_EQ(LLBC_TimeSpan::FromMillis(123, 456).GetTotalMicros(), 123456);
    EXPECT_EQ(LLBC_TimeSpan::FromSeconds(1, 2, 3).GetTotalMicros(), 1002003);
    EXPECT_EQ(LLBC_TimeSpan::FromMinutes(1, 2, 3, 4).GetTotalMicros(), 62003004);
    EXPECT_EQ(LLBC_TimeSpan::FromHours(1, 2, 3, 4, 5).GetTotalMicros(), 3723004005);

    EXPECT_EQ(LLBC_TimeSpan::zero, LLBC_TimeSpan::FromMicros(0));
    EXPECT_EQ(LLBC_TimeSpan::oneMicrosec, LLBC_TimeSpan::FromMicros(1));
    EXPECT_EQ(LLBC_TimeSpan::oneMillisec, LLBC_TimeSpan::FromMillis(1));
    EXPECT_EQ(LLBC_TimeSpan::oneSec, LLBC_TimeSpan::FromSeconds(1));
    EXPECT_EQ(LLBC_TimeSpan::oneMin, LLBC_TimeSpan::FromMinutes(1));
    EXPECT_EQ(LLBC_TimeSpan::oneHour, LLBC_TimeSpan::FromHours(1));
    EXPECT_EQ(LLBC_TimeSpan::oneDay, LLBC_TimeSpan::FromDays(1));
    EXPECT_EQ(LLBC_TimeSpan::oneWeek, LLBC_TimeSpan::FromDays(7));
    EXPECT_EQ(LLBC_TimeSpan::negOneMicrosec, LLBC_TimeSpan::FromMicros(-1));
    EXPECT_EQ(LLBC_TimeSpan::negOneMillisec, LLBC_TimeSpan::FromMillis(-1));
    EXPECT_EQ(LLBC_TimeSpan::negOneSec, LLBC_TimeSpan::FromSeconds(-1));
    EXPECT_EQ(LLBC_TimeSpan::negOneMin, LLBC_TimeSpan::FromMinutes(-1));
    EXPECT_EQ(LLBC_TimeSpan::negOneHour, LLBC_TimeSpan::FromHours(-1));
    EXPECT_EQ(LLBC_TimeSpan::negOneDay, LLBC_TimeSpan::FromDays(-1));
    EXPECT_EQ(LLBC_TimeSpan::negOneWeek, LLBC_TimeSpan::FromDays(-7));
}

// Duration arithmetic is deliberately value based: every operation returns or
// mutates the underlying microsecond count with integer truncation for scaling.
TEST(TimeSpanTest, ArithmeticAndComparisons)
{
    const auto base = LLBC_TimeSpan::FromSeconds(10, 500);
    const auto incremented = LLBC_TimeSpan::zero
                                 .AddDays(1)
                                 .AddHours(2)
                                 .AddMinutes(3)
                                 .AddSeconds(4)
                                 .AddMillis(5)
                                 .AddMicros(6);
    EXPECT_EQ(incremented, LLBC_TimeSpan::FromDays(1, 2, 3, 4, 5, 6));

    EXPECT_EQ(-base, LLBC_TimeSpan::FromMillis(-10500));
    EXPECT_EQ(base + LLBC_TimeSpan::FromSeconds(2), LLBC_TimeSpan::FromMillis(12500));
    EXPECT_EQ(base - LLBC_TimeSpan::FromSeconds(2), LLBC_TimeSpan::FromMillis(8500));
    EXPECT_EQ(base * 1.5, LLBC_TimeSpan::FromMillis(15750));
    EXPECT_EQ(base / 2.0, LLBC_TimeSpan::FromMillis(5250));
    EXPECT_EQ(base % 1000000, LLBC_TimeSpan::FromMillis(500));
    EXPECT_EQ(base % LLBC_TimeSpan::FromSeconds(4), LLBC_TimeSpan::FromMillis(2500));

    auto mutableSpan = base;
    EXPECT_EQ((mutableSpan += LLBC_TimeSpan::FromSeconds(1)), LLBC_TimeSpan::FromMillis(11500));
    EXPECT_EQ((mutableSpan -= LLBC_TimeSpan::FromMillis(500)), LLBC_TimeSpan::FromSeconds(11));
    EXPECT_EQ((mutableSpan *= 1.5), LLBC_TimeSpan::FromMillis(16500));
    EXPECT_EQ((mutableSpan /= 3.0), LLBC_TimeSpan::FromMillis(5500));
    EXPECT_EQ((mutableSpan %= LLBC_TimeConst::numOfMicrosPerSecond),
              LLBC_TimeSpan::FromMillis(500));
    EXPECT_EQ((mutableSpan %= LLBC_TimeSpan::FromMicros(300)), LLBC_TimeSpan::FromMicros(200));

    const auto same = LLBC_TimeSpan::FromSeconds(10, 500);
    const auto smaller = LLBC_TimeSpan::FromSeconds(10, 499);
    EXPECT_TRUE(base == same);
    EXPECT_FALSE(base != same);
    EXPECT_TRUE(base != smaller);
    EXPECT_TRUE(base > smaller);
    EXPECT_TRUE(base >= same);
    EXPECT_TRUE(base >= smaller);
    EXPECT_TRUE(smaller < base);
    EXPECT_TRUE(smaller <= base);
    EXPECT_TRUE(base <= same);
}

// A span string is a human-facing duration format, not a decimal seconds format:
// the fraction after '.' is parsed directly as a microsecond integer.
TEST(TimeSpanTest, ParsesSupportedFormatsAndStringOverloads)
{
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr("03"), LLBC_TimeSpan::FromSeconds(3));
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr("02:03"), LLBC_TimeSpan::FromMinutes(2, 3));
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr("01:02:03"), LLBC_TimeSpan::FromHours(1, 2, 3));
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr("8 01:02:03.4"),
              LLBC_TimeSpan::FromDays(8, 1, 2, 3, 0, 4));
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr(" \t 05   01:02:03.04 \t "),
              LLBC_TimeSpan::FromDays(5, 1, 2, 3, 0, 4));

    const std::string stdString = "05 01:02:03.04";
    const LLBC_String llbcString(stdString);
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr(stdString),
              LLBC_TimeSpan::FromDays(5, 1, 2, 3, 0, 4));
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr(llbcString),
              LLBC_TimeSpan::FromDays(5, 1, 2, 3, 0, 4));
    EXPECT_EQ(LLBC_TimeSpan(stdString), LLBC_TimeSpan::FromDays(5, 1, 2, 3, 0, 4));
    EXPECT_EQ(LLBC_TimeSpan(llbcString), LLBC_TimeSpan::FromDays(5, 1, 2, 3, 0, 4));
    EXPECT_EQ(LLBC_TimeSpan(stdString.c_str()), LLBC_TimeSpan::FromDays(5, 1, 2, 3, 0, 4));

    LLBC_TimeSep separators;
    separators.datetimeSep = '#';
    separators.HMSSep = '/';
    separators.microSecSep = '*';
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr("6#08/56/55*123456", separators),
              LLBC_TimeSpan::FromDays(6, 8, 56, 55, 123, 456));
}

// Parsing must reject unsafe numeric fragments and be safe for empty or null
// C-string inputs. Empty components are interpreted as zero by the parser.
TEST(TimeSpanTest, ParsesDefensiveAndBoundaryInputs)
{
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr("1"), LLBC_TimeSpan::FromSeconds(1));
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr(""), LLBC_TimeSpan::zero);
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr(" \t "), LLBC_TimeSpan::zero);
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr(static_cast<const char *>(nullptr)), LLBC_TimeSpan::zero);
    EXPECT_EQ(LLBC_TimeSpan(static_cast<const char *>(nullptr)), LLBC_TimeSpan::zero);

    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr(":2"), LLBC_TimeSpan::FromSeconds(2));
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr("1::2"), LLBC_TimeSpan::FromHours(1, 0, 2));
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr("1:2:."), LLBC_TimeSpan::FromMinutes(62));

    LLBC_TimeSep noTimeAfterDateSeparator;
    noTimeAfterDateSeparator.datetimeSep = '#';
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr("1#", noTimeAfterDateSeparator),
              LLBC_TimeSpan::FromDays(1));

    // The implementation uses a fixed 12-byte numeric buffer. An overlong part
    // must stop parsing rather than overflow that buffer.
    EXPECT_EQ(LLBC_TimeSpan::FromSpanStr("123456789012 01:02:03"), LLBC_TimeSpan::zero);
}

// Time spans are serializable duration values. Failed reads must not overwrite an
// existing value, which lets callers safely retry with a complete stream.
TEST(TimeSpanTest, SerializesAndFormatsForStreams)
{
    const auto source = LLBC_TimeSpan::FromDays(2, 3, 4, 5, 6, 7);
    LLBC_Stream stream;
    source.Serialize(stream);

    LLBC_TimeSpan decoded = LLBC_TimeSpan::zero;
    ASSERT_TRUE(decoded.Deserialize(stream));
    EXPECT_EQ(decoded, source);

    LLBC_Stream emptyStream;
    LLBC_TimeSpan unchanged = LLBC_TimeSpan::FromSeconds(42);
    EXPECT_FALSE(unchanged.Deserialize(emptyStream));
    EXPECT_EQ(unchanged, LLBC_TimeSpan::FromSeconds(42));

    std::ostringstream output;
    output << source;
    EXPECT_EQ(output.str(), "02 03:04:05.006007");
}
