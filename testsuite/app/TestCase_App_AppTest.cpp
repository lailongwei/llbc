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
    class TestComp : public LLBC_Component
    {
    public:
        virtual bool OnStart(bool &startFinished)
        {
            LLBC_PrintLn("Simulate comp start failed case...");
            return false;
        }
    };

    class TestApp : public LLBC_App
    {
    public:
        TestApp()
        {
            _testSvc = nullptr;
        }

    public:
        virtual int OnStart(int argc, char *arg[], bool &startFinished)
        {
            LLBC_PrintLn("Application start, name:%s", GetName().c_str());

            _testSvc = LLBC_Service::Create("TestSvc");
            _testSvc->AddComponent(new TestComp);

            if (_testSvc->Start() != LLBC_OK)
            {
                LLBC_PrintLn("Start %s failed, err:%s", _testSvc->GetName().c_str(), LLBC_FormatLastError());
                return LLBC_FAILED;
            }

            return LLBC_OK;
        }

        virtual void OnStop(bool &stopFinished)
        {
            LLBC_PrintLn("Application stop");
            LLBC_XDelete(_testSvc);
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

    TestApp app;
    app.Start(argc, argv);

    return LLBC_OK;
}
