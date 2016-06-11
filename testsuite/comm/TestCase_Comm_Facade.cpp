/**
 * @file    TestCase_Comm_Facade.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/06/09
 * @version 1.0
 *
 * @brief
 */

#include "comm/TestCase_Comm_Facade.h"

namespace
{
    class TestTimer : public LLBC_BaseTimer
    {
    public:
        virtual bool OnTimeout()
        {
            LLBC_PrintLine("Timer timeout!");
            return true;
        }

        virtual void OnCancel()
        {
            LLBC_PrintLine("Time cancelled!");
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
            LLBC_PrintLine("Service destroy");
        }

        virtual void OnStart()
        {
            LLBC_PrintLine("Service start");
            _timer = LLBC_New(TestTimer);
            _timer->Schedule(2000, 2000);
        }

        virtual void OnStop()
        {
            LLBC_PrintLine("Service stop");
            _timer->Cancel();
            LLBC_XDelete(_timer);
        }

    public:
        virtual void OnUpdate()
        {
            LLBC_PrintLine("Update...");
        }

        virtual void OnIdle(int idleTime)
        {
            LLBC_PrintLine("Idle, times: %d...", idleTime);
        }

    private:
        TestTimer *_timer;
    };
}

TestCase_Comm_Facade::TestCase_Comm_Facade()
{
}

TestCase_Comm_Facade::~TestCase_Comm_Facade()
{
}

int TestCase_Comm_Facade::Run(int argc, char *argv[])
{
    LLBC_PrintLine("Facade test:");

    // Parse arguments.
    if (argc < 4)
    {
        LLBC_FilePrintLine(stderr, "argument error, eg: ./a {internal-drive | external-drive} <host> <port>");
        return LLBC_FAILED;
    }

    const int port = LLBC_Str2Int32(argv[3]);
    const LLBC_String driveType = LLBC_String(argv[1]).tolower();
    if (driveType == "internal-drive")
        return TestInInternalDriveService(argv[2], port);
    else
        return TestInExternalDriveService(argv[2], port);
}

int TestCase_Comm_Facade::TestInInternalDriveService(const LLBC_String &host, int port)
{
    LLBC_PrintLine("Facade test(In internal-drive service), host: %s, port: %d", host.c_str(), port);

    // Create and init service.
    LLBC_IService *svc = LLBC_IService::Create(LLBC_IService::Normal);
    svc->SetId(1);
    svc->SetFPS(1);
    svc->RegisterFacade(LLBC_New(TestFacade));

    LLBC_PrintLine("Start service...");
    svc->Start(10);

    LLBC_PrintLine("Press any key to restart service(stop->start)...");
    getchar();
    svc->Stop();
    svc->Start(5);

    LLBC_PrintLine("Press any key to stop service...");
    getchar();
    svc->Stop();

    LLBC_PrintLine("Press any key to destroy service...");
    getchar();
    LLBC_Delete(svc);

    return LLBC_OK;
}

int TestCase_Comm_Facade::TestInExternalDriveService(const LLBC_String &host, int port)
{
    LLBC_PrintLine("Facade test(In external-drive service), host: %s, port: %d", host.c_str(), port);

    // Create and init service.
    LLBC_IService *svc = LLBC_IService::Create(LLBC_IService::Normal);
    svc->SetId(1);
    svc->SetFPS(1);
    svc->RegisterFacade(LLBC_New(TestFacade));
    svc->SetDriveMode(LLBC_IService::ExternalDrive);

    LLBC_PrintLine("Start service...");
    svc->Start(2);

    int waitSecs = 10, nowWaited = 0;
    LLBC_PrintLine("Calling Service.OnSvc, %d seconds later will restart service...", waitSecs);
    while (nowWaited != waitSecs)
    {
        svc->OnSvc();
        ++nowWaited;
    }

    LLBC_PrintLine("Restart service...");
    svc->Stop();
    svc->Start(2);

    LLBC_PrintLine("Calling Service.OnSvc, %d seconds later will stop service...", waitSecs);
    nowWaited = 0;
    while (nowWaited != waitSecs)
    {
        svc->OnSvc();
        ++nowWaited;
    }

    LLBC_PrintLine("Stop service...");
    svc->Stop();

    LLBC_PrintLine("Press any key to destroy service...");
    getchar();
    LLBC_Delete(svc);

    return LLBC_OK;
}
