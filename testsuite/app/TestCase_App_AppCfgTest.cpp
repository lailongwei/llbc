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

#include "app/TestCase_App_AppCfgTest.h"
using namespace llbc;

namespace
{

class TestCompA final : public LLBC_Component
{
public:
    int OnStart(bool &finished) override
    {
        const LLBC_String compName = LLBC_GetTypeName(TestCompA);
        std::cout << "Comp " << compName << " start..." << std::endl;
        std::cout << "- Cfg:\n" << GetConfig().ToString().c_str() << std::endl;

        if (GetService()->GetName() == "TestSvc1")
        {
            cfgReloadTimer_.SetTimeoutHandler(this, &TestCompA::OnTimeout_ReloadCfg);
            cfgReloadTimer_.Schedule(LLBC_TimeSpan::FromSeconds(10));
        }

        return LLBC_OK;
    }

    void OnStop(bool &finished) override
    {
        cfgReloadTimer_.Cancel();
    }

    void OnReload() override
    {
        std::cout << "[" << GetService()->GetName()
                  << "."
                  << LLBC_GetTypeName(*this)
                  << ", ptr:"
                  << this
                  << "] reload"
                  << std::endl;
        std::cout << "- CfgType:" << GetConfigType() << std::endl;
        std::cout << "- Cfg:\n" << GetConfig().ToString().c_str() << std::endl;
    }

private:
    void OnTimeout_ReloadCfg(LLBC_Timer *timer)
    {
        LLBC_App::ThisApp()->Reload();
    }

private:
    LLBC_Timer cfgReloadTimer_;
};

class TestCompB final : public LLBC_Component
{
public:
    int OnStart(bool &finished) override
    {
        const LLBC_String compName = LLBC_GetTypeName(TestCompB);
        std::cout << "Comp " << compName << " start..." << std::endl;
        std::cout << "- Cfg:\n" << GetConfig().ToString().c_str() << std::endl;

        return LLBC_OK;
    }
};

class TestApp final : public LLBC_App
{
public:
    TestApp(LLBC_TimeSpan startNeedTime, LLBC_TimeSpan stopNeedTime)
    : _startNeedTime(startNeedTime)
    , _stopNeedTime(stopNeedTime)
    {
    }

    ~TestApp() override
    {
        Stop();
    }

public:
    int OnEarlyStart(int argc, char *argv[], bool &finished) override
    {
        if (HasConfig())
        {
            std::cout << "App has config" << std::endl;
            std::cout << "- cfg type:" << GetConfigType() << std::endl;
            std::cout << "- cfg path:" << GetConfigPath() << std::endl;
            std::cout << "- cfg cnt:" << GetConfig().ToString() << std::endl;
        }

        return LLBC_OK;
    }

    int OnStart(int argc, char *argv[], bool &startFinished) override
    {
        if (_startTime == LLBC_Time::utcBegin)
        {
            _startTime = LLBC_Time::Now();
            startFinished = false;
        }

        auto cost = LLBC_Time::Now() - _startTime;
        if (cost < _startNeedTime)
        {
            startFinished = false;
            // std::cout <<"App " << GetName() << " starting, cost:" << cost <<std::endl;
            return LLBC_OK;
        }

        std::cout <<"App " << GetName() << "start finished" <<std::endl;

        // Create services.
        std::cout << "Create service <TestSvc1>..." << std::endl;
        auto svc1 = LLBC_Service::Create("TestSvc1");
        svc1->AddComponent(new TestCompA);
        svc1->AddComponent(new TestCompB);
        svc1->Start();
        std::cout << "- TestSvc1 started, fps:" << svc1->GetFPS()
                  << ", interval:" << svc1->GetFrameInterval() << std::endl;

        std::cout << "Create service <TestSvc2>..." << std::endl;
        auto svc2 = LLBC_Service::Create("TestSvc2");
        svc2->AddComponent(new TestCompA);
        svc2->AddComponent(new TestCompB);
        svc2->Start();
        std::cout << "- TestSvc2 started, fps:" << svc2->GetFPS()
                  << ", interval:" << svc2->GetFrameInterval() << std::endl;

        return LLBC_OK;
    }

    void OnLateStart(int argc, char *argv[]) override
    {
        std::cout << "App " <<GetName() <<"late start finished" <<std::endl;
    }

    void OnStop(bool &stopFinished) override
    {
        if (_stopTime == LLBC_Time::utcBegin)
            _stopTime = LLBC_Time::Now();

        auto cost = LLBC_Time::Now() - _stopTime;
        if (cost < _stopNeedTime)
        {
            stopFinished = false;
            // std::cout <<"App " <<GetName() <<" stopping, cost:" <<cost <<std::endl;
        }
        else
        {
            std::cout <<"App " <<GetName() <<" stop finished" <<std::endl;
        }
    }

    void OnUpdate(bool &doNothing) override
    {
        std::cout <<"TestApp::OnUpdate()..." <<std::endl;
        LLBC_Sleep(500);
    }

private:
    LLBC_Time _startTime;
    LLBC_Time _stopTime;
    LLBC_TimeSpan _startNeedTime;
    LLBC_TimeSpan _stopNeedTime;
};

}

TestCase_App_AppCfgTest::TestCase_App_AppCfgTest()
{
}

TestCase_App_AppCfgTest::~TestCase_App_AppCfgTest()
{
}

int TestCase_App_AppCfgTest::Run(int argc, char *argv[])
{
    // Define app object.
    TestApp app(LLBC_TimeSpan::FromSeconds(3), LLBC_TimeSpan::FromSeconds(3));

    // Set config path.
    // If not specific config path, application will auto reload config(order Ini->Cfg->Xml).
    // - ini format config.
    // app.SetConfigPath("./AppCfgTest.ini");
    // properties format config.
    // app.SetConfigPath("./AppCfgTest.cfg");
    // xml format config.
    app.SetConfigPath("./AppCfgTest.xml");

    // Startup app object.
    std::cout <<"Start app..." <<std::endl;
    if (app.Start(argc, argv, "AppCfgTest") != LLBC_OK)
    {
        std::cerr <<"App start failed, err:" <<LLBC_FormatLastError() <<std::endl;
        return LLBC_FAILED;
    }

    return LLBC_OK;
}
