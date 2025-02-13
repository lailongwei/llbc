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

#include "app/TestCase_App_AppPhaseWaitingTest.h"

namespace
{

class TestWaiter
{
public:
    void operator()(bool isApp,
                    const LLBC_String &name,
                    const char *phaseStr,
                    LLBC_Time &begTime,
                    const LLBC_TimeSpan &waitTime,
                    LLBC_Time &notFinishedLogTime,
                    bool &finFlag)
    {
        const char *operatorType = isApp ? "App" : "Service";
        if (begTime == LLBC_Time::utcBegin)
        {
            begTime = LLBC_Time::Now();
            notFinishedLogTime = begTime;
        }

        const auto now = LLBC_Time::Now();
        const auto alreadyWait = now - begTime;
        if (alreadyWait < waitTime)
        {
            finFlag = false;
            if (now - notFinishedLogTime >= LLBC_TimeSpan::oneMillisec * 200)
            {
                fprintf(stdout,
                        "[%s]%s %s %sing...\n",
                        alreadyWait.ToString().c_str(),
                        operatorType,
                        name.c_str(),
                        phaseStr);

                notFinishedLogTime = now;
            }
        }
        else
        {
            finFlag = true;
            fprintf(stdout,
                    "[%s]%s %s %s finished\n",
                    alreadyWait.ToString().c_str(), operatorType, name.c_str(), phaseStr);
        }
    }
};

class TestCompBase : public LLBC_Component
{
public:
    TestCompBase()
    {
        _initWaitTime = _lateInitWaitTime = _earlyDestroyWaitTime = _destroyWaitTime = LLBC_TimeSpan::FromSeconds(3);
        _startWaitTime = _lateStartWaitTime = _earlyStopWaitTime = _stopWaitTime = LLBC_TimeSpan::FromSeconds(2);
    }

    ~TestCompBase() override = default;

public:
    int OnInit(bool &initFinished) override
    {
        // In service initing, timer will not be scheduled.
        if (_initTime == LLBC_Time::utcBegin)
            SchedulePhaseTimer(_initTimer, "Init", false);

        TestWaiter()(false, LLBC_GetTypeName(*this), "init", _initTime, _initWaitTime, _notFinishedLogTime, initFinished);

        if (initFinished)
            _initTimer.Cancel();

        return LLBC_OK;
    }

    int OnLateInit(bool &lateInitFinished) override
    {
        // In service late-initing, timer will not be scheduled.
        if (_lateInitTime == LLBC_Time::utcBegin)
            SchedulePhaseTimer(_lateInitTimer, "LateInit", false);

        TestWaiter()(false, LLBC_GetTypeName(*this), "late-init", _lateInitTime, _lateInitWaitTime, _notFinishedLogTime, lateInitFinished);

        if (lateInitFinished)
            _lateInitTimer.Cancel();

        return LLBC_OK;
    }

    void OnEarlyDestroy(bool &earlyDestroyFinished) override
    {
        // In service early-destroying, timer will not be scheduled.
        if (_earlyDestroyTime == LLBC_Time::utcBegin)
            SchedulePhaseTimer(_earlyDestroyTimer, "EarlyDestroy", false);

        TestWaiter()(false, LLBC_GetTypeName(*this), "early-destroy", _earlyDestroyTime, _earlyDestroyWaitTime, _notFinishedLogTime, earlyDestroyFinished);

        if (earlyDestroyFinished)
            _earlyDestroyTimer.Cancel();
    }

    void OnDestroy(bool &destroyFinished) override
    {
        // In service destroying, timer will not be scheduled.
        if (_destroyTime == LLBC_Time::utcBegin)
            SchedulePhaseTimer(_destroyTimer, "Destroy", false);

        TestWaiter()(false, LLBC_GetTypeName(*this), "destroy", _destroyTime, _destroyWaitTime, _notFinishedLogTime, destroyFinished);

        if (destroyFinished)
            _destroyTimer.Cancel();
    }

    int OnStart(bool &startFinished) override
    {
        // In service starting, timer will be scheduled.
        if (_startTime == LLBC_Time::utcBegin)
            SchedulePhaseTimer(_startTimer, "Start", true);

        TestWaiter()(false, LLBC_GetTypeName(*this), "start", _startTime, _startWaitTime, _notFinishedLogTime, startFinished);

        if (startFinished)
            _startTimer.Cancel();

        return LLBC_OK;
    }

