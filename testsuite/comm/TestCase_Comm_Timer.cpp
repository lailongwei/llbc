/**
 * @file    TestCase_Comm_Timer.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/12/03
 * @version 1.0
 *
 * @brief
 */

#include "comm/TestCase_Comm_Timer.h"

namespace
{
class TestFacade : public LLBC_IFacade
{
public:
    void OnInitialize()
    {
        LLBC_PrintLine("Service startup, startup timers...");

        _timeoutTimes = 0;
        _cancelTimes = 0;

        // Create long time timer and try to cancel
        LLBC_Timer *longTimeTimer = new LLBC_Timer(new LLBC_Delegate1<void, TestFacade, LLBC_Timer *>(this, &TestFacade::OnTimerTimeout),
                                                   new LLBC_Delegate1<void, TestFacade, LLBC_Timer *>(this, &TestFacade::OnTimerCancel));
        longTimeTimer->Schedule(LLBC_CFG_CORE_TIMER_LONG_TIMEOUT_TIME + 1);
        delete longTimeTimer;

        for(int i = 1; i <=2000000; i++) 
        {
            LLBC_Timer *timer = new LLBC_Timer(new LLBC_Delegate1<void, TestFacade, LLBC_Timer *>(this, &TestFacade::OnTimerTimeout),
                                               new LLBC_Delegate1<void, TestFacade, LLBC_Timer *>(this, &TestFacade::OnTimerCancel));
            timer->Schedule(LLBC_Random::RandInt32cmcn(5000, 15000), 
                LLBC_Random::RandInt32cmcn(5000, 15000));
        }

        LLBC_PrintLine("Done!");
    }

    void OnDestroy()
    {
        LLBC_PrintLine("Service destroy!");
    }

public:
    void OnTimerTimeout(LLBC_Timer *timer)
    {
        if (++_timeoutTimes % 10000 == 0)
            LLBC_PrintLine("Timer <%s> trigger %d times timeout", timer->ToString().c_str(), _timeoutTimes);
        timer->Schedule(LLBC_Random::RandInt32cmcn(5000, 15000));
    }

    void OnTimerCancel(LLBC_Timer *timer)
    {
        if (++_cancelTimes % 10000 == 0)
            LLBC_PrintLine("Timer <%s> trigger %d times cancel", timer->ToString().c_str(), _cancelTimes);
    }

private:
    static int _timeoutTimes;
    static int _cancelTimes;
};

int TestFacade::_timeoutTimes = 0;
int TestFacade::_cancelTimes = 0;
}

TestCase_Comm_Timer::TestCase_Comm_Timer()
{
}

TestCase_Comm_Timer::~TestCase_Comm_Timer()
{
}

int TestCase_Comm_Timer::Run(int argc, char *argv[])
{
    LLBC_PrintLine("Timer testcase:");

    LLBC_IService *svc = LLBC_IService::Create(LLBC_IService::Normal, "TimerTest");
    svc->RegisterFacade(LLBC_New(TestFacade));
    if(svc->Start() != LLBC_OK)
    {
        LLBC_PrintLine("Start service failed: reason: %s", LLBC_FormatLastError());
        delete svc;
        return -1;
    }

    svc->SetFPS(1000);

#if LLBC_TARGET_PLATFORM_IPHONE
    LLBC_PrintLine("Wait 100 seconds to exit...");
    LLBC_ThreadManager::Sleep(100 * 1000);
#else // Non-iPhone
    LLBC_PrintLine("Press any key to continue...");
    getchar();
#endif // iPhone

    delete svc;

    return 0;
}
