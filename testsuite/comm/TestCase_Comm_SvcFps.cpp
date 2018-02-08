/**
 * @file    TestCase_Comm_SvcFps.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2018/02/08
 * @version 1.0
 * 
 * @brief
 */

#include "comm/TestCase_Comm_SvcFps.h"

namespace
{
    class TestFacade : public LLBC_IFacade
    {
        virtual void OnStart()
        {
            _updateTimes = 0;
            _beginUpdateTime = 0;
            LLBC_PrintLine("Service %s start", GetService()->GetName().c_str());
        }

        virtual void OnStop()
        {
            LLBC_PrintLine("Service %s stop", GetService()->GetName().c_str());
        }

        virtual void OnUpdate()
        {
            if (_updateTimes == 0)
                _beginUpdateTime = LLBC_GetMilliSeconds();

            ++_updateTimes;
            if (_updateTimes % 10000000 == 0)
            {
                double elapsed = static_cast<double>(LLBC_GetMilliSeconds() - _beginUpdateTime);
                double updateSpeed = _updateTimes / elapsed;

                LLBC_PrintLine("Elapsed time: %f, updateSpeed(per ms): %.3f", elapsed, updateSpeed);
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
    LLBC_PrintLine("Service FPS test:");

    LLBC_IService *fpsTestSvc = LLBC_IService::Create(LLBC_IService::Normal, "FPSTestService");
    fpsTestSvc->RegisterFacade(LLBC_New(TestFacade));
    fpsTestSvc->SetFPS(LLBC_INFINITE);

    fpsTestSvc->Start();

    LLBC_PrintLine("Press any key to continue...");
    getchar();

    LLBC_Delete(fpsTestSvc);

    return LLBC_OK;
}