    int OnLateStart(bool &lateStartFinished) override
    {
        // In service late-starting, timer will be scheduled.
        if (_lateStartTime == LLBC_Time::utcBegin)
            SchedulePhaseTimer(_lateStartTimer, "LateStart", true);

        TestWaiter()(false, LLBC_GetTypeName(*this), "late-start", _lateStartTime, _lateStartWaitTime, _notFinishedLogTime, lateStartFinished);

        if (lateStartFinished)
            _lateStartTimer.Cancel();

        return LLBC_OK;
    }

    void OnEarlyStop(bool &earlyStopFinished) override
    {
        // In service early-stopping, timer will be scheduled.
        if (_earlyStopTime == LLBC_Time::utcBegin)
            SchedulePhaseTimer(_earlyStopTimer, "EarlyStop", true);

        TestWaiter()(false, LLBC_GetTypeName(*this), "early-stop", _earlyStopTime, _earlyStopWaitTime, _notFinishedLogTime, earlyStopFinished);

        if (earlyStopFinished)
            _earlyStopTimer.Cancel();
    }

    void OnStop(bool &stopFinished) override
    {
        // In service stopping, timer will be scheduled.
        if (_stopTime == LLBC_Time::utcBegin)
            SchedulePhaseTimer(_stopTimer, "Stop", true);

        TestWaiter()(false, LLBC_GetTypeName(*this), "stop", _stopTime, _stopWaitTime, _notFinishedLogTime, stopFinished);

        if (stopFinished)
            _stopTimer.Cancel();
    }

    void OnEvent(int eventType, const LLBC_Variant &eventParams) override
    {
        switch(eventType)
        {
            case LLBC_ComponentEventType::AppWillStart:
            {
                OnAppWillStart();
                break;
            }
            case LLBC_ComponentEventType::AppStartFailed:
            {
                OnAppStartFailed();
                break;
            }
            case LLBC_ComponentEventType::AppStartFinished:
            {
                OnAppStartFinished();
                break;
            }
            case LLBC_ComponentEventType::AppWillStop:
            {
                OnAppWillStop();
                break;
            }
            default:
                break;
        }
    }

private:
    void OnAppWillStart()
    {
        std::cout << LLBC_GetTypeName(*this) << ": OnAppWillStart()..." << std::endl;
    }

    void OnAppStartFailed()
    {
        std::cout << LLBC_GetTypeName(*this) << ": OnAppStartFailed()..." << std::endl;
    }

    void OnAppStartFinished()
    {
        std::cout << LLBC_GetTypeName(*this) << ": OnAppStartFinished()..." << std::endl;
    }

    void OnAppWillStop()
    {
        std::cout << LLBC_GetTypeName(*this) << ": OnAppWillStop()..." << std::endl;
    }

    void SchedulePhaseTimer(LLBC_Timer &timer, const char *phaseDesc, bool exceptTimeout)
    {
        timer.SetTimeoutHandler([this, phaseDesc, exceptTimeout](LLBC_Timer *timer) {
            assert(exceptTimeout);
            LLBC_PrintLn("%s-%s Timeout: timeout times:%lu",
                         LLBC_GetTypeName(*this), phaseDesc, timer->GetTimeoutTimes());
        });

        timer.Schedule(LLBC_TimeSpan::FromMillis(LLBC_Rand(200, 400)));
    }

private:
    LLBC_Time _notFinishedLogTime;

    LLBC_Time _initTime, _lateInitTime;
    LLBC_Time _earlyDestroyTime, _destroyTime;
    LLBC_Time _startTime, _lateStartTime;
    LLBC_Time _earlyStopTime, _stopTime;

    LLBC_TimeSpan _initWaitTime, _lateInitWaitTime;
    LLBC_TimeSpan _earlyDestroyWaitTime, _destroyWaitTime;
    LLBC_TimeSpan _startWaitTime, _lateStartWaitTime;
    LLBC_TimeSpan _earlyStopWaitTime, _stopWaitTime;

private:
    LLBC_Timer _initTimer, _lateInitTimer;
    LLBC_Timer _startTimer, _lateStartTimer;
    LLBC_Timer _earlyStopTimer, _stopTimer;
    LLBC_Timer _earlyDestroyTimer, _destroyTimer;
};

class TestCompA final : public TestCompBase
{};

class TestCompB final : public TestCompBase
{};

class TestCompC final : public TestCompBase
{};

class PreAddTestComp final : public LLBC_Component
{
    class TestEvIds
    {
    public:
        enum
        {
            Begin = 1,
            Ev1 = Begin,
            Ev2,

