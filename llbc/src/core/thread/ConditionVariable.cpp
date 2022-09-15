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

#include "llbc/core/thread/ILock.h"
#include "llbc/core/thread/ConditionVariable.h"

__LLBC_NS_BEGIN

LLBC_ConditionVariable::LLBC_ConditionVariable()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_cond_init(&m_handle, nullptr);
#else // LLBC_TARGET_PLATFORM_WIN32
    _cond.waitersBlocked = 0;
    _cond.waitersGone = 0;
    _cond.waitersToUnblock = 0;

    _cond.blockQueue = ::CreateSemaphoreW(nullptr, 0, LONG_MAX, nullptr);

    _cond.blockLock = ::CreateSemaphoreW(nullptr, 1, LONG_MAX, nullptr);
    ::InitializeCriticalSection(&_cond.unblockLock);

#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

LLBC_ConditionVariable::~LLBC_ConditionVariable()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_cond_destroy(&m_handle);
#else
    ::CloseHandle(_cond.blockQueue);
    _cond.blockQueue = nullptr;

    ::CloseHandle(_cond.blockLock);
    _cond.blockLock = nullptr;
    ::DeleteCriticalSection(&_cond.unblockLock);
#endif
}

void LLBC_ConditionVariable::Notify()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_cond_signal(&m_handle);
#else
    Notify(false);
#endif
}

void LLBC_ConditionVariable::Broadcast()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_cond_broadcast(&m_handle);
#else
    Notify(true);
#endif
}

void LLBC_ConditionVariable::Wait(LLBC_ILock &lock)
{
    TimedWait(lock, LLBC_INFINITE);
}

int LLBC_ConditionVariable::TimedWait(LLBC_ILock &lock, int milliSeconds)
{
    if (UNLIKELY(milliSeconds < 0 && milliSeconds != static_cast<int>(LLBC_INFINITE)))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_mutex_t *mtx = reinterpret_cast<pthread_mutex_t *>(lock.Handle());
    ASSERT(mtx);

    if (milliSeconds == static_cast<int>(LLBC_INFINITE))
    {
        pthread_cond_wait(&m_handle, mtx);
        return LLBC_OK;
    }

    struct timeval tvStart, tvEnd;
    struct timespec ts;

    gettimeofday(&tvStart, nullptr);
    tvEnd = tvStart;
    tvEnd.tv_sec += milliSeconds / 1000;
    tvEnd.tv_usec += (milliSeconds % 1000) * 1000;
    tvEnd.tv_sec += tvEnd.tv_usec / (1000 * 1000);
    tvEnd.tv_usec = tvEnd.tv_usec % (1000 * 1000);

    TIMEVAL_TO_TIMESPEC(&tvEnd, &ts);

    int errNo = pthread_cond_timedwait(&m_handle, mtx, &ts);
    if (errNo != 0)
    {
        errno = errNo;
        if (errNo == ETIMEDOUT)
        {
            LLBC_SetLastError(LLBC_ERROR_TIMEOUTED);
        }
        else
        {
            LLBC_SetLastError(LLBC_ERROR_CLIB);
        }

        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
    ::WaitForSingleObject(_cond.blockLock, INFINITE);
    ++ _cond.waitersBlocked;
    ::ReleaseSemaphore(_cond.blockLock, 1, nullptr);

 #ifdef _MSC_VER
  #pragma inline_depth(0)
 #endif

    lock.Unlock();
    DWORD waitTimes = (milliSeconds == LLBC_INFINITE) ? (INFINITE) : milliSeconds;
    DWORD waitRet = ::WaitForSingleObject(_cond.blockQueue, waitTimes);
    ASSERT((waitRet == WAIT_OBJECT_0 || waitRet == WAIT_TIMEOUT) && "LLBC_CriticalVariable::TimedWait() error!");

    bool successed = (waitRet == WAIT_OBJECT_0) ? true : false;
    if (!successed)
    {
        LLBC_SetLastError(LLBC_ERROR_TIMEOUTED);
    }

    AfterWait(lock);

 #ifdef _MSC_VER
  #pragma inline_depth()
 #endif

    return successed ? LLBC_OK : LLBC_FAILED;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

#if LLBC_TARGET_PLATFORM_WIN32
void LLBC_ConditionVariable::Notify(bool unblockAll)
{
    int signalsToIssue = 0;

    ::EnterCriticalSection(&_cond.unblockLock);

    if (_cond.waitersToUnblock != 0)
    {
        if (_cond.waitersBlocked == 0)
        {
            ::LeaveCriticalSection(&_cond.unblockLock);
            return;
        }
        
        if (unblockAll)
        {
            _cond.waitersToUnblock += (signalsToIssue = _cond.waitersBlocked);
            _cond.waitersBlocked = 0;
        }
        else
        {
            signalsToIssue = 1;
            ++ _cond.waitersToUnblock;
            -- _cond.waitersBlocked;
        }
    }
    else if (_cond.waitersBlocked > _cond.waitersGone)
    {
        ::WaitForSingleObject(_cond.blockLock, INFINITE);
        if (_cond.waitersGone != 0)
        {
            _cond.waitersBlocked -= _cond.waitersGone;
            _cond.waitersGone = 0;
        }

        if (unblockAll)
        {
            signalsToIssue = _cond.waitersToUnblock = _cond.waitersBlocked;
            _cond.waitersBlocked = 0;
        }
        else
        {
            signalsToIssue = _cond.waitersToUnblock = 1;
            -- _cond.waitersBlocked;
        }
    }
    else
    {
        ::LeaveCriticalSection(&_cond.unblockLock);
        return;
    }

    ::LeaveCriticalSection(&_cond.unblockLock);
    ::ReleaseSemaphore(_cond.blockQueue, signalsToIssue, nullptr);
}

void LLBC_ConditionVariable::AfterWait(LLBC_ILock &lock)
{
    int errNo = LLBC_GetLastError();
    int subErrNo = LLBC_GetSubErrorNo();

    int signalsWasLeft = 0;

    ::EnterCriticalSection(&_cond.unblockLock);

    if ((signalsWasLeft = _cond.waitersToUnblock) != 0)
    {
        -- _cond.waitersToUnblock;
    }
    else if (_cond.waitersBlocked == ++ _cond.waitersGone)
    {
        ::WaitForSingleObject(_cond.blockLock, INFINITE);
        _cond.waitersBlocked -= _cond.waitersGone;
        ::ReleaseSemaphore(_cond.blockLock, 1, nullptr);

        _cond.waitersGone = 0;
    }

    ::LeaveCriticalSection(&_cond.unblockLock);

    if (signalsWasLeft == 1)
    {
        ::ReleaseSemaphore(_cond.blockLock, 1, nullptr);
    }

    lock.Lock();

    LLBC_SetLastError(errNo);
    LLBC_SetSubErrorNo(subErrNo);
}
#endif // LLBC_TARGET_PLATFORM_WIN32

__LLBC_NS_END
