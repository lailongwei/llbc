/**
 * @file    TestCase_Com_Compiler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/26
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COM_COMPILER_H__
#define __LLBC_TEST_CASE_COM_COMPILER_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Com_Compiler : public LLBC_BaseTestCase
{
public:
    TestCase_Com_Compiler();
    virtual ~TestCase_Com_Compiler();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_COM_COMPILER_H__

