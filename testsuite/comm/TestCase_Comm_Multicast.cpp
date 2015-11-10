/**
 * @file    TestCase_Comm_Multicast.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/06
 * @version 1.0
 *
 * @brief
 */

#include "comm/TestCase_Comm_Multicast.h"

namespace
{

const int OPCODE = 1;

class TestFacade : public LLBC_IFacade
{
public:
    TestFacade(bool asClient, bool useBst)
    : _asClient(asClient)
    , _useBst(useBst)
    {
    }

public:
    virtual void OnInitialize()
    {
        LLBC_PrintLine("Service created!");
    }

    virtual void OnDestroy()
    {
        LLBC_PrintLine("Service destroy!");
    }

    virtual void OnSessionCreate(const LLBC_SessionInfo &sessionInfo)
    {
        _sessionIds.push_back(sessionInfo.GetSessionId());
    }

    virtual void OnSessionDestroy(int sessionId)
    {
        for (LLBC_SessionIdListIter it = _sessionIds.begin();
             it != _sessionIds.end();
             it++)
            if (*it == sessionId)
            {
                _sessionIds.erase(it);
                break;
            }
    }

    virtual void OnUpdate()
    {
        if (_asClient)
            return;

        LLBC_IService *svc = this->GetService();
        if (_useBst)
            svc->Broadcast(OPCODE, "Hello world!", 14, 0);
        else
            svc->Multicast(_sessionIds, OPCODE, "Hello, world!", 14, 0);
    }

public:
    virtual void OnRecv(LLBC_Packet &packet)
    {
        const int sessionId = packet.GetSessionId();
        const char *data = reinterpret_cast<char *>(packet.GetPayload());

        LLBC_PrintLine("Session[%4d] received data: %s", sessionId, data);
    }

private:
    bool _asClient;
    bool _useBst;

    LLBC_SessionIdList _sessionIds;
};

}

const int TestCase_Comm_Multicast::_clientCnt = 50;

TestCase_Comm_Multicast::TestCase_Comm_Multicast()
: _asClient(true)
, _svcType(LLBC_IService::Normal)

, _runIp("127.0.0.1")
, _runPort(0)

, _useBst(false)
{
}

TestCase_Comm_Multicast::~TestCase_Comm_Multicast()
{
}

int TestCase_Comm_Multicast::Run(int argc, char *argv[])
{
    LLBC_PrintLine("Service multicast test:");
    if (argc < 5)
    {
        LLBC_FilePrintLine(stderr, "Argument error, eg: ./a [client/server], [normal/raw] ip port [useBst=False]");
        return LLBC_RTN_FAILED;
    }

    // Fetch arguments.
    this->FetchArgs(argc, argv);

    LLBC_IService *svc = LLBC_IService::Create(_svcType);

    TestFacade *facade = LLBC_New2(TestFacade, _asClient, _useBst);
    svc->RegisterFacade(facade);
    svc->Subscribe(OPCODE, facade, &TestFacade::OnRecv);

    if (_asClient)
        this->PrepareClientLogic(svc);
    else
        this->PrepareServerLogic(svc);

    svc->Start();

    LLBC_PrintLine("Press any key to continue...");
    getchar();

    LLBC_Delete(svc);

    return LLBC_RTN_OK;
}

void TestCase_Comm_Multicast::FetchArgs(int argc, char *argv[])
{
    _asClient = LLBC_ToLower(argv[1]) == "client" ? true : false;
    _svcType = LLBC_ToLower(argv[2]) == "normal" ? LLBC_IService::Normal : LLBC_IService::Raw;

    _runIp = argv[3];
    _runPort = LLBC_Str2Int32(argv[4]);

    if (argc >= 6)
        _useBst = LLBC_ToLower(argv[5]) == "true" ? true : false;
}

int TestCase_Comm_Multicast::PrepareClientLogic(LLBC_IService *svc)
{
    for (int i = 0; i < _clientCnt; i++)
    {
        const int sessionId = svc->Connect(_runIp.c_str(), _runPort);
        if (sessionId == 0)
        {
            LLBC_PrintLine("Connect to %s:%d failed, err: %s",
                _runIp.c_str(), _runPort, LLBC_FormatLastError());
            return LLBC_RTN_FAILED;
        }

        LLBC_PrintLine("Connect to %s:%d succeed, sid: %d",
            _runIp.c_str(), _runPort, sessionId);
    }

    return LLBC_RTN_OK;
}

int TestCase_Comm_Multicast::PrepareServerLogic(LLBC_IService *svc)
{
    const int sessionId = svc->Listen(_runIp.c_str(), _runPort);
    if (sessionId == 0)
    {
        LLBC_FilePrintLine(stderr, "Listen on %s:%d failed, err: %s",
            _runIp.c_str(), _runPort, LLBC_FormatLastError());
        return LLBC_RTN_FAILED;
    }

    LLBC_PrintLine("Server listening on %s:%d...", _runIp.c_str(), _runPort);

    return LLBC_RTN_OK;
}
