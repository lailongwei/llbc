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
// @coverage-target: llbc/src/core/time/Time.cpp
// @coverage-target: llbc/include/llbc/core/time/TimeInl.h

// Calendar constructors and accessors use local time consistently. Month is
// deliberately zero-based, matching tm::tm_mon and the public documentation.
TEST(TimeTest, ConstructsFormatsAndExposesCalendarParts)
{
    const auto time = LLBC_Time::FromTimeParts(2024, 2, 29, 23, 58, 57, 123, 456);

    EXPECT_EQ(time.GetYear(), 2024);
    EXPECT_EQ(time.GetMonth(), 1);
    EXPECT_EQ(time.GetDayOfMonth(), 29);
    EXPECT_EQ(time.GetDayOfWeek(), 4);       // Thursday, Sunday-based.
    EXPECT_EQ(time.GetDayOfWeek(false), 3);  // Thursday, Monday-based.
    EXPECT_EQ(time.GetHour(), 23);
    EXPECT_EQ(time.GetMinute(), 58);
    EXPECT_EQ(time.GetSecond(), 57);
    EXPECT_EQ(time.GetMillisecond(), 123);
    EXPECT_EQ(time.GetMicrosecond(), 456);
    EXPECT_EQ(time.Format(), "2024-02-29 23:58:57");
    EXPECT_EQ(time.ToString(), "2024-02-29 23:58:57.123456");
    EXPECT_FALSE(time.FormatAsGmt().empty());

    const timeval timevalValue {
        static_cast<time_t>(time.GetTimestampInSecs()),
        123456,
    };
    const auto fromTimeVal = LLBC_Time::FromTimeVal(timevalValue);
    EXPECT_EQ(fromTimeVal.GetTimestampInSecs(), time.GetTimestampInSecs());
    EXPECT_EQ(fromTimeVal.GetMicrosecond(), 456);

    const timespec timeSpecValue {
        static_cast<time_t>(time.GetTimestampInSecs()),
        123456789,
    };
    const auto fromTimeSpec = LLBC_Time::FromTimeSpec(timeSpecValue);
    EXPECT_EQ(fromTimeSpec.GetTimestampInSecs(), time.GetTimestampInSecs());
    EXPECT_EQ(fromTimeSpec.GetMillisecond(), 123);
    EXPECT_EQ(fromTimeSpec.GetMicrosecond(), 456);
}

// Parse string representations through every public overload, including custom
// separators and a null C-string, which must safely return the UTC epoch value.
TEST(TimeTest, ParsesTimeStringsAndCustomSeparators)
{
    const auto parsed = LLBC_Time::FromTimeStr("2024-02-29 23:58:57.123456");
    EXPECT_EQ(parsed.ToString(), "2024-02-29 23:58:57.123456");

    const std::string stdString = "2025-01-01 20:05:08.123456";
    const LLBC_String llbcString(stdString);
    EXPECT_EQ(LLBC_Time::FromTimeStr(stdString).ToString(), stdString);
    EXPECT_EQ(LLBC_Time::FromTimeStr(llbcString).ToString(), stdString);

    LLBC_TimeSep separators;
    separators.YMDSep = '/';
    separators.HMSSep = '.';
    separators.microSecSep = '#';
    EXPECT_EQ(LLBC_Time::FromTimeStr("2025/01/01 20.05.08#123456", separators).ToString(),
              "2025-01-01 20:05:08.123456");

    EXPECT_EQ(LLBC_Time::FromTimeStr(static_cast<const char *>(nullptr)), LLBC_Time::utcBegin);
    EXPECT_EQ(LLBC_Time::FromTimeStr(" \t "), LLBC_Time::utcBegin);
}

// Calendar arithmetic preserves valid dates across leap years and month-end
// boundaries, while duration arithmetic and comparisons operate in microseconds.
TEST(TimeTest, AddsCalendarAndDurationIntervalsCorrectly)
{
    const auto leapDay = LLBC_Time::FromTimeParts(2020, 2, 29, 12, 0, 0, 1, 2);
    EXPECT_EQ(leapDay.AddYears(1).ToString(), "2021-02-28 12:00:00.001002");
    EXPECT_EQ(leapDay.AddYears(4).ToString(), "2024-02-29 12:00:00.001002");

    const auto januaryEnd = LLBC_Time::FromTimeParts(2024, 1, 31, 1, 2, 3);
    EXPECT_EQ(januaryEnd.AddMonths(1).ToString(), "2024-02-29 01:02:03.000000");
    const auto marchEnd = LLBC_Time::FromTimeParts(2024, 3, 31, 1, 2, 3);
    EXPECT_EQ(marchEnd.AddMonths(-1).ToString(), "2024-02-29 01:02:03.000000");

    const auto base = LLBC_Time::FromTimeParts(2024, 1, 1, 1, 2, 3, 4, 5);
    const auto shifted = base.AddDays(1)
                             .AddHours(2)
                             .AddMinutes(3)
                             .AddSeconds(4)
                             .AddMillis(5)
                             .AddMicros(6);
    EXPECT_EQ(shifted - base, LLBC_TimeSpan::FromDays(1, 2, 3, 4, 5, 6));
    EXPECT_EQ(base + LLBC_TimeSpan::oneSec, base.AddSeconds(1));
    EXPECT_EQ(base - LLBC_TimeSpan::oneSec, base.AddSeconds(-1));
    EXPECT_LT(base, shifted);
    EXPECT_GT(shifted, base);
    EXPECT_NE(base, shifted);
}

