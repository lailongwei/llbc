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

#include "application/TestCase_App_AppTest.h"

namespace
{
    class TestApp : public LLBC_IApplication
    {
    public:
        TestApp()
        {
            _testSvc = NULL;
        }

    public:
        virtual int OnStart(int argc, char *arg[])
        {
            LLBC_PrintLine("Application start, create new service for test");

            // You can uncomment below codes for test Application.Wait()
            // _testSvc = LLBC_IService::Create(LLBC_IService::Normal);
            // _testSvc->Start();

            // You can uncomment below codes for test SetDumpFile()
            // #if LLBC_TARGET_PLATFORM_WIN32
            // const LLBC_String &dumpFileName = GetName();
            // LLBC_PrintLine("Set dump file: %s", dumpFileName.c_str());
            // SetDumpFile(dumpFileName);

            // LLBC_PrintLine("Raise division by 0 error!");
            // int val1 = 3;
            // int val2 = 0;
            // val1 /= val2;
            // #endif // Win32

            return 0;
        }

        virtual void OnWait()
        {
            LLBC_PrintLine("Application wait");
        }

        virtual void OnStop()
        {
            LLBC_PrintLine("Application stop");
            LLBC_XDelete(_testSvc);
        }

    private:
        LLBC_IService *_testSvc;
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
    LLBC_PrintLine("Application/AppTest:");

    TestApp app;
    app.Start("TestApp", argc, argv);
    app.Stop();

    LLBC_PrintLine("Press any key to continue...");
    getchar();

    return LLBC_OK;
}


