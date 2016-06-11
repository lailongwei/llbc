/**
 * @file    TestCase_Comm_Facade.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/06/09
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COMM_FACADE_H__
#define __LLBC_TEST_CASE_COMM_FACADE_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Comm_Facade : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_Facade();
    virtual ~TestCase_Comm_Facade();

public:
    virtual int Run(int argc, char *argv[]);

private:
    int TestInInternalDriveService(const LLBC_String &host, const int port);
    int TestInExternalDriveService(const LLBC_String &host, const int port);
};

#endif // __LLBC_TEST_CASE_COMM_FACADE_H__
