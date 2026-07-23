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

#include "comm/Example_Comm_ServiceMessaging.h"

namespace
{

const char *MSG_IP = "127.0.0.1";
const uint16 MSG_PORT = 34502;

// Uses the library's default NormalProtocolFactory (opcode-based packet splitting), so real business opcodes can be used for routing.
// Different opcodes can be registered to different handlers, achieving dispatch by message number.
const int OPCODE_PING = 100;

// This component self-listens and self-connects within the same Service, demonstrating send/receive by opcode.
class MessagingComp final : public LLBC_Component
{
public:
    volatile sint32 gotPing = 0;

public:
    int OnInit(bool &initFinished) override
    {
        // Subscribe a handler by opcode.
        GetService()->Subscribe(OPCODE_PING, this, &MessagingComp::OnPing);
        return LLBC_OK;
    }

private:
    void OnPing(LLBC_Packet &pkt)
    {
        LLBC_String body(reinterpret_cast<const char *>(pkt.GetPayload()), pkt.GetPayloadLength());
        LLBC_PrintLn("  [handler] Received opcode:%d, session:%d, body:%s",
                     pkt.GetOpcode(), pkt.GetSessionId(), body.c_str());
        LLBC_AtomicCompareAndExchange(&gotPing, 1, 0);
    }
};

}

int Example_Comm_ServiceMessaging::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Comm ServiceMessaging example: register a handler by opcode and send/receive a message");

    // The default protocol factory is NormalProtocolFactory, so it need not be passed explicitly here.
    LLBC_Service *svc = LLBC_Service::Create("MsgSvc");
    // This example sends raw bytes rather than registering a Coder for OPCODE_PING.
    // Without this setting, the default CodecProtocol drops packets whose opcode has no CoderFactory.
    svc->SuppressCoderNotFoundWarning();
    MessagingComp *comp = new MessagingComp;
    svc->AddComponent(comp);

    if (svc->Start() != LLBC_OK)
    {
        LLBC_PrintLn("- Failed to start Service, error:%s", LLBC_FormatLastError());
        delete svc;
        return LLBC_OK;
    }

    // Self-listen + self-connect (loopback) within the same Service, to demonstrate message routing in a single process.
    int listenSid = svc->Listen(MSG_IP, MSG_PORT);
    if (listenSid == 0)
    {
        LLBC_PrintLn("- Failed to listen on %s:%d (possibly blocked by the sandbox), error:%s", MSG_IP, MSG_PORT, LLBC_FormatLastError());
        svc->Stop();
        delete svc;
        return LLBC_OK;
    }

    int connSid = svc->Connect(MSG_IP, MSG_PORT);
    if (connSid == 0)
    {
        LLBC_PrintLn("- Failed to connect to %s:%d, error:%s", MSG_IP, MSG_PORT, LLBC_FormatLastError());
        svc->Stop();
        delete svc;
        return LLBC_OK;
    }
    LLBC_PrintLn("- Loopback connection established: listenSid:%d, connSid:%d", listenSid, connSid);

    // Send a message with opcode=OPCODE_PING to the peer.
    const char *body = "ping payload";
    if (svc->Send(connSid, OPCODE_PING, body, strlen(body)) != LLBC_OK)
    {
        LLBC_PrintLn("- Failed to send message, error:%s", LLBC_FormatLastError());
        svc->Stop();
        delete svc;
        return LLBC_OK;
    }
    LLBC_PrintLn("- Sent opcode:%d, body:%s", OPCODE_PING, body);

    // Bounded wait for the handler to receive it (about 2 seconds at most).
    for (int i = 0; i < 40 && LLBC_AtomicGet(&comp->gotPing) == 0; ++i)
        LLBC_Sleep(50);

    LLBC_PrintLn(LLBC_AtomicGet(&comp->gotPing) != 0 ? "- Message routing succeeded" : "- Timed out waiting for message");

    svc->Stop();
    delete svc;

    return LLBC_OK;
}
