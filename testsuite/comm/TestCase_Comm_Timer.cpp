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


#include "comm/TestCase_Comm_Timer.h"

namespace
{
class TestComp : public LLBC_IComponent
{
public:
    bool OnInitialize()
    {
        LLBC_PrintLine("Service startup, startup timers...");

        _timeoutTimes = 0;
        _cancelTimes = 0;

        // Create long time timer and try to cancel
        LLBC_Timer *longTimeTimer = LLBC_New(LLBC_Timer,
                                             std::bind(&TestComp::OnTimerTimeout, this, std::placeholders::_1),
                                             std::bind(&TestComp::OnTimerCancel, this, std::placeholders::_1));
        longTimeTimer->Schedule(LLBC_TimeSpan::FromMillis(LLBC_CFG_CORE_TIMER_LONG_TIMEOUT_TIME + 1));
        LLBC_Delete(longTimeTimer);

        #ifdef LLBC_DEBUG
        const int testTimerCount = 200;
        #else // !defined(LLBC_DEBUG)
        const int testTimerCount = 4000000;
        #endif
        for(int i = 1; i <=testTimerCount ; ++i) 
        {
            // LLBC_Timer *timer = LLBC_New(LLBC_Timer,
                                         // LLBC_New(__Deleg, this, &TestComp::OnTimerTimeout),
                                         // LLBC_New(__Deleg, this, &TestComp::OnTimerCancel));

            LLBC_Timer *timer = LLBC_New(LLBC_Timer,
                                         std::bind(&TestComp::OnTimerTimeout, this, std::placeholders::_1),
                                         std::bind(&TestComp::OnTimerCancel, this, std::placeholders::_1));

            timer->Schedule(LLBC_TimeSpan::FromMillis(LLBC_RandInt(5000, 15001)),
                            LLBC_TimeSpan::FromMillis(LLBC_RandInt(5000, 15001)));
        }

        LLBC_PrintLine("Done!");

        return true;
    }

    void OnDestroy()
    {
        LLBC_PrintLine("Service destroy!");
    }

public:
    void OnTimerTimeout(LLBC_Timer *timer)
    {
        #ifdef LLBC_DEBUG
        if (++_timeoutTimes % 100 == 0)
        #else
        if (++_timeoutTimes % 20000 == 0)
        #endif
            LLBC_PrintLine("%s: Timer <%s> trigger %d times timeout",
                           LLBC_Time::Now().ToString().c_str(),
                           timer->ToString().c_str(), _timeoutTimes);
        timer->Schedule(LLBC_TimeSpan::FromMillis(LLBC_RandInt(5000, 15001)));
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

int TestComp::_timeoutTimes = 0;
int TestComp::_cancelTimes = 0;
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
    svc->RegisterComponent(LLBC_New(TestComp));
    if(svc->Start() != LLBC_OK)
    {
        LLBC_PrintLine("Start service failed: reason: %s", LLBC_FormatLastError());
        LLBC_Delete(svc);
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

    LLBC_Delete(svc);

    return 0;
}
