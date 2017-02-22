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
class TestTimer : public LLBC_BaseTimer
{
public:
    TestTimer()
    {
        _times = 0;
    }

public:
    bool OnTimeout()
    {
        Schedule(LLBC_Random::RandInt32cmcn(1000, 5000), 0);
        return false;
    }

    void OnCancel()
    {
        delete this;
    }

private:
    int _times;
};

class TestFacade : public LLBC_IFacade
{
public:
    void OnInitialize()
    {
        LLBC_PrintLine("Service startup, startup timers...");
        for(int i = 1; i <= 500000; i ++)
        {
            TestTimer *timer = new TestTimer();
            timer->Schedule(LLBC_Random::RandInt32cmcn(10000, 100000), 
                LLBC_Random::RandInt32cmcn(10000, 50000));
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
