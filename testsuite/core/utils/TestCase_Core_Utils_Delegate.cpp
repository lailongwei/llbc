/**
 * @file    TestCase_Core_Utils_Delegate.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */

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

    void *Func_FooEx(const LLBC_String &arg)
    {
        std::cout <<"Func_FooEx() called" <<std::endl;
        std::cout <<"\t arg: " <<arg <<std::endl;

        return NULL;
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
    LLBC_IDelegate0<void> *deleg0 = 
        new LLBC_Delegate0<void, DelegObj>(this, &DelegObj::Foo_0);
    deleg0->Invoke();
    delete deleg0;

    LLBC_IDelegate0<int> *deleg0_rtn_int =
        new LLBC_Delegate0<int, DelegObj>(this, &DelegObj::Foo_0_Rtn_Int);
    std::cout << "Call Foo_0_Rtn_Int() return: " << deleg0_rtn_int->Invoke() << std::endl;

    LLBC_IDelegate0<void> *funcDeleg0 =
        new LLBC_Func0<void>(&Func_Foo_0);
    funcDeleg0->Invoke();
    delete funcDeleg0;

    // Call delegate1.
    LLBC_IDelegate1<void, int> *deleg1 =
        new LLBC_Delegate1<void, DelegObj, int>(this, &DelegObj::Foo_1);
    deleg1->Invoke(arg1);
    delete deleg1;

    LLBC_IDelegate1<void, int> *funcDeleg1 =
        new LLBC_Func1<void, int>(&Func_Foo_1);
    funcDeleg1->Invoke(arg1);
    delete funcDeleg1;

    // Call delegate2.
    LLBC_IDelegate2<void, int, int> *deleg2 =
        new LLBC_Delegate2<void, DelegObj, int, int>(this, &DelegObj::Foo_2);
    deleg2->Invoke(arg1, arg2);
    delete deleg2;

    LLBC_IDelegate2<void, int, int> *funcDeleg2 =
        new LLBC_Func2<void, int, int>(&Func_Foo_2);
    funcDeleg2->Invoke(arg1, arg2);
    delete funcDeleg2;

    // Call delegate3.
    LLBC_IDelegate3<void, int, int, int> *deleg3 =
        new LLBC_Delegate3<void, DelegObj, int, int, int>(this, &DelegObj::Foo_3);
    deleg3->Invoke(arg1, arg2, arg3);
    delete deleg3;

    LLBC_IDelegate3<void, int, int, int> *funcDeleg3 =
        new LLBC_Func3<void, int, int, int>(&Func_Foo_3);
    funcDeleg3->Invoke(arg1, arg2, arg3);
    delete funcDeleg3;

    // Call delegate4.
    LLBC_IDelegate4<void, int, int, int, int> *deleg4 =
        new LLBC_Delegate4<void, DelegObj, int, int, int, int>(this, &DelegObj::Foo_4);
    deleg4->Invoke(arg1, arg2, arg3, arg4);
    delete deleg4;

    LLBC_IDelegate4<void, int, int, int, int> *funcDeleg4 =
        new LLBC_Func4<void, int, int, int, int>(&Func_Foo_4);
    funcDeleg4->Invoke(arg1, arg2, arg3, arg4);
    delete funcDeleg4;

    // Call delegate5.
    LLBC_IDelegate5<void, int, int, int, int, int> *deleg5 =
        new LLBC_Delegate5<void, DelegObj, int, int, int, int, int>(this, &DelegObj::Foo_5);
    deleg5->Invoke(arg1, arg2, arg3, arg4, arg5);
    delete deleg5;

    LLBC_IDelegate5<void, int, int, int, int, int> *funcDeleg5 =
        new LLBC_Func5<void, int, int, int, int, int>(&Func_Foo_5);
    funcDeleg5->Invoke(arg1, arg2, arg3, arg4, arg5);
    delete funcDeleg5;

    // Call delegate6.
    LLBC_IDelegate6<void, int, int, int, int, int, int> *deleg6 =
        new LLBC_Delegate6<void, DelegObj, int, int, int, int, int, int>(this, &DelegObj::Foo_6);
    deleg6->Invoke(arg1, arg2, arg3, arg4, arg5, arg6);
    delete deleg6;

    LLBC_IDelegate6<void, int, int, int, int, int, int> *funcDeleg6 =
        new LLBC_Func6<void, int, int, int, int, int, int>(&Func_Foo_6);
    funcDeleg6->Invoke(arg1, arg2, arg3, arg4, arg5, arg6);
    delete funcDeleg6;

    // Call delegate7.
    LLBC_IDelegate7<void, int, int, int, int, int, int, int> *deleg7 =
        new LLBC_Delegate7<void, DelegObj, int, int, int, int, int, int, int>(this, &DelegObj::Foo_7);
    deleg7->Invoke(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    delete deleg7;

    LLBC_IDelegate7<void, int, int, int, int, int, int, int> *funcDeleg7 =
        new LLBC_Func7<void, int, int, int, int, int, int, int>(&Func_Foo_7);
    funcDeleg7->Invoke(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    delete funcDeleg7;

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
