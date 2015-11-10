/**
 * @file    TestCase_Core_Log.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/12
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_LOG_H__
#define __LLBC_TEST_CASE_CORE_LOG_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Log : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Log();
    virtual ~TestCase_Core_Log();

public:
    int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_CORE_LOG_H__
