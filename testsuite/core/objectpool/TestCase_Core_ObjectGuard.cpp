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

#include "core/objectpool/TestCase_Core_ObjectGuard.h"

namespace
{
    class TestObj
    {
    public:
        TestObj()
        {
            std::cout << ">>>> TestObj::TestObj() called, this:0x" << this << std::endl;
        }

        ~TestObj()
        {
            std::cout << "<<<< TestObj::~TestObj() called, this:0x" << this << std::endl;   
        }

    public:
        LLBC_String SayHello(const LLBC_String &helloMsg)
        {
            return LLBC_String("TestObj::SayHello() called, helloMsg:") + helloMsg;
        }

        LLBC_String SayHello(const LLBC_String &helloMsg) const
        {
            return LLBC_String("TestObj::SayHello() const called, helloMsg:") + helloMsg;
        }
    };
}

TestCase_Core_ObjectGuard::TestCase_Core_ObjectGuard()
{
}

TestCase_Core_ObjectGuard::~TestCase_Core_ObjectGuard()
{
}

int TestCase_Core_ObjectGuard::Run(int argc, char *argv[])
{
    if (BaseTest() != LLBC_OK)
        return LLBC_FAILED;

    if (WeakRefTest() != LLBC_OK)
        return LLBC_FAILED;

    if (AccessorOperatorsTest() != LLBC_OK)
        return LLBC_FAILED;

    return LLBC_OK;
}

int TestCase_Core_ObjectGuard::BaseTest()
{
    std::cout << "Base test..." << std::endl;

    // Test basic apis.
    LLBC_UnsafetyObjectPool objPool;
    auto guard1 = objPool.GetGuarded<TestObj>();
    const auto &constGuard1 = guard1;
    std::cout << "- Create obj guard:" << guard1 << std::endl;
    std::cout << "  - GetObj() return:" << guard1.GetObj() << std::endl;
    std::cout << "  - GetObj() const return:" << constGuard1.GetObj() << std::endl;
    std::cout << "  - IsWeakRef() return:" << guard1.IsWeakRef() << std::endl;
    std::cout << "  - ToString() return:" << guard1.ToString() << std::endl;
    std::cout << "  - Guard obj available?:" << (guard1 ? "true" : "false") << std::endl;

    // Test object detach.
    auto obj = guard1.DetachObj();
    std::cout << "- After detach obj, obj:0x" << obj << ", guard obj:" << guard1 << std::endl;
    std::cout << "- Manual release detached obj:0x" << obj << std::endl;
    objPool.Release(obj);

    // Test object release.
    auto guard2 = objPool.GetGuarded<TestObj>();
    std::cout << "- Create new obj guard(used for test ReleaseObj() api):" << guard2 << std::endl;
    std::cout << "- Call ReleaseObj() api" <<std::endl;
    guard2.ReleaseObj();
    std::cout << "- After called ReleaseObj():" <<guard2 <<std::endl;

    std::cout << std::endl;
    return LLBC_OK;
}

int TestCase_Core_ObjectGuard::WeakRefTest()
{
    std::cout << "Weakref test..." << std::endl;

    LLBC_UnsafetyObjectPool objPool;
    auto guard1 = objPool.GetGuarded<TestObj>();
    std::cout << "- New guard obj:" << guard1 << std::endl;

    auto guard2 = guard1;
    std::cout << "- After copy construct(guard2 = guard1):" << std::endl;
    std::cout << "  - guard1:" <<guard1 <<std::endl;
    std::cout << "  - guard2:" <<guard2 <<std::endl;
    if (!guard1.IsWeakRef() ||
        guard2.IsWeakRef())
    {
        std::cerr << "Copy construct has some bugs, please check ObjectGuard code!" << std::endl;
        return LLBC_FAILED;
    }

    LLBC_ObjectGuard<TestObj> guard3;
    std::cout << "- Default construct guard obj:" << guard3 << std::endl;
    guard3 = guard2;
    std::cout << "- After assignment(guard3 = guard2):" << std::endl;
    std::cout << "  - guard2:" << guard2 << std::endl;
    std::cout << "  - guard3:" << guard3 << std::endl;
    if (!guard2.IsWeakRef() ||
        guard3.IsWeakRef())
    {
        std::cerr << "Assignment operator has some bugs, please check ObjectGuard code!" << std::endl;
        return LLBC_FAILED;
    }

    std::cout << "- Try release weakref guard obj(guard2)..." << std::endl;
    guard2.ReleaseObj();
    std::cout << "- After release guard2:" << std::endl;
    std::cout << "  - guard2:" << guard2 << std::endl;
    std::cout << "  - aurd2.GetObj():" << guard2.GetObj() << std::endl;
    std::cout << "  - guard3->SayHello():" << guard3->SayHello("After release weakref guard obj") << std::endl;

    std::cout <<"-Try release strongref guard obj(guard3)..." <<std::endl;
    guard3.ReleaseObj();
    std::cout << "- After release guard3:" << std::endl;
    std::cout << "  - guard3:" << guard3 << std::endl;
    std::cout << "  - guard3.GetObj()" << guard3.GetObj() << std::endl;

    std::cout << std::endl;
    return LLBC_OK;
}

int TestCase_Core_ObjectGuard::AccessorOperatorsTest()
{
    std::cout << "Accessor operators test..." << std::endl;

    LLBC_UnsafetyObjectPool objPool;
    auto guard = objPool.GetGuarded<TestObj>();
    const auto &constGuard = guard;

    std::cout << "  - operator*()/operator*() const test..." <<std::endl;
    std::cout << "    - operator*().SayHello():" << (*guard).SayHello("Used operator*() call") << std::endl;
    std::cout << "    - operator*() const.SayHello():" << (*constGuard).SayHello("Used operator*() const call") << std::endl;

    std::cout <<"  - operator->()/operator->() const test..." <<std::endl;
    std::cout <<"    - operator->()->SayHello():" << guard->SayHello("Used operator->() call") <<std::endl;
    std::cout <<"    - operator->() const->SayHello():" << constGuard->SayHello("Used operator->() const call") <<std::endl;

    std::cout << std::endl;
    return LLBC_OK;
}
