/**
 * @file    TestCase_Core_Library.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/10/01
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_LIBRARY_H__
#define __LLBC_TEST_CASE_CORE_LIBRARY_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Library : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Library();
    virtual ~TestCase_Core_Library();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_CORE_LIBRARY_H__
