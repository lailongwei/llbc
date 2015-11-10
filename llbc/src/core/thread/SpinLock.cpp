/**
 * @file    SpinLock.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/thread/SpinLock.h"

__LLBC_NS_BEGIN

LLBC_SpinLock::LLBC_SpinLock()
{
#if LLBC_TARGET_PLATFORM_LINUX
    pthread_spin_init(&_handle, 0);
#elif LLBC_TARGET_PLATFORM_WIN32
    ::InitializeCriticalSectionAndSpinCount(&_handle, 8000);
#elif LLBC_TARGET_PLATFORM_MAC
    pthread_mutex_init(&_handle, NULL);
#elif LLBC_TARGET_PLATFORM_IPHONE
    pthread_mutex_init(&_handle, NULL);
#elif LLBC_TARGET_PLATFORM_ANDROID
    pthread_spin_init(&_handle, NULL);
#endif
}

LLBC_SpinLock::~LLBC_SpinLock()
{
#if LLBC_TARGET_PLATFORM_LINUX
    pthread_spin_destroy(&_handle);
#elif LLBC_TARGET_PLATFORM_WIN32
    ::DeleteCriticalSection(&_handle);
#elif LLBC_TARGET_PLATFORM_MAC
    pthread_mutex_destroy(&_handle);
#elif LLBC_TARGET_PLATFORM_IPHONE
    pthread_mutex_destroy(&_handle);
#elif LLBC_TARGET_PLATFORM_ANDROID
    pthread_spin_destroy(&_handle);
#endif
}

void LLBC_SpinLock::Lock()
{
#if LLBC_TARGET_PLATFORM_LINUX
    pthread_spin_lock(&_handle);
#elif LLBC_TARGET_PLATFORM_WIN32
    ::EnterCriticalSection(&_handle);
#elif LLBC_TARGET_PLATFORM_IPHONE
    pthread_mutex_lock(&_handle);
#elif LLBC_TARGET_PLATFORM_MAC
    pthread_mutex_lock(&_handle);
#elif LLBC_TARGET_PLATFORM_ANDROID
    pthread_spin_lock(&_handle);
#endif
}

bool LLBC_SpinLock::TryLock()
{
#if LLBC_TARGET_PLATFORM_LINUX
    return pthread_spin_trylock(&_handle) == 0;
#elif LLBC_TARGET_PLATFORM_WIN32
    return ::TryEnterCriticalSection(&_handle) != 0;
#elif LLBC_TARGET_PLATFORM_MAC
    return pthread_mutex_trylock(&_handle) == 0 ? true : false;
#elif LLBC_TARGET_PLATFORM_IPHONE
    return pthread_mutex_trylock(&_handle) == 0 ? true : false;
#elif LLBC_TARGET_PLATFORM_ANDROID
    return pthread_spin_trylock(&_handle) == 0;
#endif
}

void LLBC_SpinLock::Unlock()
{
#if LLBC_TARGET_PLATFORM_LINUX
    pthread_spin_unlock(&_handle);
#elif LLBC_TARGET_PLATFORM_WIN32
    ::LeaveCriticalSection(&_handle);
#elif LLBC_TARGET_PLATFORM_IPHONE
    pthread_mutex_unlock(&_handle);
#elif LLBC_TARGET_PLATFORM_MAC
    pthread_mutex_unlock(&_handle);
#elif LLBC_TARGET_PLATFORM_ANDROID
    pthread_spin_unlock(&_handle);
#endif
}

#if LLBC_TARGET_PLATFORM_NON_WIN32
void *LLBC_SpinLock::Handle()
{
    return NULL;
}
#endif

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
