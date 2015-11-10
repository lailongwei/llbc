/**
 * @file    TestCase_Comm_Multicast.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/06
 * @version 1.0
 *
 * @brief   The llbc library Service Multicast() methods testcase.
 */
#ifndef __LLBC_TEST_CASE_COMM_MULTICAST_H__
#define __LLBC_TEST_CASE_COMM_MULTICAST_H__

# include "llbc.h"
using namespace llbc;

class TestCase_Comm_Multicast : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_Multicast();
    virtual ~TestCase_Comm_Multicast();

public:
    virtual int Run(int argc, char *argv[]);

private:
    void FetchArgs(int argc, char *argv[]);

    int PrepareClientLogic(LLBC_IService *svc);
    int PrepareServerLogic(LLBC_IService *svc);

private:
    bool _asClient;
    LLBC_IService::Type _svcType;

    LLBC_String _runIp;
    int _runPort;

    bool _useBst;

    static const int _clientCnt;
};

#endif // !__LLBC_TEST_CASE_COMM_MULTICAST_H__
