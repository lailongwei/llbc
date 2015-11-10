/**
 * @file    TestCase_Core_Utils_MD5_H__
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/12
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_UTILS_MD5_H__
#define __LLBC_TEST_CASE_CORE_UTILS_MD5_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Utils_MD5 : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Utils_MD5();
    virtual ~TestCase_Core_Utils_MD5();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_CORE_UTILS_MD5_H__
