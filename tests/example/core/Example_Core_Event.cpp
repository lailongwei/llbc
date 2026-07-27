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

#include "core/Example_Core_Event.h"

namespace
{
    // Custom event Id.
    enum EventIds
    {
        PlayerLogin = 1,
    };
}

int Example_Core_Event::Run(int argc, char *argv[])
{
    // Event manager: handles listener registration/removal and event dispatch.
    LLBC_EventMgr evMgr;

    // ========================= Register listener =========================
    // AddListener() returns a stub that can later be used to remove that exact listener.
    LLBC_PrintLn("Registering a PlayerLogin event listener");
    LLBC_ListenerStub stub = evMgr.AddListener(EventIds::PlayerLogin, [](LLBC_Event &ev) {
        // Inside the listener, read event-carried parameters (LLBC_Variant) via GetParam(key).
        LLBC_PrintLn("- [listener] Received PlayerLogin event, eventId:%d", ev.GetId());
        LLBC_PrintLn("  - playerId: %llu", ev.GetParam("playerId").As<uint64>());
        LLBC_PrintLn("  - playerName: %s", ev.GetParam("playerName").As<LLBC_String>().c_str());
        LLBC_PrintLn("  - level: %d", ev.GetParam("level").As<int>());
    });

    // ========================= Fire event =========================
    // BeginFire() starts building an event, chained SetParam() attaches parameters, Fire() dispatches to all listeners.
    LLBC_PrintLn("Firing PlayerLogin event");
    evMgr.BeginFire(EventIds::PlayerLogin)
         .SetParam("playerId", 9527llu)
         .SetParam("playerName", "Judy")
         .SetParam("level", 99)
         .Fire();

    // ========================= Remove listener =========================
    // Remove the listener using the previously saved stub; firing again afterwards gets no response.
    LLBC_PrintLn("Removing listener and firing again (no output expected)");
    evMgr.RemoveListener(stub);
    evMgr.BeginFire(EventIds::PlayerLogin)
         .SetParam("playerId", 10000llu)
         .Fire();

    LLBC_PrintLn("Event example complete");

    return LLBC_OK;
}
