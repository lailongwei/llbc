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


#include "core/time/TestCase_Core_Time_Time.h"

TestCase_Core_Time_Time::TestCase_Core_Time_Time()
{
}

TestCase_Core_Time_Time::~TestCase_Core_Time_Time()
{
}

int TestCase_Core_Time_Time::Run(int argc, char *argv[])
{
    std::cout << "core/time/time test: " << std::endl;

    OSTimeTest();
    std::cout << std::endl;

    TimeClassTest();
    std::cout << std::endl;

    TimeSpanClassTest();
    std::cout << std::endl;

    CpuTimeTest();
    std::cout << std::endl;

    GetIntervalToTest();
    std::cout << std::endl;

    CrossTimePeriodTest();
    std::cout << std::endl;

    std::cout << "Press any key to continue ... ..." << std::endl;
    getchar();

    return 0;
}

void TestCase_Core_Time_Time::OSTimeTest()
{
    std::cout <<"OS_Time part test: " <<std::endl;

    std::cout <<"Now timeoffset: " <<LLBC_GetTimezone() <<std::endl;
    sint64 nowTimeInMs = LLBC_GetMilliseconds();
    std::cout <<"LLBC_GetMilliseconds(): "<<nowTimeInMs <<std::endl;
    sint64 nowTimeInUs = LLBC_GetMicroseconds();
    std::cout <<"LLBC_GetMicroseconds(): " <<nowTimeInUs <<std::endl;

    struct timeval tv;
    LLBC_GetTimeOfDay(&tv, nullptr);
    std::cout <<"LLBC_GetTimeOfDay(), tv_sec: " <<tv.tv_sec <<", tv_usec: " <<tv.tv_usec <<std::endl;
#if LLBC_TARGET_PLATFORM_WIN32 && LLBC_CFG_OS_IMPL_GETTIMEOFDAY
    gettimeofday(&tv, nullptr);
    std::cout <<"WIN32 spec: gettimeofday(), tv_sec: " <<tv.tv_sec <<", tv_usec: " <<tv.tv_usec <<std::endl;
#endif
}

