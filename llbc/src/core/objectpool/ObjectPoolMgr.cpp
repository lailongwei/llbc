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

#include "llbc/core/objectpool/ObjectPoolMgr.h"

__LLBC_INTERNAL_NS_BEGIN

static LLBC_NS LLBC_SafetyObjectPool *__g_globalObjectPool = nullptr;
static LLBC_NS LLBC_SafetyObjectPool *__g_entryThreadSafetyObjectPool = nullptr;
static LLBC_NS LLBC_UnsafetyObjectPool *__g_entryThreadUnsafetyObjectPool = nullptr;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

int LLBC_ThreadObjectPoolMgr::CreateEntryThreadObjectPools()
{
    LLBC_INL_NS __g_globalObjectPool = new LLBC_SafetyObjectPool;

    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    if (!tls->coreTls.entryThread)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }
    else if (tls->coreTls.safetyObjectPool ||
             tls->coreTls.unsafetyObjectPool)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    tls->coreTls.safetyObjectPool = 
        LLBC_INL_NS __g_entryThreadSafetyObjectPool = new LLBC_SafetyObjectPool;
    tls->coreTls.unsafetyObjectPool =
        LLBC_INL_NS __g_entryThreadUnsafetyObjectPool = new ::llbc::LLBC_UnsafetyObjectPool;

    return LLBC_OK;
}

int LLBC_ThreadObjectPoolMgr::DestroyEntryThreadObjectPools()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    if (!tls->coreTls.entryThread)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }
    else if (!tls->coreTls.safetyObjectPool ||
             !tls->coreTls.unsafetyObjectPool)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    tls->coreTls.safetyObjectPool = nullptr;
    tls->coreTls.unsafetyObjectPool = nullptr;

    LLBC_XDelete(LLBC_INL_NS __g_entryThreadSafetyObjectPool);
    LLBC_XDelete(LLBC_INL_NS __g_entryThreadUnsafetyObjectPool);

    LLBC_XDelete(LLBC_INL_NS __g_globalObjectPool);

    return LLBC_OK;
}

LLBC_SafetyObjectPool *LLBC_ThreadObjectPoolMgr::GetEntryThreadSafetyObjectPool()
{
    return LLBC_INTERNAL_NS __g_entryThreadSafetyObjectPool;
}

LLBC_UnsafetyObjectPool *LLBC_ThreadObjectPoolMgr::GetEntryThreadUnsafetyObjectPool()
{
    return LLBC_INTERNAL_NS __g_entryThreadUnsafetyObjectPool;
}

LLBC_SafetyObjectPool *LLBC_ThreadObjectPoolMgr::GetCurThreadSafetyObjectPool()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    if (UNLIKELY(tls->coreTls.safetyObjectPool == nullptr))
        tls->coreTls.safetyObjectPool = new LLBC_SafetyObjectPool;

    return reinterpret_cast<LLBC_SafetyObjectPool *>(tls->coreTls.safetyObjectPool);
}

LLBC_UnsafetyObjectPool *LLBC_ThreadObjectPoolMgr::GetCurThreadUnsafetyObjectPool()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    if (UNLIKELY(tls->coreTls.unsafetyObjectPool == nullptr))
        tls->coreTls.unsafetyObjectPool = new LLBC_UnsafetyObjectPool;

    return reinterpret_cast<LLBC_UnsafetyObjectPool *>(tls->coreTls.unsafetyObjectPool);
}

__LLBC_NS_END
