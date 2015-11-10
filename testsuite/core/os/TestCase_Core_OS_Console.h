/**
 * @file    TestCase_Core_OS_Console.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_OS_CONSOLE_H__
#define __LLBC_TEST_CASE_CORE_OS_CONSOLE_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_OS_Console : public LLBC_BaseTestCase
{
public:
    TestCase_Core_OS_Console();
    virtual ~TestCase_Core_OS_Console();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_CORE_OS_CONSOLE_H__

