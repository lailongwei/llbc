/**
 * @file    TestCase_Core_Random.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/12
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_RANDOM_H__
#define __LLBC_TEST_CASE_CORE_RANDOM_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Random : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Random();
    virtual ~TestCase_Core_Random();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_CORE_RANDOM_H__