void TestCase_Core_Time_Time::TimeClassTest()
{
    std::cout <<"LLBC_Time test: " <<std::endl;

    // Dump LLBC_Time size.
    std::cout <<"sizeof(LLBC_Time): " <<sizeof(LLBC_Time) <<std::endl;

    // Default time test.
    LLBC_Time defaultTime;
    std::cout <<"Default time: " <<defaultTime <<std::endl;

    // NowTimeStampInXXX() tet.
    std::cout << "LLBC_Time::NowTimestampInSecs(): " << LLBC_Time::NowTimestampInSecs() << std::endl;
    std::cout << "LLBC_Time::NowTimestampInMillis(): " << LLBC_Time::NowTimestampInMillis() << std::endl;
    std::cout << "LLBC_Time::NowTimestampInMicros(): " << LLBC_Time::NowTimestampInMicros() << std::endl;

    // Now(), GetTimeTick(), Format(), FormatAsGmt() test.
    LLBC_Time now = LLBC_Time::Now();
    std::cout <<"now time: " <<now 
              <<", Timestamp(in secs): " <<now.GetTimestampInSecs()
              <<", Timestamp(in millis): " <<now.GetTimestampInMillis()
              <<", Timestamp(in micros): " <<now.GetTimestampInMicros() <<std::endl;
    std::cout <<"Default format, LOCAL: " <<now.Format() <<", UTC: " <<now.FormatAsGmt() <<std::endl;
    std::cout <<"Special format(%y-%m) LOCAL: " <<now.Format("%y-%m") <<", UTC: " <<now.FormatAsGmt("%y-%m") <<std::endl;

    // Get Time parts test.
    std::cout <<"Now year:" <<now.GetYear() 
        <<", month:" <<now.GetMonth() <<", day:" <<now.GetDayOfMonth() <<std::endl;
    std::cout <<"Now hour:" <<now.GetHour() 
        <<", minute:" <<now.GetMinute() <<", second:" <<now.GetSecond() <<std::endl;
    std::cout <<"Now millisecond: " <<now.GetMillisecond() 
        <<", microsecond: " <<now.GetMicrosecond() <<std::endl;

    // GetDate(), GetTimeOfDay() test.
    std::cout <<"GetDate(): " <<now.GetDate() <<std::endl;
    std::cout <<"GetTimeOfDay(): " <<now.GetTimeOfDay() <<std::endl;

    // GetGmtTime(), GetLocalTime() test.
    std::cout <<"GetGmtTime():" <<std::endl;
    PrintCLibTimeStruct(now.GetGmtTime());
    std::cout <<"GetLocalTime():" <<std::endl;
    PrintCLibTimeStruct(now.GetLocalTime());

    // AddYears/Months/... test
    std::cout <<"AddYears() test:" <<std::endl;
    for (int i = 1; i <= 4; ++i)
        std::cout <<i <<": " <<now.AddYears(i) <<", -" <<i <<": " <<now.AddYears(-i) <<"\n";
    std::cout <<std::endl;

    std::cout <<"AddMonths() test:" <<std::endl;
    for (int i = 1; i <= 15; ++i)
        std::cout <<i <<": " <<now.AddMonths(i) <<", -" <<i <<": " <<now.AddMonths(-i) <<"\n";
    std::cout <<std::endl;

    std::cout <<"AddDays() test:" <<std::endl;
    for (int i = 1; i <= 70; ++i)
        std::cout <<i <<": " <<now.AddDays(i) <<", -" <<i <<": " <<now.AddDays(-i) <<"\n";
    std::cout <<std::endl;

    std::cout <<"AddHours() test:" <<std::endl;
    for (int i = 1; i <= 62; ++i)
        std::cout <<i <<": " <<now.AddHours(i) <<", -" <<i <<": " <<now.AddHours(-i) <<"\n";
    std::cout <<std::endl;

    std::cout <<"AddMinutes() test:" <<std::endl;
    for (int i = 1; i <= 62; ++i)
        std::cout <<i <<": " <<now.AddMinutes(i) <<", -" <<i <<": " <<now.AddMinutes(-i) <<"\n";
    std::cout <<std::endl;

    std::cout <<"AddSeconds() test:" <<std::endl;
    for (int i = 1; i <= 62; ++i)
        std::cout <<i <<": " <<now.AddSeconds(i) <<", -" <<i <<": " <<now.AddSeconds(-i) <<"\n";
    std::cout <<std::endl;

    int addMillisStep = 500;
    std::cout <<"AddMillis() test(step:+/- " <<addMillisStep <<" milli-seconds):" <<std::endl;
    for (int i = 1; i <= 3; ++i)
    {
        int addMillis = addMillisStep * i;
        std::cout << i <<"(" <<addMillis <<"): " <<now.AddMillis(addMillis).GetTimestampInMicros() <<", -"
            <<i <<"(" <<-addMillis <<"): " <<now.AddMillis(-addMillis).GetTimestampInMicros() <<"\n";
    }
    std::cout <<std::endl;

    int addMicrosStep = 500;
    std::cout <<"AddMicros() test(step:+/- " <<addMicrosStep <<" micro-seconds):" <<std::endl;
    for (int i = 1; i <= 3; ++i)
    {
        int addMicros = addMicrosStep * i;
        std::cout <<i <<"(" <<addMicros <<"): " <<now.AddMicros(addMicros).GetTimestampInMicros() <<", -"
            <<i <<"(" <<-addMicros <<"): " <<now.AddMicros(-addMicros).GetTimestampInMicros() <<"\n";
    }
    std::cout <<std::endl;

    // IsLeapYear() test.
    std::cout <<"IsLeapYear() test:" <<std::endl;
    std::cout <<"1900 is leapyear?" <<LLBC_Time::IsLeapYear(1900) <<std::endl;
    std::cout <<"1904 is leapyear?" <<LLBC_Time::IsLeapYear(1904) <<std::endl;
    std::cout <<"2000 is leapyear?" <<LLBC_Time::IsLeapYear(2000) <<std::endl;
    std::cout <<"2004 is leapyear?" <<LLBC_Time::IsLeapYear(2004) <<std::endl;
    std::cout <<"2015 is leapyear?" <<LLBC_Time::IsLeapYear(2015) <<std::endl;
    std::cout <<"2016 is leapyear?" <<LLBC_Time::IsLeapYear(2016) <<std::endl;

    // GetMonthMaxDays() test.
    std::cout <<"GetMonthMaxDays() test:" <<std::endl;
    for (int i = 0; i <= 13; ++i)
        std::cout <<"2012-" <<i <<": max days: " <<LLBC_Time::GetMonthMaxDays(2012, i) <<std::endl;
    for (int i = 0; i <= 13; ++i)
        std::cout <<"2015-" <<i <<": max days: " <<LLBC_Time::GetMonthMaxDays(2015, i) <<std::endl;

    // FromXXX() test.
    std::cout <<"FromSeconds(1.0): " <<LLBC_Time::FromSeconds(1) <<std::endl;
    std::cout <<"FromMinutes(10001): " <<LLBC_Time::FromMillis(10001) <<std::endl;
    std::cout <<"FromMicros(10000000): " <<LLBC_Time::FromMicros(10000000) <<std::endl;
    timeval tv;
    tv.tv_sec = 10; tv.tv_usec = 10;
    std::cout <<"FromTimeVal(tv.sec=10, tv.usec=10): " <<LLBC_Time::FromTimeVal(tv) <<std::endl;
#if LLBC_TARGET_PLATFORM_WIN32
    LLBC_NS timespec ts;
#else
    timespec ts;
#endif
    ts.tv_sec = 10000000; ts.tv_nsec = 123456;
    std::cout <<"FromTimeSpec(ts.sec=10000000, ts.tv_nsec=123456): " <<LLBC_Time::FromTimeSpec(ts) <<std::endl;

    auto fromTimeStrTest = [](const char *timeStr) {
        LLBC_Time fromTimeRepr = LLBC_Time::FromTimeStr(timeStr);
        std::cout <<"FromTimeStr(" <<timeStr <<"): " <<fromTimeRepr <<std::endl;
        std::cout <<"    millisec: " <<fromTimeRepr.GetMillisecond() <<", microsec: " <<fromTimeRepr.GetMicrosecond() <<std::endl;
    };

    std::cout <<LLBC_Time::FromTimeStr("2018-9-1 10:00:00.333333") <<std::endl;

    fromTimeStrTest("35");
    fromTimeStrTest("35.345678");
    fromTimeStrTest("19:35.345678");
    fromTimeStrTest("13 19:35.345678");
    fromTimeStrTest("13 21:19:35.345678");
    fromTimeStrTest("13 39");
    fromTimeStrTest("13 .345678");
    fromTimeStrTest("12-13 19:35.345678");
    fromTimeStrTest("12-13 21:19:35.345678");
    fromTimeStrTest("2000-12-13 19:35");
    fromTimeStrTest("2000-12-13 19:35.345678");
    fromTimeStrTest("   \t2000-12-13 \t19:35.345678\t    ");
    fromTimeStrTest("- 19:35.345678");
    fromTimeStrTest("-- 19:35.345678");
    fromTimeStrTest("- :.345678");
    fromTimeStrTest("-- :.345678");
    fromTimeStrTest("-- ::.345678");

    size_t fromTimeStrPerfTestTimes = 100000;
    std::vector<LLBC_String> testTimeStrs {
        "2024-09-01 21:55:55.332345",
        "2024-08-30 13:05:37.324868",
        "1970-05-04 03:36:29.892342",
        "1998-02-13 16:05:37.829346",
        "2019-12-13 13:07:19.719054",
    };

    const auto begTime = LLBC_GetMicroseconds();
    for (size_t i = 0; i < fromTimeStrPerfTestTimes; ++i)
    {
        for (size_t j = 0; j < testTimeStrs.size(); ++j)
            LLBC_Time::FromTimeStr(testTimeStrs[j]);
    }
    const auto costTime = LLBC_GetMicroseconds() - begTime;
    std::cout << "LLBC_Time::FromTimeStr() perf test finished"
              << ", test times: " << fromTimeStrPerfTestTimes * testTimeStrs.size()
              << ", cost time: " << costTime << " micro sec"
              << ", per time cost: " << static_cast<double>(costTime) /
                (fromTimeStrPerfTestTimes * testTimeStrs.size()) << " micro sec"
              << std::endl;

    int yearPart = 1971, monthPart = 12, dayPart = 21;
    int hourPart = 13, minutePart = 23, secondPart = 32;
    int milliPart = 322, microPart = 321;
    LLBC_Time fromParts = LLBC_Time::FromTimeParts(
        yearPart, monthPart, dayPart, hourPart, minutePart, secondPart, milliPart, microPart);
    std::cout <<"FromTimeParts(" <<"year=" <<yearPart <<", month=" <<monthPart <<", day=" <<dayPart
        <<", hour=" <<hourPart <<", minute=" <<minutePart <<", second=" <<secondPart
        <<", milliSecond=" <<milliPart <<", microSecond=" <<microPart <<"):"
        << fromParts <<std::endl;
    std::cout <<"    millisec: " <<fromParts.GetMillisecond() <<", microsec: " <<fromParts.GetMicrosecond() <<std::endl;
}

