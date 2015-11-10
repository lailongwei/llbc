/**
 * @file    TestCase_Core_OS_Directory.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/24
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_OS_DIRECTORY_H__
#define __LLBC_TEST_CASE_CORE_OS_DIRECTORY_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_OS_Directory : public LLBC_BaseTestCase
{
public:
    TestCase_Core_OS_Directory();
    virtual ~TestCase_Core_OS_Directory();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_CORE_OS_DIRECTORY_H__