// Begin/offset helpers and serialization drive timer scheduling. They must
// normalize the calendar portions without losing subsecond precision.
TEST(TimeTest, CalculatesBeginsOffsetsIntervalsAndSerialization)
{
    const auto time = LLBC_Time::FromTimeParts(2024, 2, 29, 23, 58, 57, 123, 456);

    EXPECT_EQ(time.GetBeginTimeOfHour().ToString(), "2024-02-29 23:00:00.000000");
    EXPECT_EQ(time.GetBeginTimeOfDay().ToString(), "2024-02-29 00:00:00.000000");
    EXPECT_EQ(time.GetBeginTimeOfMonth().ToString(), "2024-02-01 00:00:00.000000");
    EXPECT_EQ(time.GetOffsetTimeOfHour(), LLBC_TimeSpan::FromMinutes(58, 57, 123, 456));
    EXPECT_EQ(time.GetOffsetTimeOfDay(), LLBC_TimeSpan::FromHours(23, 58, 57, 123, 456));
    EXPECT_EQ(time.GetOffsetTimeOfMonth(),
              LLBC_TimeSpan::FromDays(28, 23, 58, 57, 123, 456));

    EXPECT_EQ(time.GetIntervalToTimeOfHour(LLBC_TimeSpan::FromMinutes(10)),
              LLBC_TimeSpan::FromMinutes(11, 2, 876, 544));
    EXPECT_EQ(time.GetIntervalToTimeOfDay(LLBC_TimeSpan::FromHours(1)),
              LLBC_TimeSpan::FromHours(1, 1, 2, 876, 544));

    LLBC_Stream stream;
    time.Serialize(stream);
    LLBC_Time decoded;
    ASSERT_TRUE(decoded.Deserialize(stream));
    EXPECT_EQ(decoded, time);

    LLBC_Stream empty;
    const auto unchanged = decoded;
    EXPECT_FALSE(decoded.Deserialize(empty));
    EXPECT_EQ(decoded, unchanged);

    std::ostringstream output;
    output << time;
    EXPECT_EQ(output.str(), time.ToString());
}

// Short date/time forms are intentionally accepted for convenient scheduling
// input. This exercises the one- and zero-separator parser branches as well as
// whitespace normalization and the current-time helper.
TEST(TimeTest, ParsesPartialFormsAndTracksCurrentTime)
{
    const auto secondOnly = LLBC_Time::FromTimeStr("35");
    EXPECT_EQ(secondOnly.GetSecond(), 35);
    EXPECT_EQ(secondOnly.GetMinute(), 0);

    const auto minuteSecond = LLBC_Time::FromTimeStr("19:35");
    EXPECT_EQ(minuteSecond.GetMinute(), 19);
    EXPECT_EQ(minuteSecond.GetSecond(), 35);

    const auto dayTime = LLBC_Time::FromTimeStr("13 19:35");
    EXPECT_EQ(dayTime.GetDayOfMonth(), 13);
    EXPECT_EQ(dayTime.GetHour(), 0);
    EXPECT_EQ(dayTime.GetMinute(), 19);
    EXPECT_EQ(dayTime.GetSecond(), 35);

    const auto monthDayTime = LLBC_Time::FromTimeStr("12-13 19:35");
    EXPECT_EQ(monthDayTime.GetMonth(), 11);
    EXPECT_EQ(monthDayTime.GetDayOfMonth(), 13);

    EXPECT_EQ(LLBC_Time::FromTimeStr(" \t2024-02-29 23:58:57.123456 \t ").ToString(),
              "2024-02-29 23:58:57.123456");
    EXPECT_EQ(LLBC_Time::FromTimeStr("123456789012 00:00:00"), LLBC_Time::utcBegin);

    const sint64 before = LLBC_Time::NowTimestampInMicros();
    const auto now = LLBC_Time::Now();
    const sint64 after = LLBC_Time::NowTimestampInMicros();
    EXPECT_GE(now.GetTimestampInMicros(), before);
    EXPECT_LE(now.GetTimestampInMicros(), after);
    EXPECT_FALSE(LLBC_Time::Format(now.GetTimestampInSecs(), nullptr).empty());
    EXPECT_FALSE(LLBC_Time::FormatAsGmt(now.GetTimestampInSecs(), nullptr).empty());
}

