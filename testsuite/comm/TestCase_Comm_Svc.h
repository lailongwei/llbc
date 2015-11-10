/**
 * @file    TestCase_Comm_Svc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COMM_SVC_H__
#define __LLBC_TEST_CASE_COMM_SVC_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Comm_Svc : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_Svc();
    virtual ~TestCase_Comm_Svc();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_COMM_SVC_H__
