/**
 * @file    TestCase_Comm_ReleasePool.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/31
 * @version 1.0
 *
 * @brief
 */

#include "comm/TestCase_Comm_ReleasePool.h"

namespace
{

class TestObj : public LLBC_Object
{
public:
    TestObj()
    {
        LLBC_PrintLine("TestObj construct: %p", this);
    }

    virtual ~TestObj()
    {
        LLBC_PrintLine("TestObj destruct: %p", this);
    }
};

class MyFacade : public LLBC_IFacade
{
public:
    MyFacade() {  }
    virtual ~MyFacade() {  }

public:
    virtual void OnInitialize()
    {
        LLBC_PrintLine("MyFacade initialize ...");

        TestObj *obj = new TestObj;
        obj->AutoRelease();

        obj->Retain();
        obj->AutoRelease();
        
        obj->Retain();
        obj->AutoRelease();
    }

    virtual void OnDestroy()
    {
        LLBC_PrintLine("MyFacade destroy...");
    }

    virtual void OnUpdate()
    {
        // Create new release pool.
        LLBC_AutoReleasePool *pool = new LLBC_AutoReleasePool;

        LLBC_Object *obj = new TestObj;
        obj->AutoRelease();

        delete pool;
    }
};

}

TestCase_Comm_ReleasePool::TestCase_Comm_ReleasePool()
{
}

TestCase_Comm_ReleasePool::~TestCase_Comm_ReleasePool()
{
}

int TestCase_Comm_ReleasePool::Run(int argc, char *argv[])
{
    LLBC_PrintLine("ReleasePool test:");

    // Create service.
    LLBC_IService *svc = LLBC_IService::Create(LLBC_IService::Normal);
    svc->RegisterFacade(LLBC_New(MyFacade));
    svc->Start();

    std::cout <<"press any key to continue ..." <<std::endl;
    getchar();

    delete svc;

    return 0;
}
