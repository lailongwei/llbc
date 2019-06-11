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

#ifndef __LLBC_CORE_OBJECT_POOL_THREAD_OBJECT_POOL_MANAGER_H__
#define __LLBC_CORE_OBJECT_POOL_THREAD_OBJECT_POOL_MANAGER_H__

#include "llbc/common/Common.h"
#include "llbc/core/objectpool/ObjectPool.h"

__LLBC_NS_BEGIN

/**
 * \brief The thread object pool manager class encapsulation.
 */
class LLBC_EXPORT LLBC_ThreadObjectPoolManager
{
public:
    /**
     * Create the entry thread(Call LLBC_Startup()/LLBC_Cleanup() methods' thread) object-pool.
     * Call by LLBC framework, do not try to call it.
     */
    static void CreateEntryThreadObjectPool();

    /**
     * Destroy entry thread(Call LLBC_Startup()/LLBC_Cleanup() methods' thread) object-pool.
     * Call by LLBC framework, do not try to call it.
     */
    static void DestroyEntryThreadObjectPool();

public:
    /**
     * Get entry thread(Call LLBC_Startup()/LLBC_Cleanup() methods' thread) object-pool.
     * @return LLBC_ThreadObjectPool * - object-pool pointer.
     * Note:
     *      If you use entry thread to get object-pool object, you must call this method to fetch
     *      the entry thread object-pool.
     */
    static LLBC_ThreadObjectPool *GetEntryThreadObjectPool();

    /**
     * Get current thread object pool, thread must be LLBC style thread.
     * @return LLBC_ThreadObjectPool * - object-pool pointer.
     */
    static LLBC_ThreadObjectPool *GetCurrentThreadObjectPool();
};

__LLBC_NS_END

#endif // !__LLBC_CORE_OBJECT_POOL_THREAD_OBJECT_POOL_MANAGER_H__
