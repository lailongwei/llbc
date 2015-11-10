/**
 * @file    TestCase_Core_VariantTest.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/20
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CORE_VARIANT_TEST_H__
#define __LLBC_TEST_CORE_VARIANT_TEST_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_VariantTest : public LLBC_BaseTestCase
{
public:
    virtual int Run(int argc, char *argv[]);

private:
    void BasicTest();
    void ArithmeticTest();

    void DictTtest();
};

#endif // !__LLBC_TEST_CORE_VARIANT_TEST_H__
