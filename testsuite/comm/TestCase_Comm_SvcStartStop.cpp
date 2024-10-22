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


#include "comm/TestCase_Comm_SvcStartStop.h"

namespace
{
    class SvcStartStopTestComp : public LLBC_Component
    {
    public:
        virtual bool OnInit(bool &initFinished)
        {
            LLBC_PrintLn("Service initialize");
            return true;
        }

        virtual void OnDestroy(bool &destroyFinished)
        {
            LLBC_PrintLn("Service destroy");
        }

        virtual bool OnStart(bool &startFinished)
        {
            _updateOutputTimes = 0;
            _idleOutputTimes = 0;
            LLBC_PrintLn("Service start");

            return true;
        }

        virtual void OnStop(bool &stopFinished)
        {
            LLBC_PrintLn("Service stop");
        }

        virtual void OnUpdate()
        {
            if (++_updateOutputTimes % 5 == 0)
                LLBC_PrintLn("OnUpdate call, times: %d", _updateOutputTimes);
        }

        virtual void OnIdle(const LLBC_TimeSpan &idleTime)
        {
            if (++_idleOutputTimes % 5 == 0)
                LLBC_PrintLn("OnIdle call, times: %d", _idleOutputTimes);
        }

    private:
        int _updateOutputTimes;
        int _idleOutputTimes;
    };
}

TestCase_Comm_SvcStartStop::TestCase_Comm_SvcStartStop()
{
}

TestCase_Comm_SvcStartStop::~TestCase_Comm_SvcStartStop()
{
}

int TestCase_Comm_SvcStartStop::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Service start/stop test:");
    LLBC_Service *svc = LLBC_Service::Create("SvcStartStopTest");
    svc->AddComponent(new SvcStartStopTestComp);
    svc->Start();

    LLBC_PrintLn("Service started, sleep 2 sectonds...");
    LLBC_Sleep(2000);

    LLBC_PrintLn("ReStart service...");
    svc->Stop();
    svc->Start(2);

    LLBC_PrintLn("Service restarted, sleep 5 seconds...");
    LLBC_Sleep(5000);

    LLBC_PrintLn("Stop service");
    svc->Stop();
    delete svc;

    LLBC_PrintLn("Press any key to exit...");
    getchar();

    return 0;
}
