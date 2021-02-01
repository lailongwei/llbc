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

    void Func_Foo_2(int arg1, int arg2)
    {
        std::cout <<"Func_Foo_2() called" <<std::endl;
        std::cout <<"\targ1: " <<arg1 <<std::endl;
        std::cout <<"\targ2: " <<arg2 <<std::endl;
    }

    void Func_Foo_3(int arg1, int arg2, int arg3)
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

    void Func_Foo_5(int arg1, int arg2, int arg3, int arg4, int arg5)
    {
        std::cout <<"Func_Foo_5() called" <<std::endl;
        std::cout <<"\targ1: " <<arg1 <<std::endl;
        std::cout <<"\targ2: " <<arg2 <<std::endl;
        std::cout <<"\targ3: " <<arg3 <<std::endl;
        std::cout <<"\targ4: " <<arg4 <<std::endl;
        std::cout <<"\targ5: " <<arg5 <<std::endl;
    }

    void Func_Foo_6(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6)
    {
        std::cout <<"Func_Foo_6() called" <<std::endl;
        std::cout <<"\targ1: " <<arg1 <<std::endl;
        std::cout <<"\targ2: " <<arg2 <<std::endl;
        std::cout <<"\targ3: " <<arg3 <<std::endl;
        std::cout <<"\targ4: " <<arg4 <<std::endl;
        std::cout <<"\targ5: " <<arg5 <<std::endl;
        std::cout <<"\targ6: " <<arg6 <<std::endl;
    }

    void Func_Foo_7(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7)
    {
        std::cout <<"Func_Foo_7() called" <<std::endl;
        std::cout <<"\targ1: " <<arg1 <<std::endl;
        std::cout <<"\targ2: " <<arg2 <<std::endl;
        std::cout <<"\targ3: " <<arg3 <<std::endl;
        std::cout <<"\targ4: " <<arg4 <<std::endl;
        std::cout <<"\targ5: " <<arg5 <<std::endl;
        std::cout <<"\targ6: " <<arg6 <<std::endl;
        std::cout <<"\targ7: " <<arg7 <<std::endl;
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

    int arg1 = 1, arg2 = 2, arg3 = 3;
    int arg4 = 4, arg5 = 5, arg6 = 6, arg7 = 7;

    // Call delegate0, funcDelegate0.
    typedef LLBC_Delegate0<void, DelegObj> __Deleg0_RtnVoid;
    LLBC_IDelegate0<void> *deleg0 = LLBC_New(__Deleg0_RtnVoid, this, &DelegObj::Foo_0);
    deleg0->Invoke();
    LLBC_Delete(deleg0);

    typedef LLBC_Delegate0<int, DelegObj> __Deleg0_RtnInt;
    LLBC_IDelegate0<int> *deleg0_rtn_int =
            LLBC_New(__Deleg0_RtnInt, this, &DelegObj::Foo_0_Rtn_Int);
    std::cout << "Call Foo_0_Rtn_Int() return: " << deleg0_rtn_int->Invoke() << std::endl;

    typedef LLBC_Func0<void> __Func0_RtnVoid;
    LLBC_IDelegate0<void> *funcDeleg0 = 
            LLBC_New(__Func0_RtnVoid, &Func_Foo_0);
    funcDeleg0->Invoke();
    LLBC_Delete(funcDeleg0);

    // Call delegate1.
    typedef LLBC_Delegate1<void, DelegObj, int> __Deleg1_Int_RtnVoid;
    LLBC_IDelegate1<void, int> *deleg1 = 
            LLBC_New(__Deleg1_Int_RtnVoid, this, &DelegObj::Foo_1);
    deleg1->Invoke(arg1);
    LLBC_Delete(deleg1);

    typedef LLBC_Func1<void, int> __Func1_Int_RtnVoid;
    LLBC_IDelegate1<void, int> *funcDeleg1 = 
            LLBC_New(__Func1_Int_RtnVoid, &Func_Foo_1);
    funcDeleg1->Invoke(arg1);
    LLBC_Delete(funcDeleg1);

    // Call delegate2.
    typedef LLBC_Delegate2<void, DelegObj, int, int> __Deleg2_Int_RtnVoid;
    LLBC_IDelegate2<void, int, int> *deleg2 =  
            LLBC_New(__Deleg2_Int_RtnVoid, this, &DelegObj::Foo_2);
    deleg2->Invoke(arg1, arg2);
    LLBC_Delete(deleg2);

    typedef LLBC_Func2<void, int, int> __Func2_Int_RtnVoid;
    LLBC_IDelegate2<void, int, int> *funcDeleg2 = 
            LLBC_New(__Func2_Int_RtnVoid, &Func_Foo_2);
    funcDeleg2->Invoke(arg1, arg2);
    LLBC_Delete(funcDeleg2);

    // Call delegate3.
    typedef LLBC_Delegate3<void, DelegObj, int, int, int> __Deleg3_Int_RtnVoid;
    LLBC_IDelegate3<void, int, int, int> *deleg3 = 
            LLBC_New(__Deleg3_Int_RtnVoid, this, &DelegObj::Foo_3);
    deleg3->Invoke(arg1, arg2, arg3);
    LLBC_Delete(deleg3);

    typedef LLBC_Func3<void, int, int, int> __Func3_Int_RtnVoid;
    LLBC_IDelegate3<void, int, int, int> *funcDeleg3 = 
            LLBC_New(__Func3_Int_RtnVoid, &Func_Foo_3);
    funcDeleg3->Invoke(arg1, arg2, arg3);
    LLBC_Delete(funcDeleg3);

    // Call delegate4.
    typedef LLBC_Delegate4<void, DelegObj, int, int, int, int> __Deleg4_Int_RtnVoid;
    LLBC_IDelegate4<void, int, int, int, int> *deleg4 =  
            LLBC_New(__Deleg4_Int_RtnVoid, this, &DelegObj::Foo_4);
    deleg4->Invoke(arg1, arg2, arg3, arg4);
    LLBC_Delete(deleg4);

    typedef LLBC_Func4<void, int, int, int, int> __Func4_Int_RtnVoid;
    LLBC_IDelegate4<void, int, int, int, int> *funcDeleg4 =
            LLBC_New(__Func4_Int_RtnVoid, &Func_Foo_4);
    funcDeleg4->Invoke(arg1, arg2, arg3, arg4);
    LLBC_Delete(funcDeleg4);

    // Call delegate5.
    typedef LLBC_Delegate5<void, DelegObj, int, int, int, int, int> __Deleg5_Int_RtnVoid;
    LLBC_IDelegate5<void, int, int, int, int, int> *deleg5 = 
            LLBC_New(__Deleg5_Int_RtnVoid, this, &DelegObj::Foo_5);
    deleg5->Invoke(arg1, arg2, arg3, arg4, arg5);
    LLBC_Delete(deleg5);

    typedef LLBC_Func5<void, int, int, int, int, int> __Func5_Int_RtnVoid;
    LLBC_IDelegate5<void, int, int, int, int, int> *funcDeleg5 = 
            LLBC_New(__Func5_Int_RtnVoid, &Func_Foo_5);
    funcDeleg5->Invoke(arg1, arg2, arg3, arg4, arg5);
    LLBC_Delete(funcDeleg5);

    // Call delegate6.
    typedef LLBC_Delegate6<void, DelegObj, int, int, int, int, int, int> __Deleg6_Int_RtnVoid;
    LLBC_IDelegate6<void, int, int, int, int, int, int> *deleg6 =
            LLBC_New(__Deleg6_Int_RtnVoid, this, &DelegObj::Foo_6);
    deleg6->Invoke(arg1, arg2, arg3, arg4, arg5, arg6);
    LLBC_Delete(deleg6);

    typedef LLBC_Func6<void, int, int, int, int, int, int> __Func6_Int_RtnVoid;
    LLBC_IDelegate6<void, int, int, int, int, int, int> *funcDeleg6 =
            LLBC_New(__Func6_Int_RtnVoid, &Func_Foo_6);
    funcDeleg6->Invoke(arg1, arg2, arg3, arg4, arg5, arg6);
    LLBC_Delete(funcDeleg6);

    // Call delegate7.
    typedef LLBC_Delegate7<void, DelegObj, int, int, int, int, int, int, int> __Deleg7_Int_RtnVoid;
    LLBC_IDelegate7<void, int, int, int, int, int, int, int> *deleg7 =
            LLBC_New(__Deleg7_Int_RtnVoid, this, &DelegObj::Foo_7);
    deleg7->Invoke(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    LLBC_Delete(deleg7);

    typedef LLBC_Func7<void, int, int, int, int, int, int, int> __Func7_Int_RtnVoid;
    LLBC_IDelegate7<void, int, int, int, int, int, int, int> *funcDeleg7 =
            LLBC_New(__Func7_Int_RtnVoid, &Func_Foo_7);
    funcDeleg7->Invoke(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    LLBC_Delete(funcDeleg7);

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

void TestCase_Core_Utils_Delegate::Foo_2(int arg1, int arg2)
{
    std::cout <<"Foo_2() called" <<std::endl;
    std::cout <<"\t arg1: " <<arg1 <<std::endl;
    std::cout <<"\t arg2: " <<arg2 <<std::endl;
}

void TestCase_Core_Utils_Delegate::Foo_3(int arg1, int arg2, int arg3)
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

    return NULL;
}
