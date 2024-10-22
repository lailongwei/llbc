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

#pragma once

#include "llbc/core/thread/ILock.h"

__LLBC_NS_BEGIN

// --- Check if we can support the assembly language level implementation,
// 	   if not will use system API.
#if (defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))) || \
	(defined(_MSC_VER) && (defined(_M_IX86)/* || defined(_M_X64) */))
#define LLBC_FAST_LOCK_ASM
#else
#define LLBC_FAST_LOCK_SYS
#endif

/**
 * \brief LLBC_FastMutex class define, this is a fast mutex implementa, if
 *        your compile is gcc or vs compile, and your machine is iX86
 *        or x64 intel cpu, use asm implementation, otherwise use system API.
 */
class LLBC_EXPORT LLBC_FastLock : public LLBC_ILock
{
public:
    LLBC_FastLock();
    virtual ~LLBC_FastLock();

public:
    /**
     * Acquire lock.
     */
    virtual void Lock();

    /**
     * Try acquire lock.
     */
    virtual bool TryLock();

    /**
     * Release lock.
     */
    virtual void Unlock();

    /**
     * Dummy lock check.
     * @return bool - return true if is dummy lock, otherwise false.
     */
    virtual bool IsDummyLock() const;

#if LLBC_TARGET_PLATFORM_NON_WIN32
private:
    friend class LLBC_ConditionVariable;
    virtual void *Handle();
#endif

    LLBC_DISABLE_ASSIGNMENT(LLBC_FastLock);

private:
#ifdef LLBC_FAST_LOCK_ASM
    int _lock;
#else // !defined(LLBC_FAST_LOCK_ASM)
 #if LLBC_TARGET_PLATFORM_WIN32
    bool _locked;
 #endif // LLBC_TARGET_PLATFORM_WIN32
    LLBC_MutexHandle _handle;
#endif // LLBC_FAST_LOCK_ASM
};

__LLBC_NS_END
