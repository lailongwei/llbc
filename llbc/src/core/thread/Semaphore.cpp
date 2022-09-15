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


#include "llbc/common/Export.h"

#if LLBC_TARGET_PLATFORM_IPHONE || LLBC_TARGET_PLATFORM_MAC
 #include "llbc/core/os/OS_Time.h"
 #include "llbc/core/os/OS_Thread.h"
 #include "llbc/core/helper/GUIDHelper.h"
#endif // iPhone or Mac platform

#include "llbc/core/thread/Semaphore.h"

__LLBC_NS_BEGIN

LLBC_Semaphore::LLBC_Semaphore(int initVal)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
 #if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
    sem_init(&_sem, 0, initVal);
 #else
    LLBC_GUID guid = LLBC_GUIDHelper::Gen();
    LLBC_String str = LLBC_GUIDHelper::Format(guid);
    str = str.substr(0, 20);

    _sem = sem_open(str.c_str(), O_CREAT | O_EXCL, 0644, 0);
 #endif
#else
    _sem = ::CreateSemaphore(nullptr,
                             initVal,
                             LONG_MAX,
                             nullptr);
#endif
}

LLBC_Semaphore::~LLBC_Semaphore()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
 #if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
    sem_destroy(&_sem);
 #else
    sem_close(_sem);
 #endif
#else
    ::CloseHandle(_sem);
    _sem = INVALID_HANDLE_VALUE;
#endif
}

void LLBC_Semaphore::Wait()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_testcancel();
 #if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
    while(sem_wait(&_sem) != 0 && errno ==  EINTR);
 #else
    while(sem_wait(_sem) != 0 && errno ==  EINTR);
 #endif
#else
    ::WaitForSingleObject(_sem, INFINITE);
#endif
}

bool LLBC_Semaphore::TryWait()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_testcancel();

    int waitRet = 0;
 #if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
    while((waitRet = sem_trywait(&_sem)) != 0 && errno == EINTR);
 #else
    while((waitRet = sem_trywait(_sem)) != 0 && errno == EINTR);
 #endif
    return waitRet == 0;
#else
    if(::WaitForSingleObject(_sem, 0) == WAIT_OBJECT_0)
    {
        return true;
    }

    return false;
#endif
}

bool LLBC_Semaphore::TimedWait(int milliSeconds)
{
#if LLBC_TARGET_PLATFORM_LINUX || LLB_TARGET_PLATFORM_ANDROID
    struct timeval tvStart, tvEnd;
    struct timespec ts;

    gettimeofday(&tvStart, nullptr);
    tvEnd = tvStart;
    tvEnd.tv_sec += milliSeconds / 1000;
    tvEnd.tv_usec += (milliSeconds % 1000) * 1000;
    tvEnd.tv_sec += tvEnd.tv_usec / (1000 * 1000);
    tvEnd.tv_usec = tvEnd.tv_usec % (1000 * 1000);

    TIMEVAL_TO_TIMESPEC(&tvEnd, &ts);
    int waitRet = 0;
    while((waitRet = sem_timedwait(&_sem, &ts)) != 0 && errno == EINTR);
    if(waitRet == 0)
    {
        return true;
    }

    LLBC_SetLastError(LLBC_ERROR_CLIB);
    return false;
#elif LLBC_TARGET_PLATFORM_IPHONE || LLBC_TARGET_PLATFORM_MAC
    if(milliSeconds <= 0)
    {
        return TryWait();
    }

    sint64 expireTime = LLBC_GetMilliSeconds() + milliSeconds;
    do
    {
        if(TryWait())
        {
            return true;
        }

        LLBC_Sleep(10);

        if(TryWait())
        {
            return true;
        }
    } while(LLBC_GetMilliSeconds() < expireTime);

    LLBC_SetLastError(LLBC_ERROR_TIMEOUTED);
    return false;
#else
    DWORD waitRet = 0;
    if((waitRet = ::WaitForSingleObject(_sem, milliSeconds)) == WAIT_OBJECT_0)
    {
        return true;
    }
    else if(waitRet == WAIT_TIMEOUT)
    {
        LLBC_SetLastError(LLBC_ERROR_TIMEOUTED);
        return false;
    }

    LLBC_SetLastError(LLBC_ERROR_OSAPI);
    return false;
#endif
}

void LLBC_Semaphore::Post(int count)
{
    if(count <= 0)
    {
        count = 1;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    for(int i = 0; i < count; ++i)
    {
 #if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
        sem_post(&_sem);
 #else
        sem_post(_sem);
 #endif
    }

    pthread_testcancel();
#else
    ::ReleaseSemaphore(_sem, count, nullptr);
#endif
}

__LLBC_NS_END