// Calendar-cycle helpers support timers that fire at hour/day/week/month
// boundaries. Test cross-year month math, weekly offsets, assignment, and
// positive/non-positive cycle crossing behavior.
TEST(TimeTest, HandlesCrossYearMonthAndCycleCalculations)
{
    const auto novemberEnd = LLBC_Time::FromTimeParts(2023, 11, 30, 1, 2, 3);
    EXPECT_EQ(novemberEnd.AddMonths(3).ToString(), "2024-02-29 01:02:03.000000");
    const auto januaryEnd = LLBC_Time::FromTimeParts(2024, 1, 31, 1, 2, 3);
    EXPECT_EQ(januaryEnd.AddMonths(-2).ToString(), "2023-11-30 01:02:03.000000");

    EXPECT_TRUE(LLBC_Time::IsLeapYear(2024));
    EXPECT_FALSE(LLBC_Time::IsLeapYear(2023));
    EXPECT_EQ(LLBC_Time::GetMonthMaxDays(2024, 2), 29);
    EXPECT_EQ(LLBC_Time::GetMonthMaxDays(2023, 2), 28);
    EXPECT_EQ(LLBC_Time::GetMonthSpanDays(2024, 2), 60);

    const auto from = LLBC_Time::FromTimeParts(2024, 1, 1, 10, 0, 0);
    const auto to = LLBC_Time::FromTimeParts(2024, 1, 3, 12, 0, 0);
    EXPECT_EQ(LLBC_Time::GetCrossedHours(from, to), 50);
    EXPECT_EQ(LLBC_Time::GetCrossedDays(from, to), 2);
    EXPECT_EQ(LLBC_Time::GetCrossedHours(to, from), 0);
    EXPECT_EQ(LLBC_Time::GetCrossedDays(to, from), 0);

    const auto weeklyOffset = from.GetOffsetTimeOfWeek();
    EXPECT_GE(weeklyOffset, LLBC_TimeSpan::zero);
    EXPECT_LT(weeklyOffset, LLBC_TimeSpan::oneWeek);
    EXPECT_EQ(from.GetBeginTimeOfWeek() + weeklyOffset, from);

    LLBC_Time assigned;
    assigned = from;
    EXPECT_EQ(assigned, from);
    const LLBC_Time &assignedAlias = assigned;
    assigned = assignedAlias;
    EXPECT_EQ(assigned, from);
}

// Weekly interval normalization accepts negative/overflowing offsets, and
// crossed-month calculation handles both ordinary ranges and defensive inputs.
// Extra date/time whitespace is accepted by the parser's separator handoff.
TEST(TimeTest, NormalizesWeeklyIntervalsAndCountsMonthBoundaries)
{
    const auto parsed = LLBC_Time::FromTimeStr("2024-02-29   23:58:57");
    EXPECT_EQ(parsed.ToString(), "2024-02-29 23:58:57.000000");

    const auto time = LLBC_Time::FromTimeParts(2024, 1, 15, 12, 0, 0);
    const auto negativeWeekOffset =
        time.GetIntervalToTimeOfWeek(-LLBC_TimeSpan::oneDay, false);
    const auto overflowingWeekOffset =
        time.GetIntervalToTimeOfWeek(LLBC_TimeSpan::oneWeek + LLBC_TimeSpan::oneDay, true);
    EXPECT_GE(negativeWeekOffset, LLBC_TimeSpan::zero);
    EXPECT_LT(negativeWeekOffset, LLBC_TimeSpan::oneWeek);
    EXPECT_GE(overflowingWeekOffset, LLBC_TimeSpan::zero);
    EXPECT_LT(overflowingWeekOffset, LLBC_TimeSpan::oneWeek);

    const auto from = LLBC_Time::FromTimeParts(2024, 1, 1, 0, 0, 0);
    const auto to = LLBC_Time::FromTimeParts(2024, 3, 1, 0, 0, 0);
    EXPECT_EQ(LLBC_Time::GetCrossedMonths(from, to), 2);
    EXPECT_EQ(LLBC_Time::GetCrossedMonths(to, from), 0);
    EXPECT_EQ(LLBC_Time::GetCrossedMonths(from, to, LLBC_TimeSpan::oneDay * 31), 0);

    const auto weekStart = from.GetBeginTimeOfWeek();
    EXPECT_EQ(LLBC_Time::GetCrossedWeeks(weekStart, weekStart.AddDays(15)), 2);

    EXPECT_GT(LLBC_Time::NowTimestampInSecs(), 0);
    EXPECT_GT(LLBC_Time::NowTimestampInMillis(), 0);
    const auto microsRoundTrip = LLBC_Time::FromMicros(time.GetTimestampInMicros());
    EXPECT_EQ(microsRoundTrip, time);
    EXPECT_EQ(time.GetTimestampInMillis(),
              time.GetTimestampInMicros() / LLBC_TimeConst::numOfMicrosPerMillisecond);
    EXPECT_EQ(time.GetDayOfYear(), 14);

    const tm gmt = time.GetGmtTime();
    EXPECT_GE(gmt.tm_year, 0);
    const tm local = time.GetLocalTime();
    EXPECT_EQ(local.tm_year + 1900, time.GetYear());
    EXPECT_TRUE(time >= time);
    EXPECT_TRUE(time <= time);
}
