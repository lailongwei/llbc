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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

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

    /**
     * Check given thread priority enumeration valid or not.
     * @param[in] threadPriority - the thread priority.
     * @return bool - the valid flag.
     */
    static bool IsValid(int threadPriority);
};

/**
 * Create thread, in WIN32 platform, use ::CreateThread system API to implement,
 * In LINUX platform, use posix-thread API to implement.
 * @param[in] proc          - thread routine.
 * @param[in] arg           - thread routine's argument.
 * @param[out] nativeHandle - native thread handle, in WIN32, type is HANDLE,
 *                            in LINUX platform, type is pthread_t, see
 *                            llbc/common/BasicDataType.h header file.
 * @param[in] priority      - thread priority, see LLBC_ThreadPriority class.
 * @param[in] stackSize     - thread stack size, in bytes.
 * @return int - return 0 if create successed, otherwise return false.
 */
LLBC_EXPORT int LLBC_CreateThread(LLBC_ThreadProc proc,
                                  LLBC_ThreadArg arg,
                                  LLBC_NativeThreadHandle *nativeHandle = nullptr,
                                  int priority = LLBC_ThreadPriority::Normal,
                                  int stackSize = 0);

/**
 * Get current native thread handle.
 * @return LLBC_NativeThreadHandle - native thread handle.
 */
LLBC_EXPORT LLBC_NativeThreadHandle LLBC_GetCurrentThread();

/**
 * Get current native thread Id.
 * @return LLBC_ThreadId - native thread Id.
 */
LLBC_EXPORT LLBC_ThreadId LLBC_GetCurrentThreadId();

/**
 * Get thread priority.
 * @param[in] handle - native thread handle.
 * @return int - the thread priority, if return -1, it means error occurred, call
 *               LLBC_GetLastError()/LLBC_StrError() to get more error describe.
 */
LLBC_EXPORT int LLBC_GetThreadPriority(LLBC_NativeThreadHandle handle);

/**
 * Set thread priority.
 * @param[in] handle   - native thread handle.
 * @param[in] priority - thread priority.
 * @return int - old thread priority, if error occurred, return -1.
 */
LLBC_EXPORT int LLBC_SetThreadPriority(LLBC_NativeThreadHandle handle, int priority);

/**
 * Suspend thread.
 * @param[in] handle - native thread handle.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXPORT int LLBC_SuspendThread(LLBC_NativeThreadHandle handle);

/**
 * Resume thread.
 * @param[in] handle - native thread handle.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXPORT int LLBC_ResumeThread(LLBC_NativeThreadHandle handle);

/**
 * Join thread.
 * @param[in] handle - native thread handle.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXPORT int LLBC_JoinThread(LLBC_NativeThreadHandle handle);

/**
 * Cancel thread.
 * @param[in] handle - native thread handle.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXPORT int LLBC_CancelThread(LLBC_NativeThreadHandle handle);

/**
 * Kill thread.
 * @param[in] handle - native thread handle.
 * @param[in] sig    - signal number.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXPORT int LLBC_KillThread(LLBC_NativeThreadHandle handle, int sig);

/**
 * Sleep function.
 * @param[in] milliSeconds - time-out values.
 */
LLBC_EXPORT void LLBC_Sleep(int milliSeconds);

/**
 * Relax CPU.
 */
void LLBC_CPURelax();

/**
 * Alloc thread local storage handle.
 * @param[out] handle - thread local storage handle.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_TlsAlloc(LLBC_TlsHandle *handle);

/**
 * Free thread local storage handle.
 * @param[in] handle - thread local storage handle.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXPORT int LLBC_TlsFree(LLBC_TlsHandle handle);

/**
 * Get specific local storage handle's value.
 * @param[in] handle - thread local storage handle.
 * @return void * - if return nullptr and LLBC_GetLastError() != 0, 
 *                  it success, otherwise means error occurred.
 */
LLBC_EXPORT void *LLBC_TlsGetValue(LLBC_TlsHandle handle);

/**
 * Set specific local storage handle's value.
 * @param[in] handle - thread local storage handle.
 * @param[in] value  - thread specific value.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_TlsSetValue(LLBC_TlsHandle handle, void *value);

__LLBC_NS_END

#include "llbc/core/os/OS_ThreadInl.h"


