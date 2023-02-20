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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_ReferencablePoolObj;

class LLBC_IObjectPool;

template <typename PoolLockType, typename PoolInstLockType>
class LLBC_ObjectPool;

template <typename ObjectType>
class LLBC_ObjectPoolInst;

class LLBC_ObjectPoolInstStat;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
* \brief The object pool instance interface encapsulation.
*/
class LLBC_EXPORT LLBC_IObjectPoolInst
{
public:
    explicit LLBC_IObjectPoolInst(LLBC_IObjectPool *objPool);
    virtual ~LLBC_IObjectPoolInst() {  }

public:
    /**
    * Get object.
    * @return void * - the object pointer.
    */
    virtual void *Get() = 0;
    /**
     * Get referencable object.
     * @return void * - the object pointer.
     */
    virtual void *GetReferencable() = 0;

    /**
    * Release object.
    * @param[in] obj - the object pointer.
    */
    virtual void Release(void *obj) = 0;

public:
    /**
     * Get pool instance name.
     * @return const char * - the pool instance name.
     */
    virtual const char *GetPoolInstName() = 0;

    /**
     * Get object pool interface object(self owner).
     * @return LLBC_IObjectPool * - the object pool interface object.
     */
    LLBC_IObjectPool *GetIObjectPool();

    /**
     * Get object pool.
     * @return LLBC_ObjectPool<PoolLockType, PoolInstLockType> * - the object pool.
     */
    template <typename PoolLockType, typename PoolInstLockType>
    LLBC_ObjectPool<PoolLockType, PoolInstLockType> *GetObjectPool();

    /**
     * Check this object pool instance is thread safety or not.
     * @return bool - return true if is thread safety, otherwise thread unsafety.
     */
    virtual bool IsThreadSafety() const = 0;

public:
    /**
     * Perform object pool statistic.
     * @param[out] stat - the statstic info.
     */
    virtual void Stat(LLBC_ObjectPoolInstStat &stat) const = 0;

protected:
    // Friend class: Referencable pool object.
    // Access methods:
    //    ReleaseReferencable(): use to release referencable object.
    friend class LLBC_ReferencablePoolObj;

    /**
     * Release referencable object.
    * @param[in] obj - the object pointer.
     */
    virtual void ReleaseReferencable(void *obj) = 0;

protected:
    /**
     * Set pool instance to referencable pool object.
     */
    void SetPoolInstToReferencablePoolObj(void *obj);

    /**
     * Clear pool instance from referencable pool object.
     */
    void ClearPoolInstFromReferencablePoolObj(void *obj);

    /**
     * Check object ref&auto-ref count.
     */
    void CheckRefCount(void *obj);

private:
    // Disable assignment.
    LLBC_DISABLE_ASSIGNMENT(LLBC_IObjectPoolInst);

private:
    LLBC_IObjectPool *_objPool;
};

__LLBC_NS_END

#include "llbc/core/objectpool/IObjectPoolInstInl.h"


