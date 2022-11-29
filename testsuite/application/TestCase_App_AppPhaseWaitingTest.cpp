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

#include "application/TestCase_App_AppPhaseWaitingTest.h"

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
    virtual bool OnInitialize(bool &initFinished)
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

    virtual void OnApplicationEarlyStart()
    {
        std::cout << LLBC_GetTypeName(*this) << ": OnApplicationEarlyStart()..." << std::endl;
    }

    virtual void OnApplicationStartFail()
    {
        std::cout << LLBC_GetTypeName(*this) << ": OnApplicationStartFail()..." << std::endl;
    }

    virtual void OnApplicationStartFinish()
    {
        std::cout << LLBC_GetTypeName(*this) << ": OnApplicationStartFinish()..." << std::endl;
    }

    virtual void OnApplicationEarlyStop()
    {
        std::cout << LLBC_GetTypeName(*this) << ": OnApplicationEarlyStop()..." << std::endl;
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

class TestApp : public LLBC_Application
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
            testSvc->AddComponent(new TestCompA);
            testSvc->AddComponent(new TestCompB);

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