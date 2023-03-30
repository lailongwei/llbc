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


#include "comm/TestCase_Comm_CompReflectionMethod.h"

namespace
{
    class ITestComp : public LLBC_Component
    {
    public:
        ITestComp() : LLBC_Component(LLBC_ComponentEvents::AllEvents) {}
        virtual ~ITestComp() = default;
    };

    class TestComp : public ITestComp
    {
    public:
        TestComp()
        {
            _timer = nullptr;
        }

        virtual ~TestComp() = default;

    public:
        void OnPrint()
        {
            LLBC_PrintLn("TestComp");
        }

    public:
        virtual bool OnInit(bool &initFinished)
        {
            AddMethod("Foo", [this](const LLBC_Variant &callArgs, LLBC_Variant &callRet) {
                callRet = Foo(callArgs["x"], callArgs["y"].AsStr());
                return 0;
            });

            AddMethod("Goo", [this](const LLBC_Variant &callArgs, LLBC_Variant &callRet) {
                callRet = Goo(callArgs["x"], callArgs["y"].AsStr());
                return 0;
            });

            return true;
        }

        virtual bool OnStart(bool &startFinished)
        {
            LLBC_PrintLn("Service start");
            _timer = new LLBC_Timer(
                std::bind(&TestComp::OnTimerTimeout, this, std::placeholders::_1),
                std::bind(&TestComp::OnTimerCancel, this, std::placeholders::_1));
            _timer->Schedule(LLBC_TimeSpan::FromSeconds(2), LLBC_TimeSpan::FromSeconds(5));

            return true;
        }

        virtual void OnStop(bool &stopFinished)
        {
            LLBC_PrintLn("Service stop");
            _timer->Cancel();
            LLBC_XDelete(_timer);
        }

    public:
        int Foo(int x, LLBC_CString y)
        {
            return static_cast<int>(x + y.size());
        }

        LLBC_String Goo(int x, LLBC_CString y)
        {
            return LLBC_Num2Str(x) + y.c_str();
        }

    private:
        virtual void OnTimerTimeout(LLBC_Timer *timer)
        {
            LLBC_PrintLn("Call methods:");
            LLBC_PrintLn("- Call Foo(3, \"hello world\") ret:%d", Foo(3, "hello world"));
            LLBC_PrintLn("- Call Goo(6, \"hey judy\") ret:%s", Goo(6, "hey judy").c_str());

            LLBC_PrintLn("Call reflection methods:");
            LLBC_Variant callArgs, callRet;
            callArgs["x"] = 3;
            callArgs["y"] = "hello world";

            int ret = CallMethod("Foo", callArgs, callRet);
            LLBC_PrintLn("- Reflection call Foo(3, \"hello world\") ret:%d, retParams:%s", ret, callRet.ToString().c_str());

            callArgs["x"] = 6;
            callArgs["y"] = "hey judy";
            callRet.Clear();
            ret = CallMethod("Goo", callArgs, callRet);
            LLBC_PrintLn("- Reflection call Call Goo(6, \"hey judy\") ret:%d, retParams:%s", ret, callRet.ToString().c_str());
        }

        virtual void OnTimerCancel(LLBC_Timer *timer)
        {
            LLBC_PrintLn("Time cancelled!");
        }

    private:
        LLBC_Timer *_timer;
    };

    class TestCompFactory : public LLBC_ComponentFactory
    {
    public:
        virtual ITestComp *Create() const
        {
            return new TestComp;
        }
    };
}

int TestCase_Comm_CompReflectionMethod::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Comp reflection method test:");

    auto svc = LLBC_Service::Create("TestSvc");
    svc->AddComponent<TestCompFactory>();

    svc->Start();

    LLBC_Defer(delete svc);
    LLBC_PrintLn("Press any key to continue...");
    getchar();

    return LLBC_OK;
}
