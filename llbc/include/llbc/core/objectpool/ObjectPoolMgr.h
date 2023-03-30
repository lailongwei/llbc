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

#include "llbc/core/objectpool/ObjectPool.h"

__LLBC_NS_BEGIN

/**
 * The global object pool define.
 */
LLBC_EXPORT LLBC_SafetyObjectPool &__LLBC_GlobalObjectPool();
#define LLBC_GlobalObjectPool (LLBC_NS __LLBC_GlobalObjectPool())

/**
 * \brief The thread object pool manager class encapsulation.
 */
class LLBC_EXPORT LLBC_ThreadObjectPoolMgr
{
public:
    /**
     * Create the entry thread(Call LLBC_Startup()/LLBC_Cleanup() methods' thread) object-pools.
     * Call by LLBC framework, do not try to call it.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int CreateEntryThreadObjectPools();

    /**
     * Destroy entry thread(Call LLBC_Startup()/LLBC_Cleanup() methods' thread) object-pools.
     * Call by LLBC framework, do not try to call it.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int DestroyEntryThreadObjectPools();

public:
    /**
     * Get entry thread(Call LLBC_Startup()/LLBC_Cleanup() methods' thread) thread safety object-pool.
     * @return LLBC_SafetyObjectPool * - safety object-pool pointer.
     * Note:
     *      If you use entry thread to get object-pool object, you must call this method to fetch
     *      the entry thread object-pool.
     */
    static LLBC_SafetyObjectPool *GetEntryThreadSafetyObjectPool();

    /**
     * Get entry thread(Call LLBC_Startup()/LLBC_Cleanup() methods' thread) thread unsafety object-pool.
     * @return LLBC_UnsafetyObjectPool * - unsafety object-pool pointer.
     * Note:
     *      If you use entry thread to get object-pool object, you must call this method to fetch
     *      the entry thread object-pool.
     */
    static LLBC_UnsafetyObjectPool *GetEntryThreadUnsafetyObjectPool();

public:
    /**
     * Get current thread safety object pool, thread must be LLBC style thread.
     * @return LLBC_SafetyObjectPool * - safety object-pool pointer.
     */
    static LLBC_SafetyObjectPool *GetCurThreadSafetyObjectPool();

    /**
     * Get current thread unsafety object pool, thread must be LLBC style thread.
     * @return LLBC_UnsafetyObjectPool * - unsafety object-pool pointer.
     */
    static LLBC_UnsafetyObjectPool *GetCurThreadUnsafetyObjectPool();
};

/**
 * Object pool converience method, get object from safety object-pool.
 * @return OjectType * - the object pointer.
 */
template <typename ObjectType>
ObjectType *LLBC_GetObjectFromSafetyPool();

/**
 * Object pool converience method, release object to safety object-pool.
 * @param[in] obj - the will release object pointer.
 */
template <typename ObjectType>
void LLBC_ReleaseObjectToSafetyPool(ObjectType *obj);

/**
 * Object pool converience method, get object from unsafety object-pool.
 * @return ObjectType * - the object pointer.
 */
template <typename ObjectType>
ObjectType *LLBC_GetObjectFromUnsafetyPool();

/**
 * Object pool converience method, release object to unsafety object-pool.
 * @param[in] obj - the will release object pointer.
 */
template <typename ObjectType>
void LLBC_ReleaseObjectToUnsafetyPool(ObjectType *obj);

/**
 * Object pool converience method, get guarded object from safety object-pool.
 * @return LLBC_ObjectGuard<ObjectType> - the guarded object.
 */
template <typename ObjectType>
LLBC_ObjectGuard<ObjectType> LLBC_GetGuardedObjectFromSafetyPool();

/**
 * Object pool converience method, get guarded object from unsafety object-pool.
 * @return LLBC_ObjectGuard<ObjectType> - the guarded object.
 */
template <typename ObjectType>
LLBC_ObjectGuard<ObjectType> LLBC_GetGuardedObjectFromUnsafetyPool();

/**
 * Object pool converience method, get referencable object from object pool(unsafety object-pool).
 * @param[in] autoRelease - the auto-release flag, default is false.
 * @return ReferencableObjectType * - the referencable object pointer.
 */
template <typename ReferencableObjectType>
ReferencableObjectType *LLBC_GetReferencableObjectFromPool(bool autoRelease = false);

__LLBC_NS_END

#include "llbc/core/objectpool/ObjectPoolMgrInl.h"


