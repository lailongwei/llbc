/**
 * @file    TestCase_Core_Config_Config.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/29
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_CONFIG_CONFIG_H__
#define __LLBC_TEST_CASE_CORE_CONFIG_CONFIG_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Config_Config : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Config_Config();
    virtual ~TestCase_Core_Config_Config();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_CORE_CONFIG_CONFIG_H__
