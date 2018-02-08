/**
 * @file    TestCase_Comm_Event.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/07/21
 * @version 1.0
 *
 * @brief
 */

#include "comm/TestCase_Comm_Event.h"

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

        TestEvent *ev = new TestEvent(TestEvent::TEST_EV_ID1);
        ev->facade = this;
        ev->data.format("Hello, I'm event data[id:%d]", ev->GetId());

        svc->FireEvent(ev);

        ev = new TestEvent(TestEvent::TEST_EV_ID2);
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

TestCase_Comm_Event::TestCase_Comm_Event()
{
}

TestCase_Comm_Event::~TestCase_Comm_Event()
{
}

int TestCase_Comm_Event::Run(int argc, char *argv[])
{
    std::cout <<"Core/Event component testcase..." <<std::endl;

    // We create a service to test.
    LLBC_IService *svc = LLBC_IService::Create(LLBC_IService::Normal, "EventTest");
    svc->RegisterFacade(LLBC_New(EventTestFacade));
    svc->Start();

    std::cout <<"Press any key to continue..." <<std::endl;
    getchar();

    delete svc;

    return LLBC_OK;
}