            End,
        };
    };

public:
    ~PreAddTestComp() override
    {
        LLBC_PrintLn("PreAddTestComp::~PreAddTestComp() called");
    }

public:
    int OnInit(bool &finished) override
    {
        _timer.SetTimeoutHandler(this, &PreAddTestComp::OnTimeout);
        return LLBC_OK;
    }

    void OnDestroy(bool &finished) override
    {
        _timer.SetTimeoutHandler(nullptr);
    }

    int OnStart(bool &finished) override
    {
        LLBC_Service *svc = GetService();
        svc->SubscribeEvent(TestEvIds::Ev1, [](LLBC_Event &ev)
        {
            LLBC_PrintLn("%s, Ev1 handler called, ev[\"val\"]:%s",
                           LLBC_GetTypeName(PreAddTestComp),
                           ev["val"].ToString().c_str());
        });

        svc->SubscribeEvent(TestEvIds::Ev2, [](LLBC_Event &ev)
        {
            LLBC_PrintLn("%s, Ev2 handler called, ev[\"val\"]:%s",
                           LLBC_GetTypeName(PreAddTestComp),
                           ev["val"].ToString().c_str());
        });

        _timer.Schedule(LLBC_TimeSpan::FromSeconds(5));

        LLBC_PrintLn("%s: OnStart", LLBC_GetTypeName(PreAddTestComp));

        return LLBC_OK;
    }

    void OnStop(bool &finshed) override
    {
        LLBC_PrintLn("%s: OnStop", LLBC_GetTypeName(PreAddTestComp));

        _timer.Cancel();

        LLBC_Service *svc = GetService();
        svc->UnsubscribeEvent(TestEvIds::Ev1);
        svc->UnsubscribeEvent(TestEvIds::Ev2);
    }

private:
    void OnTimeout(LLBC_Timer *timer)
    {
        LLBC_PrintLn("%s: timeout, timer can working....", LLBC_GetTypeName(PreAddTestComp));

        const int evId = LLBC_Rand(TestEvIds::Begin, TestEvIds::End);
        LLBC_Event *ev = new LLBC_Event(evId);
        (*ev)["val"] = LLBC_String().format("Event id is:%d", evId);
        GetService()->FireEvent(ev);
    }

private:
    LLBC_Timer _timer;
};

class TestApp final : public LLBC_App
{
public:
    TestApp()
    {
        _startWaitTime = LLBC_TimeSpan::FromSeconds(5);
        _stopWaitTime = LLBC_TimeSpan::FromSeconds(5);
    }
    ~TestApp() override = default;

public:
    int OnStart(int argc, char *argv[], bool &startFinished) override
    {
        TestWaiter()(true, GetName(), "start", _startTime, _startWaitTime, _notFinishedLogTime, startFinished);
        
        if (startFinished)
        {
            auto testSvc = LLBC_Service::Create();
            testSvc->AddComponent(new PreAddTestComp);
            testSvc->AddComponent(new TestCompA);
            testSvc->AddComponent(new TestCompB);
            testSvc->AddComponent(new TestCompC);

            if (testSvc->Start() != LLBC_OK)
            {
                LLBC_FilePrintLn(stderr, "Start service failed, err:%s", LLBC_FormatLastError());
                return LLBC_FAILED;
            }
        }

        return LLBC_OK;
    }

    void OnStop(bool &stopFinished) override
    {
        TestWaiter()(true, GetName(), "stop", _stopTime, _stopWaitTime, _notFinishedLogTime, stopFinished);
    }

private:
    LLBC_Time _notFinishedLogTime;

    LLBC_Time _startTime;
    LLBC_Time _stopTime;
    LLBC_TimeSpan _startWaitTime;
    LLBC_TimeSpan _stopWaitTime;
};

}

int TestCase_App_AppPhaseWaitingTest::Run(int argc, char *argv[])
{
    TestApp app;
    if (app.Start(argc, argv, "AppPhaseWaitingTestApp") != LLBC_OK)
    {
        fprintf(stderr, "App start failed, err:%s\n", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    app.Stop();

    return 0;
}