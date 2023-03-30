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


#include "comm/TestCase_Comm_Multicast.h"

namespace
{

const int OPCODE = 1;

class TestComp : public LLBC_Component
{
public:
    TestComp(bool asClient, bool useBst)
    : LLBC_Component(LLBC_ComponentEvents::DefaultEvents | LLBC_ComponentEvents::OnUpdate)
    , _asClient(asClient)
    , _useBst(useBst)
    {
    }

public:
    virtual bool OnInit(bool &initFinished)
    {
        LLBC_PrintLn("Service created!");
        return true;
    }

    virtual void OnDestroy(bool &destroyFinished)
    {
        LLBC_PrintLn("Service destroy!");
    }

    virtual void OnSessionCreate(const LLBC_SessionInfo &sessionInfo)
    {
        if (sessionInfo.IsListenSession())
            return;

        _sessionIds.push_back(sessionInfo.GetSessionId());
    }

    virtual void OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo)
    {
        int sessionId = destroyInfo.GetSessionId();
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

        LLBC_Service *svc = GetService();
        if (_useBst)
            svc->Broadcast(OPCODE, "Hello world!", 14, 0);
        else
            svc->Multicast(_sessionIds, OPCODE, "Hello, world!", 14, 0);
    }

public:
    virtual void OnRecv(LLBC_Packet &packet)
    {
        const int sessionId = packet.GetSessionId();
        const char *data = reinterpret_cast<const char *>(packet.GetPayload());

        LLBC_PrintLn("Session[%4d] received data: %s", sessionId, data);
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
, _useNmlProtocolFactory(true)

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
    LLBC_PrintLn("Service multicast test:");
    if (argc < 5)
    {
        LLBC_FilePrintLn(stderr, "Argument error, eg: ./a [client/server], [normal/raw] ip port [useBst=False]");
        return LLBC_FAILED;
    }

    // Fetch arguments.
    FetchArgs(argc, argv);

    LLBC_IProtocolFactory *protoFactory;
    if (_useNmlProtocolFactory)
        protoFactory = new LLBC_NormalProtocolFactory;
    else
        protoFactory = new LLBC_RawProtocolFactory;
    LLBC_Service *svc = LLBC_Service::Create("MulticastTest", protoFactory);
    svc->SuppressCoderNotFoundWarning();

    TestComp *comp = new TestComp(_asClient, _useBst);
    svc->AddComponent(comp);
    svc->Subscribe(OPCODE, comp, &TestComp::OnRecv);

    if (_asClient)
        PrepareClientLogic(svc);
    else
        PrepareServerLogic(svc);

    svc->SetFPS(10);
    svc->Start();

    LLBC_PrintLn("Press any key to continue...");
    getchar();

    delete svc;

    return LLBC_OK;
}

void TestCase_Comm_Multicast::FetchArgs(int argc, char *argv[])
{
    _asClient = LLBC_ToLower(argv[1]) == "client" ? true : false;
    _useNmlProtocolFactory = LLBC_ToLower(argv[2]) == "normal" ? true : false;

    _runIp = argv[3];
    _runPort = LLBC_Str2Int32(argv[4]);

    if (argc >= 6)
        _useBst = LLBC_ToLower(argv[5]) == "true" ? true : false;
}

int TestCase_Comm_Multicast::PrepareClientLogic(LLBC_Service *svc)
{
    for (int i = 0; i < _clientCnt; ++i)
    {
        const int sessionId = svc->Connect(_runIp.c_str(), _runPort);
        if (sessionId == 0)
        {
            LLBC_PrintLn("Connect to %s:%d failed, err: %s",
                _runIp.c_str(), _runPort, LLBC_FormatLastError());
            return LLBC_FAILED;
        }

        LLBC_PrintLn("Connect to %s:%d succeed, sid: %d",
            _runIp.c_str(), _runPort, sessionId);
    }

    return LLBC_OK;
}

int TestCase_Comm_Multicast::PrepareServerLogic(LLBC_Service *svc)
{
    const int sessionId = svc->Listen(_runIp.c_str(), _runPort);
    if (sessionId == 0)
    {
        LLBC_FilePrintLn(stderr,
                         "Listen on %s:%d failed, err: %s",
                         _runIp.c_str(), _runPort, LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    LLBC_PrintLn("Server listening on %s:%d...", _runIp.c_str(), _runPort);

    return LLBC_OK;
}
