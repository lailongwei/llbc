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

#ifndef __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_REFLECTION_H__
#define __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_REFLECTION_H__

#include "llbc/common/Common.h"

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
              bool (ObjectType::*)() const, // Object-Pool object reflaction: bool IsPoolObject() const
              LLBC_IObjectPoolInst *(ObjectType::*)(), // Object-Pool object reflaction: LLBC_IObjectPoolInst *GetPoolInst()
              void (ObjectType::*)()> // Release to object pool support: void GiveBackToPool()
    struct poolobject_reflection_detect_type;

// The pool object reflection detect type definition.
#define __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_REFLECTION_DETECT_TYPE_DEF \
    poolobject_reflection_detect_type<ObjectType,                      \
                                      &ObjectType::MarkPoolObject,     \
                                      &ObjectType::IsPoolObject,       \
                                      &ObjectType::GetPoolInst,        \
                                      &ObjectType::GiveBackToPool> \

#define __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_INHERIT_DETECT_TRUE std::true_type
#define __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_INHERIT_DETECT_FALSE std::false_type

public:
    /**
     * Check given object type is supported pool obje ct reflection or not.
     */
    template <typename ObjectType>
    static bool IsSupportedPoolObjectReflection();

private:
    template <typename ObjectType>
    static bool IsSupportedPoolObjectReflectionInl(__LLBC_CORE_OBJECT_POOL_POOL_OBJECT_REFLECTION_DETECT_TYPE_DEF *);

    template <typename ObjectType>
    static bool IsSupportedPoolObjectReflectionInl(...);

public:
    /**
     * Mark given object is pool object(get from object pool), call by llbc framework.
     */
    template <typename ObjectType>
    static void MarkPoolObject(ObjectType *&obj,
                               LLBC_IObjectPoolInst *poolInst);

private:
    template <typename ObjectType>
    static void MarkPoolObjectInl(ObjectType *&obj,
                                  LLBC_IObjectPoolInst *poolInst,
                                  __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_REFLECTION_DETECT_TYPE_DEF *);

    template <typename ObjectType>
    static void MarkPoolObjectInl(ObjectType *&obj,
                                  LLBC_IObjectPoolInst *poolInst,
                                  ...);

    template <typename ObjectType>
    static void MarkPoolObjectInl(ObjectType *&obj, 
                                  LLBC_IObjectPoolInst *poolInst,
                                  __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_INHERIT_DETECT_TRUE);

    template <typename ObjectType>
    static void MarkPoolObjectInl(ObjectType *&obj, 
                                  LLBC_IObjectPoolInst *poolInst,
                                  __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_INHERIT_DETECT_FALSE);

public:
    /**
     * Check given object is pool object or not(get from object pool).
     */
    template <typename ObjectType>
    static bool IsPoolObject(ObjectType *&obj);

private:
    template <typename ObjectType>
    static bool IsPoolObjectInl(ObjectType *&obj,
                                __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_REFLECTION_DETECT_TYPE_DEF *);

    template <typename ObjectType>
    static bool IsPoolObjectInl(ObjectType *&obj,
                                ...);

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
    static LLBC_IObjectPoolInst *GetPoolInstInl(ObjectType *&obj,
                                                ...);

    template <typename ObjectType>
    static LLBC_IObjectPoolInst *GetPoolInstInl(ObjectType *&obj,
                                                __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_INHERIT_DETECT_TRUE);

    template <typename ObjectType>
    static LLBC_IObjectPoolInst *GetPoolInstInl(ObjectType *&obj,
                                                __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_INHERIT_DETECT_FALSE);

public:
    /**
     * Give back object to object pool.
     */
    template <typename ObjectType>
    static void GiveBackToPool(ObjectType *&obj);

private:
    template <typename ObjectType>
    static void GiveBackToPoolInl(ObjectType *&obj,
                                  __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_REFLECTION_DETECT_TYPE_DEF *);

    template <typename ObjectType>
    static void GiveBackToPoolInl(ObjectType *&obj,
                                  ...);

    template <typename ObjectType>
    static void GiveBackToPoolInl(ObjectType *&obj, 
                                  __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_INHERIT_DETECT_TRUE);

    template <typename ObjectType>
    static void GiveBackToPoolInl(ObjectType *&obj, 
                                  __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_INHERIT_DETECT_FALSE);

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
    static void RecycleInl(ObjectType *&obj,
                           ...);

    template <typename ObjectType>
    static void RecycleInl(ObjectType *&obj, 
                           __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_INHERIT_DETECT_TRUE);

    template <typename ObjectType>
    static void RecycleInl(ObjectType *&obj, 
                           __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_INHERIT_DETECT_FALSE);

    template <typename ObjectType>
    static void RecycleXInl(ObjectType *&obj,
                            __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_REFLECTION_DETECT_TYPE_DEF *);

    template <typename ObjectType>
    static void RecycleXInl(ObjectType *&obj,
                            ...);

    template <typename ObjectType>
    static void RecycleXInl(ObjectType *&obj, 
                            __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_INHERIT_DETECT_TRUE);

    template <typename ObjectType>
    static void RecycleXInl(ObjectType *&obj, 
                            __LLBC_CORE_OBJECT_POOL_POOL_OBJECT_INHERIT_DETECT_FALSE);
};

__LLBC_NS_END

#include "llbc/core/objectpool/PoolObjectReflectionImpl.h"

#endif // !__LLBC_CORE_OBJECT_POOL_POOL_OBJECT_REFLECTION_H__
