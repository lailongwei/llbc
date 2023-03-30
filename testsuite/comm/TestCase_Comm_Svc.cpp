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


#include "comm/TestCase_Comm_Svc.h"

namespace
{

const int OPCODE = 0;

class TestComp : public LLBC_Component
{
public:
    TestComp()
    {
        _recvBytes = 0;
        _prevRecv100MB = 0;
        
        _packets = 0;
    }

public:
    virtual bool OnInit(bool &initFinished)
    {
        LLBC_PrintLn("Service create!");
        return true;
    }

    virtual void OnDestroy(bool &destroyFinished)
    {
        LLBC_PrintLn("Service destroy!");
    }

public:
    virtual void OnSessionCreate(const LLBC_SessionInfo &sessionInfo)
    {
        LLBC_PrintLn("Session Create: %s", sessionInfo.ToString().c_str());
    }

    virtual void OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo)
    {
        LLBC_PrintLn("Session Destroy, info: %s", destroyInfo.ToString().c_str());
    }

    virtual void OnAsyncConnResult(const LLBC_AsyncConnResult &result)
    {
        LLBC_PrintLn("Async-Conn result: %s", result.ToString().c_str());
    }

    virtual void OnUnHandledPacket(const LLBC_Packet &packet)
    {
        LLBC_PrintLn("Unhandled packet, sessionId: %d, opcode: %d, payloadLen: %ld",
            packet.GetSessionId(), packet.GetOpcode(), packet.GetPayloadLength());
    }

    virtual void OnProtoReport(const LLBC_ProtoReport &report)
    {
        LLBC_PrintLn("Proto report: %s", report.ToString().c_str());
    }

public:
    void OnDataArrival(LLBC_Packet &packet)
    {
        if ((_packets += 1) % 1000 == 0)
            LLBC_PrintLn("[%s]Received %ld packets!", LLBC_Time::Now().ToString().c_str(), _packets);

        _recvBytes += packet.GetPayloadLength();
        uint64 recv100MB = _recvBytes / (100 * 1024 * 1024);
        if (recv100MB > _prevRecv100MB)
        {
            LLBC_PrintLn("[%s]Received %llu MB data!", LLBC_Time::Now().ToString().c_str(), _recvBytes / (1024 * 1024));
            _prevRecv100MB = recv100MB;
        }

        // LLBC_Packet *resPacket = new LLBC_Packet;
        LLBC_Packet *resPacket = GetService()->GetPacketObjectPool().GetObject();
        resPacket->SetHeader(packet, OPCODE, 0);
        resPacket->Write(packet.GetPayload(), packet.GetPayloadLength());

        GetService()->Send(resPacket);
    }

private:
    uint64 _recvBytes;
    uint64 _prevRecv100MB;

    size_t _packets;
};

}

TestCase_Comm_Svc::TestCase_Comm_Svc()
{
}

TestCase_Comm_Svc::~TestCase_Comm_Svc()
{
}

int TestCase_Comm_Svc::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Server/Client test:");
    if (argc < 5)
    {
        LLBC_PrintLn("argument error, eg: ./a [client/server] [normal/raw] ip port");
        return -1;
    }

    // Parse arguments.
    const char *ip = argv[3];
    const int port = LLBC_Str2Int32(argv[4]);
    const bool asClient = LLBC_String(argv[1]) == "client" ? true : false;
    LLBC_IProtocolFactory *protoFactory;
    if (LLBC_String(argv[2]) == "normal")
        protoFactory = new LLBC_NormalProtocolFactory;
    else
        protoFactory = new LLBC_RawProtocolFactory;
    LLBC_PrintLn("Will start %s type service, service type: %s",
        asClient ? "CLIENT" : "SERVER",
        LLBC_String(argv[2]) == "normal" ? "Normal" : "Raw");

    // Create service
    LLBC_Service *svc = LLBC_Service::Create("SvcTest", protoFactory);
    TestComp *comp = new TestComp;
    svc->AddComponent(comp);
    svc->Subscribe(OPCODE, comp, &TestComp::OnDataArrival);
    svc->SuppressCoderNotFoundWarning();
    svc->Start(8);

    // Connect to server / Create listen session to wait client connect.
    LLBC_SessionOpts sessionOpts;
    // sessionOpts.SetSockSendBufSize(1 * 1024 * 1024);
    // sessionOpts.SetSockRecvBufSize(1 * 1024 * 1024);
    if (!asClient)
    {
        LLBC_PrintLn("Will listening in %s:%d", ip, port);
        int sessionId = svc->Listen(ip, port, nullptr, sessionOpts);
        if (sessionId == 0)
        {
            LLBC_PrintLn("Create session failed, reason: %s", LLBC_FormatLastError());
            delete svc;

            return -1;
        }
    }
    else
    {
        // Client service, we create some clients to test service.
        int clientCount;
        const int pollerType = LLBC_PollerType::Str2Type(LLBC_CFG_COMM_POLLER_MODEL);
        if (pollerType == LLBC_PollerType::SelectPoller)
            clientCount = 50;
        else
            clientCount = 200;

        LLBC_PrintLn("Create %d clients to test", clientCount);
        for (int i = 0; i < clientCount; ++i)
        {
            int sessionId = svc->Connect(ip, port, -1, nullptr, sessionOpts);
            if (sessionId == 0)
            {
                LLBC_PrintLn("Connect to %s:%d failed, err:%s", ip, port, LLBC_FormatLastError());
                continue;
            }

            const int dataSize = 512 * 1024;
            char *data = LLBC_Malloc(char, dataSize);
            memset(data, 1, dataSize);

            // LLBC_Packet *packet = new LLBC_Packet;
            LLBC_Packet *packet = svc->GetPacketObjectPool().GetObject();
            packet->SetHeader(sessionId, OPCODE, 0);
            packet->Write(data, dataSize);

            free(data);

            svc->Send(packet);

            // Test unhandled packet(unsubscribe opcode).
            // LLBC_Packet *unhandledPacket = new LLBC_Packet;
            LLBC_Packet *unhandledPacket = svc->GetPacketObjectPool().GetObject();
            unhandledPacket->SetHeader(sessionId, OPCODE + 10000, 0);
            unhandledPacket->Write("Hello World", 12);

            svc->Send(unhandledPacket);
        }
    }

    LLBC_PrintLn("Press any key to continue...");
    getchar();

    delete svc;

    return 0;
}
