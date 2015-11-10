/**
 * @file    TestCase_Core_Thread_Tls.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/10
 * @version 1.0
 *
 * @brief
 */

#include "core/thread/TestCase_Core_Thread_Tls.h"

namespace
{
    static LLBC_Tls<int> __g_tls;
    static const uint32 __g_threadNum = 5;
}

static int ThreadProc(void *arg)
{
    int threadIdx = static_cast<int>(reinterpret_cast<long>(arg));
    LLBC_PrintLine("thread %d startup", threadIdx);

    __g_tls.SetValue(new int);
    (*__g_tls) = threadIdx;
    for(int i = 0; i < 5000000; i ++)
        (*__g_tls) += 1;

    LLBC_PrintLine("thread [%d] tls value: %d", threadIdx, *__g_tls);

    return 0;
}

TestCase_Core_Thread_Tls::TestCase_Core_Thread_Tls()
{
}

TestCase_Core_Thread_Tls::~TestCase_Core_Thread_Tls()
{
}

int TestCase_Core_Thread_Tls::Run(int argc, char *argv[])
{
    LLBC_PrintLine("core/thread/tls test");

    // Create threads.
    LLBC_NativeThreadHandle threads[__g_threadNum] = {LLBC_INVALID_NATIVE_THREAD_HANDLE};
    for(uint32 i = 0; i < __g_threadNum; i ++)
    {
        LLBC_CreateThread(&threads[i], &ThreadProc, reinterpret_cast<void *>(i));
    }

    // Join threads.
    for(uint32 i = 0; i < __g_threadNum; i ++)
    {
        LLBC_JoinThread(threads[i]);
    }

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
