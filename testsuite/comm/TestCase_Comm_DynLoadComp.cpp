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


#include "comm/TestCase_Comm_DynLoadComp.h"

TestCase_Comm_DynLoadComp::TestCase_Comm_DynLoadComp()
: _svc(LLBC_Service::Create("DynLoadTestSvc"))
{
}

TestCase_Comm_DynLoadComp::~TestCase_Comm_DynLoadComp()
{
    delete _svc;
}

int TestCase_Comm_DynLoadComp::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Communication Service Dynamic Load Comp Test:");
    LLBC_PrintLn("Note: You must be build your comp library first!");

    LLBC_String libPath, compName;
    LLBC_Print("Please input your comp library path:");
    std::cin >> libPath;

    LLBC_Print("Please input your comp name:");
    std::cin >> compName;

    LLBC_PrintLn("Will register comp in service");
    int ret = _svc->AddComponent(libPath, compName);
    if (ret != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr, "Failed to register dynamic comp, error:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Register dynamic comp success, try start service");
    if ((ret = _svc->Start()) != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr, "Start service failed, error:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    LLBC_PrintLn("Try call comp method: Foo");
    LLBC_Component *comp = _svc->GetComponent("TestComp");
    LLBC_Variant arg(3);
    LLBC_Variant callRet(4);
    ret = comp->CallMethod("Foo", arg, callRet);
    if (ret != LLBC_OK)
    {
        LLBC_PrintLn("Call method Foo success, but return code not equal to 0, error(maybe incorrect):%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    else
    {
        LLBC_PrintLn("Call method Foo success, callRet:%s", callRet.ToString().c_str());
    }

    LLBC_PrintLn("Press any key to continue...");
    getchar();

    return 0;
}

