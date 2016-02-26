/**
 * @file    TestCase_Core_Utils_Network.cpp
 * @author  Longwei Lai
 * @date    2014/12/19
 * @version 1.0
 *
 * @biref
 */

#include "core/utils/TestCase_Core_Utils_Network.h"

TestCase_Core_Utils_Network::TestCase_Core_Utils_Network()
{
}

TestCase_Core_Utils_Network::~TestCase_Core_Utils_Network()
{
}

int TestCase_Core_Utils_Network::Run(int argc, char *argv[])
{
    LLBC_PrintLine("llbc library core/utils/Util_Network test:");

    VerifyIPv4Addr("www.baidu.com");
    VerifyIPv4Addr("127.0.0.1");
    VerifyIPv4Addr("0.0.0.0");
    VerifyIPv4Addr("255.255..");
    VerifyIPv4Addr("10.1.1.1.222");
    VerifyIPv4Addr("a.b.1.3");

    LLBC_Print("Press any key to continue...");
    getchar();

    return LLBC_OK;
}

void TestCase_Core_Utils_Network::VerifyIPv4Addr(const LLBC_String &addr)
{
    LLBC_PrintLine("%s is IPv4 address format? %s", 
            addr.c_str(), LLBC_IsIPv4Addr(addr) ? "true" : "false");
}

