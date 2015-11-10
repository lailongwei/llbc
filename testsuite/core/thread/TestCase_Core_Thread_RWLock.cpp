/**
 * @file    TestCase_Core_Thread_RWLock.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/05
 * @version 1.0
 *
 * @brief
 */

#include "core/thread/TestCase_Core_Thread_RWLock.h"

static const int __g_readerCount = 5;
static const int __g_updateTimes = 50000;
static LLBC_SimpleLock __g_outLock;

static struct
{
    LLBC_RWLock lock;
    int value;
} __g_rwValue;

static int Reader_ThreadProc(void *arg)
{
    int threadIndex = static_cast<int>(reinterpret_cast<long>(arg));
    __g_outLock.Lock();
    std::cout <<"I'm reader: " <<threadIndex <<std::endl;
    __g_outLock.Unlock();

    for(int i = 1; ; i ++)
    {
        __g_rwValue.lock.ReadLock();
        if(i % 50000 == 0)
        {
            i = 0;
            __g_outLock.Lock();
            std::cout <<"reader [" <<threadIndex <<"] read data: " <<__g_rwValue.value <<std::endl;
            __g_outLock.Unlock();
        }

        __g_rwValue.lock.ReadUnlock();

        LLBC_Sleep(0);
    }

    return 0;
}

static int Writer_ThreadProc(void *arg)
{
    __g_outLock.Lock();
    std::cout <<"I'm writer" <<std::endl;
    __g_outLock.Unlock();

    for(int i = 0; i < __g_updateTimes; i ++)
    {
        __g_rwValue.lock.WriteLock();
        __g_rwValue.value += 1;
        __g_rwValue.lock.WriteUnlock();
        LLBC_Sleep(0);
    }

    __g_outLock.Lock();
    std::cout <<"Writer thread update value completed, final value: " <<__g_rwValue.value <<std::endl;
    __g_outLock.Unlock();

    return 0;
}

TestCase_Core_Thread_RWLock::TestCase_Core_Thread_RWLock()
{
}

TestCase_Core_Thread_RWLock::~TestCase_Core_Thread_RWLock()
{
}

int TestCase_Core_Thread_RWLock::Run(int argc, char *argv[])
{
    std::cout <<"core/thread/rwlock test: " <<std::endl;

    // Create readers.
    LLBC_NativeThreadHandle readers[__g_readerCount] = {LLBC_INVALID_NATIVE_THREAD_HANDLE};
    for(int i = 0; i < __g_readerCount; i ++)
    {
        LLBC_CreateThread(&readers[i], &Reader_ThreadProc, reinterpret_cast<void *>(i));
    }

    // Create writer.
    LLBC_NativeThreadHandle writer = LLBC_INVALID_NATIVE_THREAD_HANDLE;
    LLBC_CreateThread(&writer, &Writer_ThreadProc, NULL);

    // Join writer.
    LLBC_JoinThread(writer);

    // Cancel readers and join it.
    for(int i = 0; i < __g_readerCount; i ++)
    {
        LLBC_CancelThread(readers[i]);
        LLBC_JoinThread(readers[i]);
    }

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    return 0;
}
