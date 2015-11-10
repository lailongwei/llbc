/**
 * @file    TestCase_Comm_SvcBase.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/26
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COMM_SVC_BASE_H__
#define __LLBC_TEST_CASE_COMM_SVC_BASE_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Comm_SvcBase : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_SvcBase();
    virtual ~TestCase_Comm_SvcBase();

public:
    virtual int Run(int argc, char *argv[]);

private:
    void ListenTest(const char *ip, uint16 port);
    void ConnectTest(const char *ip, uint16 port);
    void AsyncConnTest(const char *ip, uint16 port);
    void SendRecvTest(const char *ip, uint16 port);

private:
    LLBC_IService *_svc;
};

#endif // !__LLBC_TEST_CASE_COMM_SVC_BASE_H__

