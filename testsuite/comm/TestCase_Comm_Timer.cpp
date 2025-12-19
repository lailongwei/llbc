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
class TestComp final : public LLBC_Component
{
public:
    int OnInit(bool &initFinished) override
    {
        if (!_testStarted)
        {
            _testStarted = true;
            LLBC_Expect(BasicFuncTest() == LLBC_OK);
            LLBC_Expect(BasicScheduleTest() == LLBC_OK);

            return LLBC_OK;
        }

        if (!_testFinished)
            initFinished = false;

        return LLBC_OK;
    }

    void OnDestroy(bool &destroyFinished) override
    {
        _testStarted = false;
        _testFinished = false;
        _perfTest_TriggeredCount = 0;
        _perfTest_BeginTestTime = 0;

        LLBC_PrintLn("Service destroy!");
    }

private:
    int BasicFuncTest()
    {
        LLBC_PrintLn("Timer basic function test:");

        LLBC_Timer timer;
        LLBC_PrintLn("- Unschedule timer ToString(): %s", timer.ToString().c_str());

        // GetTimerId()/GetFirstPeriod()/GetPeriod()
        LLBC_Expect(timer.GetTimerId() == 0, "timerId: %llu", timer.GetTimerId());
        LLBC_Expect(timer.GetFirstPeriod() == LLBC_TimeSpan::zero);
        LLBC_Expect(timer.GetPeriod() == LLBC_TimeSpan::zero);

        // GetTimeoutHandler()/GetCancelHandler():
        LLBC_PrintLn("- Not set timeout&cancel handler:");
        LLBC_Expect(timer.GetTimeoutHandler() == nullptr, "Not set timeout handler");
        LLBC_Expect(timer.GetCancelHandler() == nullptr, "Not set cancel handler");

        // SetTimeoutHandler()/SetCancelHandler():
        timer.SetTimeoutHandler([](LLBC_Timer *timer) {});
        timer.SetCancelHandler([](LLBC_Timer *) {});
        LLBC_PrintLn("- Set timeout&cancel handler:");
        LLBC_Expect(timer.GetTimeoutHandler(), "After call SetTimeoutHandler()");
        LLBC_Expect(timer.GetCancelHandler(), "After call SetCancelHandler()");

        // Reset timeout/cancel handler:
        LLBC_PrintLn("- Reset timeout&cancel handler:");
        timer.SetTimeoutHandler(nullptr);
        timer.SetCancelHandler(nullptr);
        LLBC_Expect(timer.GetTimeoutHandler() == nullptr);
        LLBC_Expect(timer.GetCancelHandler() == nullptr);

        // GetTotalTriggerCount()/GetTriggeredCount():
        LLBC_PrintLn("- GetTotalTriggerCount()/GetTriggeredCount():");
        LLBC_Expect_EQ(timer.GetTotalTriggerCount(), static_cast<size_t>(LLBC_INFINITE));
        LLBC_Expect_EQ(timer.GetTriggeredCount(), 0);

        // IsScheduled()/IsHandlingTimeout()/IsHandlingCancel():
        LLBC_PrintLn("- Status check methods(IsXxx()):");
        LLBC_Expect(!timer.IsScheduled());
        LLBC_Expect(!timer.IsHandlingTimeout());
        LLBC_Expect(!timer.IsHandlingCancel());

        LLBC_PrintLn("- GetTimerData():");
        LLBC_Expect(timer.GetTimerData() == LLBC_Variant::nil,
                    "timer.timerData:%s", timer.GetTimerData().ToString().c_str());
        timer.GetTimerData() = 333;
        LLBC_Expect(timer.GetTimerData() == 333,
                    "timer.timerData:%s", timer.GetTimerData().ToString().c_str());

        // GetTimeoutTime():
        LLBC_PrintLn("- GetTimeoutTime():");
        LLBC_Expect(timer.GetTimeoutTime() == LLBC_Time::utcBegin);

        return LLBC_OK;
    }

