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


#include "comm/TestCase_Comm_SvcFps.h"

namespace
{
    class TestComp : public LLBC_Component
    {
    public:
        TestComp()
        : LLBC_Component(LLBC_ComponentEvents::DefaultEvents | LLBC_ComponentEvents::OnUpdate)
        {
        }

        virtual bool OnStart(bool &startFinished)
        {
            _updateTimes = 0;
            _beginUpdateTime = 0;
            LLBC_PrintLn("Service %s start", GetService()->GetName().c_str());

            return true;
        }

        virtual void OnStop(bool &stopFinished)
        {
            LLBC_PrintLn("Service %s stop", GetService()->GetName().c_str());
        }

        virtual void OnUpdate()
        {
            if (_updateTimes == 0)
                _beginUpdateTime = LLBC_GetMilliSeconds();

            ++_updateTimes;
            if (_updateTimes % 1000000 == 0)
            {
                double elapsed = static_cast<double>(LLBC_GetMilliSeconds() - _beginUpdateTime);
                double updateSpeed = _updateTimes / elapsed;

                LLBC_PrintLn("Elapsed time: %f, updateSpeed(per ms): %.3f", elapsed, updateSpeed);
            }
        }

    private:
        int _updateTimes;
        sint64 _beginUpdateTime;
    };
}

TestCase_Comm_SvcFps::TestCase_Comm_SvcFps()
{
}

TestCase_Comm_SvcFps::~TestCase_Comm_SvcFps()
{
}

int TestCase_Comm_SvcFps::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Service FPS test:");

    LLBC_Service *fpsTestSvc = LLBC_Service::Create("FPSTestService");
    fpsTestSvc->AddComponent(new TestComp);
    fpsTestSvc->SetFPS(LLBC_INFINITE);

    fpsTestSvc->Start();

    LLBC_PrintLn("Press any key to continue...");
    getchar();

    delete fpsTestSvc;

    return LLBC_OK;
}
