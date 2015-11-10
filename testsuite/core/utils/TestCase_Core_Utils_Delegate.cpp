/**
 * @file    TestCase_Core_Utils_Delegate.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */

#include "core/utils/TestCase_Core_Utils_Delegate.h"

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

    // Call delegate0.
    LLBC_IDelegate0 *deleg0 = 
        new LLBC_Delegate0<DelegObj>(this, &DelegObj::Foo_0);
    deleg0->Invoke();

    // Call delegate1.
    LLBC_IDelegate1<int> *deleg1 =
        new LLBC_Delegate1<DelegObj, int>(this, &DelegObj::Foo_1);
    deleg1->Invoke(arg1);

    // Call delegate2.
    LLBC_IDelegate2<int, int> *deleg2 =
        new LLBC_Delegate2<DelegObj, int, int>(this, &DelegObj::Foo_2);
    deleg2->Invoke(arg1, arg2);

    // Call delegate3.
    LLBC_IDelegate3<int, int, int> *deleg3 =
        new LLBC_Delegate3<DelegObj, int, int, int>(this, &DelegObj::Foo_3);
    deleg3->Invoke(arg1, arg2, arg3);

    // Call delegate4.
    LLBC_IDelegate4<int, int, int, int> *deleg4 =
        new LLBC_Delegate4<DelegObj, int, int, int, int>(this, &DelegObj::Foo_4);
    deleg4->Invoke(arg1, arg2, arg3, arg4);

    // Call delegate5.
    LLBC_IDelegate5<int, int, int, int, int> *deleg5 =
        new LLBC_Delegate5<DelegObj, int, int, int, int, int>(this, &DelegObj::Foo_5);
    deleg5->Invoke(arg1, arg2, arg3, arg4, arg5);

    // Call delegate6.
    LLBC_IDelegate6<int, int, int, int, int, int> *deleg6 =
        new LLBC_Delegate6<DelegObj, int, int, int, int, int, int>(this, &DelegObj::Foo_6);
    deleg6->Invoke(arg1, arg2, arg3, arg4, arg5, arg6);

    // Call delegate7.
    LLBC_IDelegate7<int, int, int, int, int, int, int> *deleg7 =
        new LLBC_Delegate7<DelegObj, int, int, int, int, int, int, int>(this, &DelegObj::Foo_7);
    deleg7->Invoke(arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    // Call delegateex.
    LLBC_IDelegateEx<const LLBC_String &> *delegex =
        new LLBC_DelegateEx<DelegObj, const LLBC_String &>(this, &DelegObj::FooEx);
    delegex->Invoke("Hello, I'm FooEx()'s argument!");

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    return 0;
}

void TestCase_Core_Utils_Delegate::Foo_0()
{
    std::cout <<"Foo_0() called" <<std::endl;
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
