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

#include "llbc/core/objectpool/PoolObject.h"

__LLBC_NS_BEGIN

/**
 * \brief The pool object reflection support encapsulation.
 */
class LLBC_PoolObjectReflection
{
public:
    /**
     * The pool object reflection detect type definition.
     */
    template <typename ObjectType,
              void (ObjectType::*)(LLBC_IObjectPoolInst &poolInst), // Mark object-pool object support: void MarkPoolObject(LLBC_IObjectPoolInst &poolInst)
              LLBC_IObjectPoolInst *(ObjectType::*)()> // Object-Pool object reflaction: LLBC_IObjectPoolInst *GetPoolInst()
    struct poolobject_reflection_detect_type;

// The pool object reflection detect type definition.
#define __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_REFLECTION_DETECT_TYPE_DEF \
    poolobject_reflection_detect_type<ObjectType,                      \
                                      &ObjectType::MarkPoolObject,     \
                                      &ObjectType::GetPoolInst>        \

public:
    /**
     * Check given object type is supported pool obje ct reflection or not.
     */
    template <typename ObjectType>
    static constexpr bool IsSupportedPoolObjectReflection();

private:
    template <typename ObjectType>
    static constexpr bool IsSupportedPoolObjectReflectionInl(
        __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_REFLECTION_DETECT_TYPE_DEF *);

    template <typename ObjectType>
    static constexpr
    typename std::enable_if<std::is_base_of<LLBC_PoolObject, ObjectType>::value, bool>::type
    IsSupportedPoolObjectReflectionInl(std::nullptr_t);

    template <typename ObjectType>
    static constexpr
    typename std::enable_if<std::is_base_of<LLBC_ReferencablePoolObj, ObjectType>::value, bool>::type
    IsSupportedPoolObjectReflectionInl(std::nullptr_t);

    template <typename ObjectType>
    static constexpr bool IsSupportedPoolObjectReflectionInl(...);

public:
    /**
     * Mark given object is pool object(get from object pool), call by llbc framework.
     */
    template <typename ObjectType>
    static void MarkPoolObject(ObjectType *&obj,
                               LLBC_IObjectPoolInst *poolInst,
                               bool referencableObj);

private:
    template <typename ObjectType>
    static void MarkPoolObjectInl(ObjectType *&obj,
                                  LLBC_IObjectPoolInst *poolInst,
                                  bool referencableObj,
                                  __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_REFLECTION_DETECT_TYPE_DEF *);

    template <typename ObjectType>
    static
    typename std::enable_if<std::is_base_of<LLBC_PoolObject, ObjectType>::value, void>::type
    MarkPoolObjectInl(ObjectType *&obj,
                      LLBC_IObjectPoolInst *poolInst,
                      bool referencableObj,
                      std::nullptr_t);

    template <typename ObjectType>
    static
    typename std::enable_if<std::is_base_of<LLBC_ReferencablePoolObj, ObjectType>::value, void>::type
    MarkPoolObjectInl(ObjectType *&obj,
                      LLBC_IObjectPoolInst *poolInst,
                      bool referencableObj,
                      std::nullptr_t);

    template <typename ObjectType>
    static void MarkPoolObjectInl(ObjectType *&obj,
                                  LLBC_IObjectPoolInst *poolInst,
                                  bool referencableObj,
                                  ...);

public:
    /**
     * Check given object is pool object or not(get from object pool).
     */
    template <typename ObjectType>
    static bool IsPoolObject(ObjectType *&obj);

public:
    /**
     * Get object owned pool instance.
     */
    template <typename ObjectType>
    static LLBC_IObjectPoolInst *GetPoolInst(ObjectType *&obj);

private:
    template <typename ObjectType>
    static LLBC_IObjectPoolInst *GetPoolInstInl(ObjectType *&obj,
                                                __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_REFLECTION_DETECT_TYPE_DEF *);

    template <typename ObjectType>
    static
    typename std::enable_if<std::is_base_of<LLBC_PoolObject, ObjectType>::value, LLBC_IObjectPoolInst *>::type
    GetPoolInstInl(ObjectType *&obj,
                   std::nullptr_t);

    template <typename ObjectType>
    static
    typename std::enable_if<std::is_base_of<LLBC_ReferencablePoolObj, ObjectType>::value, LLBC_IObjectPoolInst *>::type
    GetPoolInstInl(ObjectType *&obj,
                   std::nullptr_t);

    template <typename ObjectType>
    static LLBC_IObjectPoolInst *GetPoolInstInl(ObjectType *&obj,
                                                ...);

public:
    /**
     * Recycle object, if object is pool object, will give back to object pool, otherwise delete it.
     */
    template <typename ObjectType>
    static void Recycle(ObjectType *obj);

    /**
     * Same as Recycle(), the difference is after recycle, will reset the object pointer to nullptr.
     */
    template <typename ObjectType>
    static void RecycleX(ObjectType *&obj);

private:
    template <typename ObjectType>
    static void RecycleInl(ObjectType *&obj,
                           __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_REFLECTION_DETECT_TYPE_DEF *);

    template <typename ObjectType>
    static
    typename std::enable_if<std::is_base_of<LLBC_PoolObject, ObjectType>::value, void>::type
    RecycleInl(ObjectType *&obj,
               std::nullptr_t);

    template <typename ObjectType>
    static
    typename std::enable_if<std::is_base_of<LLBC_ReferencablePoolObj, ObjectType>::value, void>::type
    RecycleInl(ObjectType *&obj,
               std::nullptr_t);

    template <typename ObjectType>
    static void RecycleInl(ObjectType *&obj,
                           ...);
};

__LLBC_NS_END

#include "llbc/core/objectpool/PoolObjectReflectionInl.h"


