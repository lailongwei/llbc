/**
 * @file    TestCase_Com_Error.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/21
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COM_ERROR_H__
#define __LLBC_TEST_CASE_COM_ERROR_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Com_Error : public LLBC_BaseTestCase
{
public:
    TestCase_Com_Error();
    virtual ~TestCase_Com_Error();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_COM_ERROR_H__
