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


#include "comm/TestCase_Comm_CompBase.h"

namespace
{
    class ITestComp : public LLBC_Component
    {
    public:
        ITestComp() : LLBC_Component(LLBC_ComponentEvents::AllEvents) {}
        virtual ~ITestComp() = default;
    };
    
    class TestComp : public ITestComp
    {
    public:
        TestComp()
        {
            _timer = nullptr;
        }

        virtual ~TestComp() = default;

    public:
        void OnPrint()
        {
            LLBC_PrintLn("TestComp");
        }

    public:
        virtual bool OnInit(bool &initFinished)
        {
            LLBC_PrintLn("Service initialize");
            return true;
        }

        virtual void OnDestroy(bool &destroyFinished)
        {
            LLBC_PrintLn("Service destroy");
        }

        virtual bool OnStart(bool &startFinished)
        {
            LLBC_PrintLn("Service start");
            _timer = new LLBC_Timer(
                              std::bind(&TestComp::OnTimerTimeout, this, std::placeholders::_1),
                              std::bind(&TestComp::OnTimerCancel, this, std::placeholders::_1));
            _timer->Schedule(LLBC_TimeSpan::FromSeconds(2), LLBC_TimeSpan::FromSeconds(5));

            return true;
        }

        virtual void OnStop(bool &stopFinished)
        {
            LLBC_PrintLn("Service stop");
            _timer->Cancel();
            LLBC_XDelete(_timer);
        }

    public:
        virtual void OnUpdate()
        {
            LLBC_PrintLn("Update...");
        }

        virtual void OnIdle(const LLBC_TimeSpan &idleTime)
        {
            LLBC_PrintLn("Idle, idle time: %s...", idleTime.ToString().c_str());
        }

    public:
        virtual void OnTimerTimeout(LLBC_Timer *timer)
        {
            LLBC_PrintLn("Timer timeout!");
        }

        virtual void OnTimerCancel(LLBC_Timer *timer)
        {
            LLBC_PrintLn("Time cancelled!");
        }

    private:
        LLBC_Timer *_timer;
    };

    class TestCompFactory : public LLBC_ComponentFactory
    {
    public:
        virtual ITestComp *Create() const
        {
            return new TestComp;
        }
    };

    class IEchoComp : public LLBC_Component
    {
    public:
        IEchoComp() : LLBC_Component(LLBC_ComponentEvents::AllEvents) {}
        virtual ~IEchoComp() = default;
    };

    class EchoComp : public IEchoComp
    {
    public:
        void OnPrint()
        {
            LLBC_PrintLn("EchoComp");
        }
    };

    class EchoCompFactory : public LLBC_ComponentFactory
    {
    public:
        virtual IEchoComp *Create() const
        {
            return new EchoComp;
        }
    };
}

TestCase_Comm_CompBase::TestCase_Comm_CompBase()
{
}

TestCase_Comm_CompBase::~TestCase_Comm_CompBase()
{
}

int TestCase_Comm_CompBase::Run(int argc, char *argv[])
{
    LLBC_PrintLn("CompBase test:");

    // Parse arguments.
    if (argc < 4)
    {
        LLBC_FilePrintLn(
            stderr, "argument error, eg: ./a {internal-drive | external-drive} <host> <port>");
        return LLBC_FAILED;
    }

    const int port = LLBC_Str2Int32(argv[3]);
    const LLBC_String driveType = LLBC_String(argv[1]).tolower();
    if (driveType == "internal-drive")
        return TestInInternalDriveService(argv[2], port);
    else
        return TestInExternalDriveService(argv[2], port);
}

