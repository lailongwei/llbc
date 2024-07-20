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

#include "llbc/common/Export.h"

#include "llbc/core/objpool/ThreadSpecObjPool.h"

__LLBC_NS_BEGIN
int LLBC_ThreadSpecObjPool::Initialize()
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (!libTls->coreTls.entryThread)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    if (libTls->coreTls.safeObjPool ||
        libTls->coreTls.unsafeObjPool)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    libTls->coreTls.safeObjPool = new LLBC_ObjPool(true);
    libTls->coreTls.unsafeObjPool = new LLBC_ObjPool(false);

    return LLBC_OK;
}

void LLBC_ThreadSpecObjPool::Finalize()
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (!libTls->coreTls.entryThread)
        return;

    if (libTls->coreTls.safeObjPool)
    {
        delete reinterpret_cast<LLBC_ObjPool *>(libTls->coreTls.safeObjPool);
        libTls->coreTls.safeObjPool = nullptr;
    }

    if (libTls->coreTls.unsafeObjPool)
    {
        delete reinterpret_cast<LLBC_ObjPool *>(libTls->coreTls.unsafeObjPool);
        libTls->coreTls.unsafeObjPool = nullptr;
    }
}
__LLBC_NS_END
