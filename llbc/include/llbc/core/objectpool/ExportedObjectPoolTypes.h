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

#ifndef __LLBC_CORE_OBJECT_POOL_EXPORT_OBJECT_POOL_TYPES_H__
#define __LLBC_CORE_OBJECT_POOL_EXPORT_OBJECT_POOL_TYPES_H__

#include "llbc/common/Common.h"
#include "llbc/core/thread/SpinLock.h"
#include "llbc/core/objectpool/ObjectPool.h"

__LLBC_NS_BEGIN

/**
 * Export thread-unsafety & thread-safety object pool template class instances.
 */
template class LLBC_EXPORT LLBC_ObjectPool<LLBC_SpinLock, LLBC_SpinLock>;
template class LLBC_EXPORT LLBC_ObjectPool<LLBC_DummyLock, LLBC_DummyLock>;

/**
 * Typedef thread-unsafety & thread-safety object pool template class instances.
 */
typedef LLBC_ObjectPool<LLBC_SpinLock, LLBC_SpinLock> LLBC_SafetyObjectPool;
typedef LLBC_ObjectPool<LLBC_DummyLock, LLBC_DummyLock> LLBC_UnsafetyObjectPool;

__LLBC_NS_END

#endif // !__LLBC_CORE_OBJECT_POOL_EXPORT_OBJECT_POOL_TYPES_H__
