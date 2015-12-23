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

    time_t nowTime = time(NULL);
    std::cout <<"now time in seconds: " <<nowTime <<std::endl;

    LLBC_TimeStruct ts;
    LLBC_LocalTime(nowTime, ts);
    std::cout <<"Call LLBC_LocalTime(), return value: " <<std::endl;
    PrintTimeStruct(ts);

    time_t tmp = LLBC_MkTime(ts);
    std::cout <<"Convert to UTC time: " <<tmp <<std::endl;

    LLBC_GMTime(tmp, ts);
    std::cout <<"Call LLBC_GMTime(), return value: " <<std::endl;
    PrintTimeStruct(ts);

    tmp = LLBC_MkTime(ts);
    std::cout <<"Convert to UTC time: " <<tmp <<std::endl;

    sint64 nowTimeInMs = LLBC_GetMilliSeconds();
    std::cout <<"now time in milli seconds: "<<nowTimeInMs <<std::endl;
}

void TestCase_Core_Time_Time::TimeClassTest()
{
    std::cout <<"LLBC_Time/LLBC_TimeSpan test: " <<std::endl;

    LLBC_Time now;
    std::cout <<"now time(UTC): " <<now.FormatGmt() <<std::endl;
    std::cout <<"now time(LOCAL): " <<now.Format() <<std::endl;

    LLBC_TimeSpan span(20);
    now -= span;
    std::cout <<"now -= span: " <<now.Format() <<std::endl;
    now += span;
    std::cout <<"now += span: " <<now.Format() <<std::endl;

    LLBC_Time now2;
    LLBC_TimeSpan diff = now2 - now;
    std::cout <<"now2 - now: " <<diff.GetTotalSeconds() <<std::endl;
}

void TestCase_Core_Time_Time::PrintTimeStruct(const LLBC_NS LLBC_TimeStruct &ts)
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
