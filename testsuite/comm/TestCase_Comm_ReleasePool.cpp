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


#include "comm/TestCase_Comm_ReleasePool.h"

namespace
{

class TestObj : public LLBC_Object
{
public:
    TestObj()
    {
        LLBC_PrintLn("TestObj construct: %p", this);
    }

    virtual ~TestObj()
    {
        LLBC_PrintLn("TestObj destruct: %p", this);
    }
};

class MyComp : public LLBC_Component
{
public:
    MyComp() {  }
    virtual ~MyComp() {  }

public:
    virtual bool OnInit(bool &initFinished)
    {
        LLBC_PrintLn("MyComp initialize ...");

        TestObj *obj = new TestObj;
        obj->AutoRelease();

        obj->Retain();
        obj->AutoRelease();
        
        obj->Retain();
        obj->AutoRelease();

        return true;
    }

    virtual void OnDestroy(bool &destroyFinished)
    {
        LLBC_PrintLn("MyComp destroy...");
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
    LLBC_PrintLn("ReleasePool test:");

    // Create service.
    LLBC_Service *svc = LLBC_Service::Create("ReleasePoolTest");
    svc->AddComponent(new MyComp);
    svc->Start();

    std::cout <<"press any key to continue ..." <<std::endl;
    getchar();

    delete svc;

    return 0;
}
