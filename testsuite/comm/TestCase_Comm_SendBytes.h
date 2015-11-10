/**
 * @file    TestCase_Comm_SendBytes.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/06
 * @version 1.0
 *
 * @brief   The llbc library Service Send(void *bytes) method test case.
 */
# ifndef __LLBC_TEST_CASE_COMM_SEND_BYTES_H__
# define __LLBC_TEST_CASE_COMM_SEND_BYTES_H__

# include "llbc.h"
using namespace llbc;

class TestCase_Comm_SendBytes : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_SendBytes();
    virtual ~TestCase_Comm_SendBytes();

public:
    virtual int Run(int argc, char *argv[]);

private:
    void FetchArgs(int argc, char *argv[]);

private:
    bool _asClient;
    LLBC_IService::Type _svcType;

    LLBC_String _runIp;
    int _runPort;
};

#endif // !__LLBC_TEST_CASE_COMM_SEND_BYTES_H__
