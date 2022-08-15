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

#include "llbc/core/thread/FastLock.h"

__LLBC_NS_BEGIN

LLBC_FastLock::LLBC_FastLock()
{
#ifdef LLBC_FAST_LOCK_ASM
    _lock = 0;
#else
 #if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_mutex_init(&_handle, nullptr);
 #else
    ::InitializeCriticalSection(&_handle);
    _locked = false;
 #endif
#endif
}

LLBC_FastLock::~LLBC_FastLock()
{
#ifdef LLBC_FAST_LOCK_ASM
#else
 #if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_mutex_destroy(&_handle);
 #else
    ::DeleteCriticalSection(&_handle);
 #endif
#endif
}

void LLBC_FastLock::Lock()
{
#ifdef LLBC_FAST_LOCK_ASM
    bool gotLock = false;
    do
    {
        gotLock = TryLock();
        if (!gotLock)
        {
 #if LLBC_TARGET_PLATFORM_NON_WIN32
            sched_yield();
 #else
            ::Sleep(0);
 #endif
        }
    } while (!gotLock);
#else
 #if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_testcancel();
    pthread_mutex_lock(&_handle);
 #else
    ::EnterCriticalSection(&_handle);
    while (_locked)
    {
        ::Sleep(1000);
    }

    _locked = true;
 #endif
#endif // LLBC_FAST_LOCK_ASM
}

bool LLBC_FastLock::TryLock()
{
#ifdef LLBC_FAST_LOCK_ASM
    int oldLock;
 #if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
    pthread_testcancel();

    asm volatile (
        "movl $1, %%eax\n\t"
        "xchg %%eax, %0\n\t"
        "movl %%eax, %1\n\t"
        : "=m" (_lock), "=m" (oldLock)
        :
        : "%eax", "memory"
        );
 #elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
    int *lockPtr = &_lock;
    __asm {
        mov eax, 1
        mov ecx, lockPtr 
        xchg eax, [ecx]
        mov oldLock, eax
    }
 #endif
    return (oldLock == 0);
#else
 #if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_testcancel();
    return pthread_mutex_trylock(&_handle) == 0;
 #else
    bool ret = ::TryEnterCriticalSection(&_handle) ? true : false;

    if (ret)
    {
        if (_locked)
        {
            ::LeaveCriticalSection(&_handle);
            ret = false;
        }
        else
        {
            _locked = true;
        }
    }

    return ret;
 #endif
#endif // LLBC_FAST_LOCK_ASM
}

void LLBC_FastLock::Unlock()
{
#ifdef LLBC_FAST_LOCK_ASM
 #if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
    asm volatile (
        "movl $0,%%eax\n\t"
        "xchg %%eax,%0\n\t"
        : "=m" (_lock)
        :
        : "%eax", "memory"
        );

    pthread_testcancel();
 #elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
    int *lockPtr = &_lock;
    __asm {
        mov eax, 0
        mov ecx, lockPtr 
        xchg eax, [ecx]
    }
 #endif
#else
 #if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_mutex_unlock(&_handle);
    pthread_testcancel();
 #else
    _locked = false;
    ::LeaveCriticalSection(&_handle);
 #endif
#endif // LLBC_FAST_LOCK_ASM
}

bool LLBC_FastLock::IsDummyLock() const
{
    return false;
}

#if LLBC_TARGET_PLATFORM_NON_WIN32
void *LLBC_FastLock::Handle()
{
    return nullptr;
}
#endif

__LLBC_NS_END
