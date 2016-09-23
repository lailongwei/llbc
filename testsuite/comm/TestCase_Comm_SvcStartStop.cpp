/**
 * @file    TestCase_Comm_SvcStartStop.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/09/23
 * @version 1.0
 *
 * @brief
 */

#include "comm/TestCase_Comm_SvcStartStop.h"

namespace
{
    class SvcStartStopTestFacade : public LLBC_IFacade
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
            _updateOutputTimes = 0;
            _idleOutputTimes = 0;
            LLBC_PrintLine("Service start");
        }

        virtual void OnStop()
        {
            LLBC_PrintLine("Service stop");
        }

        virtual void OnUpdate()
        {
            if (_updateOutputTimes == 5)
                return;

            LLBC_PrintLine("OnUpdate call, times: %d", ++_updateOutputTimes);
        }

        virtual void OnIdle(int idleTime)
        {
            if (_idleOutputTimes == 5)
                return;

            LLBC_PrintLine("OnIdle call, times: %d", ++_idleOutputTimes);
        }

    private:
        int _updateOutputTimes;
        int _idleOutputTimes;
    };
}

TestCase_Comm_SvcStartStop::TestCase_Comm_SvcStartStop()
{
}

TestCase_Comm_SvcStartStop::~TestCase_Comm_SvcStartStop()
{
}

int TestCase_Comm_SvcStartStop::Run(int argc, char *argv[])
{
    LLBC_PrintLine("Service start/stop test:");
    LLBC_IService *svc = LLBC_IService::Create(LLBC_IService::Normal);
    svc->RegisterFacade(LLBC_New(SvcStartStopTestFacade));
    svc->Start();

    LLBC_PrintLine("Service started, sleep 2 sectonds...");
    LLBC_Sleep(2000);

    LLBC_PrintLine("ReStart service...");
    svc->Stop();
    svc->Start(2);

    LLBC_PrintLine("Service restarted, sleep 5 seconds...");
    LLBC_Sleep(5000);

    LLBC_PrintLine("Stop service");
    svc->Stop();
    LLBC_Delete(svc);

    LLBC_PrintLine("Press any key to exit...");
    getchar();

    return 0;
}
