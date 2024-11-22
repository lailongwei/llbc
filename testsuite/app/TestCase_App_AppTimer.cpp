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

#include "app/TestCase_App_AppTimer.h"
using namespace llbc;

namespace
{

class TestApp final : public LLBC_App
{
public:
    TestApp()
    : _phaseTimers{new LLBC_Timer(), new LLBC_Timer(), new LLBC_Timer(), new LLBC_Timer()}
    , _phaseMaxWaitTimes{LLBC_TimeSpan::oneSec * 5,
                         LLBC_TimeSpan::oneSec * 5,
                         LLBC_TimeSpan::oneSec * 3.5,
                         LLBC_TimeSpan::oneSec * 3.5}
    , _phaseNames{"EarlyStart", "Start", "EarlyStop", "Stop"}
    {
    }

    ~TestApp() override
    {
        for (size_t i = 0; i < sizeof(_phaseTimers) / sizeof(_phaseTimers[0]); ++i)
            delete _phaseTimers[i];
    }

public:
    int OnEarlyStart(int argc, char *argv[], bool &earlyStartFinished) override
    {
        // Log app will start.
        if (!_phaseTimers[0]->IsScheduling())
            LLBC_PrintLn("App[%s] will start...", GetName().c_str());

        return _OnPhase(0, earlyStartFinished);
    }

    int OnStart(int argc, char *argv[], bool &startFinished) override
    {
        return _OnPhase(1, startFinished);
    }

    void OnLateStart(int argc, char *argv[]) override
    {
        // Cancel EarlyStart/Start timers.
        _phaseTimers[0]->Cancel();
        _phaseTimers[1]->Cancel();

        // Log start finished.
        LLBC_PrintLn("App start finished(press <Ctrl + C> to exit app)");

        // Schedule update timer.
        _updateTimer.SetTimeoutHandler([](LLBC_Timer *timer) {
            LLBC_PrintLn("- %s Update timer timeout...", LLBC_Time::Now().ToString().c_str());
        });
        _updateTimer.Schedule(LLBC_TimeSpan::oneMillisec * 500);
    }

    void OnEarlyStop(bool &earlyStopFinished) override
    {
        // Log app will stop & Cancel update timer.
        if (!_phaseTimers[2]->IsScheduling())
        {
            LLBC_PrintLn("App will stop...");
            _updateTimer.Cancel();
        }

        (void)_OnPhase(2, earlyStopFinished);
    }

    void OnStop(bool &stopFinished) override
    {
        (void)_OnPhase(3, stopFinished);
    }

    void OnLateStop() override
    {
        // Cancel EarlyStop/Stop timers.
        _phaseTimers[2]->Cancel();
        _phaseTimers[3]->Cancel();

        // Log app stop finished.
        LLBC_PrintLn("App stop finished");
    }

private:
    int _OnPhase(int phaseIndex, bool &finished)
    {
        const auto now = LLBC_Time::Now();
        auto phaseName = _phaseNames[phaseIndex];
        auto &phaseTimer = *_phaseTimers[phaseIndex];
        auto &phaseMaxWaitTime = _phaseMaxWaitTimes[phaseIndex];
        if (!phaseTimer.IsScheduling())
        {
            _phaseBeginTime = now;
            phaseTimer.SetTimeoutHandler([phaseName](LLBC_Timer *timer) {
                LLBC_PrintLn("- %s %s timer timeout...", LLBC_Time::Now().ToString().c_str(), phaseName);
            });

            phaseTimer.Schedule(LLBC_TimeSpan::oneMillisec * 500);
        }

        finished = (now - _phaseBeginTime >= phaseMaxWaitTime);
        if (finished)
            LLBC_PrintLn("%s finished", phaseName);

        return LLBC_OK;
    }

private:
    // Phase timers: EarlyStart/Start/EarlyStop/Stop
    LLBC_Timer *_phaseTimers[4];
    // Phase max wait times: EarlyStart/Start/EarlyStop/Stop.
    LLBC_TimeSpan _phaseMaxWaitTimes[4];
    // Phase names: EarlyStart/Start/EarlyStop/Stop.
    const char *_phaseNames[4];
    // Phase begin time.
    LLBC_Time _phaseBeginTime;

    // Update timer.
    LLBC_Timer _updateTimer;
};

}

TestCase_App_AppTimer::TestCase_App_AppTimer()
{
}

TestCase_App_AppTimer::~TestCase_App_AppTimer()
{
}

int TestCase_App_AppTimer::Run(int argc, char *argv[])
{
    LLBC_PrintLn("App timer test:");

    TestApp app;
    if (app.Start(argc, argv) != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr, "Start app failed, error:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    return LLBC_OK;
}
