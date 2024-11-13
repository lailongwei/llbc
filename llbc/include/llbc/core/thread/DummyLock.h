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

#include "llbc/core/thread/ILock.h"

__LLBC_NS_BEGIN

/**
 * \brief dummy lock encapsulation.
 */
class LLBC_EXPORT LLBC_DummyLock : public LLBC_ILock
{
public:
    LLBC_DummyLock();
    ~LLBC_DummyLock() override;

public:
    /**
     * Acquire lock.
     */
    void Lock() override;

    /**
     * Try acquire lock.
     */
    bool TryLock() override;

    /**
     * Release lock.
     */
    void Unlock() override;

    /**
     * Dummy lock check.
     * @return bool - return true if is dummy lock, otherwise false.
     */
    bool IsDummyLock() const override;

private:
#if LLBC_TARGET_PLATFORM_NON_WIN32
    friend class LLBC_ConditionVariable;
    void *Handle() override;
#endif

    LLBC_DISABLE_ASSIGNMENT(LLBC_DummyLock);
};

__LLBC_NS_END

#include "llbc/core/thread/DummyLockInl.h"


