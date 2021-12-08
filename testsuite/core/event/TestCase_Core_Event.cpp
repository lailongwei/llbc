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

#include "core/event/TestCase_Core_Event.h"

namespace
{
    class EventIds
    {
    public:
        enum
        {
            Event1 = 1,
            Event2 = 2,
        };
    };

    static LLBC_EventManager evMgr;
}

TestCase_Core_Event::TestCase_Core_Event()
{
}

TestCase_Core_Event::~TestCase_Core_Event()
{
}

int TestCase_Core_Event::Run(int argc, char *argv[])
{
    std::cout <<"core/event test:" <<std::endl;

    // Fire Event1 to test params pass.
    std::cout <<"Add Event1 Listener OnEvent1()" <<std::endl;
    _ev1Stub = evMgr.AddListener(EventIds::Event1, this, &TestCase_Core_Event::OnEvent1);
    std::cout <<"Add Event1 Listener OnEvent1Too()" <<std::endl;
    _ev1TooStub = evMgr.AddListener(EventIds::Event1, this, &TestCase_Core_Event::OnEvent1Too);

    // Build event.
    LLBC_Event *ev = LLBC_New(LLBC_Event, EventIds::Event1);
    // Attach int key indexed params.
    ev->SetParam(0, 1);
    ev->SetParam(1, true);
    ev->SetParam(2, "Hello world");
    ev->SetParam(2, "Hello world(overrided)");
    (*ev)[3] = 1.5;
    (*ev)[4] = -1.5f;
    (*ev)[4] = -3.0f;
    // Attach string key indexed params.
    ev->SetParam("Key1", "Hello world");
    (*ev)["Key2"] =  "Hey, Judy";
    // Fire Event1(Event2 listener already add in Event1 handler).
    evMgr.FireEvent(ev);

    std::cout <<"Fire Event2" <<std::endl;
    evMgr.FireEvent(LLBC_New(LLBC_Event, EventIds::Event2));

    std::cout <<"Fire Event1" <<std::endl;
    evMgr.FireEvent(LLBC_New(LLBC_Event, EventIds::Event1));

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}

void TestCase_Core_Event::OnEvent1(LLBC_Event &ev)
{
    std::cout <<"OnEvent1() called! event int key indexed params:" <<std::endl;
    const std::map<int, LLBC_Variant> &intKeyParams = ev.GetIntKeyParams();
    for (std::map<int, LLBC_Variant>::const_iterator it = intKeyParams.begin();
         it != intKeyParams.end();
         it++)
        std::cout <<"  " <<it->first <<": " <<it->second <<std::endl;

    std::cout <<"string key indexed params: " <<std::endl;
    const std::map<LLBC_String, LLBC_Variant> &strKeyParams = ev.GetStrKeyParams();
    for (std::map<LLBC_String, LLBC_Variant>::const_iterator it = strKeyParams.begin();
         it != strKeyParams.end();
         it++)
        std::cout <<"  " <<it->first <<": " <<it->second <<std::endl;
}

void TestCase_Core_Event::OnEvent1Too(LLBC_Event &ev)
{
    std::cout <<"OnEvent1Too()called! add Event2 event listener OnEvent2()" <<std::endl;

    // Add Event2 listener.
    evMgr.AddListener(EventIds::Event2, this, &TestCase_Core_Event::OnEvent2);
}

void TestCase_Core_Event::OnEvent2(LLBC_Event &ev)
{
    std::cout <<"OnEvent2() called! remove Event1 event listener OnEvent1Too() and remove self listener" <<std::endl;

    // Remove Event1 OnEvent1Too() listener.
    evMgr.RemoveListener(_ev1TooStub);
    // Remove Event2 OnEvent2() listener.
    evMgr.RemoveListener(EventIds::Event2);
}
