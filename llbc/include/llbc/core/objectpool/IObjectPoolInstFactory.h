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
class LLBC_IObjectPool;
class LLBC_IObjectPoolInst;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The object pool instance factory, use for dynamic create pool instance.
 */
class LLBC_EXPORT LLBC_IObjectPoolInstFactory
{
public:
    virtual ~LLBC_IObjectPoolInstFactory() {  }

public:
    /**
     * Get object pool instance name(object type name).
     * @return const char * - the object pool instance name.
     */
    virtual const char *GetName() const = 0;

    /**
     * Create object pool instance.
     * @param[in] pool - the object pool.
     * @param[in] lock - the object pool instance lock.
     * @return LLBC_IObjectPoolInst * - the object pool instance.
     */
    virtual LLBC_IObjectPoolInst *Create(LLBC_IObjectPool *pool, LLBC_ILock *lock) = 0;
};

__LLBC_NS_END
