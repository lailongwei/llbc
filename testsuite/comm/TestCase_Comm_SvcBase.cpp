/**
 * @file    TestCase_Comm_SvcBase.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/26
 * @version 1.0
 *
 * @brief
 */

#include "comm/TestCase_Comm_SvcBase.h"

namespace
{

struct TestData : public LLBC_ICoder
{
    int iVal;
    LLBC_String strVal;

    virtual bool Encode(LLBC_Packet &packet)
    {
        packet <<iVal <<strVal;
        return true;
    }

    virtual bool Decode(LLBC_Packet &packet)
    {
        packet >>iVal >>strVal;
        return true;
    }
};

class TestDataFactory : public LLBC_ICoderFactory
{
public:
    virtual LLBC_ICoder *Create() const
    {
        return LLBC_New(TestData);
    }
};

class TestFacade : public LLBC_IFacade
{
public:
    virtual void OnInitialize()
    {
        LLBC_PrintLine("Service initialize");
    }

    virtual void OnDestroy()
    {
        LLBC_PrintLine("Service Destroy");
    }

    virtual void OnStart()
    {
        LLBC_PrintLine("Service start");
    }

    virtual void OnStop()
    {
        LLBC_PrintLine("Service stop");
    }

public:
    virtual void OnUpdate()
    {
        int fps = LLBC_Random::RandInt32cmon(20, 60);
        // LLBC_PrintLine("Service update, set fps to %d", fps);

        GetService()->SetFPS(fps);
    }

    virtual void OnIdle(int idleTime)
    {
        // LLBC_PrintLine("Service idle, idle time: %d", idleTime);
    }

    virtual void OnSessionCreate(const LLBC_SessionInfo &sessionInfo)
    {
        LLBC_PrintLine("SessionCreate: %s", sessionInfo.ToString().c_str());
        // std::cout <<"Session Create: " <<si <<std::endl;
    }

    virtual void OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo)
    {
        LLBC_PrintLine("Session destroy, info: %s", destroyInfo.ToString().c_str());
    }

    virtual void OnAsyncConnResult(const LLBC_AsyncConnResult &result)
    {
        LLBC_PrintLine("Async-Conn result: %s", result.ToString().c_str());
        // std::cout <<"Async-Conn result: " <<result <<std::endl;
    }

    virtual void OnProtoReport(const LLBC_ProtoReport &report)
    {
        LLBC_PrintLine("Protocol report: %s", report.ToString().c_str());
        // std::cout <<"Protocol report: " <<report <<std::endl;
    }

public:
    void OnRecvData(LLBC_Packet &packet)
    {
        TestData *data = (TestData *)packet.GetDecoder();
        LLBC_PrintLine("Receive data[%d], iVal: %d, strVal: %s", 
            packet.GetSessionId(), data->iVal, data->strVal.c_str());

        TestData *resData = LLBC_New(TestData);
        *resData = *data;

        LLBC_Packet *resPacket = LLBC_New(LLBC_Packet);
        resPacket->SetHeader(packet, packet.GetOpcode(), 0);
        resPacket->SetEncoder(resData);

        GetService()->Send(resPacket);
    }

    bool OnPreRecvData(LLBC_Packet &packet)
    {
        TestData *data = (TestData *)packet.GetDecoder();
        LLBC_PrintLine("Pre-Receive data[%d], iVal: %d, strVal: %s",
            packet.GetSessionId(), data->iVal, data->strVal.c_str());

        packet.SetPreHandleResult(LLBC_Malloc(char, 4096), this, &TestFacade::DelPreHandleResult);

        return true;
    }

    bool OnUnifyPreRecvData(LLBC_Packet &packet)
    {
        LLBC_PrintLine("Unify Pre-Receive data[%d]", packet.GetSessionId());

        packet.SetPreHandleResult(LLBC_Malloc(char, 4096), this, &TestFacade::DelPreHandleResult);

        return true;
    }

public:
    void OnStatus_1(LLBC_Packet &packet)
    {
        LLBC_PrintLine("Recv status != 0 packet, op: %d, status: %d",
            packet.GetOpcode(), packet.GetStatus());
#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
        LLBC_PrintLine("The status desc: %s", packet.GetStatusDesc().c_str());
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC
    }

private:
    void DelPreHandleResult(void *result)
    {
        LLBC_Free(result);
    }
};

}

TestCase_Comm_SvcBase::TestCase_Comm_SvcBase()
: _svc(LLBC_IService::Create(LLBC_IService::Normal, "SvcBaseTest"))
{
}

TestCase_Comm_SvcBase::~TestCase_Comm_SvcBase()
{
    LLBC_Delete(_svc);
}

