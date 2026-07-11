// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "core/utils/TestCase_Core_Utils_Network.h"

TestCase_Core_Utils_Network::TestCase_Core_Utils_Network()
{
}

TestCase_Core_Utils_Network::~TestCase_Core_Utils_Network()
{
}

int TestCase_Core_Utils_Network::Run(int argc, char *argv[])
{
    LLBC_PrintLn("llbc library core/utils/Util_Network test:");

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
    LLBC_PrintLn("%s is IPv4 address format? %s", 
            addr.c_str(), LLBC_IsIPv4Addr(addr) ? "true" : "false");
}

