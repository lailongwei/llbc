/**
 * @file    TestCase_Core_OS_Thread.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/01
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_OS_THREAD_H__
#define __LLBC_TEST_CASE_CORE_OS_THREAD_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_OS_Thread : public LLBC_BaseTestCase
{
public:
    TestCase_Core_OS_Thread();
    virtual ~TestCase_Core_OS_Thread();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_CORE_OS_THREAD_H__
