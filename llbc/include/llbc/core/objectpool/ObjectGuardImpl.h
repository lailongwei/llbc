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

#ifdef __LLBC_CORE_OBJECT_POOL_OBJECT_GUARD_H__

#include "llbc/core/objectpool/IObjectPoolInst.h"

__LLBC_NS_BEGIN

template <typename ObjectType>
inline LLBC_ObjectGuard<ObjectType>::LLBC_ObjectGuard(ObjectType *obj, LLBC_IObjectPoolInst *poolInst)
: _obj(obj)
, _poolInst(poolInst)

, _weakRef(false)
{
}

template <typename ObjectType>
inline LLBC_ObjectGuard<ObjectType>::LLBC_ObjectGuard(const LLBC_ObjectGuard<ObjectType> &another)
: _obj(another._obj)
, _poolInst(another._poolInst)

, _weakRef(false)
{
    another.MakeWeakRef();
}

template <typename ObjectType>
inline LLBC_ObjectGuard<ObjectType>::~LLBC_ObjectGuard()
{
    if (!_weakRef && _obj && _poolInst)
        _poolInst->Release(_obj);
}

template <typename ObjectType>
inline ObjectType *&LLBC_ObjectGuard<ObjectType>::operator ->()
{
    return _obj;
}

template <typename ObjectType>
inline const ObjectType * const &LLBC_ObjectGuard<ObjectType>::operator ->() const
{
    return _obj;
}

template <typename ObjectType>
ObjectType &LLBC_ObjectGuard<ObjectType>::operator *()
{
    return *_obj;
}

template <typename ObjectType>
const ObjectType &LLBC_ObjectGuard<ObjectType>::operator *() const
{
    return *_obj;
}

template <typename ObjectType>
inline LLBC_ObjectGuard<ObjectType>::operator ObjectType *()
{
    return _obj;
}

template <typename ObjectType>
inline LLBC_ObjectGuard<ObjectType>::operator const ObjectType *() const
{
    return _obj;
}

template <typename ObjectType>
inline ObjectType *LLBC_ObjectGuard<ObjectType>::GetObj()
{
    return _obj;
}

template <typename ObjectType>
inline const ObjectType *LLBC_ObjectGuard<ObjectType>::GetObj() const
{
    return _obj;
}

template <typename ObjectType>
inline ObjectType *LLBC_ObjectGuard<ObjectType>::DetachObj()
{
    ObjectType *obj = _obj;
    _obj = nullptr;

    return obj;
}

template <typename ObjectType>
inline void LLBC_ObjectGuard<ObjectType>::MakeWeakRef() const
{
    const_cast<LLBC_ObjectGuard<ObjectType> *>(this)->_weakRef = true;
}

__LLBC_NS_END

#endif // __LLBC_CORE_OBJECT_POOL_OBJECT_GUARD_H__
