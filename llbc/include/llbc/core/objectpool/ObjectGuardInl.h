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

#include "llbc/core/objectpool/IObjectPoolInst.h"

__LLBC_NS_BEGIN

template <typename ObjectType>
LLBC_ObjectGuard<ObjectType>::LLBC_ObjectGuard()
: _obj(nullptr)
, _poolInst(nullptr)

, _weakRef(false)
{
}

template <typename ObjectType>
LLBC_ObjectGuard<ObjectType>::LLBC_ObjectGuard(ObjectType *obj, LLBC_IObjectPoolInst *poolInst)
: _obj(obj)
, _poolInst(poolInst)

, _weakRef(false)
{
}

template <typename ObjectType>
LLBC_ObjectGuard<ObjectType>::LLBC_ObjectGuard(const LLBC_ObjectGuard<ObjectType> &another)
: _obj(another._obj)
, _poolInst(another._poolInst)

, _weakRef(another._weakRef)
{
    if (another._obj && !another._weakRef)
        another._weakRef = true;
}

template <typename ObjectType>
LLBC_ObjectGuard<ObjectType>::LLBC_ObjectGuard(LLBC_ObjectGuard<ObjectType> &&another)
: _obj(another._obj)
, _poolInst(another._poolInst)

, _weakRef(another._weakRef)
{
    if (another._obj && !another._weakRef)
        another._weakRef = true;
    another.ReleaseObj();
}

template <typename ObjectType>
LLBC_ObjectGuard<ObjectType>::~LLBC_ObjectGuard()
{
    if (_obj && !_weakRef)
        _poolInst->Release(_obj);
}

template <typename ObjectType>
ObjectType *LLBC_ObjectGuard<ObjectType>::operator->()
{
    return _obj;
}

template <typename ObjectType>
const ObjectType *LLBC_ObjectGuard<ObjectType>::operator->() const
{
    return _obj;
}

template <typename ObjectType>
ObjectType &LLBC_ObjectGuard<ObjectType>::operator*()
{
    return *_obj;
}

template <typename ObjectType>
const ObjectType &LLBC_ObjectGuard<ObjectType>::operator*() const
{
    return *_obj;
}

template <typename ObjectType>
LLBC_ObjectGuard<ObjectType>::operator bool() const
{
    return _obj != nullptr;
}

template <typename ObjectType>
LLBC_ObjectGuard<ObjectType> &LLBC_ObjectGuard<ObjectType>::operator=(const LLBC_ObjectGuard &another)
{
    if (this == &another)
        return *this;

    if (_obj && !_weakRef)
        _poolInst->Release(_obj);

    _obj = another._obj;
    _poolInst = another._poolInst;
    _weakRef = another._weakRef;

    if (another._obj && !another._weakRef)
        another._weakRef = true;

    return *this;
}

template <typename ObjectType>
LLBC_ObjectGuard<ObjectType> &LLBC_ObjectGuard<ObjectType>::operator=(LLBC_ObjectGuard &&another)
{
    if (this == &another)
        return *this;

    if (_obj && !_weakRef)
        _poolInst->Release(_obj);

    _obj = another._obj;
    _poolInst = another._poolInst;
    _weakRef = another._weakRef;

    if (another._obj && !another._weakRef)
        another._weakRef = true;

    another.ReleaseObj();

    return *this;
}

template <typename ObjectType>
LLBC_ObjectGuard<ObjectType>::operator ObjectType*()
{
    return _obj;
}

template <typename ObjectType>
LLBC_ObjectGuard<ObjectType>::operator const ObjectType*() const
{
    return _obj;
}

template <typename ObjectType>
ObjectType *LLBC_ObjectGuard<ObjectType>::GetObj()
{
    return _obj;
}

template <typename ObjectType>
const ObjectType *LLBC_ObjectGuard<ObjectType>::GetObj() const
{
    return _obj;
}

template <typename ObjectType>
bool LLBC_ObjectGuard<ObjectType>::IsWeakRef() const
{
    return _weakRef;
}

template <typename ObjectType>
ObjectType *LLBC_ObjectGuard<ObjectType>::DetachObj()
{
    if (_obj)
    {
        ObjectType *obj = _obj;
        _obj = nullptr;
        _poolInst = nullptr;
        _weakRef = false;

        return obj;
    }
    else
    {
        return nullptr;
    }
}

template <typename ObjectType>
void LLBC_ObjectGuard<ObjectType>::ReleaseObj()
{
    if (!_obj)
        return;

    if (!_weakRef)
        _poolInst->Release(_obj);
    else
        _weakRef = false;

    _obj = nullptr;
    _poolInst = nullptr;
}

template <typename ObjectType>
LLBC_String LLBC_ObjectGuard<ObjectType>::ToString() const
{
    if (!_obj)
        return "ObjectGuard[null]";

    LLBC_String repr;
    return LLBC_String().format("ObjectGuard[obj(%s):0x%p, poolInst:0x%p, weak?:%s]",
                                LLBC_GetTypeName(_obj), _obj, _poolInst, _weakRef ? "true" : "false");
}

__LLBC_NS_END

template <typename ObjectType>
std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_ObjectGuard<ObjectType> &objGuard)
{
    return (o << objGuard.ToString());
}


