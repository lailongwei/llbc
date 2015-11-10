/**
 * @file    TestCase_Core_Config_Property.h
 * @author  Longwei Lai
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_CONFIG_PROPERY_H__
#define __LLBC_TEST_CASE_CORE_CONFIG_PROPERY_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Config_Property : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Config_Property();
    ~TestCase_Core_Config_Property();

public:
    virtual int Run(int argc, char *argv[]);

private:
    LLBC_String _file;
};

#endif // !__LLBC_TEST_CASE_CORE_CONFIG_PROPERY_H__
