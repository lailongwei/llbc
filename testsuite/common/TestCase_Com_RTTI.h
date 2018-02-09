/**
 * @file    TestCase_Com_RTTI.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2018/02/09
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COM_RTTI_H__
#define __LLBC_TEST_CASE_COM_RTTI_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Com_RTTI : public LLBC_BaseTestCase
{
public:
    TestCase_Com_RTTI();
    virtual ~TestCase_Com_RTTI();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_COM_RTTI_H__