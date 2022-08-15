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

#include "llbc/core/thread/SpinLock.h"

__LLBC_NS_BEGIN

LLBC_SpinLock::LLBC_SpinLock()
{
#if LLBC_TARGET_PLATFORM_LINUX
    pthread_spin_init(&_handle, 0);
#elif LLBC_TARGET_PLATFORM_WIN32
    ::InitializeCriticalSectionAndSpinCount(&_handle, 8000);
#elif LLBC_TARGET_PLATFORM_MAC
    pthread_mutex_init(&_handle, nullptr);
#elif LLBC_TARGET_PLATFORM_IPHONE
    pthread_mutex_init(&_handle, nullptr);
#elif LLBC_TARGET_PLATFORM_ANDROID
    pthread_spin_init(&_handle, nullptr);
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

bool LLBC_SpinLock::IsDummyLock() const
{
    return false;
}

#if LLBC_TARGET_PLATFORM_NON_WIN32
void *LLBC_SpinLock::Handle()
{
    return nullptr;
}
#endif

__LLBC_NS_END
