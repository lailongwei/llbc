/**
 * @file    TestCase_Comm_LazyTask.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/03/16
 * @version 1.0
 *
 * @brief
 */

#include "comm/TestCase_Comm_LazyTask.h"

namespace
{
    class LazyClass
    {
    public:
        void BeforeRun(LLBC_IService *svc)
        {
            LLBC_PrintLine("Hello, I'm lazy task, func: %s", "BeforeRun()");
            svc->Post(this, &LazyClass::AfterRun);
        }

        void AfterRun(LLBC_IService *svc)
        {
            LLBC_PrintLine("Hello, I'm lazy task, func: %s", "AfterRun()");
            svc->Post(this, &LazyClass::BeforeRun);
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
    svc->Post(taskObj, &LazyClass::BeforeRun);

    svc->Start();

    LLBC_PrintLine("Press any key to exit...");
    getchar();

    LLBC_Delete(svc);
    LLBC_Delete(taskObj);

    return LLBC_OK;
}
