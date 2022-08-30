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

#include "application/TestCase_App_AppCfgTest.h"
using namespace llbc;

namespace
{

class TestApp : public LLBC_Application
{
public:
    TestApp(LLBC_TimeSpan startNeedTime, LLBC_TimeSpan stopNeedTime)
    : _startNeedTime(startNeedTime)
    , _stopNeedTime(stopNeedTime)
    {
    }

    virtual ~TestApp()
    {
        Stop();
    }

public:
    virtual int OnStart(int argc, char *argv[], bool &startFinished)
    {
        if (_startTime == LLBC_Time::UTCBegin)
        {
            _startTime = LLBC_Time::Now();
            startFinished = false;
        }

        auto cost = LLBC_Time::Now() - _startTime;
        if (cost < _startNeedTime)
        {
            startFinished = false;
            std::cout <<"App " <<GetName() << " starting, cost:" <<cost <<std::endl;
        }
        else
        {
            std::cout <<"App " <<GetName() <<"start finished" <<std::endl;
        }

        return LLBC_OK;
    }

    virtual bool OnStop()
    {
        if (_stopTime == LLBC_Time::UTCBegin)
            _stopTime = LLBC_Time::Now();

        auto cost = LLBC_Time::Now() - _stopTime;
        if (cost < _stopNeedTime)
        {
            std::cout <<"App " <<GetName() <<" stopping, cost:" <<cost <<std::endl;
            return false;
        }
        else
        {
            std::cout <<"App " <<GetName() <<" stop finished" <<std::endl;
            return true;
        }
    }

private:
    LLBC_Time _startTime;
    LLBC_Time _stopTime;
    LLBC_TimeSpan _startNeedTime;
    LLBC_TimeSpan _stopNeedTime;
};

}

TestCase_App_AppCfgTest::TestCase_App_AppCfgTest()
{}

TestCase_App_AppCfgTest::~TestCase_App_AppCfgTest()
{}

int TestCase_App_AppCfgTest::Run(int argc, char *argv[])
{
    // Define app object.
    TestApp app(LLBC_TimeSpan::FromSeconds(5), LLBC_TimeSpan::FromSeconds(5));

    // Set config path.
    // If not specific config path, application will auto reload config.
    // app.SetConfigPath("./CfgTestApp.cfg");

    // Startup app object.
    std::cout <<"Start app..." <<std::endl;
    if (app.Start("CfgTestApp", argc, argv) != LLBC_OK)
    {
        std::cerr <<"App start failed, err:" <<LLBC_FormatLastError() <<std::endl;
        return LLBC_FAILED;
    }
    std::cout <<"App start finished" <<std::endl;

    if (app.HasConfig())
    {
        std::cout <<"App has config" <<std::endl;
        std::cout <<"- cfg path:" <<app.GetConfigPath() <<std::endl;
        if (app.GetConfigType() == LLBC_ApplicationConfigType::Property)
        {
            LLBC_String propCnt;
            app.GetPropertyConfig().SaveToContent(propCnt);
            std::cout <<"- cfg cnt:" <<propCnt <<std::endl;
        }
        else
        {
            std::cout <<"- cfg cnt:" <<app.GetNonPropertyConfig().ToString() <<std::endl;
        }
    }

    // Do some stuff.
    // ... ...

    // Stop app object(you can skip this step, because app will stop in app destructor).
    app.Stop();

    return LLBC_OK;
}
