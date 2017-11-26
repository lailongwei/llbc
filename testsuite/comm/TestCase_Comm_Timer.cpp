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
class TestTimer : public LLBC_Timer
{
public:
    TestTimer()
    {
    }

    virtual ~TestTimer()
    {
        Cancel();
    }

public:
    void OnTimeout()
    {
        if (++_timeoutTimes % 10000 == 0)
            LLBC_PrintLine("Timer <%s> trigger %d times timeout", this->ToString().c_str(), _timeoutTimes);
        Schedule(LLBC_Random::RandInt32cmcn(5000, 15000));
    }

    void OnCancel()
    {
        if (++_cancelTimes % 10000 == 0)
            LLBC_PrintLine("Timer <%s> trigger %d times cancel", this->ToString().c_str(), _cancelTimes);
    }

private:
    static int _timeoutTimes;
    static int _cancelTimes;
};

int TestTimer::_timeoutTimes = 0;
int TestTimer::_cancelTimes = 0;

class TestFacade : public LLBC_IFacade
{
public:
    void OnInitialize()
    {
        LLBC_PrintLine("Service startup, startup timers...");

        // Create long time timer and try to cancel
        TestTimer *longTimeTimer = new TestTimer();
        longTimeTimer->Schedule(LLBC_CFG_CORE_TIMER_LONG_TIMEOUT_TIME + 1);
        delete longTimeTimer;

        for(int i = 1; i <=2000000; i++) 
        {
            TestTimer *timer = new TestTimer();
            timer->Schedule(LLBC_Random::RandInt32cmcn(5000, 15000), 
                LLBC_Random::RandInt32cmcn(5000, 15000));
        }

        LLBC_PrintLine("Done!");
    }

    void OnDestroy()
    {
        LLBC_PrintLine("Service destroy!");
    }
};
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