int TestCase_Comm_CompBase::TestInInternalDriveService(const LLBC_String &host, int port)
{
    LLBC_PrintLn("Comp test(In internal-drive service), host: %s, port: %d", host.c_str(), port);

    // Create and init service.
    LLBC_Service *svc = LLBC_Service::Create("CompTest");
    svc->SetFPS(1);
    svc->AddComponent<TestCompFactory>();
    svc->AddComponent<EchoCompFactory>();

    // Try init library comp(not exist)
    const LLBC_String notExistCompName = "Not exist comp name";
    const LLBC_String notExistCompLibPath = "!!!!!!!!Not exist library!!!!!!!!";
    LLBC_PrintLn("Test try register not exist third-party comp, libPath:%s, compName:%s",
                   notExistCompLibPath.c_str(), notExistCompName.c_str());
    int ret = svc->AddComponent(notExistCompLibPath, notExistCompName);
    if (ret != LLBC_OK)
    {
        LLBC_PrintLn("Register not exist third-party comp failed, error:%s", LLBC_FormatLastError());
    }
    else
    {
        LLBC_PrintLn("Register not exist third-party comp success, internal error!!!");
        getchar();
        delete svc;

        return LLBC_FAILED;
    }

    LLBC_PrintLn("Start service...");
    if (svc->Start(10) != LLBC_OK)
    {
        LLBC_PrintLn("Failed to start service, error: %s", LLBC_FormatLastError());
        getchar();
        delete svc;

        return LLBC_FAILED;
    }
    LLBC_PrintLn("Test componet load success.");
    LLBC_PrintLn("Call Service::Start() finished!");

    auto* testComp = svc->GetComponent<ITestComp>();
    if(testComp == nullptr)
    {
        LLBC_PrintLn("Test component get failed, error: %s", LLBC_FormatLastError());
        getchar();
        delete svc;

        return LLBC_FAILED;
    }
    dynamic_cast<TestComp *>(testComp)->OnPrint();

    auto* echoComp = svc->GetComponent<IEchoComp>();
    if(echoComp == nullptr)
    {
        LLBC_PrintLn("Echo component get failed, error: %s", LLBC_FormatLastError());
        getchar();
        delete svc;

        return LLBC_FAILED;
    }
    dynamic_cast<EchoComp *>(testComp)->OnPrint();

    

    LLBC_PrintLn("Press any key to restart service(stop->start)...");
    getchar();
    svc->Stop();
    if (svc->Start(5) != LLBC_OK)
    {
        LLBC_PrintLn("Failed to restart service, error: %s", LLBC_FormatLastError());
        getchar();
        delete svc;

        return LLBC_FAILED;
    }
    LLBC_PrintLn("Call Service::Start() finished!");

    LLBC_PrintLn("Press any key to stop service...");
    getchar();
    svc->Stop();

    LLBC_PrintLn("Press any key to destroy service...");
    getchar();
    delete svc;

    return LLBC_OK;
}

int TestCase_Comm_CompBase::TestInExternalDriveService(const LLBC_String &host, int port)
{
    LLBC_PrintLn("Comp test(In external-drive service), host: %s, port: %d", host.c_str(), port);

    // Create and init service.
    LLBC_Service *svc = LLBC_Service::Create("CompTest");
    svc->SetFPS(1);
    svc->AddComponent<TestCompFactory>();
    svc->SetDriveMode(LLBC_Service::ExternalDrive);

    LLBC_PrintLn("Start service...");
    if (svc->Start(2) != LLBC_OK)
    {
        LLBC_PrintLn("Failed to start service, error: %s", LLBC_FormatLastError());
        getchar();
        delete svc;

        return LLBC_FAILED;
    }
    LLBC_PrintLn("Call Service::Start() finished!");

    int waitSecs = 10, nowWaited = 0;
    LLBC_PrintLn("Calling Service.OnSvc, %d seconds later will restart service...", waitSecs);
    while (nowWaited != waitSecs)
    {
        svc->OnSvc();
        ++nowWaited;
    }

    LLBC_PrintLn("Restart service...");
    svc->Stop();
    if (svc->Start(2) != LLBC_OK)
    {
        LLBC_PrintLn("Calling Service.OnSvc, %d seconds later will restart service...", waitSecs);
        getchar();
        delete svc;

        return LLBC_FAILED;
    }
    LLBC_PrintLn("Call Service::Start() finished!");

    LLBC_PrintLn("Calling Service.OnSvc, %d seconds later will stop service...", waitSecs);
    nowWaited = 0;
    while (nowWaited != waitSecs)
    {
        svc->OnSvc();
        ++nowWaited;
    }

    LLBC_PrintLn("Stop service...");
    svc->Stop();

    LLBC_PrintLn("Press any key to destroy service...");
    getchar();
    delete svc;

    return LLBC_OK;
}
