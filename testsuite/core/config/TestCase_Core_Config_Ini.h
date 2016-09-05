/**
 * @file    TestCase_Core_Config_Ini.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/09/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_CONFIG_INI_H__
#define __LLBC_TEST_CASE_CORE_CONFIG_INI_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Config_Ini : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Config_Ini();
    virtual ~TestCase_Core_Config_Ini();

public:
    virtual int Run(int argc, char *argv[]);
};

// TODO: Will continue

#endif // !__LLBC_TEST_CASE_CORE_CONFIG_INI_H__
