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
class TestComp : public LLBC_Component
{
public:
    bool OnInit(bool &initFinished)
    {
        LLBC_PrintLn("Service startup, startup timers...");

        _timeoutTimes = 0;
        _cancelTimes = 0;

        // Create long time timer and try to cancel
        LLBC_Timer *longTimeTimer = new LLBC_Timer(std::bind(&TestComp::OnTimerTimeout, this, std::placeholders::_1),
                                                   std::bind(&TestComp::OnTimerCancel, this, std::placeholders::_1));
        longTimeTimer->Schedule(LLBC_TimeSpan::FromMillis(LLBC_CFG_CORE_TIMER_LONG_TIMEOUT_TIME + 1));
        delete longTimeTimer;

        // Test reschedule timer in OnCancel event meth
        LLBC_Timer *rescheduleInCancelTimer = new LLBC_Timer([](LLBC_Timer *timer) {
            LLBC_PrintLn("RescheduleInCancelTimer timeout, cancel it!!!!!!");
            timer->Cancel();
        },
                                                             [](LLBC_Timer *timer) {
            LLBC_PrintLn("RescheduleInCancelTimer cancel, try reschedule!");
            timer->Schedule(LLBC_TimeSpan::FromSeconds(2));
        });
        rescheduleInCancelTimer->Schedule(LLBC_TimeSpan::FromSeconds(1));
        rescheduleInCancelTimer->Cancel();

        // Timer performance test
        #ifdef LLBC_DEBUG
        const int testTimerCount = 200;
        #else // !defined(LLBC_DEBUG)
        const int testTimerCount = 4000000;
        #endif
        for(int i = 1; i <=testTimerCount ; ++i) 
        {
            LLBC_Timer *timer = new LLBC_Timer(std::bind(&TestComp::OnTimerTimeout, this, std::placeholders::_1),
                                               std::bind(&TestComp::OnTimerCancel, this, std::placeholders::_1));

            timer->Schedule(LLBC_TimeSpan::FromMillis(LLBC_Rand(5000, 15001)),
                            LLBC_TimeSpan::FromMillis(LLBC_Rand(5000, 15001)));
        }

        LLBC_PrintLn("Done!");

        return true;
    }

    void OnDestroy(bool &destroyFinished)
    {
        LLBC_PrintLn("Service destroy!");
    }

public:
    void OnTimerTimeout(LLBC_Timer *timer)
    {
        #ifdef LLBC_DEBUG
        if (++_timeoutTimes % 100 == 0)
        #else
        if (++_timeoutTimes % 20000 == 0)
        #endif
            LLBC_PrintLn("%s: Timer <%s> trigger %d times timeout",
                           LLBC_Time::Now().ToString().c_str(),
                           timer->ToString().c_str(), _timeoutTimes);
        timer->Schedule(LLBC_TimeSpan::FromMillis(LLBC_Rand(5000, 15001)));
    }

    void OnTimerCancel(LLBC_Timer *timer)
    {
        if (++_cancelTimes % 10000 == 0)
            LLBC_PrintLn("Timer <%s> trigger %d times cancel", timer->ToString().c_str(), _cancelTimes);
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
    LLBC_PrintLn("Timer testcase:");

    LLBC_Service *svc = LLBC_Service::Create("TimerTest");
    svc->AddComponent(new TestComp);
    if(svc->Start() != LLBC_OK)
    {
        LLBC_PrintLn("Start service failed: reason: %s", LLBC_FormatLastError());
        delete svc;
        return -1;
    }

    svc->SetFPS(1000);

#if LLBC_TARGET_PLATFORM_IPHONE
    LLBC_PrintLn("Wait 100 seconds to exit...");
    LLBC_ThreadMgr::Sleep(100 * 1000);
#else // Non-iPhone
    LLBC_PrintLn("Press any key to continue...");
    getchar();
#endif // iPhone

    delete svc;

    return 0;
}
