/**
 * @file    SimpleLock.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/thread/SimpleLock.h"

__LLBC_NS_BEGIN

LLBC_SimpleLock::LLBC_SimpleLock()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_mutex_init(&_handle, NULL);
#else
    _locked = false;
    ::InitializeCriticalSection(&_handle);
#endif
}

LLBC_SimpleLock::~LLBC_SimpleLock()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_mutex_destroy(&_handle);
#else
    ::DeleteCriticalSection(&_handle);
#endif
}

void LLBC_SimpleLock::Lock()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_testcancel();
    pthread_mutex_lock(&_handle);
#else
    ::EnterCriticalSection(&_handle);

    //! Simulate deadlock.
    while (_locked)
    {
        ::Sleep(1000);
    }

    _locked = true;
#endif
}

bool LLBC_SimpleLock::TryLock()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_testcancel();
    return pthread_mutex_trylock(&_handle) == 0 ? true : false;
#else
    bool ret = ::TryEnterCriticalSection(&_handle) ? true : false;
    if (ret)
    {
        if (_locked)
        {
            ::LeaveCriticalSection(&_handle);
            return false;
        }

        _locked = true;
    }

    return ret;
#endif
}

void LLBC_SimpleLock::Unlock()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_mutex_unlock(&_handle);
    pthread_testcancel();
#else
    _locked = false;
    ::LeaveCriticalSection(&_handle);
#endif
}

#if LLBC_TARGET_PLATFORM_NON_WIN32
void *LLBC_SimpleLock::Handle()
{
    return &_handle;
}
#endif

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
