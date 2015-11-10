/**
 * @file    TestCase_Comm_ExternalDriveSvc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/12/02
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COMM_EXTERNAL_DRIVE_SVC_H__
#define __LLBC_TEST_CASE_COMM_EXTERNAL_DRIVE_SVC_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Comm_ExternalDriveSvc : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_ExternalDriveSvc();
    virtual ~TestCase_Comm_ExternalDriveSvc();

public:
    virtual int Run(int argc, char *argv[]);

private:
    LLBC_IService *_svc;
};

#endif // !__LLBC_TEST_CASE_COMM_EXTERNAL_DRIVE_SVC_H__
