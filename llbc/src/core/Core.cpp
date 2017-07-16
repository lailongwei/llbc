/**
 * @file    Core.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/19
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

int __LLBC_CoreStartup()
{
    // Create main bundle.
    if(LLBC_Bundle::CreateMainBundle() != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    // Set timezone.
    LLBC_TZSet();

    // WIN32 specific, initialize performance frequency.
#if LLBC_TARGET_PLATFORM_WIN32
    LLBC_CPUTime::InitFrequency();
#endif

    // Set entry thread timer scheduler.
    LLBC_TimerScheduler::CreateEntryThreadScheduler();

    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    tls->coreTls.timerScheduler = LLBC_TimerScheduler::GetEntryThreadScheduler();

    // Set random seed.
    LLBC_Random::Seed(static_cast<uint32>(::time(NULL)));

    // Initialize network library.
    if (tls->coreTls.needInitWinSock)
        LLBC_StartupNetLibrary();

    return LLBC_OK;
}

void __LLBC_CoreCleanup()
{
    // Cleanup network library.
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    if (tls->coreTls.needInitWinSock)
        LLBC_CleanupNetLibrary();

    // Destroy entry thread timer scheduler.
    tls->coreTls.timerScheduler = NULL;

    LLBC_TimerScheduler::DestroyEntryThreadScheduler();

    // Destroy main bundle.
    LLBC_Bundle::DestroyMainBundle();
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
