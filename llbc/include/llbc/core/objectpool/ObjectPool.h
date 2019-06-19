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

#ifndef __LLBC_CORE_OBJECT_POOL_OBJECT_POOL_H__
#define __LLBC_CORE_OBJECT_POOL_OBJECT_POOL_H__

#include "llbc/common/Common.h"

#include "llbc/core/thread/DummyLock.h"

#include "llbc/core/objectpool/ObjectGuard.h"
#include "llbc/core/objectpool/IObjectPool.h"
#include "llbc/core/objectpool/ObjectPoolInst.h"

__LLBC_NS_BEGIN

// Pre-declare some classes.
template <typename ObjectType>
class LLBC_ObjectGuard;

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
    * Get pool instance.
    * @return LLBC_ObjectPoolInst<ObjectType, PoolInstLockType> * - the object instance pointer, never null.
    */
    template <typename ObjectType>
    LLBC_ObjectPoolInst<ObjectType, PoolInstLockType> *GetPoolInst();

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

public:
    /**
     * Release object.
     * @param[in] objectType - the object type string representation.
     * @param[in] obj        - the object pointer.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Release(const char *objectType, void *obj);

private:
    PoolLockType _lock;
    std::map<const char *, LLBC_IObjectPoolInst *> _poolDict;
};

/**
 * Export some usually use classes.
 */
//template class LLBC_EXPORT LLBC_ObjectPool<LLBC_DummyLock, LLBC_DummyLock>;

__LLBC_NS_END

#include "llbc/core/objectpool/ObjectPoolImpl.h"

__LLBC_NS_BEGIN

/**
 * Export & typedef some usually used thread pool classes.
 */
typedef LLBC_ObjectPool<LLBC_DummyLock, LLBC_DummyLock> LLBC_ThreadObjectPool;

__LLBC_NS_END

#endif // !__LLBC_CORE_OBJECT_POOL_OBJECT_POOL_H__
