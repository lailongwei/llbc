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

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_ObjectPoolStat;
class LLBC_IObjectPoolInst;
class LLBC_IObjectPoolInstFactory;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The object pool interface encapsulation.
 */
class LLBC_EXPORT LLBC_IObjectPool
{
public:
    LLBC_IObjectPool() {  }
    virtual ~LLBC_IObjectPool() {  }

public:
    /**
     * Release object.
     * @param[in] objectType - the object type string representation.
     * @param[in] obj        - the object pointer.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Release(const char *objectType, void *obj) = 0;

    /**
     * Get object pool instance interface object.
     * Note: If this object type pool instance not create before, this method will return nullptr.
     * @param[in] objectType - the object type.
     * @return LLBC_IObjectPoolInst * - the object pool instance interface object.
     */
    virtual LLBC_IObjectPoolInst *GetIPoolInst(const char *objectType) = 0;

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
    virtual int AcquireOrderedDeletePoolInst(const char *frontObjectTypeName, const char *backObjectTypeName) = 0;

public:
    /**
     * Register object pool instance factory.
     * @param[in] instFactory - the pool instance factory.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int RegisterPoolInstFactory(LLBC_IObjectPoolInstFactory *instFactory);

    /**
     * Destroy all object pool instance factories(call by framework when framework destroy).
     */
    static void DestroyAllPoolInstFactories();

public:
    /**
     * Perform object pool statistic.
     * @param[out] stat - the statstic info.
     */
    virtual void Stat(LLBC_ObjectPoolStat &stat) const = 0;

private:
    // Disable assignment.
    LLBC_DISABLE_ASSIGNMENT(LLBC_IObjectPool);

protected:
    static LLBC_SpinLock _poolInstFactoryLock;
    static std::map<LLBC_CString, LLBC_IObjectPoolInstFactory *> _poolInstFactories;
};

__LLBC_NS_END

#include "llbc/core/objectpool/IObjectPoolInl.h"