    int BasicScheduleTest()
    {
        LLBC_PrintLn("Basic schedule test:");

        // Generate firstPeriod/period.
        const auto firstPeriod = LLBC_TimeSpan::FromMillis(LLBC_Rand(50, 1000));
        const auto period = LLBC_TimeSpan::FromMillis(LLBC_Rand(1000, 2000));
        const size_t triggerCount = LLBC_Rand(0, 5);

        LLBC_PrintLn("- Generated firstPeriod: %s, period: %s",
                     firstPeriod.ToString().c_str(), period.ToString().c_str());

        // Create test timer && set timeout handler.
        auto testTimer = new LLBC_Timer;
        testTimer->SetTimeoutHandler([this, firstPeriod, period](LLBC_Timer *timer) {
            LLBC_PrintLn("- Timeout, timer:%s", timer->ToString().c_str());

            // Expect timer scheduled.
            LLBC_Expect(timer->IsScheduled());

            // Expect timer is handling timeout.
            LLBC_Expect(timer->IsHandlingTimeout());

            // Expect timer is not in handling cancel.
            LLBC_Expect(!timer->IsHandlingCancel());

            // Expect timer firstPeriod/period is correct.
            LLBC_Expect(timer->GetFirstPeriod() == firstPeriod);
            LLBC_Expect(timer->GetPeriod() == period);

            // Expect has been trigger count > 0.
            LLBC_Expect(timer->GetTriggeredCount() > 0);
            // Expect has been trigger count <= max trigger count.
            LLBC_Expect(timer->GetTriggeredCount() <= timer->GetTotalTriggerCount(),
                        "hasBeenTriggerCount:%zu, maxTriggerCount:%zu",
                        timer->GetTriggeredCount(), timer->GetTotalTriggerCount());

            if (timer->GetTriggeredCount() == timer->GetTotalTriggerCount())
            {
                LLBC_Defer(delete timer);
                LLBC_Defer(timer->Cancel());
                ZeroFirstPeriodTest();
            }
        });

        // Schedule timer.
        LLBC_PrintLn("- Schedule timer...");
        LLBC_Expect(testTimer->Schedule(firstPeriod, period, triggerCount) == LLBC_OK,
                    "timer: %s", testTimer->ToString().c_str());

        // Expect timerId != 0.
        LLBC_Expect(testTimer->GetTimerId() != 0);
        // Expect period/firstPeriod values are corrected.
        LLBC_Expect(testTimer->GetPeriod() == period);
        LLBC_Expect(testTimer->GetFirstPeriod() == firstPeriod);

        // Expect triggerCount is a correct value.
        if (triggerCount == 0)
            LLBC_Expect(testTimer->GetTotalTriggerCount() == 1);
        else
            LLBC_Expect(testTimer->GetTotalTriggerCount() == triggerCount,
                        "timer.totalTriggerCount:%zu, triggerCount:%zu",
                        testTimer->GetTotalTriggerCount(), triggerCount);

        // Expect triggeredCount is 0.
        LLBC_Expect(testTimer->GetTriggeredCount() == 0);

        // Expect timer is scheduled/is timeout handling/is cancel handling.
        LLBC_Expect(testTimer->IsScheduled());
        LLBC_Expect(!testTimer->IsHandlingTimeout());
        LLBC_Expect(!testTimer->IsHandlingCancel());

        return LLBC_OK;
    }

    int ZeroFirstPeriodTest()
    {
        LLBC_PrintLn("Zero first period test:");

        // Generate period.
        const auto period = LLBC_TimeSpan::FromMillis(LLBC_Rand(100, 1000));
        LLBC_PrintLn("- period: %s", period.ToString().c_str());

        // Create test timer && set timeout handler.
        auto testTimer = new LLBC_Timer;
        testTimer->SetTimeoutHandler([this, period](LLBC_Timer *timer) {
            LLBC_Defer(delete timer);
            LLBC_PrintLn("- Timeout, timer: %s", timer->ToString().c_str());

            // Expect timer firstPeriod is 0.
            LLBC_Expect(timer->GetFirstPeriod() == LLBC_TimeSpan::zero);
            // Expect timer period is correct.
            LLBC_Expect(timer->GetPeriod() == period);

            // Execute next test.
            LLBC_Expect(ZeroPeriodTest() == LLBC_OK);
        });

        LLBC_PrintLn("- Schedule timer...");
        LLBC_Expect(testTimer->Schedule(LLBC_TimeSpan::zero, period) == LLBC_OK,
                    "timer: %s",
                    testTimer->ToString().c_str());

        return LLBC_OK;
    }

