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

#include "core/thread/TestCase_Core_Thread_Guard.h"

namespace
{
    void GuardFunc(void *data)
    {
        std::cout <<"Test Guard Function Called!" <<std::endl;
        std::cout <<"  The data argument: " <<data <<std::endl;
    }

    void GuardFuncArg3(void *data, int a1, int a2)
    {
        std::cout << "Test Guard 3 Args Function Called!" << std::endl;
        fprintf(stdout, "  The data argument: %p, %d, %d\n", data, a1, a2);
    }

    class GuardCls
    {
    public:
        void GuardMeth(void *data)
        {
            std::cout <<"Test Guard Method Called!" <<std::endl;
            std::cout <<"  The data argument: " <<data <<std::endl;
        }

        void GuardFuncArg3(void *data, int a1, int a2) 
        {
            std::cout << "Test Guard 3 Args Function Called!" << std::endl;
            fprintf(stdout, "  The data argument: %p, %d, %d\n", data, a1, a2);
        }
    };
}

TestCase_Core_Thread_Guard::TestCase_Core_Thread_Guard()
{
}

TestCase_Core_Thread_Guard::~TestCase_Core_Thread_Guard()
{
}

int TestCase_Core_Thread_Guard::Run(int argc, char *argv[])
{
    std::cout <<"core/thread/guard test:" <<std::endl;
    TestGuardFunc();
    TestGuardMeth();
    TestGuardVariableArgument();
    std::cout <<"Press any key to continue...";
    getchar();

    return 0;
}

void TestCase_Core_Thread_Guard::TestGuardFunc()
{
    std::cout <<"Guard Function test:" <<std::endl;
    LLBC_InvokeGuard guard(&GuardFunc, (void *)0x01);
}

void TestCase_Core_Thread_Guard::TestGuardMeth()
{
    std::cout <<"Guard Method test:" <<std::endl;
    GuardCls obj;
    {
        LLBC_InvokeGuard guard(&obj, &GuardCls::GuardMeth, (void *)0x02);
    }
}

void TestCase_Core_Thread_Guard::TestGuardVariableArgument() 
{
    std::cout << "Guard Method test:" << std::endl;
    { 
        LLBC_InvokeGuard guard(&GuardFuncArg3, (void *)0x01, 1, 2);
    }

    GuardCls obj;
    { 
        LLBC_InvokeGuard guard(&obj, &GuardCls::GuardFuncArg3, (void *) 0x02, 3, 4);
    }
}
