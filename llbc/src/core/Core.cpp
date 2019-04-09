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
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

int __LLBC_CoreStartup()
{
    // Initialize Symbol(if enabled).
    #if LLBC_CFG_OS_IMPL_SYMBOL
    if (LLBC_InitSymbol() != LLBC_OK)
        return LLBC_FAILED;
    #endif // LLBC_CFG_OS_IMPL_SYMBOL

    // Initialize Variant Type->StrDesc dictionary.
    LLBC_VariantType::InitType2StrDict();

    // Create main bundle.
    if(LLBC_Bundle::CreateMainBundle() != LLBC_OK)
        return LLBC_FAILED;

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
    LLBC_SeedRand(static_cast<int>(::time(NULL)));

    // Initialize network library.
    if (tls->coreTls.needInitWinSock)
        LLBC_StartupNetLibrary();

    return LLBC_OK;
}

void __LLBC_CoreCleanup()
{
    // Destroy entry thread timer scheduler.
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    tls->coreTls.timerScheduler = NULL;
    LLBC_TimerScheduler::DestroyEntryThreadScheduler();

    // Destroy main bundle.
    LLBC_Bundle::DestroyMainBundle();

    // Finalize logger manager.
    LLBC_LoggerManagerSingleton->Finalize();

    // Cleanup network library.
    if (tls->coreTls.needInitWinSock)
        LLBC_CleanupNetLibrary();
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
