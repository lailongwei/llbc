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

__LLBC_NS_BEGIN

template <typename ObjectType>
ObjectType *LLBC_GetObjectFromSafetyPool()
{
    return LLBC_ThreadObjectPoolMgr::GetCurThreadSafetyObjectPool()->Get<ObjectType>();
}

template <typename ObjectType>
void LLBC_ReleaseObjectToSafetyPool(ObjectType *obj)
{
    LLBC_ThreadObjectPoolMgr::GetCurThreadSafetyObjectPool()->Release(obj);
}

template <typename ObjectType>
ObjectType *LLBC_GetObjectFromUnsafetyPool()
{
    return LLBC_ThreadObjectPoolMgr::GetCurThreadUnsafetyObjectPool()->Get<ObjectType>();
}

template <typename ObjectType>
void LLBC_ReleaseObjectToUnsafetyPool(ObjectType *obj)
{
    LLBC_ThreadObjectPoolMgr::GetCurThreadUnsafetyObjectPool()->Release(obj);
}

template <typename ObjectType>
LLBC_ObjectGuard<ObjectType> LLBC_GetGuardedObjectFromSafetyPool()
{
    LLBC_ObjectPoolInst<ObjectType> *poolInst =
        LLBC_ThreadObjectPoolMgr::GetCurThreadSafetyObjectPool()->GetPoolInst<ObjectType>();
    return LLBC_ObjectGuard<ObjectType>(poolInst->GetObject(), poolInst);
}

template <typename ObjectType>
LLBC_ObjectGuard<ObjectType> LLBC_GetGuardedObjectFromUnsafetyPool()
{
    LLBC_ObjectPoolInst<ObjectType> *poolInst =
        LLBC_ThreadObjectPoolMgr::GetCurThreadUnsafetyObjectPool()->GetPoolInst<ObjectType>();
    return LLBC_ObjectGuard<ObjectType>(poolInst->GetObject(), poolInst);
}

template <typename ReferencableObjectType>
ReferencableObjectType *LLBC_GetReferencableObjectFromPool(bool autoRelease)
{
    auto objPool = LLBC_ThreadObjectPoolMgr::GetCurThreadUnsafetyObjectPool();
    ReferencableObjectType *refObj = objPool->GetReferencable<ReferencableObjectType>();
    if (autoRelease)
        refObj->AutoRelease();

    return refObj;
}

__LLBC_NS_END
