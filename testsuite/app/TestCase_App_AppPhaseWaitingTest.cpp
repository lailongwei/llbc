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
    void operator ()(bool isApp,
                     const LLBC_String &name,
                     const char *phaseStr,
                     LLBC_Time &begTime,
                     LLBC_TimeSpan &waitTime,
                     bool &finFlag)
    {
        const char *operatorType = isApp ? "App" : "Service";
        if (begTime == LLBC_Time::UTCBegin)
            begTime = LLBC_Time::Now();

        const auto alreadyWait = LLBC_Time::Now() - begTime;
        if (alreadyWait < waitTime)
        {
            finFlag = false;
            fprintf(stdout,
                    "[%s]%s %s %sing...\n",
                    alreadyWait.ToString().c_str(),
                    operatorType,
                    name.c_str(),
                    phaseStr);
        }
        else
        {
            finFlag = true;
            fprintf(stdout, "%s %s %s finished\n", operatorType, name.c_str(), phaseStr);
        }
    }
};

class TestCompBase : public LLBC_Component
{
public:
    TestCompBase()
    {
        _initWaitTime = _destroyWaitTime = LLBC_TimeSpan::FromSeconds(3);
        _startWaitTime = _lateStartWaitTime = _earlyStopWaitTime = _stopWaitTime = LLBC_TimeSpan::FromSeconds(2);
    }

    virtual ~TestCompBase() = default;

public:
    virtual bool OnInit(bool &initFinished)
    {
        TestWaiter()(false, LLBC_GetTypeName(*this), "init", _initTime, _initWaitTime, initFinished);
        return true;
    }

    virtual void OnDestroy(bool &destroyFinished)
    {
        TestWaiter()(false, LLBC_GetTypeName(*this), "destroy", _destroyTime, _destroyWaitTime, destroyFinished);
    }

    virtual bool OnStart(bool &startFinished)
    {
        TestWaiter()(false, LLBC_GetTypeName(*this), "start", _startTime, _startWaitTime, startFinished);
        return true;
    }

    virtual void OnLateStart(bool &lateStartFinished)
    {
        TestWaiter()(false, LLBC_GetTypeName(*this), "late-start", _lateStartTime, _lateStartWaitTime, lateStartFinished);
    }

    virtual void OnEarlyStop(bool &earlyStopFinished)
    {
        TestWaiter()(false, LLBC_GetTypeName(*this), "early-stop", _earlyStopTime, _earlyStopWaitTime, earlyStopFinished);
    }

    virtual void OnStop(bool &stopFinished)
    {
        TestWaiter()(false, LLBC_GetTypeName(*this), "stop", _stopTime, _stopWaitTime, stopFinished);
    }

    virtual void OnAppEarlyStart()
    {
        std::cout << LLBC_GetTypeName(*this) << ": OnAppEarlyStart()..." << std::endl;
    }

    virtual void OnAppStartFail()
    {
        std::cout << LLBC_GetTypeName(*this) << ": OnAppStartFail()..." << std::endl;
    }

    virtual void OnAppStartFinish()
    {
        std::cout << LLBC_GetTypeName(*this) << ": OnAppStartFinish()..." << std::endl;
    }

    virtual void OnAppEarlyStop()
    {
        std::cout << LLBC_GetTypeName(*this) << ": OnAppEarlyStop()..." << std::endl;
    }

private:
    LLBC_Time _initTime, _destroyTime;
    LLBC_Time _startTime, _lateStartTime, _earlyStopTime, _stopTime;

    LLBC_TimeSpan _initWaitTime, _destroyWaitTime;
    LLBC_TimeSpan _startWaitTime, _lateStartWaitTime, _earlyStopWaitTime, _stopWaitTime;
};

class TestCompA : public TestCompBase
{};

class TestCompB : public TestCompBase
{};

class TestCompC : public TestCompBase
{};

class PreAddTestComp : public LLBC_Component
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
    virtual bool OnInit(bool &finished)
    {
        _timer.SetTimeoutHandler(this, &PreAddTestComp::OnTimeout);
        return true;
    }

    virtual void OnDestroy(bool &finished)
    {
        _timer.SetTimeoutHandler(nullptr);
    }

    virtual bool OnStart(bool &finished)
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

        return true;
    }

    virtual void OnStop(bool &finshed)
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

class TestApp : public LLBC_App
{
public:
    TestApp()
    {
        _startWaitTime = LLBC_TimeSpan::FromSeconds(5);
        _stopWaitTime = LLBC_TimeSpan::FromSeconds(5);
    }
    virtual ~TestApp() = default;

public:
    virtual int OnStart(int argc, char *argv[], bool &startFinished)
    {
        TestWaiter()(true, GetName(), "start", _startTime, _startWaitTime, startFinished);
        
        if (startFinished)
        {
            auto testSvc = LLBC_Service::Create();
            testSvc->AddComponent(new PreAddTestComp);
            testSvc->AddComponent(new TestCompA);
            testSvc->AddComponent(new TestCompB);
            testSvc->AddComponent(new TestCompC);

            testSvc->Start();
        }

        return LLBC_OK;
    }

    virtual void OnStop(bool &stopFinished)
    {
        TestWaiter()(true, GetName(), "stop", _stopTime, _stopWaitTime, stopFinished);
    }

private:
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