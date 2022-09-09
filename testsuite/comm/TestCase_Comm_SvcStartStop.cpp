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
        virtual bool OnInitialize(bool &initFinished)
        {
            LLBC_PrintLine("Service initialize");
            return true;
        }

        virtual void OnDestroy(bool &destroyFinished)
        {
            LLBC_PrintLine("Service destroy");
        }

        virtual bool OnStart(bool &startFinished)
        {
            _updateOutputTimes = 0;
            _idleOutputTimes = 0;
            LLBC_PrintLine("Service start");

            return true;
        }

        virtual void OnStop(bool &stopFinished)
        {
            LLBC_PrintLine("Service stop");
        }

        virtual void OnUpdate()
        {
            if (_updateOutputTimes == 5)
                return;

            LLBC_PrintLine("OnUpdate call, times: %d", ++_updateOutputTimes);
        }

        virtual void OnIdle(int idleTime)
        {
            if (_idleOutputTimes == 5)
                return;

            LLBC_PrintLine("OnIdle call, times: %d", ++_idleOutputTimes);
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
    LLBC_PrintLine("Service start/stop test:");
    LLBC_IService *svc = LLBC_IService::Create(LLBC_IService::Normal, "SvcStartStopTest");
    svc->AddComponent(LLBC_New(SvcStartStopTestComp));
    svc->Start();

    LLBC_PrintLine("Service started, sleep 2 sectonds...");
    LLBC_Sleep(2000);

    LLBC_PrintLine("ReStart service...");
    svc->Stop();
    svc->Start(2);

    LLBC_PrintLine("Service restarted, sleep 5 seconds...");
    LLBC_Sleep(5000);

    LLBC_PrintLine("Stop service");
    svc->Stop();
    LLBC_Delete(svc);

    LLBC_PrintLine("Press any key to exit...");
    getchar();

    return 0;
}
