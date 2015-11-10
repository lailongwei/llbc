/**
 * @file    TestCase_Core_Utils_Misc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_UTILS_MISC_H__
#define __LLBC_TEST_CASE_CORE_UTILS_MISC_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Utils_Misc : public LLBC_BaseTestCase
{
public:
    virtual int Run(int argc, char *argv[]);

private:
    void TestGuardFunc();
    void TestGuardMeth();
};

#endif // !__LLBC_TEST_CASE_CORE_UTILS_MISC_H__
