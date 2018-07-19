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


#include "comm/TestCase_Comm_EventInSvc.h"

namespace
{

class EventTestFacade;

class TestEvent : public LLBC_Event
{
public:
    enum
    {
        TEST_EV_ID1 = 1,
        TEST_EV_ID2 = 2,
    };

public:
    TestEvent(int evId)
    : LLBC_Event(evId)
    {
    }

    virtual ~TestEvent()
    {
    }

    EventTestFacade *facade;
    LLBC_String data;
};

class EventTestFacade : public LLBC_IFacade
{
public:
    EventTestFacade()
    : _handleTimes(0)
    , _staticHandleTimes(0)
    , _ev1HandlerStub()
    , _ev1StaticHandlerStub()
    {
    }

public:
    virtual void OnInitialize()
    {
        LLBC_ThreadManager::Sleep(1000);

        LLBC_IService *svc = GetService();
        _ev1HandlerStub = svc->SubscribeEvent(TestEvent::TEST_EV_ID1, this, &EventTestFacade::HandleEvent);
        _ev1StaticHandlerStub = svc->SubscribeEvent(TestEvent::TEST_EV_ID1, &EventTestFacade::HandleEvent_Static);
    }

    virtual void OnDestroy()
    {
        std::cout <<"Service destroy..." <<std::endl;
    }

    virtual void OnUpdate()
    {
        LLBC_IService *svc = GetService();

        TestEvent *ev = LLBC_New1(TestEvent, TestEvent::TEST_EV_ID1);
        ev->facade = this;
        ev->data.format("Hello, I'm event data[id:%d]", ev->GetId());

        svc->FireEvent(ev);

        ev = LLBC_New1(TestEvent, TestEvent::TEST_EV_ID2);
        ev->data.format("Hello, I'm event data[id:%d]", ev->GetId());
        svc->FireEvent(ev);
    }

public:
    void HandleEvent(LLBC_Event *_)
    {
        TestEvent *ev = static_cast<TestEvent *>(_);
        std::cout <<"handle event(class member method), data: " <<ev->data <<std::endl;

        ++_handleTimes;
        LLBC_IService *svc = GetService();
        if (_handleTimes == 5)
            svc->SubscribeEvent(TestEvent::TEST_EV_ID2, this, &EventTestFacade::HandleEvent);
        else if (_handleTimes == 10)
            svc->UnsubscribeEvent(_ev1HandlerStub);
        else if (_handleTimes == 1000)
            svc->UnsubscribeEvent(TestEvent::TEST_EV_ID2);
    }

    static void HandleEvent_Static(LLBC_Event *_)
    {
        TestEvent *ev = static_cast<TestEvent *>(_);
        std::cout << "handle event(class static method(like function), data: " <<ev->data <<std::endl;

        EventTestFacade *facade = ev->facade;
        ++facade->_staticHandleTimes;
        LLBC_IService *svc = facade->GetService();
        if (facade->_staticHandleTimes == 1000)
            svc->UnsubscribeEvent(facade->_ev1StaticHandlerStub);
    }

private:
    int _handleTimes;
    int _staticHandleTimes;

    LLBC_ListenerStub _ev1HandlerStub;
    LLBC_ListenerStub _ev1StaticHandlerStub;
};

}

TestCase_Comm_EventInSvc::TestCase_Comm_EventInSvc()
{
}

TestCase_Comm_EventInSvc::~TestCase_Comm_EventInSvc()
{
}

int TestCase_Comm_EventInSvc::Run(int argc, char *argv[])
{
    std::cout <<"Core/Event component testcase..." <<std::endl;

    // We create a service to test.
    LLBC_IService *svc = LLBC_IService::Create(LLBC_IService::Normal, "EventTest");
    svc->RegisterFacade(LLBC_New(EventTestFacade));
    svc->Start();

    std::cout <<"Press any key to continue..." <<std::endl;
    getchar();

    LLBC_Delete(svc);

    return LLBC_OK;
}
