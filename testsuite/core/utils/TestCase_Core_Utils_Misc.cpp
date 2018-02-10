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


#include "core/utils/TestCase_Core_Utils_Misc.h"

namespace
{
    void GuardFunc(void *data)
    {
        std::cout <<"Test Guard Function Called!" <<std::endl;
        std::cout <<"  The data argument: " <<data <<std::endl;
    }

    class GuardCls
    {
    public:
        void GuardMeth(void *data)
        {
            std::cout <<"Test Guard Method Called!" <<std::endl;
            std::cout <<"  The data argument: " <<data <<std::endl;
        }
    };
}

int TestCase_Core_Utils_Misc::Run(int argc, char *argv[])
{
    std::cout <<"core/utils/Util_Misc test:" <<std::endl;
    TestGuardFunc();
    TestGuardMeth();

    std::cout <<"Press any key to continue...";
    getchar();

    return LLBC_OK;
}

void TestCase_Core_Utils_Misc::TestGuardFunc()
{
    std::cout <<"Guard Function test:" <<std::endl;
    LLBC_InvokeGuard guard(&GuardFunc, (void *)0x01);
}

void TestCase_Core_Utils_Misc::TestGuardMeth()
{
    std::cout <<"Guard Method test:" <<std::endl;
    GuardCls obj;
    {
        LLBC_InvokeGuard guard(&obj, &GuardCls::GuardMeth, (void *)0x02);
    }
}
