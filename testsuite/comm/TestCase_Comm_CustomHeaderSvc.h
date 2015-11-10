/**
 * @file    TestCase_Comm_CustomHeaderSvc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/10/01
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COMM_CUSTOM_HEADER_SVC_H__
#define __LLBC_TEST_CASE_COMM_CUSTOM_HEADER_SVC_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Comm_CustomHeaderSvc : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_CustomHeaderSvc();
    virtual ~TestCase_Comm_CustomHeaderSvc();

public:
    virtual int Run(int argc, char *argv[]);

private:
    bool ParseArgs(int argc, char *argv[]);

    void DesignHeader();
    bool StartUpService();

private:
    bool _asClient;

    LLBC_String _ip;
    int _port;

    LLBC_IService *_svc;
};

#endif // !__LLBC_TEST_CASE_COMM_CUSTOM_HEADER_SVC_H__
