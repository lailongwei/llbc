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

#include "app/TestCase_App_ReloadFailedTest.h"

namespace
{
    class TestComp final : public LLBC_Component
    {
    public:
        void OnEvent(int eventType, const LLBC_Variant &eventParams) override
        {
            if (eventType != LLBC_ComponentEventType::AppReloadFailed)
                return;

            const auto failedInfo = eventParams.AsPtr<LLBC_AppReloadFailedInfo>();
            LLBC_PrintLn("AppReloadFailed! %s", failedInfo->ToString().c_str());
        }
    };

    class TestApp final : public LLBC_App
    {
    public:
        int OnStart(int argc, char *argv[], bool &startFinished) override
        {
            LLBC_PrintLn("Application start, name:%s", GetName().c_str());

            auto svc = LLBC_Service::Create("TestSvc");
            svc->AddComponent(new TestComp);
            return svc->Start();
        }

        void OnStop(bool &stopFinished) override
        {
            LLBC_PrintLn("Application stop");
        }
    };
}

TestCase_App_ReloadFailedTest::TestCase_App_ReloadFailedTest()
{
}

TestCase_App_ReloadFailedTest::~TestCase_App_ReloadFailedTest()
{
}

int TestCase_App_ReloadFailedTest::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Application/ReloadFailedTest(Press Ctrl+C to exit):");

    TestApp app;

    // Set server config.
    // - ini format config.
    app.SetConfigPath("./AppCfgTest.ini");
    // - properties format config.
    // app.SetConfigPath("./AppCfgTest.cfg");
    // - xml format config.
    // app.SetConfigPath("./AppCfgTest.xml");
    
    // - Broken ini config.
    // app.SetConfigPath("./AppCfgReloadFailedTest.ini");
    // - Broken cfg config.
    // app.SetConfigPath("./AppCfgReloadFailedTest.cfg");
    // - Broken xml config.
    // app.SetConfigPath("./AppCfgReloadFailedTest.xml");

    return app.Start(argc, argv);
}
