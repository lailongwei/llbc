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


#include "comm/TestCase_Comm_SendBytes.h"

namespace
{

const int OPCODE = 0;

class TestComp : public LLBC_Component
{
public:
    virtual bool OnInitialize(bool &initFinished)
    {
        LLBC_IService *svc = GetService();
        LLBC_PrintLine("Service create: %p", svc);

        return true;
    }

    virtual void OnDestroy(bool &destroyFinished)
    {
        LLBC_IService *svc = GetService();
        LLBC_PrintLine("Service destroy: %p", svc);
    }

    virtual void OnUpdate()
    {
        // LLBC_IService *svc = GetService();
        // LLBC_PrintLine("Service update: %p", svc);
    }

public:
    void OnRecv(LLBC_Packet &packet)
    {
        const char *data = reinterpret_cast<const char *>(packet.GetPayload());
        LLBC_PrintLine("Recved packet, data: %s", data);

        LLBC_IService *svc = GetService();
        svc->Send(packet.GetSessionId(), packet.GetOpcode(), "Hello, World!", 14, 0);
    }
};

}

TestCase_Comm_SendBytes::TestCase_Comm_SendBytes()
: _asClient(true)
, _useNmlProtocolFactory(true)

, _runIp("127.0.0.1")
, _runPort(0)
{
}

TestCase_Comm_SendBytes::~TestCase_Comm_SendBytes()
{
}

int TestCase_Comm_SendBytes::Run(int argc, char *argv[])
{
    LLBC_PrintLine("Servie send bytes test:");
    if (argc < 5)
    {
        LLBC_PrintLine("argument error, eg: ./a [client/server] [normal/raw] ip port");
        return LLBC_FAILED;
    }

    FetchArgs(argc, argv);
    LLBC_IProtocolFactory *protoFactory;
    if (_useNmlProtocolFactory)
        protoFactory = new LLBC_NormalProtocolFactory;
    else
        protoFactory = new LLBC_RawProtocolFactory;
    LLBC_IService *svc = LLBC_IService::Create("SendBytesTest", protoFactory);
    svc->SuppressCoderNotFoundWarning();

    TestComp *comp = new TestComp;
    svc->AddComponent(comp);

    svc->Subscribe(OPCODE, comp, &TestComp::OnRecv);

    int sid = 0;
    if (_asClient)
    {
        sid = svc->Connect(_runIp.c_str(), _runPort);
        if (sid == 0)
        {
            LLBC_FilePrintLine(stderr, "connect to %s:%d failed, err: %s",
                _runIp.c_str(), _runPort, LLBC_FormatLastError());
            delete svc;

            return LLBC_FAILED;
        }

        LLBC_PrintLine("server connect to %s:%d success", _runIp.c_str(), _runPort);
    }
    else
    {
        sid = svc->Listen(_runIp.c_str(), _runPort);
        if (sid == 0)
        {
            LLBC_FilePrintLine(stderr, "failed to listen on %s:%d, err: %s",
                _runIp.c_str(), _runPort, LLBC_FormatLastError());
            delete svc;

            return LLBC_FAILED;
        }

        LLBC_PrintLine("server listen on %s:%d", _runIp.c_str(), _runPort);
    }

    svc->Start();

    if (_asClient)
    {
        LLBC_Packet *pkt = new LLBC_Packet;
        pkt->SetHeader(sid, OPCODE, 0);
        pkt->Write("Hello, world");
        pkt->Write(0);

        svc->Send(pkt);
    }

    LLBC_PrintLine("Press any key to continue...");
    getchar();

    delete svc;

    return LLBC_OK;
}

void TestCase_Comm_SendBytes::FetchArgs(int argc, char *argv[])
{
    _asClient = LLBC_ToLower(argv[1]) == "client" ? true : false;
    _useNmlProtocolFactory = LLBC_ToLower(argv[2]) == "normal" ? true : false;

    _runIp = argv[3];
    _runPort = LLBC_Str2Int32(argv[4]);
}
