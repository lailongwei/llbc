/**
 * @file    TestCase_Core_Thread_Tls.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/10
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_THREAD_TLS_H__
#define __LLBC_TEST_CASE_CORE_THREAD_TLS_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Thread_Tls : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Thread_Tls();
    virtual ~TestCase_Core_Thread_Tls();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // __LLBC_TEST_CASE_CORE_THREAD_TLS_H__