    int ZeroPeriodTest()
    {
        LLBC_PrintLn("Zero period test:");

        // Generate first period.
        const auto firstPeriod = LLBC_TimeSpan::FromMillis(LLBC_Rand(100, 1000));
        LLBC_PrintLn("- firstPeriod: %s", firstPeriod.ToString().c_str());

        // Creawte test timer && set timeout handler.
        auto testTimer = new LLBC_Timer;
        testTimer->SetTimeoutHandler([this, firstPeriod](LLBC_Timer *timer) {
            LLBC_Defer(delete timer);
            LLBC_PrintLn("- Timeout, timer: %s", timer->ToString().c_str());

            // Expect timer firstPeriod is correct.
            LLBC_Expect(timer->GetFirstPeriod() == firstPeriod);
            // Expect timer period is Service.Interval.
            LLBC_Expect(timer->GetPeriod() == firstPeriod);

            // Execute next test.
            LLBC_Expect(ExecCancelWhenTimerNotInTimeoutHandlingTest() == LLBC_OK);
        });

        LLBC_PrintLn("- Schedule timer...");
        LLBC_Expect(testTimer->Schedule(firstPeriod) == LLBC_OK,
                    "timer: %s",
                    testTimer->ToString().c_str());

        return LLBC_OK;
    }

    int ExecCancelWhenTimerNotInTimeoutHandlingTest()
    {
        LLBC_PrintLn("Execute Cancel() when timer not in timeout-handling test:");

        // Create time(in stack).
        LLBC_Timer timer;
        LLBC_PrintLn("- timer: %s", timer.ToString().c_str());

        // Cancel timer.
        LLBC_Expect(timer.Cancel() == LLBC_OK);

        LLBC_PrintLn("- Schedule timer & cancel:");
        LLBC_Expect(timer.Schedule(LLBC_TimeSpan::zero) == LLBC_OK);
        LLBC_Expect(timer.Cancel() == LLBC_OK);

        // Execute next test.
        LLBC_Expect(ExecCancelInCancelHandler() == LLBC_OK);

        return LLBC_OK;
    }

    int ExecCancelInCancelHandler()
    {
        LLBC_PrintLn("Exec Cancel() in cancel handler test:");

        auto testTimer = new LLBC_Timer;
        testTimer->SetCancelHandler([&](LLBC_Timer *timer) {
            LLBC_PrintLn("In Cancel handler,  timer:%s", testTimer->ToString().c_str());

            LLBC_Expect(!timer->IsScheduled());
            LLBC_Expect(timer->IsHandlingCancel());
            LLBC_Expect(!timer->IsHandlingTimeout());

            LLBC_Expect(timer->Cancel() == LLBC_OK);

            LLBC_Defer(delete timer);

            // Execute next test.
            LLBC_Expect(ExecCancelInTimeoutAndCancelHandler() == LLBC_OK);
        });

        LLBC_Expect(testTimer->Schedule(LLBC_TimeSpan::oneSec * 0.5) == LLBC_OK);
        LLBC_Expect(testTimer->Cancel() == LLBC_OK);

        return LLBC_OK;
    }

    int ExecCancelInTimeoutAndCancelHandler()
    {
        LLBC_PrintLn("Exec Cancel() in timeout & cancel handler test:");

        auto testTimer = new LLBC_Timer;
        testTimer->SetCancelHandler([&](LLBC_Timer *timer) {
            LLBC_PrintLn("In Cancel handler, timer:%s", timer->ToString().c_str());

            LLBC_Expect(!timer->IsScheduled());
            LLBC_Expect(timer->IsHandlingCancel());
            LLBC_Expect(timer->IsHandlingTimeout());

            LLBC_Expect(timer->Cancel() == LLBC_OK);

            LLBC_Defer(delete timer);

            // Execute next test.
            LLBC_Expect(RescheduleTest() == LLBC_OK);
        });

        testTimer->SetTimeoutHandler([&](LLBC_Timer *timer) {
            LLBC_PrintLn("In Timeout handler, timer:%s", timer->ToString().c_str());

            LLBC_Expect(timer->IsScheduled());
            LLBC_Expect(!timer->IsHandlingCancel());
            LLBC_Expect(timer->IsHandlingTimeout());

            LLBC_Expect(timer->Cancel() == LLBC_OK);

        });

        LLBC_Expect(testTimer->Schedule(LLBC_TimeSpan::oneSec * 0.5) == LLBC_OK);

        return LLBC_OK;
    }

