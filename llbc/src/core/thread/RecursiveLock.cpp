/**
 * @file    RecursiveLock.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/thread/RecursiveLock.h"

__LLBC_NS_BEGIN

LLBC_RecursiveLock::LLBC_RecursiveLock()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&_handle, &attr);

    pthread_mutexattr_destroy(&attr);
#else
    ::InitializeCriticalSection(&_handle);
#endif
}

LLBC_RecursiveLock::~LLBC_RecursiveLock()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_mutex_destroy(&_handle);
#else
    ::DeleteCriticalSection(&_handle);
#endif
}

void LLBC_RecursiveLock::Lock()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_testcancel();
    pthread_mutex_lock(&_handle);
#else
    ::EnterCriticalSection(&_handle);
#endif
}

bool LLBC_RecursiveLock::TryLock()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_testcancel();
    return pthread_mutex_trylock(&_handle) == 0;
#else
    return ::TryEnterCriticalSection(&_handle) != 0;
#endif
}

void LLBC_RecursiveLock::Unlock()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_mutex_unlock(&_handle);
    pthread_testcancel();
#else
    ::LeaveCriticalSection(&_handle);
#endif
}

#if LLBC_TARGET_PLATFORM_NON_WIN32
void *LLBC_RecursiveLock::Handle()
{
    return &_handle;
}
#endif

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
