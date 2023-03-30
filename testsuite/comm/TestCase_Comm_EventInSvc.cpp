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

class EventTestComp;

class TestEvent : public LLBC_Event
{
public:
    enum
    {
        TEST_EV_ID1 = 1,
        TEST_EV_ID2 = 2,
        TEST_EV_ID3 = 3,
    };

public:
    TestEvent(int evId)
    : LLBC_Event(evId)
    , comp(nullptr)
    {
    }

    virtual ~TestEvent()
    {
    }

    EventTestComp *comp;
    LLBC_String data;
};

class EventTestComp : public LLBC_Component
{
public:
    EventTestComp()
    : LLBC_Component(LLBC_ComponentEvents::DefaultEvents | LLBC_ComponentEvents::OnUpdate)
    , _handleTimes(0)
    , _staticHandleTimes(0)
    , _ev1HandlerStub()
    , _ev1StaticHandlerStub()
    {
    }

public:
    virtual bool OnInit(bool &initFinished)
    {
        LLBC_Sleep(1000);

        LLBC_Service *svc = GetService();
        _ev1HandlerStub = svc->SubscribeEvent(TestEvent::TEST_EV_ID1, this, &EventTestComp::HandleEvent);
        _ev1StaticHandlerStub = svc->SubscribeEvent(TestEvent::TEST_EV_ID1, &EventTestComp::HandleEvent_Static);
        _ev3HandlerStub = svc->SubscribeEvent(TestEvent::TEST_EV_ID3, this, &EventTestComp::HandleEvent3);

        return true;
    }

    virtual void OnDestroy(bool &destroyFinished)
    {
        std::cout <<"Service destroy..." <<std::endl;
    }

    virtual void OnUpdate()
    {
        LLBC_Service *svc = GetService();

        TestEvent *ev = new TestEvent(TestEvent::TEST_EV_ID1);
        ev->comp = this;
        ev->data.format("Hello, I'm event data[id:%d]", ev->GetId());
        svc->FireEvent(ev);

        ev = new TestEvent(TestEvent::TEST_EV_ID2);
        ev->data.format("Hello, I'm event data[id:%d]", ev->GetId());
        svc->FireEvent(ev);

        svc->BeginFireEvent(TestEvent::TEST_EV_ID3)
            .SetParam("aa", 10086)
            .Fire();
    }

public:
    void HandleEvent(LLBC_Event &_)
    {
        TestEvent &ev = static_cast<TestEvent &>(_);
        std::cout <<"handle event(class member method), data: " <<ev.data <<std::endl;

        ++_handleTimes;
        LLBC_Service *svc = GetService();
        if (_handleTimes == 5)
            svc->SubscribeEvent(TestEvent::TEST_EV_ID2, this, &EventTestComp::HandleEvent);
        else if (_handleTimes == 10)
            svc->UnsubscribeEvent(_ev1HandlerStub);
        else if (_handleTimes == 1000)
            svc->UnsubscribeEvent(TestEvent::TEST_EV_ID2);
    }

    static void HandleEvent_Static(LLBC_Event &_)
    {
        TestEvent &ev = static_cast<TestEvent &>(_);
        std::cout << "handle event(class static method(like function), data: " <<ev.data <<std::endl;

        EventTestComp *comp = ev.comp;
        ++comp->_staticHandleTimes;
        LLBC_Service *svc = comp->GetService();
        if (comp->_staticHandleTimes == 1000)
            svc->UnsubscribeEvent(comp->_ev1StaticHandlerStub);
    }

    void HandleEvent3(LLBC_Event &_)
    {
        TestEvent &ev = static_cast<TestEvent &>(_);
        std::cout << "handle event 3, data: " << ev.GetParam("aa") << std::endl;
        GetService()->UnsubscribeEvent(_ev3HandlerStub); 
    }

private:
    int _handleTimes;
    int _staticHandleTimes;

    LLBC_ListenerStub _ev1HandlerStub;
    LLBC_ListenerStub _ev1StaticHandlerStub;
    LLBC_ListenerStub _ev3HandlerStub;
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
    LLBC_Service *svc = LLBC_Service::Create("EventTest");
    svc->AddComponent<EventTestComp>();
    svc->Start();

    std::cout <<"Press any key to continue..." <<std::endl;
    getchar();

    delete svc;

    return LLBC_OK;
}
