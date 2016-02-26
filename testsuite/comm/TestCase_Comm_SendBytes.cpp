/**
 * @file    TestCase_Comm_SendBytes.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/06
 * @version 1.0
 *
 * @brief
 */

#include "comm/TestCase_Comm_SendBytes.h"

namespace
{

const int OPCODE = 1;

class TestFacade : public LLBC_IFacade
{
public:
    virtual void OnInitialize()
    {
        LLBC_IService *svc = this->GetService();
        LLBC_PrintLine("Service create: %p", svc);
    }

    virtual void OnDestroy()
    {
        LLBC_IService *svc = this->GetService();
        LLBC_PrintLine("Service destroy: %p", svc);
    }

    virtual void OnUpdate()
    {
        // LLBC_IService *svc = this->GetService();
        // LLBC_PrintLine("Service update: %p", svc);
    }

public:
    void OnRecv(LLBC_Packet &packet)
    {
        const char *data = reinterpret_cast<char *>(packet.GetPayload());
        LLBC_PrintLine("Recved packet, data: %s", data);

        LLBC_IService *svc = this->GetService();
        svc->Send(packet.GetSessionId(), packet.GetOpcode(), "Hello, World!", 14, 0);
    }
};

}

TestCase_Comm_SendBytes::TestCase_Comm_SendBytes()
: _asClient(true)
, _svcType(LLBC_IService::Normal)

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

    this->FetchArgs(argc, argv);
    LLBC_IService *svc = LLBC_IService::Create(_svcType);

    TestFacade *facade = LLBC_New(TestFacade);
    svc->RegisterFacade(facade);

    svc->Subscribe(OPCODE, facade, &TestFacade::OnRecv);

    int sid = 0;
    if (_asClient)
    {
        sid = svc->Connect(_runIp.c_str(), _runPort);
        if (sid == 0)
        {
            LLBC_FilePrintLine(stderr, "connect to %s:%d failed, err: %s",
                _runIp.c_str(), _runPort, LLBC_FormatLastError());
            LLBC_Delete(svc);

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
            LLBC_Delete(svc);

            return LLBC_FAILED;
        }

        LLBC_PrintLine("server listen on %s:%d", _runIp.c_str(), _runPort);
    }

    svc->Start();

    if (_asClient)
    {
        LLBC_Packet *pkt = LLBC_New(LLBC_Packet);
        pkt->SetHeader(sid, OPCODE, 0);
        pkt->Write("Hello, world");
        pkt->Write(0);

        svc->Send(pkt);
    }

    LLBC_PrintLine("Press any key to continue...");
    getchar();

    LLBC_Delete(svc);

    return LLBC_OK;
}

void TestCase_Comm_SendBytes::FetchArgs(int argc, char *argv[])
{
    _asClient = LLBC_ToLower(argv[1]) == "client" ? true : false;
    _svcType = LLBC_ToLower(argv[2]) == "normal" ? LLBC_IService::Normal : LLBC_IService::Raw;

    _runIp = argv[3];
    _runPort = LLBC_Str2Int32(argv[4]);
}
