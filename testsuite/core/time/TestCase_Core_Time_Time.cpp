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
    std::cout <<"core/time/time test: " <<std::endl;

    OSTimeTest();
    std::cout <<std::endl;

    TimeClassTest();
    std::cout <<std::endl;

    TimeSpanClassTest();
    std::cout <<std::endl;

    CpuTimeTest();
    std::cout <<std::endl;

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    return 0;
}

void TestCase_Core_Time_Time::OSTimeTest()
{
    std::cout <<"OS_Time part test: " <<std::endl;

    std::cout <<"Now timeoffset: " <<LLBC_GetTimezone() <<std::endl;
    sint64 nowTimeInMs = LLBC_GetMilliSeconds();
    std::cout <<"LLBC_GetMilliSeconds(): "<<nowTimeInMs <<std::endl;
    sint64 nowTimeInUs = LLBC_GetMicroSeconds();
    std::cout <<"LLBC_GetMicroSeconds(): " <<nowTimeInUs <<std::endl;

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

    // Default time test.
    LLBC_Time defaultTime;
    std::cout <<"Default time: " <<defaultTime <<std::endl;

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
        <<", month:" <<now.GetMonth() <<", day:" <<now.GetDay() <<std::endl;
    std::cout <<"Now hour:" <<now.GetHour() 
        <<", minute:" <<now.GetMinute() <<", second:" <<now.GetSecond() <<std::endl;
    std::cout <<"Now millisecond: " <<now.GetMilliSecond() 
        <<", microsecond: " <<now.GetMicroSecond() <<std::endl;

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

    int addMilliSecsStep = 500;
    std::cout <<"AddMilliSeconds() test(step:+/- " <<addMilliSecsStep <<" milli-seconds):" <<std::endl;
    for (int i = 1; i <= 3; ++i)
    {
        int addMilliSecs = addMilliSecsStep * i;
        std::cout << i <<"(" <<addMilliSecs <<"): " <<now.AddMilliSeconds(addMilliSecs).GetTimestampInMicros() <<", -"
            <<i <<"(" <<-addMilliSecs <<"): " <<now.AddMilliSeconds(-addMilliSecs).GetTimestampInMicros() <<"\n";
    }
    std::cout <<std::endl;

    int addMicroSecsStep = 500;
    std::cout <<"AddMicroSeconds() test(step:+/- " <<addMicroSecsStep <<" micro-seconds):" <<std::endl;
    for (int i = 1; i <= 3; ++i)
    {
        int addMicroSecs = addMicroSecsStep * i;
        std::cout <<i <<"(" <<addMicroSecs <<"): " <<now.AddMicroSeconds(addMicroSecs).GetTimestampInMicros() <<", -"
            <<i <<"(" <<-addMicroSecs <<"): " <<now.AddMicroSeconds(-addMicroSecs).GetTimestampInMicros() <<"\n";
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

    std::string timeStr = "2000-12-13 19:21:35.333435";
    LLBC_Time fromTimeRepr = LLBC_Time::FromTimeStr(timeStr);
    std::cout <<"FromTimeStr(" <<timeStr <<"): " <<fromTimeRepr <<std::endl;
    std::cout <<"    millisec: " <<fromTimeRepr.GetMilliSecond() <<", microsec: " <<fromTimeRepr.GetMicroSecond() <<std::endl;

    timeStr = "12-13 21:35";
    fromTimeRepr = LLBC_Time::FromTimeStr(timeStr);
    std::cout <<"FromTimeStr(" <<timeStr <<"): " <<fromTimeRepr <<std::endl;
    std::cout <<"    millisec: " <<fromTimeRepr.GetMilliSecond() <<", microsec: " <<fromTimeRepr.GetMicroSecond() <<std::endl;

    timeStr = "12-13";
    fromTimeRepr = LLBC_Time::FromTimeStr(timeStr);
    std::cout <<"FromTimeStr(" <<timeStr <<"): " <<fromTimeRepr <<std::endl;
    std::cout <<"    millisec: " <<fromTimeRepr.GetMilliSecond() <<", microsec: " <<fromTimeRepr.GetMicroSecond() <<std::endl;

    timeStr = "1970-1-1 0:0:0";
    fromTimeRepr = LLBC_Time::FromTimeStr(timeStr);
    std::cout <<"FromTimeStr(" <<timeStr <<"): " <<fromTimeRepr <<std::endl;
    std::cout <<"    millisec: " <<fromTimeRepr.GetMilliSecond() <<", microsec: " <<fromTimeRepr.GetMicroSecond() <<std::endl;

    timeStr = "07:21:35";
    fromTimeRepr = LLBC_Time::FromTimeStr(timeStr);
    std::cout <<"FromTimeStr(" <<timeStr <<"): " <<fromTimeRepr <<std::endl;
    std::cout <<"    millisec: " <<fromTimeRepr.GetMilliSecond() <<", microsec: " <<fromTimeRepr.GetMicroSecond() <<std::endl;

    timeStr = "13 35";
    fromTimeRepr = LLBC_Time::FromTimeStr(timeStr);
    std::cout <<"FromTimeStr(" <<timeStr <<"): " <<fromTimeRepr <<std::endl;
    std::cout <<"    millisec: " <<fromTimeRepr.GetMilliSecond() <<", microsec: " <<fromTimeRepr.GetMicroSecond() <<std::endl;

    int yearPart = 1971, monthPart = 12, dayPart = 21;
    int hourPart = 13, minutePart = 23, secondPart = 32;
    int milliPart = 322, microPart = 321;
    LLBC_Time fromParts = LLBC_Time::FromTimeParts(
        yearPart, monthPart, dayPart, hourPart, minutePart, secondPart, milliPart, microPart);
    std::cout <<"FromTimeParts(" <<"year=" <<yearPart <<", month=" <<monthPart <<", day=" <<dayPart
        <<", hour=" <<hourPart <<", minute=" <<minutePart <<", second=" <<secondPart
        <<", milliSecond=" <<milliPart <<", microSecond=" <<microPart <<"):"
        << fromParts <<std::endl;
    std::cout <<"    millisec: " <<fromParts.GetMilliSecond() <<", microsec: " <<fromParts.GetMicroSecond() <<std::endl;

    // GetIntervalTo test:
    std::cout <<"now: " <<now <<", GetInterval To 3600 seconds: " <<now.GetIntervalTo(LLBC_TimeSpan(3600)) <<std::endl;
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
}

void TestCase_Core_Time_Time::CpuTimeTest()
{
    std::cout << "Cpu time tsc test: " << std::endl;
    for(sint32 idx = 0; idx < 10; ++idx)
    {
        auto freq = LLBC_GetCpuCounterFrequency();
        std::cout << "Current idx:" << std::to_string(idx) << ", tsc frequency:" << std::to_string(freq) << std::endl;
    }
    std::cout << "Cpu time tsc end: \n" << std::endl;

    std::cout << "Cpu time stream out test: " << std::endl;
    std::cout << "Stream out current cpu time: " << LLBC_CPUTime::Current() << std::endl;
    std::cout << "Cpu time stream out end: " << std::endl;
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