void TestCase_Core_Time_Time::TimeSpanClassTest()
{
    std::cout << "LLBC_TimeSpan test:" << std::endl;

    LLBC_TimeSpan tsFromMicros = LLBC_TimeSpan::FromMicros(123);
    LLBC_TimeSpan tsFromMillis = LLBC_TimeSpan::FromMillis(456);
    LLBC_TimeSpan tsFromMillisMicros = LLBC_TimeSpan::FromMillis(-123, -456);
    LLBC_TimeSpan tsFromSS = LLBC_TimeSpan::FromSeconds(10, 20, -30);
    LLBC_TimeSpan tsFromMMSS1 = LLBC_TimeSpan::FromMinutes(100, -100, 300);
    LLBC_TimeSpan tsFromMMSS2 = LLBC_TimeSpan::FromMinutes(-100, 100, 300);
    LLBC_TimeSpan tsFromHHMMSS = LLBC_TimeSpan::FromHours(5, -400, 500, -600, 700);
    LLBC_TimeSpan tsFromDDHHMMSS = LLBC_TimeSpan::FromDays(1, 2, 3, 4, 500, 600);
    std::cout << "LLBC_TimeSpan::FromMicros(123): " << tsFromMicros << std::endl;
    std::cout << "LLBC_TimeSpan::FromMillis(456): " << tsFromMillis << std::endl;
    std::cout << "LLBC_TimeSpan::FromMillis(-123, -456): " << tsFromMillisMicros << std::endl;
    std::cout << "LLBC_TimeSpan::FromSeconds(10, 20, -30): " << tsFromSS << std::endl;
    std::cout << "LLBC_TimeSpan::FromMinutes(100, -100, 300): " << tsFromMMSS1 << std::endl;
    std::cout << "LLBC_TimeSpan::FromMinutes(-100, 100, 300): " << tsFromMMSS2 << std::endl;
    std::cout << "LLBC_TimeSpan::FromHours(5, -400, 500, -600, 700): " << tsFromHHMMSS << std::endl;
    std::cout << "LLBC_TimeSpan::FromDays(1, 2, 3, 4, 500, 600): " << tsFromDDHHMMSS << std::endl;

    LLBC_TimeSpan ts = LLBC_TimeSpan::FromMicros(1000);
    std::cout << "LLBC_TimeSpan(1000us) * 100 = " << ts * 100 << std::endl;
    std::cout << "LLBC_TimeSpan(1000us) * 0.5 = " << ts * 0.5 << std::endl;
    std::cout << "LLBC_TimeSpan(1000us) / 100 = " << ts / 100 << std::endl;
    std::cout << "LLBC_TimeSpan(1000us) / 0.5 = " << ts / 0.5 << std::endl;
    std::cout << "LLBC_TimeSpan(1000us) % 100 = " << ts % 100 << std::endl;

    std::cout << "LLBC_TimeSpan::FromSpanStr(\"\"): " << LLBC_TimeSpan::FromSpanStr("") << std::endl;
    std::cout << "LLBC_TimeSpan::FromSpanStr(\"1\"): " << LLBC_TimeSpan::FromSpanStr("1") << std::endl;
    std::cout << "LLBC_TimeSpan::FromSpanStr(\"01\"): " << LLBC_TimeSpan::FromSpanStr("01") << std::endl;
    std::cout << "LLBC_TimeSpan::FromSpanStr(\"01:02\"): " << LLBC_TimeSpan::FromSpanStr("01:02") << std::endl;
    std::cout << "LLBC_TimeSpan::FromSpanStr(\"01:02:03\"): " << LLBC_TimeSpan::FromSpanStr("01:02:03") << std::endl;
    std::cout << "LLBC_TimeSpan::FromSpanStr(\"01:02:03.04\"): " << LLBC_TimeSpan::FromSpanStr("01:02:03.04") << std::endl;
    std::cout << "LLBC_TimeSpan::FromSpanStr(\"05 01:02:03.04\"): " << LLBC_TimeSpan::FromSpanStr("05 01:02:03.04") << std::endl;
    std::cout << "LLBC_TimeSpan::FromSpanStr(\"     05 01:02:03.04\"): " << LLBC_TimeSpan::FromSpanStr("    05 01:02:03.04") << std::endl;
    std::cout << "LLBC_TimeSpan::FromSpanStr(\"05 01:02:03.04     \"): " << LLBC_TimeSpan::FromSpanStr("05 01:02:03.04     ") << std::endl;
    std::cout << "LLBC_TimeSpan::FromSpanStr(\"   \t  05    01:02:03.04  \t   \"): " << LLBC_TimeSpan::FromSpanStr("  \t   05 01:02:03.04  \t   ") << std::endl;
    std::cout << "LLBC_TimeSpan::FromSpanStr(\"9999999999999999\"): " << LLBC_TimeSpan::FromSpanStr("9999999999999999") << std::endl;
    std::cout << "LLBC_TimeSpan::FromSpanStr(\"01:02:03:04:05\"): " << LLBC_TimeSpan::FromSpanStr("01:02:03:04:05") << std::endl;
    std::cout << "LLBC_TimeSpan::FromSpanStr(\"-8 01:02:03:04:05\"): " << LLBC_TimeSpan::FromSpanStr("-8 01:02:03:04:05") << std::endl;
    std::cout << "LLBC_TimeSpan::FromSpanStr(std::string(\"05 01:02:03.04\"): " << LLBC_TimeSpan::FromSpanStr(std::string("05 01:02:03.04")) << std::endl;
    std::cout << "LLBC_TimeSpan::FromSpanStr(LLBC_String(\"05 01:02:03.04\"): " << LLBC_TimeSpan::FromSpanStr(LLBC_String("05 01:02:03.04")) << std::endl;

    std::cout << "LLBC_TimeSpan::FromSpanStr() performance test:" << std::endl;
    #if LLBC_DEBUG
    static constexpr int testTimes = 100000;
    #else
    static constexpr int testTimes = 1000000;
    #endif

    const std::vector<LLBC_String> testSpanStrs {
        " 7:45:16.323423",
        " \t\v 8 12:30:40.123456\v\t  ",
        " 9 23:03:55.001002\v\t  ",
        "12:13.3",
        "17:45:16.998923",
    };

    LLBC_TimeSpan finalSpan;
    LLBC_Stopwatch sw;
    for (int i = 0; i < testTimes; ++i)
    {
        for (auto &spanStr : testSpanStrs)
            finalSpan += LLBC_TimeSpan::FromSpanStr(spanStr);
    }

    sw.Pause();
    std::cout << "- Finish, test times:" << testTimes * testSpanStrs.size()
              << ", cost time:" << sw.Elapsed().GetTotalMicros() << " micro sec"
              << ", per time cost: " << sw.Elapsed().GetTotalMicros() /
                 static_cast<double>(testTimes * testSpanStrs.size()) << " micro sec"
              << std::endl;
}

