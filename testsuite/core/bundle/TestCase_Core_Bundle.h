/**
 * @file    TestCase_Core_Bundle.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/25
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_BUNDLE_H__
#define __LLBC_TEST_CASE_CORE_BUNDLE_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Bundle : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Bundle();
    virtual ~TestCase_Core_Bundle();
    
public:
    int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_CORE_BUNDLE_H__
