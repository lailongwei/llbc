/**
 * @file    OS_ThreadImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2018/02/10
 * @version 1.0
 *
 * @brief   llbc library OS/Thread dependent APIs encapsulation.
 */
#ifdef __LLBC_CORE_OS_OS_THREAD_H__

__LLBC_NS_BEGIN

inline void LLBC_CPURelax()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
 #if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID || LLBC_TARGET_PLATFORM_MAC
    asm volatile ("rep;nop" : : : "memory");
 #else
    asm volatile ("nop");
 #endif
#else // WIN32 platform
    YieldProcessor();
#endif // Non-WIN32 platform
}

__LLBC_NS_END

#endif // __LLBC_CORE_OS_OS_THREAD_H__
