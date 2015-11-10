/**
 * @file    TestCase_Core_Utils_Network.h
 * @author  Longwei Lai
 * @date    2014/12/19
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_CORE_UTILS_NETWORK_H__
#define __LLBC_TEST_CASE_CORE_UTILS_NETWORK_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Utils_Network : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Utils_Network();
    virtual ~TestCase_Core_Utils_Network();

public:
    virtual int Run(int argc, char *argv[]);

private:
    void VerifyIPv4Addr(const LLBC_String &addr);
};

#endif // !__LLBC_TEST_CASE_CORE_UTILS_NETWORK_H__

