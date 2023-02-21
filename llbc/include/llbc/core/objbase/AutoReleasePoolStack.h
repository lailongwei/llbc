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

/**
 * Previous declare some classes.
 */
__LLBC_NS_BEGIN
class LLBC_Object;
class LLBC_AutoReleasePool;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The auto release pool stack class encapsulation.
 *        Use to manage the auto release pool.
 */
class LLBC_EXPORT LLBC_AutoReleasePoolStack
{
    typedef LLBC_AutoReleasePoolStack _This;

public:
    LLBC_AutoReleasePoolStack();
    ~LLBC_AutoReleasePoolStack();

public:
    /**
     * Add object to pool stack.
     * @param[in] o - the object.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddObject(LLBC_Object *o);

    /**
     * Remove object from pool stack.
     * @param[in] o - the object.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveObject(LLBC_Object *o);

    /**
     * Push release pool.
     * @param[in] pool - the auto release pool.
     * @return int - return 0 if success, otherwise return -1.
     */
    int PushPool(LLBC_AutoReleasePool *pool);

    /**
     * Pop release pool.
     * @param[in] pool - the will pop pool.
     * @return int - return 0 if success, otherwise return -1.
     */
    int PopPool(LLBC_AutoReleasePool *pool);

    /**
     * Get current release pool.
     * @return LLBC_AutoReleasePool * - the current release pool.
     */
    LLBC_AutoReleasePool *GetCurrentReleasePool();

    /**
     * Purge all pools.
     */
    void Purge();

public:
    /**
     * Get current thread release pool stack.
     * @return _This * - the current thread release pool stack.
     */
    static _This *GetCurrentThreadReleasePoolStack();

private:
    LLBC_AutoReleasePool *_head;
};

__LLBC_NS_END
