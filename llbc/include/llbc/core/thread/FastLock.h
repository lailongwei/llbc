/**
 * @file	FastLock.h
 * @author	Longwei Lai<lailongwei@126.com>
 * @date	2012/1/13
 * @version	1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_THREAD_FAST_LOCK_H__
#define __LLBC_CORE_THREAD_FAST_LOCK_H__

#include "llbc/common/Common.h"

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

#endif // !__LLBC_CORE_THREAD_FAST_LOCK_H__

