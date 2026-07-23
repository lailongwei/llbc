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

#include "comm/Example_Comm_PacketCoder.h"

namespace
{

const char *CODER_IP = "127.0.0.1";
const uint16 CODER_PORT = 34503;
const int OPCODE_LOGIN = 200;

// Custom Coder: encodes/decodes a business struct into/from a Packet.
// Inherits LLBC_Coder and implements Encode (write members into the packet) and Decode (read members back from the packet).
struct LoginReq final : public LLBC_Coder
{
    uint64 playerId = 0;
    LLBC_String playerName;
    int level = 0;

    LoginReq() = default;
    LoginReq(uint64 id, const LLBC_String &name, int lv)
    : playerId(id), playerName(name), level(lv) {}

    bool Encode(LLBC_Packet &packet) override
    {
        packet << playerId << playerName << level;
        return true;
    }

    bool Decode(LLBC_Packet &packet) override
    {
        packet >> playerId >> playerName >> level;
        return true;
    }

    LLBC_String ToString() const
    {
        return LLBC_String().format("LoginReq[id:%llu, name:%s, level:%d]",
                                    playerId, playerName.c_str(), level);
    }
};

// Coder factory: lets the Service auto-create a decoder by opcode to decode incoming packets.
class LoginReqFactory final : public LLBC_CoderFactory
{
public:
    LLBC_Coder *Create() const override { return new LoginReq; }
};

// Receiving component: fetches the framework-decoded object via GetDecoder<LoginReq>().
class CoderComp final : public LLBC_Component
{
public:
    // 0: waiting, 1: decoded as expected, -1: decoded but the values were unexpected.
    volatile sint32 decodeResult = 0;

public:
    int OnInit(bool &initFinished) override
    {
        GetService()->Subscribe(OPCODE_LOGIN, this, &CoderComp::OnLogin);
        return LLBC_OK;
    }

private:
    void OnLogin(LLBC_Packet &pkt)
    {
        LoginReq *req = pkt.GetDecoder<LoginReq>();
        const bool decodedAsExpected = req != nullptr &&
                                       req->playerId == 10086 &&
                                       req->playerName == "Nick" &&
                                       req->level == 7;
        if (req != nullptr)
            LLBC_PrintLn("  [handler] Decoded: %s", req->ToString().c_str());
        else
            LLBC_PrintLn("  [handler] LoginReq decoder is unavailable");

        LLBC_AtomicCompareAndExchange(&decodeResult, decodedAsExpected ? 1 : -1, 0);
    }
};

}

int Example_Comm_PacketCoder::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Comm PacketCoder example: custom Coder encoding/decoding");

    // ===== Part 1: local direct encode/decode (no networking, always runnable) =====
    LLBC_PrintLn("- Local direct Encode/Decode:");
    LLBC_Packet packet;
    LoginReq src(9527, "Judy", 42);
    src.Encode(packet); // write into the packet
    LLBC_PrintLn("  Before encoding: %s", src.ToString().c_str());

    LoginReq dst;
    dst.Decode(packet); // read back from the same packet
    LLBC_PrintLn("  After decoding: %s", dst.ToString().c_str());

    // ===== Part 2: the Coder flow when sending/receiving over the network (bounded, exits gracefully on failure) =====
    LLBC_PrintLn("- Coder flow over the network:");
    LLBC_Service *svc = LLBC_Service::Create("CoderSvc");
    CoderComp *comp = new CoderComp;
    svc->AddComponent(comp);
    // Register a Coder factory for the opcode; the framework auto-Decodes packets of that opcode on arrival.
    svc->AddCoderFactory(OPCODE_LOGIN, new LoginReqFactory);

    if (svc->Start() != LLBC_OK)
    {
        LLBC_PrintLn("  Failed to start Service, error:%s", LLBC_FormatLastError());
        delete svc;
        return LLBC_OK;
    }

    int listenSid = svc->Listen(CODER_IP, CODER_PORT);
    int connSid = listenSid != 0 ? svc->Connect(CODER_IP, CODER_PORT) : 0;
    if (listenSid == 0 || connSid == 0)
    {
        LLBC_PrintLn("  Failed to listen/connect (possibly blocked by the sandbox), error:%s", LLBC_FormatLastError());
        svc->Stop();
        delete svc;
        return LLBC_OK;
    }

    // Use an encoder when sending: Send(sessionId, opcode, coder); the coder is taken over and freed by the framework.
    if (svc->Send(connSid, OPCODE_LOGIN, new LoginReq(10086, "Nick", 7)) != LLBC_OK)
    {
        LLBC_PrintLn("  Failed to send LoginReq, error:%s", LLBC_FormatLastError());
        svc->Stop();
        delete svc;
        return LLBC_OK;
    }
    LLBC_PrintLn("  Sent LoginReq (encoded by the encoder)");

    for (int i = 0; i < 40 && LLBC_AtomicGet(&comp->decodeResult) == 0; ++i)
        LLBC_Sleep(50);

    const sint32 decodeResult = LLBC_AtomicGet(&comp->decodeResult);
    if (decodeResult > 0)
        LLBC_PrintLn("  Packet received and decoded successfully");
    else if (decodeResult < 0)
        LLBC_PrintLn("  Packet received, but decoded values did not match");
    else
        LLBC_PrintLn("  Timed out waiting for packet");

    svc->Stop();
    delete svc;

    return LLBC_OK;
}
