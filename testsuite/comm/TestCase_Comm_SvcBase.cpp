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


#include "comm/TestCase_Comm_SvcBase.h"

namespace
{

class MyCompEventType
{
public:
    enum
    {
        MyCompEv1 = LLBC_ComponentEventType::LogicBegin,
        MyCompEv2,
    };
};

struct TestData final : public LLBC_Coder
{
    int iVal;
    LLBC_String strVal;

    TestData()
    : iVal(0)
    {
    }

    ~TestData() override
    {
        std::cout <<"0x" <<this <<": Test data destroyed!" <<std::endl;
    }

    bool Encode(LLBC_Packet &packet) override
    {
        packet <<iVal <<strVal;
        return true;
    }

    bool Decode(LLBC_Packet &packet) override
    {
        packet >>iVal >>strVal;
        return true;
    }

    void Reuse()
    {
        iVal = 0;
        strVal.clear();
    }
};

class TestDataFactory final : public LLBC_CoderFactory
{
public:
    LLBC_Coder *Create() const override
    {
        return new TestData;
    }
};

class TestComp final : public LLBC_Component
{
public:
    TestComp()
    : LLBC_Component()
    {}

public:
    int OnInit(bool &initFinished) override
    {
        LLBC_PrintLn("Service initialize");
        return LLBC_OK;
    }

    void OnDestroy(bool &destroyFinished) override
    {
        LLBC_PrintLn("Service Destroy");
    }

    int OnStart(bool &startFinished) override
    {
        LLBC_PrintLn("Service start");
        return LLBC_OK;
    }

    void OnStop(bool &stopFinished) override
    {
        LLBC_PrintLn("Service stop");
    }

public:
    void OnUpdate() override
    {
        int fps = LLBC_Rand(20, 61);
        // LLBC_PrintLn("Service update, set fps to %d", fps);

        GetService()->SetFPS(fps);
    }

    void OnIdle(const LLBC_TimeSpan &idleTime) override
    {
        // LLBC_PrintLn("Service idle, idle time: %s", idleTime.ToString().c_str());
    }

    void OnEvent(int eventType, const LLBC_Variant &eventParams) override
    {
        switch(eventType)
        {
            case LLBC_ComponentEventType::SessionCreate:
            {
                OnSessionCreate(*eventParams.AsPtr<LLBC_SessionInfo>());
                break;
            }
            case LLBC_ComponentEventType::SessionDestroy:
            {
                OnSessionDestroy(*eventParams.AsPtr<LLBC_SessionDestroyInfo>());
                break;
            }
            case LLBC_ComponentEventType::AsyncConnResult:
            {
                OnAsyncConnResult(*eventParams.AsPtr<LLBC_AsyncConnResult>());
                break;
            }
            case LLBC_ComponentEventType::ProtoReport:
            {
                OnProtoReport(*eventParams.AsPtr<LLBC_ProtoReport>());
                break;
            }
            case MyCompEventType::MyCompEv1:
            {
                LLBC_PrintLn("Recv MyCompEv1 event, params:%s", eventParams.ToString().c_str());
                break;
            }
            case MyCompEventType::MyCompEv2:
            {
                LLBC_PrintLn("Recv MyCompEv2 event, params:%s", eventParams.ToString().c_str());
                break;
            }

            default:
                break;
        }
    }

public:
    void OnRecvData(LLBC_Packet &packet)
    {
        TestData *data = packet.GetDecoder<TestData>();
        LLBC_PrintLn("Receive data[%d], iVal: %d, strVal: %s", 
            packet.GetSessionId(), data->iVal, data->strVal.c_str());

        TestData *resData = new TestData;
        *resData = *data;

        LLBC_Packet *resPacket = GetService()->GetThreadSafeObjPool().Acquire<LLBC_Packet>();
        resPacket->SetHeader(packet.GetSessionId(), packet.GetOpcode(), 0);
        resPacket->SetEncoder(resData);

        GetService()->Send(resPacket);

        GetService()->AddComponentEvent(MyCompEventType::MyCompEv1, LLBC_Variant(10086));
        GetService()->AddComponentEvent(MyCompEventType::MyCompEv2, LLBC_Variant("Hello world"));
    }

