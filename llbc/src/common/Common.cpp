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

#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
 #include <sys/syscall.h>
#endif // Linux or Android

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

int __LLBC_CommonStartup()
{
    // Create library TLS.
    __LLBC_CreateLibTls();

    // Init errors.
    __LLBC_InitErrors();

    // Fill entry thread TLS info.
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    tls->coreTls.llbcThread = true;
    tls->coreTls.entryThread = true;
    tls->coreTls.threadHandle = LLBC_CFG_THREAD_ENTRY_THREAD_HANDLE;
    tls->coreTls.threadGroupHandle = LLBC_CFG_THREAD_ENTRY_THREAD_GROUP_HANDLE;

#if LLBC_TARGET_PLATFORM_WIN32
    tls->coreTls.threadId = ::GetCurrentThreadId();
#elif LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
    tls->coreTls.threadId = syscall(SYS_gettid);
#else // LLBC_TARGET_PLATFORM_IPHONE || LLBC_TARGET_PLATFORM_MAC (FreeBSD kernel)
    tls->coreTls.threadId = pthread_mach_thread_np(pthread_self());
#endif

#if LLBC_TARGET_PLATFORM_NON_WIN32
    tls->coreTls.nativeThreadHandle = pthread_self();
#else // LLBC_TARGET_PLATFORM_WIN32
    HANDLE pseudoHandle = ::GetCurrentThread();
    ::DuplicateHandle(GetCurrentProcess(),
                      pseudoHandle,
                      GetCurrentProcess(),
                      &tls->coreTls.nativeThreadHandle,
                      0,
                      FALSE,
                      DUPLICATE_SAME_ACCESS);
#endif // LLBC_TARGET_PLATFRM_NON_WIN32

    // Set endian type constant.
    LLBC_MachineEndian = LLBC_GetMachineEndianType();

    // Set entry-thread lib tls variable.
    __LLBC_EntryThreadLibTls = tls;

    return LLBC_OK;
}

void __LLBC_CommonCleanup()
{
    // Reset holded entry-thread lib tls variable.
    __LLBC_EntryThreadLibTls = nullptr;

    // Reset entry thread tls info.
#if LLBC_TARGET_PLATFORM_WIN32
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    ::CloseHandle(tls->coreTls.nativeThreadHandle);
    tls->coreTls.nativeThreadHandle = LLBC_INVALID_NATIVE_THREAD_HANDLE;
#endif // LLBC_TARGET_PLATFORM_WIN32
    __LLBC_ResetLibTls();

    // Destroy errors.
    __LLBC_DestroyErrors();

    // Destroy library TLS.
    __LLBC_DestroyLibTls();
}

__LLBC_NS_END