void TestCase_Core_Time_Time::CpuTimeTest()
{
    std::cout << "Cpu time tsc test: " << std::endl;
    for(sint32 idx = 0; idx < 10; ++idx)
    {
        auto freq = LLBC_GetCpuCounterFrequency();
        std::cout << "Current idx:" << std::to_string(idx)
            << ", tsc frequency:" << std::to_string(freq)
            << ", rdtsc:" << LLBC_RdTsc() << std::endl;
    }
    std::cout << "Cpu time tsc end\n" << std::endl;
}

void TestCase_Core_Time_Time::GetIntervalToTest()
{
    std::cout << "Get interval to xxx test:" << std::endl;

    auto toTimeOfHourTestLbda = [](const LLBC_Time &fromTime,
                                   const LLBC_TimeSpan &toTimeOfHour)
    {
        std::cout << "- GetIntervalToTimeOfHour test:" << std::endl;
        std::cout << "  - fromTime: " << fromTime << std::endl;
        std::cout << "  - toTimeOfHour: " << toTimeOfHour << std::endl;
        std::cout << "  - intervalTo: "
            << fromTime.GetIntervalToTimeOfHour(toTimeOfHour) << std::endl;
    };

    toTimeOfHourTestLbda(LLBC_Time::FromTimeStr("2022-07-14 07:30:00"),
                                                LLBC_TimeSpan::FromMinutes(40));
    toTimeOfHourTestLbda(LLBC_Time::FromTimeStr("2022-07-14 07:40:00"),
                                                LLBC_TimeSpan::FromHours(3, 40));
    toTimeOfHourTestLbda(LLBC_Time::FromTimeStr("2022-07-14 07:50:00"),
                                                LLBC_TimeSpan::FromDays(1, 2, 40));

    auto toTimeOfDayTestLbda = [](const LLBC_Time &fromTime,
                                  const LLBC_TimeSpan &toTimeOfDay)
    {
        std::cout << "- GetIntervalToTimeOfDay test:" << std::endl;
        std::cout << "  - fromTime: " << fromTime << std::endl;
        std::cout << "  - toTimeOfDay: " << toTimeOfDay << std::endl;
        std::cout << "  - intervalTo: "
            << fromTime.GetIntervalToTimeOfDay(toTimeOfDay) << std::endl;
    };

    toTimeOfDayTestLbda(LLBC_Time::FromTimeStr("2022-07-14 07:00:00"),
                                               LLBC_TimeSpan::oneHour * 8);
    toTimeOfDayTestLbda(LLBC_Time::FromTimeStr("2022-07-14 08:00:00"),
                                               LLBC_TimeSpan::FromDays(3, 8));
    toTimeOfDayTestLbda(LLBC_Time::FromTimeStr("2022-07-14 09:00:00"),
                                               LLBC_TimeSpan::FromDays(4, 8));

    auto toTimeOfWeekTestLbda = [](const LLBC_Time &fromTime,
                                   const LLBC_TimeSpan &toTimeOfWeek)
    {
        std::cout << "- GetIntervalToTimeOfWeek test:" << std::endl;
        std::cout << "  - fromTime: " << fromTime
            << "(" << LLBC_TimeConst::dayOfWeekDesc[fromTime.GetDayOfWeek()] << ")" << std::endl;
        std::cout << "  - toTimeOfWeek: " << toTimeOfWeek
            << "(" << LLBC_TimeConst::GetDayOfWeekDesc(toTimeOfWeek.GetTotalDays()) << ")" << std::endl;
        std::cout << "  - intervalTo: "
            << fromTime.GetIntervalToTimeOfWeek(toTimeOfWeek) << std::endl;
    };

    // Note: 2022/07/14 is Thursday
    toTimeOfWeekTestLbda(LLBC_Time::FromTimeStr(
        "2022-07-13 07:00:00"), LLBC_TimeSpan::FromDays(5, 8));
    toTimeOfWeekTestLbda(LLBC_Time::FromTimeStr(
        "2022-07-14 08:00:00"), LLBC_TimeSpan::FromDays(7 + 5, 8));
    toTimeOfWeekTestLbda(LLBC_Time::FromTimeStr(
        "2022-07-15 09:00:00"), LLBC_TimeSpan::FromDays(7 + 5, 8));
}