    int RescheduleTest()
    {
        LLBC_PrintLn("Reschedule test:");

        auto rescheduleTimeout = [this](LLBC_Timer *timer) {
            LLBC_Defer(delete timer);
            LLBC_PrintLn("- Reschedule timeout, timer: %s", timer->ToString().c_str());

            // Check timer status.
            LLBC_Expect(timer->IsScheduled());
            LLBC_Expect(timer->IsHandlingTimeout());
            LLBC_Expect(!timer->IsHandlingCancel());

            // Execute next test.
            LLBC_Expect(TimeoutTimeMeasureTest() == LLBC_OK);
        };

        // Create timer & set timeout handler.
        auto testTimer = new LLBC_Timer;
        testTimer->SetTimeoutHandler([rescheduleTimeout](LLBC_Timer *timer) {
            const auto oldTimerId = timer->GetTimerId();
            LLBC_PrintLn("- Timeout, timer: %s", timer->ToString().c_str());

            LLBC_PrintLn("- Update timeout handler");
            timer->SetTimeoutHandler(rescheduleTimeout);

            LLBC_PrintLn("- Reschedule timer in timeout handling");
            LLBC_Expect(timer->Schedule(LLBC_TimeSpan::oneMillisec) == LLBC_OK);

            LLBC_Expect(timer->GetTimerId() != oldTimerId, "timer: %s", timer->ToString().c_str());
            LLBC_Expect(timer->IsScheduled());
            LLBC_Expect(!timer->IsHandlingTimeout());
            LLBC_Expect(!timer->IsHandlingCancel());
        });

        // Schedule timer.
        LLBC_PrintLn("- Schedule timer");
        LLBC_Expect(testTimer->Schedule(LLBC_TimeSpan::zero) == LLBC_OK);

        return LLBC_OK;
    }

    int TimeoutTimeMeasureTest()
    {
        LLBC_PrintLn("Timeout time measure test:");

        // Generate firstPeriod&period.
        const auto firstPeriod = LLBC_TimeSpan::FromMillis(LLBC_Rand(1000, 5000));
        const auto period = LLBC_TimeSpan::FromMillis(LLBC_Rand(100, 500));
        LLBC_PrintLn("- firstPeriod: %s, period: %s",
                     firstPeriod.ToString().c_str(), period.ToString().c_str());

        // Create timer and set timeout handler.
        constexpr size_t triggerCount= 10;
        auto testTimer = new LLBC_Timer;
        testTimer->SetTimeoutHandler([this, firstPeriod, period](LLBC_Timer *timer) {
            LLBC_PrintLn("- Timeout, timer:%s", timer->ToString().c_str());
            LLBC_Expect(timer->GetFirstPeriod() == firstPeriod);
            LLBC_Expect(timer->GetPeriod() == period);
            LLBC_Expect(timer->GetTotalTriggerCount() == triggerCount);
            LLBC_Expect(timer->GetTriggeredCount() <= timer->GetTotalTriggerCount());
            if (timer->GetTriggeredCount() == timer->GetTotalTriggerCount())
            {
                LLBC_Defer(delete timer);
                LLBC_PrintLn("- Test done, exec next test");

                // Exec next test.
                if constexpr (LLBC_CFG_CORE_TIMER_LONG_TIMEOUT_TIME <= 5000)
                    LLBC_Expect(LongTimeoutTimeTest() == LLBC_OK);
                else
                    LLBC_Expect(PerfTest() == LLBC_OK);
            }
        });

        // Schedule timer.
        LLBC_Expect(testTimer->Schedule(firstPeriod, period, triggerCount) == LLBC_OK);

        return LLBC_OK;
    }

