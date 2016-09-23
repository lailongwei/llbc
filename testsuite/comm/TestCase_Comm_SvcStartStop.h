/**
 * @file    TestCase_Comm_SvcStartStop.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/09/23
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COMM_SVC_START_STOP_H__
#define __LLBC_TEST_CASE_COMM_SVC_START_STOP_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Comm_SvcStartStop : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_SvcStartStop();
    virtual ~TestCase_Comm_SvcStartStop();

public:
    virtual int Run(int argc, char *artv[]);
};

#endif // !__LLBC_TEST_CASE_COMM_SVC_START_STOP_H__

