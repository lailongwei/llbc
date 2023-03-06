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

#include "llbc/core/objectpool/ReferencablePoolObj.h"

__LLBC_NS_BEGIN

LLBC_FORCE_INLINE LLBC_IObjectPoolInst::LLBC_IObjectPoolInst(LLBC_IObjectPool *objPool)
: _objPool(objPool)
{
}

LLBC_FORCE_INLINE LLBC_IObjectPool *LLBC_IObjectPoolInst::GetIObjectPool()
{
    return _objPool;
}

template <typename PoolLockType, typename PoolInstLockType>
LLBC_FORCE_INLINE LLBC_ObjectPool<PoolLockType, PoolInstLockType> * LLBC_IObjectPoolInst::GetObjectPool()
{
    return static_cast<LLBC_ObjectPool<PoolLockType, PoolInstLockType> *>(_objPool);
}

LLBC_FORCE_INLINE void LLBC_IObjectPoolInst::SetPoolInstToReferencablePoolObj(void *obj)
{
    reinterpret_cast<LLBC_ReferencablePoolObj *>(obj)->_poolInst = this;
}

LLBC_FORCE_INLINE void LLBC_IObjectPoolInst::ClearPoolInstFromReferencablePoolObj(void *obj)
{
    reinterpret_cast<LLBC_ReferencablePoolObj *>(obj)->_poolInst = nullptr;
}

LLBC_FORCE_INLINE void LLBC_IObjectPoolInst::CheckRefCount(void *obj)
{
    LLBC_ReferencablePoolObj *refObj = reinterpret_cast<LLBC_ReferencablePoolObj *>(obj);
    ASSERT(refObj->GetRefCount() == 1 && refObj->GetAutoRefCount() == 0 &&
           "Referencable pool object reference count must be 1 and auto-reference count must be 0!");
}

__LLBC_NS_END
