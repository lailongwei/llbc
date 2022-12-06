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


#include "core/utils/TestCase_Core_Utils_Delegate.h"

namespace
{
    void Func_Foo_0()
    {
        std::cout <<"Func_Foo_0() called" <<std::endl;
    }

    void Func_Foo_1(int arg1)
    {
        std::cout <<"Func_Foo_1() called" <<std::endl;
        std::cout <<"\targ1: " <<arg1 <<std::endl;
    }

    void Func_Foo_2(int arg1, const LLBC_String &arg2)
    {
        std::cout <<"Func_Foo_2() called" <<std::endl;
        std::cout <<"\targ1: " <<arg1 <<std::endl;
        std::cout <<"\targ2: " <<arg2 <<std::endl;
    }

    void Func_Foo_3(int arg1, bool arg2, sint64 arg3)
    {
        std::cout <<"Func_Foo_3() called" <<std::endl;
        std::cout <<"\targ1: " <<arg1 <<std::endl;
        std::cout <<"\targ2: " <<arg2 <<std::endl;
        std::cout <<"\targ3: " <<arg3 <<std::endl;
    }

    void Func_Foo_4(int arg1, int arg2, int arg3, int arg4)
    {
        std::cout <<"Func_Foo_4() called" <<std::endl;
        std::cout <<"\targ1: " <<arg1 <<std::endl;
        std::cout <<"\targ2: " <<arg2 <<std::endl;
        std::cout <<"\targ3: " <<arg3 <<std::endl;
        std::cout <<"\targ4: " <<arg4 <<std::endl;
    }
}

TestCase_Core_Utils_Delegate::TestCase_Core_Utils_Delegate()
{
}

TestCase_Core_Utils_Delegate::~TestCase_Core_Utils_Delegate()
{
}

int TestCase_Core_Utils_Delegate::Run(int argc, char *argv[])
{
    typedef TestCase_Core_Utils_Delegate DelegObj;

    std::cout <<"Delegate test:" <<std::endl;
    LLBC_Delegate<void()> func0(&Func_Foo_0);
    func0();

    const LLBC_Delegate<void(int)> func1(&Func_Foo_1);
    func1(10086);

    LLBC_Delegate<void(int, const LLBC_String &)> func2(&Func_Foo_2);
    func2(10010, "The argument 2");

    LLBC_Delegate<void(int, bool , sint64)> func3(&Func_Foo_3);
    func3(1, false, 3ll);

    LLBC_Delegate<void(int, int, int, int)> func4(&Func_Foo_4);
    func4(4, 3, 2, 1);

    LLBC_Delegate<void()> func5(this, &DelegObj::Foo_0);
    func5();

    LLBC_Delegate<int()> meth0RtnInt(this, &DelegObj::Foo_0_Rtn_Int);
    std::cout <<"Call meth0RtnInt() ret:" <<meth0RtnInt();

    std::string s("Hello world");
    LLBC_Delegate<size_t()> strSizeMeth(&s, &std::string::size);
    std::cout <<"call string[" <<s <<"] size() meth:" <<strSizeMeth() <<std::endl;
    std::cout <<"strSizeMeth valid?:" <<!!strSizeMeth <<std::endl;

    LLBC_Delegate<void()> nullDeleg = nullptr;
    std::cout <<"nullDeleg valid?:" <<!!nullDeleg <<std::endl;
    nullDeleg = &Func_Foo_0;
    if (nullDeleg)
    {
        std::cout <<"set nullDeleg = &Func_Foo_0, valid, call it!" <<std::endl;
        nullDeleg();
    }
    else
    {
        std::cerr <<"set nullDeleg = &Func_Foo_0, but deleg still invalid, error!" <<std::endl;
        return LLBC_FAILED;
    }

    LLBC_Delegate<void(int)> stlFuncDeleg;
    stlFuncDeleg = std::function<void(int)>([](int)
    {
        std::cout << "lambda function void(int) called!" << std::endl;
    });
    stlFuncDeleg(33);

    LLBC_Delegate<void(int)> stlFuncDeleg2 = [](int)
    {
        std::cout << "lambda function void(int) called(another)!" << std::endl;
    };
    stlFuncDeleg2(44);

    std::cout << "override Foo_0 delegate to lambda function" << std::endl;
    func0 = []()
    {
        std::cout << "lambda function void() called()!" << std::endl;
    };
    func0();

    nullDeleg = nullptr;
    std::cout <<"set nullDeleg to nullptr, valid?:" <<!!nullDeleg <<std::endl;

    // latest, defer test.
    LLBC_Defer(std::cout << "The defered execute statement" << std::endl);

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    return 0;
}

