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
    class TestApp : public LLBC_Application
    {
    public:
        TestApp()
        {
            _testSvc = nullptr;
        }

    public:
        virtual int OnStart(int argc, char *arg[], bool &startFinished)
        {
            LLBC_PrintLine("Application start, create new service for test");

            // You can uncomment below codes for test Application.Wait()
            // _testSvc = LLBC_IService::Create(LLBC_IService::Normal);
            // _testSvc->Start();

            // Test unhandled exception proc
            TestUnhandledExceptionProc();

            return 0;
        }

        virtual bool OnStop()
        {
            LLBC_PrintLine("Application stop");
            LLBC_XDelete(_testSvc);

            return true;
        }

    private:
        void TestUnhandledExceptionProc()
        {
            #if LLBC_TARGET_PLATFORM_WIN32
            LLBC_PrintLine("Test unhandled exception proc:");

            const LLBC_String &dumpFileName = GetName();
            LLBC_PrintLine("Set dump file: %s", dumpFileName.c_str());
            SetDumpFile(dumpFileName);

            // Test division by 0 error
            TestUnhandledExceptionProc_DivisionByZero();

            // Test invalid pointer read
            // TestUnhandledExceptionProc_InvalidPtrRead();

            // Test invalid pointer write
            // TestUnhandledExceptionProc_InvalidPtrWrite();
            #endif // Win32
        }

        void TestUnhandledExceptionProc_DivisionByZero()
        {
            #if LLBC_TARGET_PLATFORM_WIN32
            LLBC_PrintLine("Raise division by 0 error, please input 0!");
            int val1 = 3;
            int val2 = 0;

            std::cin >> val2;
            val1 = val1 / val2;

            LLBC_PrintLine("%d", val1);
            #endif // Win32
        }

        void TestUnhandledExceptionProc_InvalidPtrWrite()
        {
            LLBC_PrintLine("Test invalid pointer write");

            int *invalidPtr4Write = nullptr;
            *invalidPtr4Write = 3;

            LLBC_PrintLine("%d", *invalidPtr4Write);
        }

        void TestUnhandledExceptionProc_InvalidPtrRead()
        {
            LLBC_PrintLine("Test invalid pointer read");

            int *invalidPtr4Write = nullptr;
            LLBC_PrintLine("%d", *invalidPtr4Write);
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
