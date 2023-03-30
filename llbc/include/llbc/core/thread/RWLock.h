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

#include "llbc/core/thread/SimpleLock.h"
#include "llbc/core/thread/ConditionVariable.h"

__LLBC_NS_BEGIN

/**
 * RWLock class encapsulation.
 */
class LLBC_EXPORT LLBC_RWLock
{
    /**
     * RWLock holder structure.
     */
    struct Holder
    {
        LLBC_SimpleLock lock;
        LLBC_ConditionVariable readCond;
        LLBC_ConditionVariable writeCond;
        int vaild;
        int r_active;
        int w_active;
        int r_wait;
        int w_wait;
    };
public:
    LLBC_RWLock();
    virtual ~LLBC_RWLock();

public:
    /**
     * Acquire read lock.
     */
    void ReadLock();

    /**
     * Try acquire read lock.
     */
    bool ReadTryLock();

    /**
     * Release read lock.
     */
    void ReadUnlock();

    /**
     * Acquire write lock.
     */
    void WriteLock();

    /**
     * Try acquire write lock.
     */
    bool WriteTryLock();

    /**
     * Release write lock.
     */
    void WriteUnlock();

private:
    Holder _holder;
};

__LLBC_NS_END
