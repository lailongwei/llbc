/**
 * @file    OS_Thread.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief   llbc library OS/Thread dependent APIs encapsulation.
 */
#ifndef __LLBC_CORE_OS_OS_THREAD_H__
#define __LLBC_CORE_OS_OS_THREAD_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Thread flag enumeration.
 */
class LLBC_EXPORT LLBC_ThreadFlag
{
public:
    enum
    {
        Joinable    = 0x00000001,
        Detached    = 0x00000002
    };
};

/**
 * Thread state enumeration.
 */
class LLBC_EXPORT LLBC_ThreadState
{
public:
    enum
    {
        Begin       = 0,

        Running     = 0,
        Suspended   = 1,
        Stoped      = 2,

        End
    };
};

/**
 * Thread priority enumeration.
 */
class LLBC_EXPORT LLBC_ThreadPriority
{
public:
    enum
    {
        Begin       = 0,

        Idle        = 0,
        Lowest      = 1,
        BelowNormal = 2,
        Normal      = 3,
        AboveNormal = 4,
        Highest     = 5,
        Critical    = 6,

        End
    };
};

/**
 * Create thread, in WIN32 platform, use ::CreateThread system API to implement,
 * In LINUX platform, use posix-thread API to implement.
 * @param[out] nativeThreadHandle - native thread handle, in WIN32, type is HANDLE,
 *                                  in LINUX platform, type is pthread_t, see
 *                                  llbc/common/BasicDataType.h header file.
 * @param[in] proc                - thread routine.
 * @param[in] arg                 - thread routine's argument.
 * @param[in] flags               - thread flags, see LLBC_ThreadFlag class.
 * @param[in] priority            - thread priority, see LLBC_ThreadPriority class.
 * @param[in] stackSize           - thread stack size, in bytes.
 * @return int - return 0 if create successed, otherwise return false.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_CreateThread(LLBC_NativeThreadHandle *handle,
                                              LLBC_ThreadProc proc,
                                              LLBC_ThreadArg arg,
                                              int flags = LLBC_ThreadFlag::Joinable,
                                              int priority = LLBC_ThreadPriority::Normal,
                                              int stackSize = LLBC_CFG_THREAD_DFT_STACK_SIZE);

/**
 * Get Current native thread handle.
 * @return LLBC_NativeThreadHandle - native thread handle.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_NativeThreadHandle LLBC_GetCurrentThread();

/**
 * Get thread priority.
 * @param[in] handle - native thread handle.
 * @return int - the thread priority, if return -1, it means error occurred, call
 *               LLBC_GetLastError()/LLBC_StrError() to get more error describe.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_GetThreadPriority(LLBC_NativeThreadHandle handle);

/**
 * Set thread priority.
 * @param[in] handle   - native thread handle.
 * @param[in] priority - thread priority.
 * @return int - old thread priority, if error occurred, return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_SetThreadPriority(LLBC_NativeThreadHandle handle, int priority);

/**
 * Suspend thread.
 * @param[in] handle - native thread handle.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_SuspendThread(LLBC_NativeThreadHandle handle);

/**
 * Resume thread.
 * @param[in] handle - native thread handle.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_ResumeThread(LLBC_NativeThreadHandle handle);

/**
 * Join thread.
 * @param[in] handle - native thread handle.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_JoinThread(LLBC_NativeThreadHandle handle);

/**
 * Cancel thread.
 * @param[in] handle - native thread handle.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_CancelThread(LLBC_NativeThreadHandle handle);

/**
 * Kill thread.
 * @param[in] handle - native thread handle.
 * @param[in] signo  - signal number.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_KillThread(LLBC_NativeThreadHandle handle, int signo);

/**
 * Sleep function.
 * @param[in] milliSeconds - time-out values.
 */
LLBC_EXTERN LLBC_EXPORT void LLBC_Sleep(int milliSeconds);

/**
 * Relax CPU.
 */
LLBC_EXTERN LLBC_EXPORT void LLBC_CPURelax();

/**
 * Alloc thread local storage handle.
 * @param[out] handle - thread local storage handle.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_TlsAlloc(LLBC_TlsHandle *handle);

/**
 * Free thread local storage handle.
 * @param[in] handle - thread local storage handle.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_TlsFree(LLBC_TlsHandle handle);

/**
 * Get specific local storage handle's value.
 * @param[in] handle - thread local storage handle.
 * @return void * - if return NULL and LLBC_GetLastError() != 0, 
 *                  it success, otherwise means error occurred.
 */
LLBC_EXTERN LLBC_EXPORT void *LLBC_TlsGetValue(LLBC_TlsHandle handle);

/**
 * Set specific local storage handle's value.
 * @param[in] handle - thread local storage handle.
 * @param[in] value  - thread specific value.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_TlsSetValue(LLBC_TlsHandle handle, void *value);

__LLBC_NS_END

#endif // __LLBC_CORE_OS_OS_THREAD_H__