    bool OnPreRecvData(LLBC_Packet &packet)
    {
        TestData *data = (TestData *)packet.GetDecoder();
        LLBC_PrintLn("Pre-Receive data[%d], iVal: %d, strVal: %s",
            packet.GetSessionId(), data->iVal, data->strVal.c_str());

        packet.SetPreHandleResult(LLBC_Malloc(char, 4096), this, &TestComp::DelPreHandleResult);

        return true;
    }

    bool OnUnifyPreRecvData(LLBC_Packet &packet)
    {
        LLBC_PrintLn("Unify Pre-Receive data[%d]", packet.GetSessionId());

        packet.SetPreHandleResult(LLBC_Malloc(char, 4096), this, &TestComp::DelPreHandleResult);

        return true;
    }

public:
    void OnStatus_1(LLBC_Packet &packet)
    {
        LLBC_PrintLn("Recv status != 0 packet, op: %d, status: %d",
            packet.GetOpcode(), packet.GetStatus());
#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
        LLBC_PrintLn("The status desc: %s", packet.GetStatusDesc().c_str());
#endif // LLBC_CFG_COMM_ENABLE_STATUS_DESC
    }

private:
    void OnSessionCreate(const LLBC_SessionInfo &sessionInfo)
    {
        LLBC_PrintLn("SessionCreate: %s", sessionInfo.ToString().c_str());
        // std::cout <<"Session Create: " <<si <<std::endl;
    }

    void OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo)
    {
        LLBC_PrintLn("Session destroy, info: %s", destroyInfo.ToString().c_str());
    }

    void OnAsyncConnResult(const LLBC_AsyncConnResult &result)
    {
        LLBC_PrintLn("Async-Conn result: %s", result.ToString().c_str());
        // std::cout <<"Async-Conn result: " <<result <<std::endl;
    }

    void OnProtoReport(const LLBC_ProtoReport &report)
    {
        LLBC_PrintLn("Protocol report: %s", report.ToString().c_str());
        // std::cout <<"Protocol report: " <<report <<std::endl;
    }

    void DelPreHandleResult(void *result)
    {
        free(result);
    }
};

class TestCompFactory final : public LLBC_ComponentFactory
{
public:
    LLBC_Component *Create(LLBC_Service *service) const override
    {
        return new TestComp;
    }
};

}

TestCase_Comm_SvcBase::TestCase_Comm_SvcBase()
: _svc(LLBC_Service::Create("SvcBaseTest"))
// : _svc(LLBC_Service::Create("SvcBaseTest", nullptr, false))
{
}

TestCase_Comm_SvcBase::~TestCase_Comm_SvcBase()
{
    delete _svc;
}

int TestCase_Comm_SvcBase::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Communication Service Basic test:");
    LLBC_PrintLn("Note: Maybe you must use gdb or windbg to trace!");

    // Register comp.
    TestComp *comp = new TestComp;
    _svc->AddComponent(comp);
    // Register coder.
    _svc->AddCoderFactory(1, new TestDataFactory);
    // Register status desc(if enabled).
#if LLBC_CFG_COMM_ENABLE_STATUS_DESC
    _svc->AddStatusDesc(1, "The test status describe");
#endif

    // Subscribe handler.
    _svc->Subscribe(1, comp, &TestComp::OnRecvData);
    _svc->PreSubscribe(1, comp, &TestComp::OnPreRecvData);
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE // If want to test unifypre-subscribe function, you must comment above row.
    _svc->UnifyPreSubscribe(comp, &TestComp::OnUnifyPreRecvData);
#endif
    // Subscribe status handler(if enabled).
