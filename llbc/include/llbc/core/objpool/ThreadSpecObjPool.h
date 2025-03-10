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

#include "llbc/core/objpool/ObjPool.h"

__LLBC_NS_BEGIN

class LLBC_EXPORT LLBC_ThreadSpecObjPool
{
public:
    /**
     * Initialize thread-spec object pool.
     * @return int - return 0 if success, otherwise return -1.
     */
    LLBC_WARN_UNUSED_RESULT static int Initialize();

    /**
     * Finalize thread-spec object pool.
     */
    static void Finalize();

public:
    /**
     * Get current thread thread-safe object pool.
     * @return LLBC_ObjPool * - the current thread thread-safe object pool.
     */
    LLBC_WARN_UNUSED_RESULT static LLBC_ObjPool *GetSafeObjPool()
    {
        return reinterpret_cast<LLBC_ObjPool *>(__LLBC_GetLibTls()->coreTls.safeObjPool);
    }

    /**
     * Get current thread thread-unsafe object pool.
     * @return LLBC_ObjPool * - the current thread thread-unsafe object pool.
     */
    LLBC_WARN_UNUSED_RESULT static LLBC_ObjPool *GetUnsafeObjPool()
    {
        return reinterpret_cast<LLBC_ObjPool *>(__LLBC_GetLibTls()->coreTls.unsafeObjPool);
    }

    /**
     * Acquire object in thread-safe object pool.
     * @tparam Obj - object type.
     * @return Obj * - the object.
     */
    template <typename Obj>
    LLBC_WARN_UNUSED_RESULT static Obj *SafeAcquire() { return GetSafeObjPool()->Acquire<Obj>(); }

    /**
     * Acquire object in thread-unsafe object pool.
     * @tparam Obj - object type.
     * @return Obj * - the object.
     */
    template <typename Obj>
    LLBC_WARN_UNUSED_RESULT static Obj *UnsafeAcquire() { return GetUnsafeObjPool()->Acquire<Obj>(); }

    /**
     * Release object to thread-unsafe object pool.
     * @tparam Obj - object type.
     * @param[in] obj - the object.
     */
    template <typename Obj>
    static void SafeRelease(Obj *obj) { GetSafeObjPool()->Release(obj); }

    /**
     * Release object to thread-unsafe object pool.
     * @tparam Obj - object type.
     * @param[in] obj - the object.
     */
    template <typename Obj>
    static void UnsafeRelease(Obj *obj) { GetUnsafeObjPool()->Release(obj); }

    /**
     * Acquire guarded object in thread-safe object pool.
     * @return LLBC_GuardedPoolObj<T> - the guarded object.
     */
    template<typename T>
    LLBC_WARN_UNUSED_RESULT static LLBC_GuardedPoolObj<T> GuardedSafeAcquire() { return GetSafeObjPool()->AcquireGuarded<T>(); }

    /**
     * Acquire guarded object in thread-unsafe object pool.
     * @return LLBC_GuardedPoolObj<T> - the guarded object.
     */
    template<typename T>
    LLBC_WARN_UNUSED_RESULT static LLBC_GuardedPoolObj<T> GuardedUnsafeAcquire() { return GetUnsafeObjPool()->AcquireGuarded<T>(); }
};
__LLBC_NS_END
