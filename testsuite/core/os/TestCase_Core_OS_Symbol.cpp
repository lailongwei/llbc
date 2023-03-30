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


#include "core/os/TestCase_Core_OS_Symbol.h"

TestCase_Core_OS_Symbol::TestCase_Core_OS_Symbol()
{
}

TestCase_Core_OS_Symbol::~TestCase_Core_OS_Symbol()
{
}

int TestCase_Core_OS_Symbol::Run(int argc, char *argv[])
{
    std::cout <<"core/os/symbol test: " <<std::endl;

    LLBC_PrintLn("Call functions: Foo1->Foo2->Foo3->Goo1->Goo2->Goo3");
    Foo1();

    fprintf(stdout, "Press any key to continue ... ...\n");
    getchar();

    return 0;
}

void TestCase_Core_OS_Symbol::Foo1()
{
    Foo2();
}

void TestCase_Core_OS_Symbol::Foo2()
{
    Foo3();
}

void TestCase_Core_OS_Symbol::Foo3()
{
    Goo1();
}

void TestCase_Core_OS_Symbol::Goo1()
{
    Goo2();
}

void TestCase_Core_OS_Symbol::Goo2()
{
    int a = 3;
    int b = 4;
    a += b;
    LLBC_Print("a:%d, b:%d\n", a, b);
    Goo3();
}

void TestCase_Core_OS_Symbol::Goo3()
{
    PrintBackTrace();
}

void TestCase_Core_OS_Symbol::PrintBackTrace()
{
    LLBC_CPUTime begTime = LLBC_CPUTime::Current();

    #if LLBC_CFG_OS_IMPL_SYMBOL
    LLBC_String backTrace = LLBC_CaptureStackBackTrace(1);
    LLBC_Print("Stack BackTrace:\n%s\n", backTrace.c_str());
    #else
    LLBC_PrintLn("Not implement Symbol function(Try enable LLBC_CFG_OS_IMPL_SYMBOL option and rebuild llbc library to enable Symbol function)");
    #endif // LLBC_CFG_OS_IMPL_SYMBOL

    LLBC_CPUTime usedTime = LLBC_CPUTime::Current() - begTime;
    LLBC_PrintLn("Capture stack backtrace used time(ms):%s", usedTime.ToString().c_str());
}
