/**
 * @file    OS_Thread.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/01
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Thread.h"

__LLBC_INTERNAL_NS_BEGIN

extern "C" {
struct __ThreadEntryArg
{
    LLBC_NS LLBC_ThreadProc proc;
    void *arg;
};
}

#if LLBC_TARGET_PLATFORM_NON_WIN32
static void *__ThreadEntry(void *arg)
#else
static unsigned int WINAPI __ThreadEntry(void *arg)
#endif
{
    __ThreadEntryArg *entryArg = reinterpret_cast<__ThreadEntryArg *>(arg);
    LLBC_NS LLBC_ThreadProc llbcThreadProc = entryArg->proc;
    void *llbcThreadArg = entryArg->arg;

    delete entryArg;

    int llbcThreadRtn = (*llbcThreadProc)(llbcThreadArg);
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return reinterpret_cast<void *>(llbcThreadRtn);
#else
    return llbcThreadRtn;
#endif
}

__LLBC_INTERNAL_NS_END

__LLBC_INTERNAL_NS_BEGIN

#if LLBC_TARGET_PLATFORM_LINUX
static int __LinuxPriority2LLBCPriority(int priorityMin, int priorityMax, int priority)
{
    int slicePriority = (priorityMax - priorityMin + 1) / LLBC_NS LLBC_ThreadPriority::End;
    if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Lowest)
    {
        return LLBC_NS LLBC_ThreadPriority::Idle;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::BelowNormal)
    {
        return LLBC_NS LLBC_ThreadPriority::Lowest;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Normal)
    {
        return LLBC_NS LLBC_ThreadPriority::BelowNormal;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::AboveNormal)
    {
        return LLBC_NS LLBC_ThreadPriority::Normal;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Highest)
    {
        return LLBC_NS LLBC_ThreadPriority::AboveNormal;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Critical)
    {
        return LLBC_NS LLBC_ThreadPriority::Highest;
    }

    return LLBC_NS LLBC_ThreadPriority::Critical;
}

static int __LLBCPriority2LinuxPriority(int priority, int priorityMin, int priorityMax)
{
    int slicePriority = (priorityMax - priorityMin + 1) / LLBC_NS LLBC_ThreadPriority::End;
    return ((priorityMin + slicePriority * priority) + (slicePriority / 2));
}
#endif // LLBC_TARGET_PLATFORM_LINUX

#if LLBC_TARGET_PLATFORM_WIN32
static int __WinPriority2LLBCPriority(int priority)
{
    switch(priority)
    {
    case THREAD_PRIORITY_IDLE:
        return LLBC_NS LLBC_ThreadPriority::Idle;

    case THREAD_PRIORITY_LOWEST:
        return LLBC_NS LLBC_ThreadPriority::Lowest;

    case THREAD_PRIORITY_BELOW_NORMAL:
        return LLBC_NS LLBC_ThreadPriority::BelowNormal;

    case THREAD_PRIORITY_NORMAL:
        return LLBC_NS LLBC_ThreadPriority::Normal;

    case THREAD_PRIORITY_ABOVE_NORMAL:
        return LLBC_NS LLBC_ThreadPriority::AboveNormal;

    case THREAD_PRIORITY_HIGHEST:
        return LLBC_NS LLBC_ThreadPriority::Highest;

    case THREAD_PRIORITY_TIME_CRITICAL:
        return LLBC_NS LLBC_ThreadPriority::Critical;
    }

    return LLBC_NS LLBC_ThreadPriority::Normal;
}

static int __LLBCPriority2WinPriority(int priority)
{
    switch(priority)
    {
    case LLBC_NS LLBC_ThreadPriority::Idle:
        return THREAD_PRIORITY_IDLE;

    case LLBC_NS LLBC_ThreadPriority::Lowest:
        return THREAD_PRIORITY_LOWEST;

    case LLBC_NS LLBC_ThreadPriority::BelowNormal:
        return THREAD_PRIORITY_BELOW_NORMAL;

    case LLBC_NS LLBC_ThreadPriority::Normal:
        return THREAD_PRIORITY_NORMAL;

    case LLBC_NS LLBC_ThreadPriority::AboveNormal:
        return THREAD_PRIORITY_ABOVE_NORMAL;

    case LLBC_NS LLBC_ThreadPriority::Highest:
        return THREAD_PRIORITY_HIGHEST;

    case LLBC_NS LLBC_ThreadPriority::Critical:
        return THREAD_PRIORITY_TIME_CRITICAL;
    }

    return THREAD_PRIORITY_NORMAL;
}
#endif // LLBC_TARGET_PLATFORM_WIN32

#if LLBC_TARGET_PLATFORM_IPHONE
static int __IphonePriority2LLBCPriority(int priorityMin, int priorityMax, int priority)
{
    int slicePriority = (priorityMax - priorityMin + 1) / LLBC_NS LLBC_ThreadPriority::End;
    if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Lowest)
    {
        return LLBC_NS LLBC_ThreadPriority::Idle;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::BelowNormal)
    {
        return LLBC_NS LLBC_ThreadPriority::Lowest;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Normal)
    {
        return LLBC_NS LLBC_ThreadPriority::BelowNormal;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::AboveNormal)
    {
        return LLBC_NS LLBC_ThreadPriority::Normal;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Highest)
    {
        return LLBC_NS LLBC_ThreadPriority::AboveNormal;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Critical)
    {
        return LLBC_NS LLBC_ThreadPriority::Highest;
    }

    return LLBC_NS LLBC_ThreadPriority::Critical;
}

static int __LLBCPriority2IphonePriority(int priority, int priorityMin, int priorityMax)
{
    int slicePriority = (priorityMax - priorityMin + 1) / LLBC_NS LLBC_ThreadPriority::End;
    return ((priorityMin + slicePriority * priority) + (slicePriority / 2));
}
#endif // LLBC_TARGET_PLATFORM_IPHONE

#if LLBC_TARGET_PLATFORM_MAC
static int __MacPriority2LLBCPriority(int priorityMin, int priorityMax, int priority)
{
    int slicePriority = (priorityMax - priorityMin + 1) / LLBC_NS LLBC_ThreadPriority::End;
    if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Lowest)
    {
        return LLBC_NS LLBC_ThreadPriority::Idle;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::BelowNormal)
    {
        return LLBC_NS LLBC_ThreadPriority::Lowest;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Normal)
    {
        return LLBC_NS LLBC_ThreadPriority::BelowNormal;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::AboveNormal)
    {
        return LLBC_NS LLBC_ThreadPriority::Normal;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Highest)
    {
        return LLBC_NS LLBC_ThreadPriority::AboveNormal;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Critical)
    {
        return LLBC_NS LLBC_ThreadPriority::Highest;
    }

    return LLBC_NS LLBC_ThreadPriority::Critical;
}

static int __LLBCPriority2MacPriority(int priority, int priorityMin, int priorityMax)
{
    int slicePriority = (priorityMax - priorityMin + 1) / LLBC_NS LLBC_ThreadPriority::End;
    return ((priorityMin + slicePriority * priority) + (slicePriority / 2));
}
#endif // LLBC_TARGET_PLATFORM_MAC

#if LLBC_TARGET_PLATFORM_ANDROID
static int __AndroidPriority2LLBCPriority(int priorityMin, int priorityMax, int priority)
{
    int slicePriority = (priorityMax - priorityMin + 1) / LLBC_NS LLBC_ThreadPriority::End;
    if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Lowest)
    {
        return LLBC_NS LLBC_ThreadPriority::Idle;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::BelowNormal)
    {
        return LLBC_NS LLBC_ThreadPriority::Lowest;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Normal)
    {
        return LLBC_NS LLBC_ThreadPriority::BelowNormal;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::AboveNormal)
    {
        return LLBC_NS LLBC_ThreadPriority::Normal;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Highest)
    {
        return LLBC_NS LLBC_ThreadPriority::AboveNormal;
    }
    else if (priority < priorityMin + slicePriority * LLBC_NS LLBC_ThreadPriority::Critical)
    {
        return LLBC_NS LLBC_ThreadPriority::Highest;
    }

    return LLBC_NS LLBC_ThreadPriority::Critical;
}

static int __LLBCPriority2AndroidPriority(int priority, int priorityMin, int priorityMax)
{
    int slicePriority = (priorityMax - priorityMin + 1) / LLBC_NS LLBC_ThreadPriority::End;
    return ((priorityMin + slicePriority * priority) + (slicePriority / 2));
}
#endif // LLBC_TARGET_PLATFORM_ANDROID

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

int LLBC_CreateThread(LLBC_NativeThreadHandle *handle,
                      LLBC_ThreadProc proc,
                      LLBC_ThreadArg arg,
                      int flags,
                      int priority,
                      int stackSize)
{
    if (!handle || !proc)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if ((flags & LLBC_ThreadFlag::Joinable) != LLBC_ThreadFlag::Joinable &&
        (flags & LLBC_ThreadFlag::Detached) != LLBC_ThreadFlag::Detached)
    {
        flags |= LLBC_ThreadFlag::Joinable;
    }
    if (priority >= LLBC_ThreadPriority::End)
    {
        priority = LLBC_ThreadPriority::Normal;
    }
    stackSize = MAX(stackSize, LLBC_CFG_THREAD_MINIMUM_STACK_SIZE);

    LLBC_INTERNAL_NS __ThreadEntryArg *threadArg = new LLBC_INTERNAL_NS __ThreadEntryArg;
    threadArg->proc = proc;
    threadArg->arg = arg;

#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // Set thread flag.
    pthread_attr_setdetachstate(&attr, (flags & LLBC_ThreadFlag::Detached) ? 
        PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE);

    // Set stack size.
    pthread_attr_setstacksize(&attr, stackSize);

    int ret = 0;
    if ((ret = pthread_create(handle,
                               &attr,
                               &LLBC_INTERNAL_NS __ThreadEntry,
                               threadArg)) != 0)
    {
        delete threadArg;

        errno = ret;
        pthread_attr_destroy(&attr);
        return LLBC_FAILED;
    }

    pthread_attr_destroy(&attr);
#else
    *handle = (HANDLE)_beginthreadex(NULL,
                                     stackSize, 
                                     &LLBC_INTERNAL_NS __ThreadEntry,
                                     threadArg,
                                     0,
                                     NULL);
    if (!*handle)
    {
        delete threadArg;

        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }
#endif

    if (LLBC_SetThreadPriority(*handle, priority) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    if ((flags | LLBC_ThreadFlag::Detached) == LLBC_ThreadFlag::Detached)
    {
#if LLBC_TARGET_PLATFORM_WIN32
        ::CloseHandle(*handle);
#endif

        *handle = LLBC_INVALID_NATIVE_THREAD_HANDLE;
    }

    return LLBC_OK;
}

LLBC_NativeThreadHandle LLBC_GetCurrentThread()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return pthread_self();
#else
    return ::GetCurrentThread();
#endif
}

int LLBC_GetThreadPriority(LLBC_NativeThreadHandle handle)
{
    if (handle == LLBC_INVALID_NATIVE_THREAD_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    int status = 0;
    int policy = 0;
    int priorityMin = 0;
    int priorityMax = 0;
    struct sched_param schedParam;
    if ((status = pthread_getschedparam(handle, &policy, &schedParam)) != 0)
    {
        errno = status;
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    priorityMin = sched_get_priority_min(policy);
    priorityMax = sched_get_priority_max(policy);
 #if LLBC_TARGET_PLATFORM_LINUX
    return LLBC_INTERNAL_NS __LinuxPriority2LLBCPriority(priorityMin, priorityMax, schedParam.sched_priority);
 #elif LLBC_TARGET_PLATFORM_IPHONE
    return LLBC_INTERNAL_NS __IphonePriority2LLBCPriority(priorityMin, priorityMax, schedParam.sched_priority);
 #elif LLBC_TARGET_PLATFORM_MAC
    return LLBC_INTERNAL_NS __MacPriority2LLBCPriority(priorityMin, priorityMax, schedParam.sched_priority);
 #elif LLBC_TARGET_PLATFORM_ANDROID
    return LLBC_INTERNAL_NS __AndroidPriority2LLBCPriority(priorityMin, priorityMax, schedParam.sched_priority);
 #endif
#else
    int nativeRtn = ::GetThreadPriority(handle);
    if (nativeRtn == THREAD_PRIORITY_ERROR_RETURN)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_INTERNAL_NS __WinPriority2LLBCPriority(nativeRtn);
#endif
}

int LLBC_SetThreadPriority(LLBC_NativeThreadHandle handle, int priority)
{
    if (handle == LLBC_INVALID_NATIVE_THREAD_HANDLE || 
        (priority < LLBC_ThreadPriority::Begin || priority >= LLBC_ThreadPriority::End))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    int status = 0;
    int policy = 0;
    int priorityMin = 0;
    int priorityMax = 0;
    struct sched_param schedParam;
    if ((status = pthread_getschedparam(handle, &policy, &schedParam)) != 0)
    {
        errno = status;
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    priorityMin = sched_get_priority_min(policy);
    priorityMax = sched_get_priority_max(policy);

 #if LLBC_TARGET_PLATFORM_LINUX
    schedParam.sched_priority = 
        LLBC_INTERNAL_NS __LLBCPriority2LinuxPriority(priority, priorityMin, priorityMax);
 #elif LLBC_TARGET_PLATFORM_IPHONE
    schedParam.sched_priority = 
        LLBC_INTERNAL_NS __LLBCPriority2IphonePriority(priority, priorityMin, priorityMax);
 #elif LLBC_TARGET_PLATFORM_MAC
    schedParam.sched_priority = 
        LLBC_INTERNAL_NS __LLBCPriority2MacPriority(priority, priorityMin, priorityMax);
 #elif LLBC_TARGET_PLATFORM_ANDROID
    schedParam.sched_priority = 
        LLBC_INTERNAL_NS __LLBCPriority2AndroidPriority(priority, priorityMin, priorityMax);
 #endif

    if ((status = pthread_setschedparam(handle, policy, &schedParam)) != 0)
    {
        errno = status;
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else
    BOOL ret = ::SetThreadPriority(handle, LLBC_INTERNAL_NS __LLBCPriority2WinPriority(priority));
    if (ret == FALSE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif
}

int LLBC_SuspendThread(LLBC_NativeThreadHandle handle)
{
    if (handle == LLBC_INVALID_NATIVE_THREAD_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#else
    if (::SuspendThread(handle) == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif
}

int LLBC_ResumeThread(LLBC_NativeThreadHandle handle)
{
    if (handle == LLBC_INVALID_NATIVE_THREAD_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#else
    if (::ResumeThread(handle) == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif
}

int LLBC_JoinThread(LLBC_NativeThreadHandle handle)
{
    if (handle == LLBC_INVALID_NATIVE_THREAD_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    int status = pthread_join(handle, NULL);
    if (status != 0)
    {
        errno = status;
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else
    if (::WaitForSingleObject(handle, INFINITE) != WAIT_OBJECT_0)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif
}

int LLBC_CancelThread(LLBC_NativeThreadHandle handle)
{
    if (handle == LLBC_INVALID_NATIVE_THREAD_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    int status = 0;
    if ((status = pthread_cancel(handle)) != 0)
    {
        errno = status;
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else
    if (::TerminateThread(handle, 0) == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif
}

int LLBC_KillThread(LLBC_NativeThreadHandle handle, int signo)
{
    if (handle == LLBC_INVALID_NATIVE_THREAD_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    int status = 0;
    if ((status = pthread_kill(handle, signo)) != 0)
    {
        errno = status;
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }
    
    return LLBC_OK;
#else
    if (raise(signo) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif
}

void LLBC_Sleep(int milliSeconds)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (milliSeconds <= 0)
    {
        sched_yield();
    }
    else
    {
        struct timespec ts;
        ts.tv_sec = milliSeconds / 1000;
        ts.tv_nsec = (milliSeconds % 1000) * 1000 * 1000;
        nanosleep(&ts, NULL);
    }
#else
    ::Sleep(milliSeconds);
#endif
}

void LLBC_CPURelax()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
 #if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID || LLBC_TARGET_PLATFORM_MAC
    asm volatile ("rep;nop" : : : "memory");
 #else
    asm volatile ("nop");
 #endif
#else // WIN32 platform
 #ifdef _M_X64
    _mm_pause();
 #else // not define _M_X64
    __asm {
        rep nop
    }
 #endif // defined _M_X64
#endif // Non-WIN32 platform
}

int LLBC_TlsAlloc(LLBC_TlsHandle *handle)
{
    if (!handle)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    int status = pthread_key_create(handle, NULL);
    if (status != 0)
    {
        errno = status;
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }
    
    return LLBC_OK;
#else
    if ((*handle = ::TlsAlloc()) == TLS_OUT_OF_INDEXES)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif
}

int LLBC_TlsFree(LLBC_TlsHandle handle)
{
    if (handle == LLBC_INVALID_TLS_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    int status = pthread_key_delete(handle);
    if (status != 0)
    {
        errno = status;
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else
    if (::TlsFree(handle) == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif
}

void *LLBC_TlsGetValue(LLBC_TlsHandle handle)
{
    if (handle == LLBC_INVALID_TLS_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return NULL;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return pthread_getspecific(handle);
#else
    void *value = ::TlsGetValue(handle);
    if (::GetLastError() != NO_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return value;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return value;
#endif
}

int LLBC_TlsSetValue(LLBC_TlsHandle handle, void *value)
{
    if (handle == LLBC_INVALID_TLS_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32 
    int status = pthread_setspecific(handle, value);
    if (status != 0)
    {
        errno = status;
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else
    if (::TlsSetValue(handle, value) == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
