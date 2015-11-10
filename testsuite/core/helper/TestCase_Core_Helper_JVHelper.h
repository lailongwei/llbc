/**
 * @file    TestCase_Core_Helper_JVHelper.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/05/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_HELPER_JV_HELPER_H__
#define __LLBC_TEST_CASE_CORE_HELPER_JV_HELPER_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Helper_JVHelper : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Helper_JVHelper();
    virtual ~TestCase_Core_Helper_JVHelper();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_CORE_HELPER_JV_HELPER_H__