void TestCase_Core_Time_Time::CrossTimePeriodTest()
{
    std::cout << "Cross time period test:" << std::endl;

    // IsCrossedHour() test:
    auto crossedHourTestLbda = [](const LLBC_Time &from,
                                  const LLBC_Time &to,
                                  const LLBC_TimeSpan &timeOfHour,
                                  bool exceptCrossed)
    {
        std::cout << "- Crossed hour test:" << std::endl;
        std::cout << "  - from: " << from << std::endl;
        std::cout << "  - to:   " << to << std::endl;
        std::cout << "  - timeOfHour: " << timeOfHour << std::endl;
        const bool crossed = LLBC_Time::IsCrossedHour(from, to, timeOfHour);
        std::cout << "  - crossed: " << crossed << std::endl;
        if (crossed != exceptCrossed)
            std::cerr << "  - !!!!!!!!! Test failed, except:" << exceptCrossed << std::endl;
    };

    crossedHourTestLbda(LLBC_Time::FromTimeStr("2020-12-01 00:00:00"),
                        LLBC_Time::FromTimeStr("2020-12-03 00:00:00"),
                        LLBC_TimeSpan::zero,
                        true);
    crossedHourTestLbda(LLBC_Time::FromTimeStr("2020-12-01 00:00:00"),
                        LLBC_Time::FromTimeStr("2020-12-01 00:00:00"),
                        LLBC_TimeSpan::zero,
                        false);
    crossedHourTestLbda(LLBC_Time::FromTimeStr("2020-12-01 00:00:00"),
                        LLBC_Time::FromTimeStr("2020-12-01 00:40:00"),
                        LLBC_TimeSpan::FromMinutes(30),
                        true);
    crossedHourTestLbda(LLBC_Time::FromTimeStr("2020-12-01 00:00:00"),
                        LLBC_Time::FromTimeStr("2020-12-01 00:40:00"),
                        LLBC_TimeSpan::FromMinutes(-30), // => 30 minutes
                        true);
    crossedHourTestLbda(LLBC_Time::FromTimeStr("2020-12-01 00:50:00"),
                        LLBC_Time::FromTimeStr("2020-12-01 01:40:00"),
                        LLBC_TimeSpan::FromMinutes(30),
                        true);

    // IsCrossedDay() test:
    auto crossedDayTestLbda = [](const LLBC_Time &from,
                                 const LLBC_Time &to,
                                 const LLBC_TimeSpan &timeOfDay,
                                 bool exceptCrossed)
    {
        std::cout << "- Crossed day test:" << std::endl;
        std::cout << "  - from: " << from << std::endl;
        std::cout << "  - to:   " << to << std::endl;
        std::cout << "  - timeOfDay: " << timeOfDay << std::endl;
        const bool crossed = LLBC_Time::IsCrossedDay(from, to, timeOfDay);
        std::cout << "   - crossed: " << crossed << std::endl;
        if (crossed != exceptCrossed)
            std::cerr << "  - !!!!!!!!! Test failed, except:" << exceptCrossed << std::endl;
    };

    crossedDayTestLbda(LLBC_Time::FromTimeStr("2020-12-01 00:00:00"),
                       LLBC_Time::FromTimeStr("2020-12-01 00:00:00"),
                       LLBC_TimeSpan::zero,
                       false);
    crossedDayTestLbda(LLBC_Time::FromTimeStr("2020-12-01 00:00:00"),
                       LLBC_Time::FromTimeStr("2020-12-01 00:00:00"),
                       LLBC_TimeSpan::oneDay * 2,
                       false);
    crossedDayTestLbda(LLBC_Time::FromTimeStr("2020-12-02 00:00:00"),
                       LLBC_Time::FromTimeStr("2020-12-01 00:00:00"),
                       LLBC_TimeSpan::zero,
                       false);
    crossedDayTestLbda(LLBC_Time::FromTimeStr("2020-12-01 00:00:00"),
                       LLBC_Time::FromTimeStr("2020-12-01 01:00:00"),
                       LLBC_TimeSpan::zero,
                       false);
    crossedDayTestLbda(LLBC_Time::FromTimeStr("2020-12-01 01:00:00"),
                       LLBC_Time::FromTimeStr("2020-12-03 00:00:00"),
                       LLBC_TimeSpan::zero,
                       true);
    crossedDayTestLbda(LLBC_Time::FromTimeStr("2020-12-01 23:00:00"),
                       LLBC_Time::FromTimeStr("2020-12-02 00:00:00"),
                       LLBC_TimeSpan::zero,
                       true);
    crossedDayTestLbda(LLBC_Time::FromTimeStr("2020-12-01 23:00:00"),
                       LLBC_Time::FromTimeStr("2020-12-02 01:00:00"),
                       LLBC_TimeSpan::zero,
                       true);
    crossedDayTestLbda(LLBC_Time::FromTimeStr("2020-12-01 02:00:00"),
                       LLBC_Time::FromTimeStr("2020-12-02 01:00:00"),
                       LLBC_TimeSpan::oneHour,
                       true);

    // IsCrossedWeek() test:
    auto crossedWeekTestLbda = [](const LLBC_Time &from,
                                  const LLBC_Time &to,
                                  const LLBC_TimeSpan &timeOfWeek,
                                  bool exceptCrossed)
    {
        std::cout << "- Crossed week test:" << std::endl;
        std::cout << "  - from: " << from
            << "(" << LLBC_TimeConst::dayOfWeekDesc[from.GetDayOfWeek()] << ")" << std::endl;
        std::cout << "  - to:   " << to
            << "(" << LLBC_TimeConst::dayOfWeekDesc[to.GetDayOfWeek()] << ")" << std::endl;
        std::cout << "  - timeOfWeek: " << timeOfWeek
            << "(" << LLBC_TimeConst::GetDayOfWeekDesc(timeOfWeek.GetTotalDays()) << ")" << std::endl;
        const bool crossed = LLBC_Time::IsCrossedWeek(from, to, timeOfWeek);
        std::cout <<"   - crossed: " << crossed << std::endl;
        if (crossed != exceptCrossed)
            std::cerr << "  - !!!!!!!!! Test failed, except:" << exceptCrossed << std::endl;
    };

    crossedWeekTestLbda(LLBC_Time::FromTimeStr("2023-07-14 00:00:00"),
                        LLBC_Time::FromTimeStr("2023-07-14 00:00:00"),
                        LLBC_TimeSpan::zero,
                        false);
    crossedWeekTestLbda(LLBC_Time::FromTimeStr("2023-07-14 00:00:00"),
                        LLBC_Time::FromTimeStr("2023-07-15 00:00:00"),
                        LLBC_TimeSpan::negOneDay, // -1 days => 6 ays => Saturday
                        true);
    crossedWeekTestLbda(LLBC_Time::FromTimeStr("2023-07-14 00:00:00"),
                        LLBC_Time::FromTimeStr("2023-07-13 00:00:00"),
                        LLBC_TimeSpan::zero,
                        false);
    crossedWeekTestLbda(LLBC_Time::FromTimeStr("2023-07-14 00:00:00"),
                        LLBC_Time::FromTimeStr("2023-07-15 00:00:00"),
                        LLBC_TimeSpan::zero,
                        false);
    crossedWeekTestLbda(LLBC_Time::FromTimeStr("2023-07-14 00:00:00"),
                        LLBC_Time::FromTimeStr("2023-07-21 00:00:00"),
                        LLBC_TimeSpan::zero,
                        true);
    crossedWeekTestLbda(LLBC_Time::FromTimeStr("2023-07-14 00:00:00"),
                        LLBC_Time::FromTimeStr("2023-07-21 01:00:00"),
                        LLBC_TimeSpan::zero,
                        true);
    crossedWeekTestLbda(LLBC_Time::FromTimeStr("2023-07-14 00:00:00"),
                        LLBC_Time::FromTimeStr("2023-07-16 01:00:00"),
                        LLBC_TimeSpan::zero,
                        true);
    crossedWeekTestLbda(LLBC_Time::FromTimeStr("2023-07-16 00:00:00"),
                        LLBC_Time::FromTimeStr("2023-07-16 01:00:00"),
                        LLBC_TimeSpan::zero,
                        false);
    crossedWeekTestLbda(LLBC_Time::FromTimeStr("2023-07-16 01:00:00"),
                        LLBC_Time::FromTimeStr("2023-07-23 00:00:00"),
                        LLBC_TimeSpan::zero,
                        true);
    crossedWeekTestLbda(LLBC_Time::FromTimeStr("2023-07-16 01:00:00"),
                        LLBC_Time::FromTimeStr("2023-07-23 02:00:00"),
                        LLBC_TimeSpan::zero,
                        true);
}

