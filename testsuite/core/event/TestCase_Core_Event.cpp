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
            Event3 = 3,
            Event4 = 4,
        };
    };

    static LLBC_EventMgr evMgr;
}

int TestCase_Core_Event::Run(int argc, char *argv[])
{
    LLBC_PrintLn("EventMgr test:");

    LLBC_ErrorAndReturnIf(BasicTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIf(InfiniteEventFireTest() != LLBC_OK, LLBC_FAILED);

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}

int TestCase_Core_Event::BasicTest()
{
    LLBC_PrintLn("Basic test:");

    // Add all event listeners.
    LLBC_EventMgr evMgr;
    LLBC_PrintLn("Add event listeners");
    auto ev1Stub1 = evMgr.AddListener(EventIds::Event1, [this, &evMgr](LLBC_Event &ev) {
        LLBC_PrintLn("- Event1 event handler[1] called, params:");
        DumpEvParams(ev);

        // Test event nested fire.
        LLBC_PrintLn("- Fire Event2...");
        evMgr.BeginFire(EventIds::Event2)
            .SetParam("Event2_Key1", "The value")
            .Fire();
    });

    auto ev1Stub2 = evMgr.AddListener(EventIds::Event1, [this](LLBC_Event &ev) {
        LLBC_PrintLn("- Event1 event handler[2] called, params:");
        DumpEvParams(ev);
    });

    evMgr.AddListener(EventIds::Event2, [this, &evMgr](LLBC_Event &ev) {
        LLBC_PrintLn("- Event2 event handler called, params:");
        DumpEvParams(ev);

        // Test event nested fire.
        LLBC_PrintLn("- Fire Event3...");
        evMgr.BeginFire(EventIds::Event3)
            .SetParam("Event3_Key1", "The value")
            .SetParam("Event3_Key2", LLBC_Rand())
            .Fire();
    });

    evMgr.AddListener(EventIds::Event3, [this, &evMgr, ev1Stub1](LLBC_Event &ev) {
        LLBC_PrintLn("Event3 event handler called, params:");
        DumpEvParams(ev);

        // Add Event4 listener.
        LLBC_PrintLn("- Add Event4 listener...");
        evMgr.AddListener(EventIds::Event4, [](LLBC_Event &ev) {
            LLBC_FilePrintLn(stderr, "Error: Event4 handler called");
        });

        // Try fire Event4.
        LLBC_PrintLn("- Try fire Event4...");
        evMgr.BeginFire(EventIds::Event4)
            .Fire();

        // Test remove listener(by stub) in event handler.
        LLBC_PrintLn("- Remove Event1-Stub1 listener, stub:%llu", ev1Stub1);
        auto ret = evMgr.RemoveListener(ev1Stub1);

        // Test remove listener(by event id) in event handler.
        // LLBC_PrintLn("- Remove Event1 listeners, Event1:%llu", EventIds::Event1);
        // auto ret = evMgr.RemoveListener(EventIds::Event1);

        // Test remove all listeners.
        // LLBC_PrintLn("- Remove all listeners");
        // auto ret = evMgr.RemoveAllListeners();

        // auto ret = evMgr.RemoveAllListeners();
        if (ret != LLBC_FAILED || LLBC_GetLastError() != LLBC_ERROR_PENDING)
        {
            LLBC_FilePrintLn(stderr,
                             "- Error: Remove Event1-Stub1 listener return errno is not: <LLBC_ERROR_PENDING>, ret:%d, errno:%d",
                             ret, LLBC_GetLastError());
        }

        // Fire Event1 again.
        LLBC_PrintLn("- Fire Event1 again..");
        evMgr.BeginFire(EventIds::Event1)
             .Fire();
        LLBC_PrintLn("- Fire Event1 finished");
    });

    // Fire Event1.
    LLBC_PrintLn("Fire Event1...");
    evMgr.BeginFire(EventIds::Event1)
        .SetParam("cstring", "cstring: hello world")
        .SetParam(std::string("string"), "string: hello world")
        .SetParam(std::string("string"), "string: hello world")
        .SetParam(std::string("string"), "string: hello world")
        .SetParam(std::string("string"), "string: hello world")
        .SetParam(std::string("string"), "string: hello world")
        .SetParam(std::string("string"), "string: hello world")
        .SetParam("Event1_Key1", LLBC_Rand())
        .Fire();

    // Test finished, remove all listeners.
    evMgr.RemoveListener(ev1Stub1);
    evMgr.RemoveListener(ev1Stub2);
    evMgr.RemoveListener(EventIds::Event2);
    evMgr.RemoveAllListeners();

    LLBC_PrintLn("Basic test finished");

    return LLBC_OK;
}

int TestCase_Core_Event::InfiniteEventFireTest()
{
    LLBC_PrintLn("Infinite event fire test:");

    // Add event listeners.
    LLBC_EventMgr evMgr;
    evMgr.AddListener(EventIds::Event1, [&evMgr](LLBC_Event &ev) {
        LLBC_PrintLn("In Event1 handler, fire Event2...");
        evMgr.BeginFire(EventIds::Event2)
            .Fire();
    });
    evMgr.AddListener(EventIds::Event2, [&evMgr](LLBC_Event &ev) {
        LLBC_PrintLn("In Event2 handler, fire Event3...");
        evMgr.BeginFire(EventIds::Event3)
        .Fire();
    });
    evMgr.AddListener(EventIds::Event3, [&evMgr](LLBC_Event &ev) {
        LLBC_PrintLn("In Event3 handler, fire Event1 too...");
        auto ret = evMgr.BeginFire(EventIds::Event1)
            .Fire();
        if (ret != LLBC_FAILED && LLBC_GetLastError() != LLBC_ERROR_REPEAT)
        {
            LLBC_FilePrintLn(stderr,
                             "- Repeated fire Event1 return errno is not: <LLBC_ERROR_REPEAT>, ret:%d, errno:%d",
                             ret, LLBC_GetLastError());
        }
        else
        {
            LLBC_PrintLn("- Infinite event fire has been checked!");
        }
    });

    // Fire Event1.
    LLBC_PrintLn("Fire Event1...");
    evMgr.BeginFire(EventIds::Event1)
        .Fire();

    // Remove all listeners.
    evMgr.RemoveAllListeners();

    LLBC_PrintLn("Infinite event fire test finished");

    return LLBC_OK;
}

void TestCase_Core_Event::DumpEvParams(const LLBC_Event &ev)
{
    for(const auto &[key, value] : ev.GetParams())
        LLBC_PrintLn("key params - key: %s, value: %s", key.c_str(), value.ToString().c_str());

    LLBC_PrintLn("key params - params num: %lu", ev.GetParams().size());
}