    int LongTimeoutTimeTest()
    {
        constexpr auto longTimeoutTime = LLBC_TimeSpan::FromMillis(LLBC_CFG_CORE_TIMER_LONG_TIMEOUT_TIME * 2);
        LLBC_PrintLn("Long timeout time test(longTimeoutTime:%s):", longTimeoutTime.ToString().c_str());

        // Test Cancel long timeout timer test(and reschedule again).
        LLBC_PrintLn("- Test cancel long timeout timer after schedule");
        auto testTimer1 = new LLBC_Timer;
        LLBC_Expect(testTimer1->Schedule(longTimeoutTime) == LLBC_OK);
        LLBC_Expect(testTimer1->Cancel() == LLBC_OK);
        LLBC_Expect(testTimer1->GetFirstPeriod() == longTimeoutTime,
                   "After Cancel Scheduled timer, timer.timerData still valid");
        LLBC_Expect(testTimer1->Schedule(longTimeoutTime) == LLBC_OK);
        LLBC_XDelete(testTimer1);

        // Test delete long timeout timer test.
        LLBC_PrintLn("- Test delete long timeout timer after schedule");
        auto testTimer2 = new LLBC_Timer;
        LLBC_Expect(testTimer2->Schedule( longTimeoutTime + LLBC_TimeSpan::oneSec) == LLBC_OK);
        LLBC_XDelete(testTimer2);

        // Test delete long timeout timer in timeout handler test.
        LLBC_PrintLn("- Test delete long timeout timer in timeout handler");
        // Note: This testcase must be less than or equal to 20 seconds.
        LLBC_Expect(longTimeoutTime <= LLBC_TimeSpan::oneSec * 20,
                    "longTimeoutTime:%s", longTimeoutTime.ToString().c_str());
        auto deleteLongTimeoutTimerTestDeleg = [&, longTimeoutTime]() {
            LLBC_PrintLn("Test delete long timeout timer in timeout handler");

            auto testTimer4 = new LLBC_Timer;
            testTimer4->SetTimeoutHandler([&](LLBC_Timer *timer) {
                LLBC_PrintLn("- Long timeout timer timeout, timer:%s", testTimer4->ToString().c_str());
                LLBC_Defer(delete timer);

                LLBC_Expect(PerfTest() == LLBC_OK);
            });

            LLBC_Expect(testTimer4->Schedule(longTimeoutTime) == LLBC_OK);
        };

        // Test cancel long timeout timer in timeout handler test.
        LLBC_PrintLn("- Test cancel long timeout timer in timeout handler");
        auto testTimer3 = new LLBC_Timer;
        testTimer3->SetTimeoutHandler([&, deleteLongTimeoutTimerTestDeleg](LLBC_Timer *timer) {
            LLBC_PrintLn("- Long timeout timer timeout, timer:%s", testTimer3->ToString().c_str());

            LLBC_Expect(timer->IsScheduled());
            LLBC_Expect(timer->IsHandlingTimeout());
            LLBC_Expect(!timer->IsHandlingCancel());

            const auto oldTimerId = timer->GetTimerId();
            LLBC_Expect(timer->Cancel() == LLBC_OK);

            LLBC_Expect(!timer->IsScheduled());
            LLBC_Expect(timer->IsHandlingTimeout());
            LLBC_Expect(!timer->IsHandlingCancel());
            LLBC_Expect(timer->GetTimerId() == oldTimerId,
                        "timerId:%llu, oldTimerId:%llu",
                        timer->GetTimerId(), oldTimerId);

            deleteLongTimeoutTimerTestDeleg();
        });
        testTimer3->Schedule(longTimeoutTime);

        return LLBC_OK;
    }

    int PerfTest()
    {
        #ifdef LLBC_DEBUG
        const size_t timerCount = 500;
        const sint64 minTimeoutTimeInMs = 100;
        const sint64 maxTimeoutTimeInMs = 1000;
        #else
        const size_t timerCount = 50000;
        const sint64 minTimeoutTimeInMs = 200;
        const sint64 maxTimeoutTimeInMs = 2000;
        #endif
        const size_t testTriggerCount = timerCount * 5;

        LLBC_PrintLn("Timer performance test(timerCount:%zu, testTriggerCount:%zu):",
                     timerCount, testTriggerCount);

        _perfTest_BeginTestTime = LLBC_RdTsc();

        std::vector<LLBC_Timer *> timers;
        timers.reserve(timerCount);
        for (size_t i = 0; i < timerCount; ++i)
        {
            auto timer = new LLBC_Timer;
            timer->SetTimeoutHandler([this](LLBC_Timer *timer) {
                #ifdef LLBC_DEBUG
                if (++_perfTest_TriggeredCount % 500 == 0)
                #else
                if (++_perfTest_TriggeredCount % 5000 == 0)
                #endif
                {
                    LLBC_PrintLn("- timeout trigger count: %zu/%zu", _perfTest_TriggeredCount, testTriggerCount);
                    LLBC_PrintLn("  - Trigger timer:%s", timer->ToString().c_str());
                }

                if (_perfTest_TriggeredCount >= testTriggerCount)
                {
                    // Mask test finished.
                    if (_perfTest_TriggeredCount == (testTriggerCount + timerCount - 1))
                    {
                        LLBC_Stopwatch sw(LLBC_RdTsc() - _perfTest_BeginTestTime);
                        LLBC_PrintLn("- Test finished, tirggeredCount:%zu, cost:%s, maxTimeoutTime:%s",
                                     _perfTest_TriggeredCount,
                                     sw.Elapsed().ToString().c_str(),
                                     LLBC_TimeSpan::FromMillis(maxTimeoutTimeInMs).ToString().c_str());

                        _testFinished = true;
                    }

                    delete timer;
                }
            });

            timer->Schedule(LLBC_TimeSpan::FromMillis(LLBC_Rand(minTimeoutTimeInMs, maxTimeoutTimeInMs)));
            timers.push_back(timer);
        }

        return LLBC_OK;
    }

private:
    bool _testStarted = false;
    bool _testFinished = false;

    sint64 _perfTest_BeginTestTime = 0;
    size_t _perfTest_TriggeredCount = 0;
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