void TestCase_Core_Time_Time::PrintTimeStruct(const LLBC_TimeStruct &ts)
{
    std::cout <<"\t tm_year: " <<ts.tm_year <<std::endl;
    std::cout <<"\t tm_mon : " <<ts.tm_mon  <<std::endl;
    std::cout <<"\t tm_yday: " <<ts.tm_yday <<std::endl;
    std::cout <<"\t tm_mday: " <<ts.tm_mday <<std::endl;
    std::cout <<"\t tm_wday: " <<ts.tm_wday <<std::endl;
    std::cout <<"\t tm_hour: " <<ts.tm_hour <<std::endl;
    std::cout <<"\t tm_min : " <<ts.tm_min  <<std::endl;
    std::cout <<"\t tm_sec : " <<ts.tm_sec  <<std::endl;
    std::cout <<"\t tm_gmtoff: " <<ts.tm_gmtoff <<std::endl;
}

void TestCase_Core_Time_Time::PrintCLibTimeStruct(const struct tm &ts)
{
    std::cout <<"\t tm_year: " <<ts.tm_year <<std::endl;
    std::cout <<"\t tm_mon : " <<ts.tm_mon  <<std::endl;
    std::cout <<"\t tm_yday: " <<ts.tm_yday <<std::endl;
    std::cout <<"\t tm_mday: " <<ts.tm_mday <<std::endl;
    std::cout <<"\t tm_wday: " <<ts.tm_wday <<std::endl;
    std::cout <<"\t tm_hour: " <<ts.tm_hour <<std::endl;
    std::cout <<"\t tm_min : " <<ts.tm_min  <<std::endl;
    std::cout <<"\t tm_sec : " <<ts.tm_sec  <<std::endl;
#if LLBC_TARGET_PLATFORM_NON_WIN32
    std::cout <<"\t tm_gmtoff: " <<ts.tm_gmtoff <<std::endl;
#endif
}
