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
 * Pre-declare TLS class.
 */
template <typename ValueType>
class LLBC_Tls;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief Singleton restricts the instantiation of a class to one object.
 */
template <typename T, typename LockType = LLBC_NS LLBC_SpinLock, bool ThreadUnit = false>
class LLBC_Singleton
{
public:
    /**
     * Get the instance of singleton.
     * @return T * - singleton instance.
     */
    static T *Instance();

    /**
     *  Release instance object.
     */
    void Release();
    
    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_Singleton);

private:
    static LockType _lock;

    static T *_instance;
    static LLBC_Tls<T> *_tls;
};

__LLBC_NS_END

#include "llbc/core/singleton/SingletonInl.h"


