/**
 * @file    TestCase_Comm_ReleasePool.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/31
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COMM_RELEASE_POOL_H__
#define __LLBC_TEST_CASE_COMM_RELEASE_POOL_H__

#include "llbc.h"
using namespace llbc;

#if LLBC_CFG_OBJBASE_ENABLED

class TestCase_Comm_ReleasePool : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_ReleasePool();
    virtual ~TestCase_Comm_ReleasePool();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // LLBC_CFG_OBJBASE_ENABLED

#endif // !__LLBC_TEST_CASE_COMM_RELEASE_POOL_H__