#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    _svc->SubscribeStatus(1, 1, comp, &TestComp::OnStatus_1);
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
    LLBC_PrintLn("Sleep 300 seconds to exit...");
    LLBC_ThreadMgr::Sleep(300 * 1000);
#else // Non-iPhone
    LLBC_PrintLn("Press any key to continue...");
    getchar();
#endif // iPhone

    return 0;
}

void TestCase_Comm_SvcBase::ListenTest(const char *ip, uint16 port)
{
    LLBC_PrintLn("Listen in %s:%d", ip, port);
    int sid = _svc->Listen(ip, port);
    if (sid == 0)
    {
        LLBC_PrintLn("listen in %s:%d failed, reason: %s", ip, port, LLBC_FormatLastError());
        return;
    }

    LLBC_PrintLn("Listening in %s:%d", ip, port);

    // Try to connect.
    const int clientCount = 10;
    LLBC_PrintLn("Create %d clients to connet to this listen session", clientCount);
    for (int i = 0; i < clientCount; ++i)
    {
        auto connSid = _svc->AsyncConn(ip, port);
        if (connSid == 0)
            LLBC_PrintLn("connect to %s:%d failed, reason: %s", ip, port, LLBC_FormatLastError());
    }
}

void TestCase_Comm_SvcBase::ConnectTest(const char *ip, uint16 port)
{
    LLBC_PrintLn("Connect to %s:%d", ip, port);
    int sid = _svc->Connect(ip, port);
    if (sid != 0)
    {
        LLBC_PrintLn("Connet to %s:%d success, sid: %d, disconnect it", ip, port, sid);
        _svc->RemoveSession(sid, "For test!!!!!");
    }
    else
    {
        LLBC_PrintLn("Fail to connect to %s:%d, reason: %s", ip, port, LLBC_FormatLastError());
    }
}

void TestCase_Comm_SvcBase::AsyncConnTest(const char *ip, uint16 port)
{
    int clientCount;
    if (strcmp(LLBC_CFG_COMM_POLLER_MODEL, "SelectPoller") == 0)
        clientCount = 5;
    else
        clientCount = 100;

    LLBC_PrintLn("Async connect to %s:%d", ip, port);
    for (int i = 0; i < clientCount; ++i)
        _svc->AsyncConn(ip, port);
}

void TestCase_Comm_SvcBase::SendRecvTest(const char *ip, uint16 port)
{
    LLBC_PrintLn("Send/Recv test:");

    // Create listen session.
    int listenSession = _svc->Listen(ip, port);
    if (listenSession == 0)
    {
        LLBC_PrintLn("Create listen session failed in %s:%d", ip, port);
        return;
    }

    // Connect to this session.
    int connSession = _svc->Connect(ip, port);
    if (connSession == 0)
    {
        LLBC_PrintLn("Connect to %s:%d failed, "
            "reason: %s", ip, port, LLBC_FormatLastError());
        _svc->RemoveSession(listenSession);
        return;
    }

    // Create packet to send to peer.
    TestData *encoder = new TestData;
    encoder->iVal = _svc->GetId();
    encoder->strVal = "Hello, llbc library";

    // LLBC_Packet *packet = new LLBC_Packet; // uncomment for test send non object-pool packet.
     LLBC_Packet *packet = _svc->GetThreadSafeObjPool().Acquire<LLBC_Packet>();  // uncomment for test send object-pool packet.
    packet->SetHeader(connSession, 1, 0);
    packet->SetEncoder(encoder);

    _svc->Send(packet);

    // Create status == 1 packet to send to peer(if enabled).
    #if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
    encoder = _svc->GetThreadSafeObjPool().Acquire<TestData>();
    encoder->iVal = _svc->GetId();
    encoder->strVal = "Hello, llbc library too";

    packet = new LLBC_Packet;
    packet->SetHeader(connSession, 1, 1);
    packet->SetEncoder(encoder);

    _svc->Send(packet);
    #endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
}

