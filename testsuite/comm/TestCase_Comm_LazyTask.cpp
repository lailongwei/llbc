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


#include "comm/TestCase_Comm_LazyTask.h"

namespace
{
    class LazyClass
    {
    public:
        void BeforeRun(LLBC_Service *svc, const LLBC_Variant &data)
        {
            LLBC_PrintLn("Hello, I'm lazy task, func: %s, data: %s", "BeforeRun()", data.ToString().c_str());
            svc->Post(this, &LazyClass::AfterRun, data);
        }

        void AfterRun(LLBC_Service *svc, const LLBC_Variant &data)
        {
            LLBC_PrintLn("Hello, I'm lazy task, func: %s, data: %s", "AfterRun()", data.ToString().c_str());

            svc->Post(this, &LazyClass::BeforeRun, data);
        }
    };
}

TestCase_Comm_LazyTask::TestCase_Comm_LazyTask()
{
}

TestCase_Comm_LazyTask::~TestCase_Comm_LazyTask()
{
}

int TestCase_Comm_LazyTask::Run(int argc, char *argv[])
{
    LLBC_PrintLn("service/lazy task test:");

    LLBC_Service *svc = LLBC_Service::Create("LazyTaskTest");

    LazyClass *taskObj = new LazyClass;
    LLBC_Variant taskData;
    taskData["IntVal"] = 51215;
    taskData["StrVal"] = "Hello World";
    taskData["DictData"] = LLBC_Variant();
    taskData["DictData"]["EmbeddedVal"] = 3.1415926;
    svc->Post(taskObj, &LazyClass::BeforeRun, taskData);

    svc->Start();

    LLBC_PrintLn("Press any key to exit...");
    getchar();

    delete svc;
    delete taskObj;

    return LLBC_OK;
}
