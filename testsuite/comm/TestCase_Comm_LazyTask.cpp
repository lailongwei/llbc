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
        void BeforeRun(LLBC_IService *svc, const LLBC_Variant *data)
        {
            LLBC_PrintLine("Hello, I'm lazy task, func: %s, data: %s", "BeforeRun()", data ? data->ToString().c_str() : "Null");

            LLBC_Variant *afterRunData = new LLBC_Variant(data ? *data : LLBC_Variant());
            svc->Post(this, &LazyClass::AfterRun, afterRunData);
        }

        void AfterRun(LLBC_IService *svc, const LLBC_Variant *data)
        {
            LLBC_PrintLine("Hello, I'm lazy task, func: %s, data: %s", "AfterRun()", data ? data->ToString().c_str() : "Null");

            LLBC_Variant *beforeRunTask = new LLBC_Variant(data ? *data : LLBC_Variant());
            svc->Post(this, &LazyClass::BeforeRun, beforeRunTask);
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
    LLBC_PrintLine("service/lazy task test:");

    LLBC_IService *svc = LLBC_IService::Create(LLBC_IService::Normal, "LazyTaskTest");

    LazyClass *taskObj = LLBC_New(LazyClass);
    LLBC_Variant *taskData = new LLBC_Variant();
    (*taskData)["IntVal"] = 51215;
    (*taskData)["StrVal"] = "Hello World";
    (*taskData)["DictData"] = LLBC_Variant();
    (*taskData)["DictData"]["EmbeddedVal"] = 3.1415926;
    svc->Post(taskObj, &LazyClass::BeforeRun, taskData);

    svc->Start();

    LLBC_PrintLine("Press any key to exit...");
    getchar();

    LLBC_Delete(svc);
    LLBC_Delete(taskObj);

    return LLBC_OK;
}
