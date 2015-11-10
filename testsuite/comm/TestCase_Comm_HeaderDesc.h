/**
 * @file    TestCase_Comm_HeaderDesc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/31
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COMM_HEADER_DESC_H__
#define __LLBC_TEST_CASE_COMM_HEADER_DESC_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Comm_HeaderDesc : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_HeaderDesc();
    virtual ~TestCase_Comm_HeaderDesc();

public:
    virtual int Run(int argc, char *argv[]);

private:
    void HeaderPartDescTest();
    void HeaderDescTest();
};

#endif // !__LLBC_TEST_CASE_COMM_HEADER_DESC_H__
