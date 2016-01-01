/**
 * @file    TestCase_Core_Time_Time.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_TIME_TIME_H__
#define __LLBC_TEST_CASE_CORE_TIME_TIME_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Time_Time : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Time_Time();
    virtual ~TestCase_Core_Time_Time();

public:
    virtual int Run(int argc, char *argv[]);

private:
    void OSTimeTest();

    void TimeClassTest();

    void PrintTimeStruct(const LLBC_TimeStruct &ts);
    void PrintCLibTimeStruct(const struct tm &ts);
};

#endif // !__LLBC_TEST_CASE_CORE_TIME_TIME_H__
