/**
 * @file    TestCase_Comm_Event.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/07/21
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COMM_EVENT_H__
#define __LLBC_TEST_CASE_COMM_EVENT_H__ 

#include "llbc.h"
using namespace llbc;

class TestCase_Comm_Event : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_Event();
    virtual ~TestCase_Comm_Event();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_COMM_EVENT_H__
