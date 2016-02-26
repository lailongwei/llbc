/**
 * @file    Common.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/19
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

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
    tls->coreTls.threadHandle = LLBC_INVALID_HANDLE;

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

    return LLBC_OK;
}

void __LLBC_CommonCleanup()
{
    // Reset entry thread tls info.
#if LLBC_TARGET_PLATFORM_WIN32
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    ::CloseHandle(tls->coreTls.nativeThreadHandle);
#endif // LLBC_TARGET_PLATFORM_WIN32
    __LLBC_ResetLibTls();

    // Destroy errors.
    __LLBC_DestroyErrors();

    // Destroy library TLS.
    __LLBC_DestroyLibTls();
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
