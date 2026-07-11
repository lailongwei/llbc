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

#include "app/TestCase_App_AppTest.h"

namespace
{
    class TestComp final : public LLBC_Component
    {
    public:
        int OnInit(bool &initFinished) override
        {
            LLBC_PrintLn("-> TestComp::OnInit...");
            return LLBC_OK;

            // LLBC_PrintLn("Simulate comp init failed case...");
            // return LLBC_FAILED;
        }

        int OnLateInit(bool &finished) override
        {
            LLBC_PrintLn("  -> TestComp::OnLateInit...");
            return LLBC_OK;

            // LLBC_PrintLn("Simulate comp late-init failed case...");
            // return LLBC_FAILED;
        }

        void OnEarlyDestroy(bool &earlyDestroyFinished) override
        {
            LLBC_PrintLn("  <- TestComp::OnEarlyDestroy...");
        }

        void OnDestroy(bool &destroyFinished) override
        {
            LLBC_PrintLn("<- TestComp::OnDestroy...");
        }

    public:
        int OnStart(bool &startFinished) override
        {
            LLBC_PrintLn("    -> TestComp::OnStart...");
            return LLBC_OK;

            // LLBC_PrintLn("Simulate comp start failed case...");
            // return LLBC_FAILED;
        }

        int OnLateStart(bool &lateStartFinished) override
        {
            LLBC_PrintLn("      -> TestComp::OnLateStart...");
            return LLBC_OK;

            // LLBC_PrintLn("Simulate comp late-start failed case...");
            // return LLBC_FAILED;
        }

        void OnEarlyStop(bool &earlyStopFinished) override
        {
            LLBC_PrintLn("      <- TestComp::OnEarlyStop...");
        }

        void OnStop(bool &stopFinished) override
        {
            LLBC_PrintLn("    <- TestComp::OnStop...");
        }
    };

    class TestApp final : public LLBC_App
    {
    public:
        TestApp()
        {
            _testSvc = nullptr;
        }

    public:
        int OnEarlyStart(int argc, char *argv[], bool &earlyStartFinished) override
        {
            LLBC_PrintLn("Application early start");
            return LLBC_OK;

            // LLBC_PrintLn("Simulate app early-start failed case...");
            // return LLBC_FAILED;
        }

        int OnStart(int argc, char *arg[], bool &startFinished) override
        {
            LLBC_PrintLn("Application start, name:%s", GetName().c_str());

            // LLBC_PrintLn("Simulate app start failed case...");
            // return LLBC_FAILED;

            _testSvc = LLBC_Service::Create("TestSvc");
            _testSvc->AddComponent(new TestComp);
            if (_testSvc->Start() != LLBC_OK)
            {
                LLBC_PrintLn("Start %s failed, err:%s", _testSvc->GetName().c_str(), LLBC_FormatLastError());
                return LLBC_FAILED;
            }

            return LLBC_OK;
        }

        void OnLateStart(int argc, char *argv[]) override
        {
            LLBC_PrintLn("Application late start");
        }

        void OnEarlyStop(bool &earlyStopFinished) override
        {
            LLBC_PrintLn("Application early stop");
        }

        void OnStop(bool &stopFinished) override
        {
            LLBC_PrintLn("Application stop");
            // Service(s) has been destroyed by Application, do not delete again service.
            // LLBC_XDelete(_testSvc);
        }

        void OnLateStop() override
        {
            LLBC_PrintLn("Application late stop");
        }

        void OnReload() override
        {
            LLBC_PrintLn("Application reload");
            LLBC_PrintLn("- App config: %s", GetConfig().ToString().c_str());
        }

    private:
        LLBC_Service *_testSvc;
    };
}

TestCase_App_AppTest::TestCase_App_AppTest()
{
}

TestCase_App_AppTest::~TestCase_App_AppTest()
{
}

int TestCase_App_AppTest::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Application/AppTest(Press Ctrl+C to exit):");

    // Create app object.
    TestApp app;

    // Set server config.
    // - ini format config.
    app.SetConfigPath("./AppCfgTest.ini");
    // - properties format config.
    // app.SetConfigPath("./AppCfgTest.cfg");
    // - xml format config.
    // app.SetConfigPath("./AppCfgTest.xml");


    return app.Start(argc, argv);
}
