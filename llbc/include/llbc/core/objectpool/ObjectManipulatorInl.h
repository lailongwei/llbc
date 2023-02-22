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

#if LLBC_CUR_COMPILER == LLBC_COMPILER_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
#endif // GCC compiler

__LLBC_NS_BEGIN

template <typename ObjectType>
LLBC_FORCE_INLINE void LLBC_ObjectManipulator::New(void *mem)
{
    new (mem) ObjectType();
}

template <typename ObjectType>
LLBC_FORCE_INLINE void LLBC_ObjectManipulator::Delete(void *obj)
{
    (reinterpret_cast<ObjectType*>(obj))->~ObjectType();
}

template <typename ObjectType>
LLBC_FORCE_INLINE bool LLBC_ObjectManipulator::Reset(void *obj)
{
    return ResetObj<ObjectType>(reinterpret_cast<ObjectType *>(obj), nullptr);
}

template <typename ObjectType>
LLBC_FORCE_INLINE size_t LLBC_ObjectManipulator::GetPoolInstPerBlockUnitsNum()
{
    return GetPoolInstPerBlockUnitsNumInl<ObjectType>(0);
}

template <typename ObjectType>
LLBC_FORCE_INLINE void LLBC_ObjectManipulator::OnPoolInstCreate(LLBC_IObjectPoolInst &poolInst)
{
    OnPoolInstCreateInl<ObjectType>(poolInst, 0);
}

template <typename ObjectType>
LLBC_FORCE_INLINE void LLBC_ObjectManipulator::OnPoolInstDestroy(LLBC_IObjectPoolInst &poolInst)
{
    OnPoolInstDestroyInl<ObjectType>(poolInst, 0);
}

#if LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_clear
template <typename ObjectType>
bool LLBC_ObjectManipulator::ResetObj(void *obj, clearable_type<ObjectType, &ObjectType::clear> *)
{
    reinterpret_cast<ObjectType *>(obj)->clear();
    return false;
}
#endif // LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_clear

#if LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Clear
template <typename ObjectType>
bool LLBC_ObjectManipulator::ResetObj(void *obj, Clearable_type<ObjectType, &ObjectType::Clear> *)
{
    reinterpret_cast<ObjectType *>(obj)->Clear();
    return false;
}
#endif // LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Clear

#if LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_reset
template <typename ObjectType>
bool LLBC_ObjectManipulator::ResetObj(void *obj, resetable_type<ObjectType, &ObjectType::reset> *)
{
    reinterpret_cast<ObjectType *>(obj)->reset();
    return false;
}
#endif // LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_reset

#if LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Reset
template <typename ObjectType>
bool LLBC_ObjectManipulator::ResetObj(void *obj, Resetable_type<ObjectType, &ObjectType::Reset> *)
{
    reinterpret_cast<ObjectType *>(obj)->Reset();
    return false;
}
#endif // LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Reset

#if LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_reuse
template <typename ObjectType>
bool LLBC_ObjectManipulator::ResetObj(void *obj, reusable_type<ObjectType, &ObjectType::reuse> *)
{
    reinterpret_cast<ObjectType *>(obj)->reuse();
    return false;
}
#endif // LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_reuse

#if LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Reuse
template <typename ObjectType>
bool LLBC_ObjectManipulator::ResetObj(void *obj, Reusable_type<ObjectType, &ObjectType::Reuse> *)
{
    reinterpret_cast<ObjectType *>(obj)->Reuse();
    return false;
}
#endif // LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Reuse

template <typename ObjectType>
bool LLBC_ObjectManipulator::ResetObj(void *obj,
                                      clearmethod_in_base_stl_container_type<ObjectType,
                                                                             &ObjectType::_Mybase::clear> *)
{
    reinterpret_cast<ObjectType *>(obj)->clear();
    return false;
}

template <typename ObjectType>
bool LLBC_ObjectManipulator::ResetObj(void *obj, ...)
{
    Delete<ObjectType>(obj);
    return true;
}

template <typename ObjectType>
size_t LLBC_ObjectManipulator::GetPoolInstPerBlockUnitsNumInl(
    poolinst_unitsnum_detectable_type<ObjectType, &ObjectType::GetPoolInstPerBlockUnitsNum> *)
{
    return reinterpret_cast<ObjectType *>(NULL)->GetPoolInstPerBlockUnitsNum();
}

template <typename ObjectType>
size_t LLBC_ObjectManipulator::GetPoolInstPerBlockUnitsNumInl(...)
{
    return LLBC_CFG_CORE_OBJECT_POOL_BLOCK_UNITS_NUMBER;
}

template <typename ObjectType>
void LLBC_ObjectManipulator::OnPoolInstCreateInl(LLBC_IObjectPoolInst &poolInst,
                                                 poolinstcreate_callable_type<ObjectType,
                                                                              &ObjectType::OnPoolInstCreate> *)
{
    reinterpret_cast<ObjectType *>(NULL)->OnPoolInstCreate(poolInst);
}

template <typename ObjectType>
void LLBC_ObjectManipulator::OnPoolInstCreateInl(LLBC_IObjectPoolInst &poolInst, ...)
{
    // Do nothing.
}

template <typename ObjectType>
void LLBC_ObjectManipulator::OnPoolInstDestroyInl(LLBC_IObjectPoolInst &poolInst,
                                                  poolinstdestroy_callable_type<ObjectType,
                                                                                &ObjectType::OnPoolInstDestroy> *)
{
    reinterpret_cast<ObjectType *>(NULL)->OnPoolInstDestroy(poolInst);
}

template <typename ObjectType>
void LLBC_ObjectManipulator::OnPoolInstDestroyInl(LLBC_IObjectPoolInst &poolInst, ...)
{
    // Do nothing.
}

__LLBC_NS_END

#if LLBC_CUR_COMPILER == LLBC_COMPILER_GCC
#pragma GCC diagnostic pop
#endif // GCC compiler


