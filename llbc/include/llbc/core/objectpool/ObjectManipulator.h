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

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_IObjectPoolInst;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The pool object manipulator.
 */
class LLBC_ObjectManipulator
{
public:
    /**
     * New object in giving memory.
     */
    template <typename ObjectType>
    static void New(void *mem);

    /**
     * Delete object.
     */
    template <typename ObjectType>
    static void Delete(void *obj);

    /**
     * Reset object.
     */
    template <typename ObjectType>
    static bool Reset(void *obj);

    /**
     * Pool instance per-block units number fetch support method, allow user-defined custom units number, 
     * default use LLBC_CFG_CORE_OBJECT_POOL_BLOCK_UNITS_NUMBER.
     * @return size_t - 
     */
    template <typename ObjectType>
    static size_t GetPoolInstPerBlockUnitsNum();

    /**
     * Pool instance create callback, this method is called when ObjectType object pool instance created.
     */
    template <typename ObjectType>
    static void OnPoolInstCreate(LLBC_IObjectPoolInst &poolInst);

    /**
     * Pool instance destroy callback, this method is called when ObjectType object pool instance will destroy.
     */
    template <typename ObjectType>
    static void OnPoolInstDestroy(LLBC_IObjectPoolInst &poolInst);

    #if LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_clear
private:
    /**
     * Reset object, this method is called when object has clear():void function.
     */
    template <typename ObjectType, void (ObjectType::*)()>
    struct clearable_type;
    template <typename ObjectType>
    static bool ResetObj(void *obj, clearable_type<ObjectType, &ObjectType::clear> *);
    #endif // LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_clear

    #if LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Clear
private:
    /**
     * Reset object, this method is called when object has Clear():void function.
     */
    template <typename ObjectType, void (ObjectType::*)()>
    struct Clearable_type;
    template <typename ObjectType>
    static bool ResetObj(void *obj, Clearable_type<ObjectType, &ObjectType::Clear> *);
    #endif // LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Clear

    #if LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_reset
private:
    /**
     * Reset object, this method is called when object has reset():void function.
     */
    template <typename ObjectType, void (ObjectType::*)()>
    struct resetable_type;
    template <typename ObjectType>
    static bool ResetObj(void *obj, resetable_type<ObjectType, &ObjectType::reset> *);
    #endif // LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_reset

    #if LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Reset
private:
    /**
     * Reset object, this method is called when object has Reset():void function.
     */
    template <typename ObjectType, void (ObjectType::*)()>
    struct Resetable_type;
    template <typename ObjectType>
    static bool ResetObj(void *obj, Resetable_type<ObjectType, &ObjectType::Reset> *);
    #endif // LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Reset

    #if LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_reuse
private:
    /**
     * Reset object, this method is called when object has reuse():void function.
     */
    template <typename ObjectType, void (ObjectType::*)()>
    struct reusable_type;
    template <typename ObjectType>
    static bool ResetObj(void *obj, reusable_type<ObjectType, &ObjectType::reuse> *);
    #endif // LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Reset

    #if LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Reuse
private:
    /**
     * Reset object, this method is called when object has Reuse():void function.
     */
    template <typename ObjectType, void (ObjectType::*)()>
    struct Reusable_type;
    template <typename ObjectType>
    static bool ResetObj(void *obj, Reusable_type<ObjectType, &ObjectType::Reuse> *);
    #endif // LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Reuse

private:
    /**
     * Some stl containers support, like:map/set.
     */
    template <typename ObjectType, void (ObjectType::_Mybase::*)()>
    struct clearmethod_in_base_stl_container_type;
    template <typename ObjectType>
    static bool ResetObj(void *obj,
                         clearmethod_in_base_stl_container_type<ObjectType,
                                                                &ObjectType::_Mybase::clear> *);

private:
    /**
     * Reset object, default method.
     */
    template <typename ObjectType>
    static bool ResetObj(void *obj, ...);

public:
    /**
     * Get user-defined object pool instance per-block units number.
     * @return size_t - the per-block units number.
     */
    template <typename ObjectType, size_t (ObjectType::*GetPoolInstPerBlockUnitsNum)()>
    struct poolinst_unitsnum_detectable_type;
    template <typename ObjectType>
    static size_t GetPoolInstPerBlockUnitsNumInl(
        poolinst_unitsnum_detectable_type<ObjectType, &ObjectType::GetPoolInstPerBlockUnitsNum> *);

    /**
     * Get default object pool instance per-block units number(LLBC_CFG_CORE_OBJECT_POOL_BLOCK_UNITS_NUMBER).
     * @return size_t - the per-block units number.
     */
    template <typename ObjectType>
    static size_t GetPoolInstPerBlockUnitsNumInl(...);

private:
    /**
     * Object pool instance create callback function caller, this method is called when object pool instance created.
     */
    template <typename ObjectType, void (ObjectType::*)(LLBC_IObjectPoolInst &)>
    struct poolinstcreate_callable_type;
    template <typename ObjectType>
    static void OnPoolInstCreateInl(LLBC_IObjectPoolInst &poolInst,
                                    poolinstcreate_callable_type<ObjectType,
                                                                 &ObjectType::OnPoolInstCreate> *);

    /**
     * Default object pool instance create callback function caller.
     */
    template <typename ObjectType>
    static void OnPoolInstCreateInl(LLBC_IObjectPoolInst &poolInst, ...);

private:
    /**
     * Object pool instance destroy callback function caller, this method is called when object pool instance will destroy.
     */
    template <typename ObjectType, void (ObjectType::*)(LLBC_IObjectPoolInst &)>
    struct poolinstdestroy_callable_type;
    template <typename ObjectType>
    static void OnPoolInstDestroyInl(LLBC_IObjectPoolInst &poolInst,
                                     poolinstdestroy_callable_type<ObjectType,
                                                                   &ObjectType::OnPoolInstDestroy> *);

    /**
     * Default object pool instance destroy callback function caller.
     */
    template <typename ObjectType>
    static void OnPoolInstDestroyInl(LLBC_IObjectPoolInst &pool, ...);

};

__LLBC_NS_END

#include "llbc/core/objectpool/ObjectManipulatorInl.h"


