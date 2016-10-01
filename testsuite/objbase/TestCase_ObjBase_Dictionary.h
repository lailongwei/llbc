/**
 * @file    TestCase_ObjBase_Dictionary.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2104/01/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_OBJBASE_DICTIONARY_H__
#define __LLBC_TEST_CASE_OBJBASE_DICTIONARY_H__

#include "llbc.h"
using namespace llbc;

#if LLBC_CFG_OBJBASE_ENABLED

class TestCase_ObjBase_Dictionary : public LLBC_BaseTestCase
{
public:
    TestCase_ObjBase_Dictionary();
    virtual ~TestCase_ObjBase_Dictionary();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // LLBC_CFG_OBJBASE_ENABLED

#endif // !__LLBC_TEST_CASE_OBJBASE_DICTIONARY_H__
