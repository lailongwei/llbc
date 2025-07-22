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


#include "comm/TestCase_Comm_ExternalDriveSvc.h"

namespace
{

const int OPCODE = 0;

class TestComp final : public LLBC_Component
{
public:
    TestComp()
    : LLBC_Component()
    , _recvTimes(0)
    {
        LLBC_PrintLn(">>> Comp ctor called!");
    }

    ~TestComp() override
    {
        LLBC_PrintLn(">>> Comp dtor called!");
    }

public:
    int OnInit(bool &initFinished) override
    {
        LLBC_PrintLn("Service initialize");
        return LLBC_OK;
    }

    void OnDestroy(bool &destroyFinished) override
    {
        LLBC_PrintLn("Service destroy");
    }

    int OnStart(bool &finished) override
    {
        LLBC_PrintLn("Service start");
        return LLBC_OK;
    }

    void OnStop(bool &finished) override
    {
        static const LLBC_TimeSpan stopDurTime = LLBC_TimeSpan::FromSeconds(15);
        if (_begStopTime == LLBC_Time::utcBegin)
        {
            LLBC_PrintLn("Service stopping[0/%d]...", stopDurTime.GetTotalSeconds());

            _begStopTime = LLBC_Time::Now();
            LLBC_Sleep(1000);

            finished = false;

            return;
        }

        const LLBC_TimeSpan costTime = LLBC_Time::Now() - _begStopTime;
        if (costTime < stopDurTime)
        {
            LLBC_PrintLn("Service stoipping[%d/%d]",
                         costTime.GetTotalSeconds(), stopDurTime.GetTotalSeconds());

            LLBC_Sleep(1000);
            finished = false;

            return;
        }

        LLBC_PrintLn("Service stop finished");
    }

    void OnUpdate() override
    {
        LLBC_PrintLn("Service update");
    }

    void OnIdle(const LLBC_TimeSpan &idleTime) override
    {
        LLBC_PrintLn("Service idle, idleTime: %s", idleTime.ToString().c_str());
    }

    void OnEvent(int eventType, const LLBC_Variant &eventParams) override
    {
        switch (eventType)
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
            default:
                break;
        }
    }

public:
    void OnDataArrival(LLBC_Packet &packet)
    {
        LLBC_PrintLn("Session[%d] receive data, len: %lu\n", 
            packet.GetSessionId(), packet.GetLength());

        if (++_recvTimes == 100)
        {
            LLBC_PrintLn("Comp received %d times, stop this service", _recvTimes);

            LLBC_PrintLn("Print any key to stop...");
            getchar();

            GetService()->Stop();

            return;
        }

        LLBC_Packet *resPacket = new LLBC_Packet;
        resPacket->SetHeader(packet.GetSessionId(), OPCODE, 0);
        resPacket->Write(packet.GetPayload(), packet.GetPayloadLength());

        GetService()->Send(resPacket);
    }

private:
    void OnSessionCreate(const LLBC_SessionInfo &sessionInfo)
    {
        LLBC_PrintLn("Session create: %s", sessionInfo.ToString().c_str());
    }

    void OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo)
    {
        LLBC_PrintLn("Session destroy, info: %s", destroyInfo.ToString().c_str());
    }

    void OnAsyncConnResult(const LLBC_AsyncConnResult &result)
    {
        LLBC_PrintLn("Async-Conn result: %s", result.ToString().c_str());
    }

private:
    int _recvTimes;
    LLBC_Time _begStopTime;
};

}

TestCase_Comm_ExternalDriveSvc::TestCase_Comm_ExternalDriveSvc()
: _svc(LLBC_Service::Create("ExternalDriveSvc"))
{
    _svc->SuppressCoderNotFoundWarning();
}

TestCase_Comm_ExternalDriveSvc::~TestCase_Comm_ExternalDriveSvc()
{
    delete _svc;
}

int TestCase_Comm_ExternalDriveSvc::Run(int argc, char *argv[])
{
    LLBC_PrintLn("ExternalDrive type service test:");

    // Parse arguments.
    if (argc < 3)
    {
        LLBC_PrintLn("Startup argument error, eg: ./a ip port");
        return -1;
    }

    const char *ip = argv[1];
    const uint16 port = LLBC_Str2Num<int>(argv[2]);

    // Create comp and register it.
    TestComp *comp = new TestComp;
    _svc->AddComponent(comp);
    _svc->Subscribe(OPCODE, comp, &TestComp::OnDataArrival);

    // Set drive mode and start it.
    _svc->SetDriveMode(LLBC_ServiceDriveMode::ExternalDrive);
    if (_svc->Start() != LLBC_OK)
    {
        LLBC_PrintLn("Service startup failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }

    // Create listen port.
    int listenSid;
    if ((listenSid = _svc->Listen(ip, port)) == 0)
    {
        LLBC_PrintLn("Listen in %s:%d failed", ip, port);
        return -1;
    }

    // Connect it.
    int connSid;
    if ((connSid = _svc->Connect(ip, port)) == 0)
    {
        LLBC_PrintLn("Connect to %s:%d failed", ip, port);
        return -1;
    }

    // Send data(Send(LLBC_Packet *) method).
    LLBC_Packet *packet = new LLBC_Packet;
    packet->SetHeader(connSid, OPCODE, 0);
    *packet <<"Hello World!";
    _svc->Send(packet);

    // Send data(high level Send() methods).
    _svc->Send(connSid, OPCODE, reinterpret_cast<LLBC_Coder *>(NULL), 0);

    while (_svc->IsStarted())
        _svc->OnSvc(true);

    return 0;
}
