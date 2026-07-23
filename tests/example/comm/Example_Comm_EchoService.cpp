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

#include "comm/Example_Comm_EchoService.h"

namespace
{

// Loopback address and port used by this example (adjust if already in use).
const char *ECHO_IP = "127.0.0.1";
const uint16 ECHO_PORT = 34501;
// Echo uses RawProtocolFactory (raw stream protocol), whose opcode is fixed to 0.
const int ECHO_OPCODE = 0;

// Server-side component: listens and echoes received data back to the client verbatim.
class EchoServerComp final : public LLBC_Component
{
public:
    int OnInit(bool &initFinished) override
    {
        // Subscribe to opcode 0 packets; dispatch them to _OnPkt on arrival.
        GetService()->Subscribe(ECHO_OPCODE, this, &EchoServerComp::_OnPkt);
        return LLBC_OK;
    }

private:
    void _OnPkt(LLBC_Packet &pkt)
    {
        LLBC_String msg(reinterpret_cast<const char *>(pkt.GetPayload()), pkt.GetPayloadLength());
        LLBC_PrintLn("  [Server] Received data on session[%d]: %s", pkt.GetSessionId(), msg.c_str());
        // Echo verbatim back to the same session.
        GetService()->Send(pkt.GetSessionId(), ECHO_OPCODE, pkt.GetPayload(), pkt.GetPayloadLength());
    }
};

// Client-side component: records the result after receiving the echo (does not send back, avoiding an infinite loop).
class EchoClientComp final : public LLBC_Component
{
public:
    // Polled by Run() to check whether the echo has been received.
    volatile sint32 gotEcho = 0;
    LLBC_String echoMsg;

public:
    int OnInit(bool &initFinished) override
    {
        GetService()->Subscribe(ECHO_OPCODE, this, &EchoClientComp::_OnPkt);
        return LLBC_OK;
    }

private:
    void _OnPkt(LLBC_Packet &pkt)
    {
        echoMsg.assign(reinterpret_cast<const char *>(pkt.GetPayload()), pkt.GetPayloadLength());
        LLBC_PrintLn("  [Client] Received echo: %s", echoMsg.c_str());
        LLBC_AtomicCompareAndExchange(&gotEcho, 1, 0);
    }
};

}

int Example_Comm_EchoService::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Comm EchoService example: single-process server/client echo demonstration");

    // ========== Server ==========
    // Each Service needs its own protocol factory instance (the factory is taken over and freed by the Service).
    LLBC_Service *server = LLBC_Service::Create("EchoServer", new LLBC_RawProtocolFactory);
    server->AddComponent(new EchoServerComp);
    if (server->Start() != LLBC_OK)
    {
        LLBC_PrintLn("- Failed to start server, error:%s", LLBC_FormatLastError());
        delete server;
        return LLBC_OK;
    }

    // Listen on the port. Listen returns the listen session Id; 0 means failure.
    int listenSid = server->Listen(ECHO_IP, ECHO_PORT);
    if (listenSid == 0)
    {
        // A sandboxed environment may forbid binding ports; exit gracefully here rather than hang.
        LLBC_PrintLn("- Failed to listen on %s:%d (possibly blocked by the sandbox), error:%s", ECHO_IP, ECHO_PORT, LLBC_FormatLastError());
        server->Stop();
        delete server;
        return LLBC_OK;
    }
    LLBC_PrintLn("- Server listening on %s:%d, sessionId:%d", ECHO_IP, ECHO_PORT, listenSid);

    // ========== Client ==========
    LLBC_Service *client = LLBC_Service::Create("EchoClient", new LLBC_RawProtocolFactory);
    EchoClientComp *clientComp = new EchoClientComp;
    client->AddComponent(clientComp);
    if (client->Start() != LLBC_OK)
    {
        LLBC_PrintLn("- Failed to start client, error:%s", LLBC_FormatLastError());
        delete client;
        server->Stop();
        delete server;
        return LLBC_OK;
    }

    // Connect to the server. Connect is a synchronous connect; 0 means failure.
    int connSid = client->Connect(ECHO_IP, ECHO_PORT);
    if (connSid == 0)
    {
        LLBC_PrintLn("- Failed to connect to %s:%d, error:%s", ECHO_IP, ECHO_PORT, LLBC_FormatLastError());
        client->Stop();
        delete client;
        server->Stop();
        delete server;
        return LLBC_OK;
    }
    LLBC_PrintLn("- Client connected, sessionId:%d", connSid);

    // Send one piece of data. Send(sessionId, opcode, bytes, len).
    const char *hello = "Hello, llbc echo!";
    client->Send(connSid, ECHO_OPCODE, hello, strlen(hello));
    LLBC_PrintLn("- Client sent: %s", hello);

    // Bounded wait for the echo: wait about 2 seconds at most.
    for (int i = 0; i < 40 && LLBC_AtomicGet(&clientComp->gotEcho) == 0; ++i)
        LLBC_Sleep(50);

    if (LLBC_AtomicGet(&clientComp->gotEcho) != 0)
        LLBC_PrintLn("- Echo succeeded: %s", clientComp->echoMsg.c_str());
    else
        LLBC_PrintLn("- Timed out waiting for echo");

    // Cleanup: client first, then server.
    client->Stop();
    delete client;
    server->Stop();
    delete server;

    return LLBC_OK;
}
