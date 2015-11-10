/**
 * @file    TestCase_Core_Thread_ThreadMgr.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/20
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_THREAD_THREAD_MGR_H__
#define __LLBC_TEST_CASE_CORE_THREAD_THREAD_MGR_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Thread_ThreadMgr : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Thread_ThreadMgr();
    virtual ~TestCase_Core_Thread_ThreadMgr();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_CORE_THREAD_THREAD_MGR_H__
