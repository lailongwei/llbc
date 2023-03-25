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
    // Initialize Variant number to number string repr fast access table.
    LLBC_Variant::InitNumber2StrFastAccessTable();

    // Create main bundle.
    if(LLBC_Bundle::CreateMainBundle() != LLBC_OK)
        return LLBC_FAILED;

    // Set timezone.
    LLBC_TZSet();
    // initialize performance frequency.
    LLBC_CPUTime::InitFrequency();
    // Set entry thread timer scheduler.
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    tls->coreTls.timerScheduler = new LLBC_TimerScheduler;

    // Set random seed.
    LLBC_SeedRand(static_cast<int>(time(nullptr)));

    // Initialize network library.
    if (tls->coreTls.needInitWinSock)
        LLBC_StartupNetLibrary();

    // Create entry thread auto-release pool stack.
    tls->objbaseTls.poolStack = new LLBC_AutoReleasePoolStack;
    new LLBC_NS LLBC_AutoReleasePool;

    // Set entry thread object pool.
    if (LLBC_ThreadObjectPoolMgr::CreateEntryThreadObjectPools() != LLBC_OK)
        return LLBC_FAILED;

    // Supported object-pool reflection types assert.
    ASSERT(LLBC_PoolObjectReflection::IsSupportedPoolObjectReflection<LLBC_LogData>());
    ASSERT(LLBC_PoolObjectReflection::IsSupportedPoolObjectReflection<LLBC_MessageBlock>());
    // Add all framework internal implemented object pool factories.
    LLBC_IObjectPool::RegisterPoolInstFactory(new LLBC_MessageBlockObjectPoolInstFactory());

    return LLBC_OK;
}

void __LLBC_CoreCleanup()
{
    // Finalize logger manager.
    LLBC_LoggerMgrSingleton->Finalize();

    // Purge auto-release pool stack.
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    if (tls->objbaseTls.poolStack)
        reinterpret_cast<LLBC_AutoReleasePoolStack *>(tls->objbaseTls.poolStack)->Purge();

    // Destroy entry thread object pool.
    (void)LLBC_ThreadObjectPoolMgr::DestroyEntryThreadObjectPools();
    // Destroy all object pool instance factories.
    LLBC_IObjectPool::DestroyAllPoolInstFactories();

    // Destroy entry thread auto-release pool stack.
    if (tls->objbaseTls.poolStack)
    {
        delete reinterpret_cast<LLBC_AutoReleasePoolStack *>(tls->objbaseTls.poolStack);
        tls->objbaseTls.poolStack = nullptr;
    }

    // Cleanup network library.
    if (tls->coreTls.needInitWinSock)
        LLBC_CleanupNetLibrary();

    // Destroy entry thread timer scheduler.
    if (tls->coreTls.timerScheduler)
    {
        delete reinterpret_cast<LLBC_TimerScheduler *>(tls->coreTls.timerScheduler);
        tls->coreTls.timerScheduler = nullptr;
    }

    // Destroy main bundle.
    LLBC_Bundle::DestroyMainBundle();

    // Destroy Variant number to number string repr fast access table.
    LLBC_Variant::DestroyNumber2StrFastAccessTable();

    // Cleanup Symbol(if enabled).
    #if LLBC_CFG_OS_IMPL_SYMBOL
    (void)LLBC_CleanupSymbol();
    #endif // LLBC_CFG_OS_IMPL_SYMBOL
}

__LLBC_NS_END
