/**
 * @file    TestCase_Core_Thread_Task.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/01
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_THREAD_TASK_H__
#define __LLBC_TEST_CASE_CORE_THREAD_TASK_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Thread_Task : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Thread_Task();
    virtual ~TestCase_Core_Thread_Task();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_CORE_THREAD_TASK_H__
