/**
 * @file    TestCase_Comm_Timer.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/12/04
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COMM_TIMER_H__
#define __LLBC_TEST_CASE_COMM_TIMER_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Comm_Timer : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_Timer();
    virtual ~TestCase_Comm_Timer();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_COMM_TIMER_H__
