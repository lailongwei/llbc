/**
 * @file    TestCase_ObjBase_Array.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_OBJBASE_ARRAY_H__
#define __LLBC_TEST_CASE_OBJBASE_ARRAY_H__

#include "llbc.h"
using namespace llbc;

class TestCase_ObjBase_Array : public LLBC_BaseTestCase
{
public:
    TestCase_ObjBase_Array();
    virtual ~TestCase_ObjBase_Array();

public:
    virtual int Run(int argc ,char *argv[]);

private:
    void BasicTest();

    void IterTest();

    void SortTest();
};

#endif // !__LLBC_TEST_CASE_OBJBASE_ARRAY_H__
