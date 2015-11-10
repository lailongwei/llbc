/**
 * @file    TestCase_Core_Utils_Delegate.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_UTILS_DELEGATE_H__
#define __LLBC_TEST_CASE_CORE_UTILS_DELEGATE_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Utils_Delegate :public LLBC_BaseTestCase
{
public:
    TestCase_Core_Utils_Delegate();
    virtual ~TestCase_Core_Utils_Delegate();

public:
    virtual int Run(int argc, char *argv[]);

private:
    void Foo_0();

    void Foo_1(int arg1);

    void Foo_2(int arg1, int arg2);

    void Foo_3(int arg1, int arg2, int arg3);

    void Foo_4(int arg1, int arg2, int arg3, int arg4);

    void Foo_5(int arg1, int arg2, int arg3, int arg4, int arg5);

    void Foo_6(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);

    void Foo_7(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7);

private:
    void *FooEx(const LLBC_NS LLBC_String &arg);
};

#endif // !__LLBC_TEST_CASE_CORE_UTILS_DELEGATE_H__
