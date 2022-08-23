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

bool LLBC_RecursiveLock::IsDummyLock() const
{
    return false;
}

#if LLBC_TARGET_PLATFORM_NON_WIN32
void *LLBC_RecursiveLock::Handle()
{
    return &_handle;
}
#endif

__LLBC_NS_END
