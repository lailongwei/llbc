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

#include "comm/Example_Comm_Session.h"

namespace
{

const char *SESS_IP = "127.0.0.1";
const uint16 SESS_PORT = 34504;

// Observe session creation and destruction via OnEvent (non-flow events all go through OnEvent).
class SessionWatchComp final : public LLBC_Component
{
public:
    void OnEvent(int eventType, const LLBC_Variant &eventParams) override
    {
        switch (eventType)
        {
            case LLBC_ComponentEventType::SessionCreate:
            {
                const LLBC_SessionInfo &info = *eventParams.As<LLBC_SessionInfo *>();
                LLBC_PrintLn("  [event] SessionCreate: sessionId:%d, isListen:%s",
                             info.GetSessionId(), info.IsListenSession() ? "true" : "false");
                break;
            }
            case LLBC_ComponentEventType::SessionDestroy:
            {
                const LLBC_SessionDestroyInfo &info = *eventParams.As<LLBC_SessionDestroyInfo *>();
                LLBC_PrintLn("  [event] SessionDestroy: sessionId:%d, reason:%s",
                             info.GetSessionId(), info.GetReason().c_str());
                break;
            }
            default:
                break;
        }
    }
};

}

int Example_Comm_Session::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Comm Session example: listen, connect, disconnect, and session events");

    LLBC_Service *svc = LLBC_Service::Create("SessionSvc");
    svc->AddComponent(new SessionWatchComp);
    if (svc->Start() != LLBC_OK)
    {
        LLBC_PrintLn("- Failed to start Service, error:%s", LLBC_FormatLastError());
        delete svc;
        return LLBC_OK;
    }

    // Listen: returns the listen session Id.
    int listenSid = svc->Listen(SESS_IP, SESS_PORT);
    if (listenSid == 0)
    {
        LLBC_PrintLn("- Failed to listen on %s:%d (possibly blocked by the sandbox), error:%s", SESS_IP, SESS_PORT, LLBC_FormatLastError());
        svc->Stop();
        delete svc;
        return LLBC_OK;
    }
    LLBC_PrintLn("- Listen session established, sessionId:%d", listenSid);

    // Connect (loopback): returns the connection session Id.
    int connSid = svc->Connect(SESS_IP, SESS_PORT);
    if (connSid == 0)
    {
        LLBC_PrintLn("- Failed to connect, error:%s", LLBC_FormatLastError());
        svc->Stop();
        delete svc;
        return LLBC_OK;
    }
    LLBC_PrintLn("- Connection session established, sessionId:%d", connSid);

    // Wait for the session-create event of the server-accepted session to be dispatched (bounded).
    LLBC_Sleep(300);

    // Query whether the session is valid.
    LLBC_PrintLn("- IsSessionValidate(connSid:%d) = %s",
                 connSid, svc->IsSessionValidate(connSid) ? "true" : "false");

    // Actively disconnect the connection session, with a disconnect reason (triggers the SessionDestroy event).
    LLBC_PrintLn("- Disconnecting sessionId:%d", connSid);
    svc->RemoveSession(connSid, "example finished");

    // Wait for the destroy event to be dispatched (bounded).
    LLBC_Sleep(300);

    svc->Stop();
    delete svc;

    return LLBC_OK;
}
