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


#include "core/thread/TestCase_Core_Thread_Lock.h"

static const int __g_threads_num = 5;
static LLBC_SimpleLock __g_outLock;

struct __LLBC_Thread_Test_Arg
{
    LLBC_ILock *lock;
    uint64 val;
};

static void ThreadProc(void *arg)
{
    int threadIndex = *(reinterpret_cast<int *>(arg));
    __LLBC_Thread_Test_Arg *threadArg;
    memcpy(&threadArg, reinterpret_cast<char *>(arg) + sizeof(int), sizeof(__LLBC_Thread_Test_Arg *));

    __g_outLock.Lock();
    std::cout <<"thread [" <<threadIndex <<"]" <<" begin call lock" <<std::endl;
    __g_outLock.Unlock();
    for(int i = 0; i < 500000; ++i)
    {
        threadArg->lock->Lock();
        threadArg->val ++;
        threadArg->lock->Unlock();
    }

    __g_outLock.Lock();
    std::cout <<"thread [" <<threadIndex <<"]" <<" end call lock" <<std::endl;
    __g_outLock.Unlock();

    free(arg);
}

TestCase_Core_Thread_Lock::TestCase_Core_Thread_Lock()
{
}

TestCase_Core_Thread_Lock::~TestCase_Core_Thread_Lock()
{
}

int TestCase_Core_Thread_Lock::Run(int argc, char *argv[])
{
    std::cout <<"core/thread/lock test:" <<std::endl;


    __LLBC_Thread_Test_Arg *threadArg = new __LLBC_Thread_Test_Arg;
    LLBC_NativeThreadHandle handles[__g_threads_num] = {LLBC_INVALID_NATIVE_THREAD_HANDLE};

    // SimpleLock test.
    std::cout <<"Test SimpleLock ..." <<std::endl;
    threadArg->lock = new LLBC_SimpleLock;
    threadArg->val = 0;
    for(int i = 0; i < __g_threads_num; ++i)
    {
        char *buf = reinterpret_cast<char *>(malloc(sizeof(int) + sizeof(__LLBC_Thread_Test_Arg *)));

        *(reinterpret_cast<int *>(buf)) = i;
        memcpy(buf + sizeof(int), &threadArg, sizeof(__LLBC_Thread_Test_Arg *));
        LLBC_CreateThread(&ThreadProc, buf, &handles[i]);
    }

    for(int i = 0; i < __g_threads_num; ++i)
    {
        LLBC_JoinThread(handles[i]);
    }
    delete threadArg->lock;
    std::cout <<"\t OK, value: " <<threadArg->val <<std::endl;

    // RecursiveLock test.
    std::cout <<"Test RecursiveLock ..." <<std::endl;
    threadArg->lock = new LLBC_RecursiveLock;
    threadArg->val = 0;
    for(int i = 0; i < __g_threads_num; ++i)
    {
        char *buf = reinterpret_cast<char *>(malloc(sizeof(int) + sizeof(__LLBC_Thread_Test_Arg *)));

        *(reinterpret_cast<int *>(buf)) = i;
        memcpy(buf + sizeof(int), &threadArg, sizeof(__LLBC_Thread_Test_Arg *));
        LLBC_CreateThread(&ThreadProc, buf, &handles[i]);
    }

    for(int i = 0; i < __g_threads_num; ++i)
    {
        LLBC_JoinThread(handles[i]);
    }
    delete threadArg->lock;
    std::cout <<"\t OK, value: " <<threadArg->val <<std::endl;

    // FastLock test.
    std::cout <<"Test FastLock ..." <<std::endl;
    threadArg->lock = new LLBC_FastLock;
    threadArg->val = 0;
    for(int i = 0; i < __g_threads_num; ++i)
    {
        char *buf = reinterpret_cast<char *>(malloc(sizeof(int) + sizeof(__LLBC_Thread_Test_Arg *)));

        *(reinterpret_cast<int *>(buf)) = i;
        memcpy(buf + sizeof(int), &threadArg, sizeof(__LLBC_Thread_Test_Arg *));
        LLBC_CreateThread(&ThreadProc, buf, &handles[i]);
    }

    for(int i = 0; i < __g_threads_num; ++i)
    {
        LLBC_JoinThread(handles[i]);
    }
    delete threadArg->lock;
    std::cout <<"\t OK, value: " <<threadArg->val <<std::endl;

    // SpinLock test.
    std::cout <<"Test SpinLock ..." <<std::endl;
    threadArg->lock = new LLBC_SpinLock;
    threadArg->val = 0;
    for(int i = 0; i < __g_threads_num; ++i)
    {
        char *buf = reinterpret_cast<char *>(malloc(sizeof(int) + sizeof(__LLBC_Thread_Test_Arg *)));

        *(reinterpret_cast<int *>(buf)) = i;
        memcpy(buf + sizeof(int), &threadArg, sizeof(__LLBC_Thread_Test_Arg *));
        LLBC_CreateThread(&ThreadProc, buf, &handles[i]);
    }

    for(int i = 0; i < __g_threads_num; ++i)
    {
        LLBC_JoinThread(handles[i]);
    }
    delete threadArg->lock;
    std::cout <<"\t OK, value: " <<threadArg->val <<std::endl;

    // DummyLock test.
    std::cout <<"Test DummyLock ..." <<std::endl;
    threadArg->lock = new LLBC_DummyLock;
    threadArg->val = 0;
    for(int i = 0; i < __g_threads_num; ++i)
    {
        char *buf = reinterpret_cast<char *>(malloc(sizeof(int) + sizeof(__LLBC_Thread_Test_Arg *)));

        *(reinterpret_cast<int *>(buf)) = i;
        memcpy(buf + sizeof(int), &threadArg, sizeof(__LLBC_Thread_Test_Arg *));
        LLBC_CreateThread(&ThreadProc, buf, &handles[i]);
    }

    for(int i = 0; i < __g_threads_num; ++i)
    {
        LLBC_JoinThread(handles[i]);
    }
    delete threadArg->lock;
    std::cout <<"\t OK, value: " <<threadArg->val <<std::endl;

    delete threadArg;

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    return 0;
}

