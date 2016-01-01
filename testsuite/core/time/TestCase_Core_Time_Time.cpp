/**
 * @file    TestCase_Core_Time_Time.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/05
 * @version 1.0
 *
 * @brief
 */

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

    this->OSTimeTest();
    std::cout <<std::endl;

    this->TimeClassTest();
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
    LLBC_GetTimeOfDay(&tv, NULL);
    std::cout <<"LLBC_GetTimeOfDay(), tv_sec: " <<tv.tv_sec <<", tv_usec: " <<tv.tv_usec <<std::endl;
#if LLBC_TARGET_PLATFORM_WIN32 && LLBC_CFG_OS_IMPL_GETTIMEOFDAY
    gettimeofday(&tv, NULL);
    std::cout <<"WIN32 spec: gettimeofday(), tv_sec: " <<tv.tv_sec <<", tv_usec: " <<tv.tv_usec <<std::endl;
#endif
}

void TestCase_Core_Time_Time::TimeClassTest()
{
    std::cout <<"LLBC_Time test: " <<std::endl;

    // Default time test.
    LLBC_Time defaultTime;
    std::cout <<"Default time: " <<defaultTime <<std::endl;

    // Now(), GetTimeTicks(), Format(), FormatAsGmt() test.
    LLBC_Time now = LLBC_Time::Now();
    std::cout <<"now time: " <<now <<", Time Ticks: " <<now.GetTimeTicks() <<std::endl;
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
    for (int i = 1; i <= 4; i++)
        std::cout <<i <<": " <<now.AddYears(i) <<", -" <<i <<": " <<now.AddYears(-i) <<"\n";
    std::cout <<std::endl;

    std::cout <<"AddMonths() test:" <<std::endl;
    for (int i = 1; i <= 13; i++)
        std::cout <<i <<": " <<now.AddMonths(i) <<", -" <<i <<": " <<now.AddMonths(-i) <<"\n";
    std::cout <<std::endl;

    std::cout <<"AddDays() test:" <<std::endl;
    for (int i = 1; i <= 32; i++)
        std::cout <<i <<": " <<now.AddDays(i) <<", -" <<i <<": " <<now.AddDays(-i) <<"\n";
    std::cout <<std::endl;

    std::cout <<"AddHours() test:" <<std::endl;
    for (int i = 1; i <= 62; i++)
        std::cout <<i <<": " <<now.AddHours(i) <<", -" <<i <<": " <<now.AddMinutes(-i) <<"\n";
    std::cout <<std::endl;

    std::cout <<"AddMinutes() test:" <<std::endl;
    for (int i = 1; i <= 62; i++)
        std::cout <<i <<": " <<now.AddMinutes(i) <<", -" <<i <<": " <<now.AddMinutes(-i) <<"\n";
    std::cout <<std::endl;

    std::cout <<"AddSeconds() test:" <<std::endl;
    for (int i = 1; i <= 62; i++)
        std::cout <<i <<": " <<now.AddSeconds(i) <<", -" <<i <<": " <<now.AddSeconds(-i) <<"\n";
    std::cout <<std::endl;

    std::cout <<"AddMilliSeconds() test:" <<std::endl;
    for (int i = 1; i <= 3; i++)
        std::cout <<i <<": " <<now.AddMilliSeconds(i * 500).GetTimeTicks() <<", -" 
        <<i <<": " <<now.AddMilliSeconds(-i * 500).GetTimeTicks() <<"\n";
    std::cout <<std::endl;

    std::cout <<"AddMicroSeconds() test:" <<std::endl;
    for (int i = 1; i <= 3; i++)
        std::cout <<i <<": " <<now.AddMicroSeconds(i * 500).GetTimeTicks() <<", -" 
        <<i <<": " <<now.AddMilliSeconds(-i * 500).GetTimeTicks() <<"\n";
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
    for (int i = 0; i <= 13; i++)
        std::cout <<"2012-" <<i <<": max days: " <<LLBC_Time::GetMonthMaxDays(2012, i) <<std::endl;
    for (int i = 0; i <= 13; i++)
        std::cout <<"2015-" <<i <<": max days: " <<LLBC_Time::GetMonthMaxDays(2015, i) <<std::endl;

    // FromXXX() test.
    std::cout <<"FromSeconds(1.0): " <<LLBC_Time::FromSeconds(1.0) <<std::endl;
    std::cout <<"FromMilliSeconds(10001): " <<LLBC_Time::FromMilliSeconds(10001) <<std::endl;
    std::cout <<"FromMicroSeconds(10000000): " <<LLBC_Time::FromMicroSeconds(10000000) <<std::endl;
    struct timeval tv;
    tv.tv_sec = 10, tv.tv_usec = 10;
    std::cout <<"FromTimeVal(tv.sec=10, tv.usec=10): " <<LLBC_Time::FromTimeVal(tv) <<std::endl;

    std::string timeRepr = "2000-12-13 19:21:35.333435";
    LLBC_Time fromTimeRepr = LLBC_Time::FromTimeRepr(timeRepr);
    std::cout <<"FromTimeRepr(" <<timeRepr <<"): " <<fromTimeRepr <<std::endl;
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
    std::cout <<"GetInterval To 3600 seconds: " <<now.GetIntervalTo(3600) <<std::endl;
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