void TestCase_Core_Utils_Delegate::Foo_0()
{
    std::cout <<"Foo_0() called" <<std::endl;
}

int TestCase_Core_Utils_Delegate::Foo_0_Rtn_Int()
{
    std::cout << "Foo_0_Rtn_Int() called, will return 10086" << std::endl;
    return 10086;
}

void TestCase_Core_Utils_Delegate::Foo_1(int arg1)
{
    std::cout <<"Foo_1() called" <<std::endl;
    std::cout <<"\t arg1: " <<arg1 <<std::endl;
}

void TestCase_Core_Utils_Delegate::Foo_2(int arg1, const LLBC_String &arg2)
{
    std::cout <<"Foo_2() called" <<std::endl;
    std::cout <<"\t arg1: " <<arg1 <<std::endl;
    std::cout <<"\t arg2: " <<arg2 <<std::endl;
}

void TestCase_Core_Utils_Delegate::Foo_3(int arg1, bool arg2, sint64 arg3)
{
    std::cout <<"Foo_3() called" <<std::endl;
    std::cout <<"\t arg1: " <<arg1 <<std::endl;
    std::cout <<"\t arg2: " <<arg2 <<std::endl;
    std::cout <<"\t arg3: " <<arg3 <<std::endl;
}

void TestCase_Core_Utils_Delegate::Foo_4(int arg1, int arg2, int arg3, int arg4)
{
    std::cout <<"Foo_4() called" <<std::endl;
    std::cout <<"\t arg1: " <<arg1 <<std::endl;
    std::cout <<"\t arg2: " <<arg2 <<std::endl;
    std::cout <<"\t arg3: " <<arg3 <<std::endl;
    std::cout <<"\t arg4: " <<arg4 <<std::endl;
}

void TestCase_Core_Utils_Delegate::Foo_5(int arg1, int arg2, int arg3, int arg4, int arg5)
{
    std::cout <<"Foo_5() called" <<std::endl;
    std::cout <<"\t arg1: " <<arg1 <<std::endl;
    std::cout <<"\t arg2: " <<arg2 <<std::endl;
    std::cout <<"\t arg3: " <<arg3 <<std::endl;
    std::cout <<"\t arg4: " <<arg4 <<std::endl;
    std::cout <<"\t arg5: " <<arg5 <<std::endl;
}

void TestCase_Core_Utils_Delegate::Foo_6(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6)
{
    std::cout <<"Foo_6() called" <<std::endl;
    std::cout <<"\t arg1: " <<arg1 <<std::endl;
    std::cout <<"\t arg2: " <<arg2 <<std::endl;
    std::cout <<"\t arg3: " <<arg3 <<std::endl;
    std::cout <<"\t arg4: " <<arg4 <<std::endl;
    std::cout <<"\t arg5: " <<arg5 <<std::endl;
    std::cout <<"\t arg6: " <<arg6 <<std::endl;
}

void TestCase_Core_Utils_Delegate::Foo_7(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7)
{
    std::cout <<"Foo_7() called" <<std::endl;
    std::cout <<"\t arg1: " <<arg1 <<std::endl;
    std::cout <<"\t arg2: " <<arg2 <<std::endl;
    std::cout <<"\t arg3: " <<arg3 <<std::endl;
    std::cout <<"\t arg4: " <<arg4 <<std::endl;
    std::cout <<"\t arg5: " <<arg5 <<std::endl;
    std::cout <<"\t arg6: " <<arg6 <<std::endl;
    std::cout <<"\t arg7: " <<arg7 <<std::endl;
}

void *TestCase_Core_Utils_Delegate::FooEx(const LLBC_String &arg)
{
    std::cout <<"FooEx() called" <<std::endl;
    std::cout <<"\t arg: " <<arg <<std::endl;

    return nullptr;
}
