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

#include "llbc/core/thread/SpinLock.h"
#include "llbc/core/thread/DummyLock.h"

#include "llbc/core/objectpool/IObjectPool.h"
#include "llbc/core/objectpool/ObjectPoolInst.h"
#include "llbc/core/objectpool/ObjectPoolOrderedDeleteNode.h"

__LLBC_NS_BEGIN

// Pre-declare some classes.
template <typename ObjectType>
class LLBC_ObjectGuard;
class LLBC_ObjectPoolOrderedDeleteNode;

/**
 * \brief The object pool class encapsulation.
 */
template <typename PoolLockType = LLBC_DummyLock, typename PoolInstLockType = PoolLockType>
class LLBC_ObjectPool : public LLBC_IObjectPool
{
public:
    LLBC_ObjectPool();
    virtual ~LLBC_ObjectPool();

public:
    /**
     * Get object.
     * @return ObjectType * - the object pointer, never null.
     */
    template <typename ObjectType>
    ObjectType *Get();

    /**
     * Get referencable object.
     * @return ObjectType * - the referencable object pointer, never null.
     */
    template <typename ObjectType>
    ObjectType *GetReferencable();

    /**
     * Get guarded object.
     * @return LLBC_ObjectGuard<Object> - the guarded object.
     */
    template <typename ObjectType>
    LLBC_ObjectGuard<ObjectType> GetGuarded();

    /**
     * Release object.
     * @param[in] obj - the object pointer.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename ObjectType>
    int Release(ObjectType *obj);

    /**
     * Release object.
     * @param[in] objectType - the object type string representation.
     * @param[in] obj        - the object pointer.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Release(const char *objectType, void *obj);

public:
    /**
    * Get pool instance.
    * @return LLBC_ObjectPoolInst<ObjectType, PoolInstLockType> * - the object instance pointer, never null.
    */
    template <typename ObjectType>
    LLBC_ObjectPoolInst<ObjectType> *GetPoolInst();

    /**
     * Get object pool instance interface object.
     * Note: If this object type pool instance not create before, this method will return nullptr.
     * @param[in] objectType - the object type.
     * @return LLBC_IObjectPoolInst * - the object pool instance interface object.
     */
    virtual LLBC_IObjectPoolInst *GetIPoolInst(const char *objectType);

public:
    /**
     * Acquire ordered delete pool instance.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename FrontObjectType, typename BackObjectType>
    int AcquireOrderedDeletePoolInst();

    /**
     * Acquire ordered delete pool instance.
     * @param[in] frontObjectTypeName - the front object type name.
     * @param[in] backObjectTypeName  - the back object type name.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int AcquireOrderedDeletePoolInst(const char *frontObjectTypeName, const char *backObjectTypeName);

public:
    /**
     * Perform object pool statistic.
     * @param[out] stat - the statstic info.
     */
    virtual void Stat(LLBC_ObjectPoolStat &stat) const;

private:
    /**
     * Try create object pool instance from factory.
     * @param[in] objectType - the object type.
     * @return LLBC_IObjectPoolInst * - the new object pool instance, if not found factory to create, return nullptr.
     */
    LLBC_IObjectPoolInst *TryCreatePoolInstFromFactory(const char *objectType);

    /**
     * Delete acquire ordered delete pool instance.
     * @param[in] node - the ordered delete node.
     */
    void DeleteAcquireOrderedDeletePoolInst(LLBC_ObjectPoolOrderedDeleteNode *node);

    /**
    * internal Get pool instance.
    * @return LLBC_ObjectPoolInst<ObjectType, PoolInstLockType> * - the object instance pointer, never null.
    */
    template <typename ObjectType>
    LLBC_ObjectPoolInst<ObjectType> *GetPoolInstInl();

private:
    /**
     * Statistic top N pool instance statistic infos.
     */
    void StatTopNPoolInstStats(LLBC_ObjectPoolStat &stat,
                               std::vector<const LLBC_ObjectPoolInstStat *> &instStats) const;

private:
    // Disable assignment.
    LLBC_DISABLE_ASSIGNMENT(LLBC_ObjectPool);

private:
    typedef std::map<LLBC_CString, LLBC_IObjectPoolInst *> _PoolInsts;

    PoolLockType _lock;
    _PoolInsts _poolInsts;
     LLBC_ObjectPoolOrderedDeleteNodes *_orderedDeleteNodes;
     LLBC_ObjectPoolOrderedDeleteNodes *_topOrderedDeleteNodes;

     // specialization pool inst of common types(std::string, LLBC_String, std::vector<int>, ...).
     LLBC_ObjectPoolInst<std::string> *_stdStringPoolInst;
     LLBC_ObjectPoolInst<LLBC_String> *_llbcStringPoolInst;
     LLBC_ObjectPoolInst<std::vector<llbc::sint32>> *_vectorInt32PoolInst;
     LLBC_ObjectPoolInst<std::vector<llbc::uint32>> *_vectorUint32PoolInst;
     LLBC_ObjectPoolInst<std::vector<llbc::sint64>> *_vectorInt64PoolInst;
     LLBC_ObjectPoolInst<std::vector<llbc::uint64>> *_vectorUint64PoolInst;
};

/**
 * Export some object pool classes.
 */
template class LLBC_EXPORT LLBC_ObjectPool<LLBC_SpinLock, LLBC_SpinLock>;
template class LLBC_EXPORT LLBC_ObjectPool<LLBC_SpinLock, LLBC_DummyLock>;
template class LLBC_EXPORT LLBC_ObjectPool<LLBC_DummyLock, LLBC_SpinLock>;
template class LLBC_EXPORT LLBC_ObjectPool<LLBC_DummyLock, LLBC_DummyLock>;

__LLBC_NS_END

#include "llbc/core/objectpool/ObjectPoolInl.h"

__LLBC_NS_BEGIN

/**
 * Typedef safety/unsafety object pool.
 */
typedef LLBC_ObjectPool<LLBC_SpinLock, LLBC_SpinLock> LLBC_SafetyObjectPool;
typedef LLBC_ObjectPool<LLBC_DummyLock, LLBC_DummyLock> LLBC_UnsafetyObjectPool;

__LLBC_NS_END
