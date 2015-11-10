/**
 * @file    TestCase_ObjBase_Object.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/28
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_OBJBASE_OBJECT_H__
#define __LLBC_TEST_CASE_OBJBASE_OBJECT_H__

#include "llbc.h"
using namespace llbc;

class TestCase_ObjBase_Object : public LLBC_BaseTestCase
{
public:
    TestCase_ObjBase_Object();
    virtual ~TestCase_ObjBase_Object();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_OBJBASE_OBJECT_H__
