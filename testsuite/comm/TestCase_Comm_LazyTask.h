/**
 * @file    TestCase_Comm_LazyTask.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/03/16
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COMM_LAZY_TASK_H__
#define __LLBC_TEST_CASE_COMM_LAZY_TASK_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Comm_LazyTask : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_LazyTask();
    virtual ~TestCase_Comm_LazyTask();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_COMM_LAZY_TASK_H__
