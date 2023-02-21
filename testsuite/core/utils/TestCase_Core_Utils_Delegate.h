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

#pragma once

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Utils_Delegate :public LLBC_BaseTestCase
{
public:
    TestCase_Core_Utils_Delegate();
    virtual ~TestCase_Core_Utils_Delegate();

public:
    virtual int Run(int argc, char *argv[]);

private:
    void Foo_0();
    int Foo_0_Rtn_Int();

    void Foo_1(int arg1);

    void Foo_2(int arg1, const LLBC_String &arg2);

    void Foo_3(int arg1, bool arg2, sint64 arg3);

    void Foo_4(int arg1, int arg2, int arg3, int arg4);

    void Foo_5(int arg1, int arg2, int arg3, int arg4, int arg5);

    void Foo_6(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);

    void Foo_7(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7);

private:
    void *FooEx(const LLBC_NS LLBC_String &arg);
};