int TestCase_Comm_SvcBase::Run(int argc, char *argv[])
{
    LLBC_PrintLine("Communication Service Basic test:");
    LLBC_PrintLine("Note: Maybe you must use gdb or windbg to trace!");

    // Register facade.
    TestFacade *facade = LLBC_New(TestFacade);
    _svc->RegisterFacade(facade);
    // Register coder.
    _svc->RegisterCoder(1, LLBC_New(TestDataFactory));
    // Register status desc(if enabled).
#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    _svc->RegisterStatusDesc(1, "The test status describe");
#endif

    // Subscribe handler.
    _svc->Subscribe(1, facade, &TestFacade::OnRecvData);
    _svc->PreSubscribe(1, facade, &TestFacade::OnPreRecvData);
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE // If want to test unifypre-subscribe function, you must comment above row.
    _svc->UnifyPreSubscribe(facade, &TestFacade::OnUnifyPreRecvData);
#endif
    // Subscribe status handler(if enabled).
#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    _svc->SubscribeStatus(1, 1, facade, &TestFacade::OnStatus_1);
#endif

    _svc->Start();

    // Connect test.
    ConnectTest("www.baidu.com", 80);
    ConnectTest("127.0.0.1", 7788);
  
    // Listen test.
    ListenTest("127.0.0.1", 7788);
  
    // Async connect test.
    AsyncConnTest("www.baidu.com", 80);

    // Send/Recv test.
    SendRecvTest("127.0.0.1", 7789);

#if LLBC_TARGET_PLATFORM_IPHONE
    LLBC_PrintLine("Sleep 300 seconds to exit...");
    LLBC_ThreadManager::Sleep(300 * 1000);
#else // Non-iPhone
    LLBC_PrintLine("Press any key to continue...");
    getchar();
#endif // iPhone

    return 0;
}

void TestCase_Comm_SvcBase::ListenTest(const char *ip, uint16 port)
{
    LLBC_PrintLine("Listen in %s:%d", ip, port);
    int sid = _svc->Listen(ip, port);
    if (sid == 0)
    {
        LLBC_PrintLine("listen in %s:%d failed, reason: %s", LLBC_FormatLastError());
        return;
    }

    LLBC_PrintLine("Listening in %s:%d", ip, port);

    // Try to connect.
    const int clientCount = 10;
    LLBC_PrintLine("Create %d clients to connet to this listen session", clientCount);
    for (int i = 0; i < clientCount; i++)
        _svc->AsyncConn(ip, port);
}

void TestCase_Comm_SvcBase::ConnectTest(const char *ip, uint16 port)
{
    LLBC_PrintLine("Connect to %s:%d", ip, port);
    int sid = _svc->Connect(ip, port);
    if (sid != 0)
    {
        LLBC_PrintLine("Connet to %s:%d success, sid: %d", ip, port, sid);
        LLBC_PrintLine("Disconnet it");
        _svc->RemoveSession(sid, "For test!!!!!");
    }
    else
    {
        LLBC_PrintLine("Fail to connect to %s:%d, reason: %s", ip, port, LLBC_FormatLastError());
    }
}

void TestCase_Comm_SvcBase::AsyncConnTest(const char *ip, uint16 port)
{
    int clientCount;
    if (LLBC_StrCmpA(LLBC_CFG_COMM_POLLER_MODEL, "SelectPoller") == 0)
        clientCount = 5;
    else
        clientCount = 50;

    LLBC_PrintLine("Async connect to %s:%d", ip, port);
    for (int i = 0; i < clientCount; i++)
        _svc->AsyncConn(ip, port);
}

void TestCase_Comm_SvcBase::SendRecvTest(const char *ip, uint16 port)
{
    LLBC_PrintLine("Send/Recv test:");

    // Create listen session.
    int listenSession = _svc->Listen(ip, port);
    if (listenSession == 0)
    {
        LLBC_PrintLine("Create listen session failed in %s:%d", ip, port);
        return;
    }

    // Connect to this session.
    int connSession = _svc->Connect(ip, port);
    if (connSession == 0)
    {
        LLBC_PrintLine("Connect to %s:%d failed, "
            "reason: %s", ip, port, LLBC_FormatLastError());
        _svc->RemoveSession(listenSession);
        return;
    }

    // Create packet to send to peer.
    TestData *encoder = LLBC_New(TestData);
    encoder->iVal = _svc->GetId();
    encoder->strVal = "Hello, llbc library";

    LLBC_Packet *packet = LLBC_New(LLBC_Packet);
    packet->SetHeader(connSession, 1, 0);
    packet->SetEncoder(encoder);

    _svc->Send(packet);

    // Create status == 1 packet to send to peer(if enabled).
#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    encoder = LLBC_New(TestData);
    encoder->iVal = _svc->GetId();
    encoder->strVal = "Hello, llbc library too";

    packet = LLBC_New(LLBC_Packet);
    packet->SetHeader(connSession, 1, 1);
    packet->SetEncoder(encoder);

    _svc->Send(packet);
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    
}

