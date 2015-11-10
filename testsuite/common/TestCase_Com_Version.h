/**
 * @file    TestCase_Com_Version.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/18
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COM_VERSION_H__
#define __LLBC_TEST_CASE_COM_VERSION_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Com_Version : public LLBC_BaseTestCase
{
public:
    TestCase_Com_Version();
    virtual ~TestCase_Com_Version();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_COM_VERSION_H__
