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


#include "llbc/common/Export.h"

#include "llbc/common/Config.h"

#include "llbc/core/objbase/ObjectFactory.h"
#include "llbc/core/objbase/ObjectMacro.h"
#include "llbc/core/objbase/Array.h"

__LLBC_NS_BEGIN

LLBC_Array::LLBC_Array()
: _size(0)
, _capacity(0) 
, _objs(nullptr)

, _objFactory(nullptr)
{
    Recapacity(1);
}

LLBC_Array::~LLBC_Array()
{
    Erase(Begin(), End());
    LLBC_XFree(_objs);

    LLBC_XRelease(_objFactory);
}

void LLBC_Array::Clear()
{
    Erase(Begin(), End());
}

LLBC_Array::size_type LLBC_Array::GetSize() const
{
    return _size;
}

LLBC_Array::size_type LLBC_Array::GetCapacity() const
{
    return _capacity;
}

bool LLBC_Array::IsEmpty() const
{
    return _size == 0;
}

LLBC_Array::Iter LLBC_Array::Insert(LLBC_Array::difference_type n0, LLBC_Array::Obj *o)
{
    return Insert(Begin() + n0, o);
}

LLBC_Array::Iter LLBC_Array::Insert(LLBC_Array::Iter n0, LLBC_Array::Obj *o)
{
    if (UNLIKELY(!o))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return End();
    }
    if (!(n0 >= Begin() && n0 <= End()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return End();
    }

    if (_size == _capacity)
        Recapacity(MAX(1, _size * 2));

    memmove(_objs + n0._idx + 1, _objs + n0._idx, (_size - n0._idx) * sizeof(Obj *));

    _objs[n0._idx] = o;
    _objs[n0._idx]->Retain();

    _size += 1;

    return Iter(_objs, n0._idx);
}

LLBC_Array::Iter LLBC_Array::Insert(LLBC_Array::difference_type n0, const LLBC_Array &other)
{
    return Replace(n0, n0, other);
}

LLBC_Array::Iter LLBC_Array::Insert(LLBC_Array::Iter n0, const LLBC_Array &other)
{
    return Replace(n0, n0, other);
}

LLBC_Array::Iter LLBC_Array::Replace(LLBC_Array::difference_type n0, Obj *o)
{
    return Replace(Begin() + n0, o);
}

LLBC_Array::Iter LLBC_Array::Replace(LLBC_Array::Iter n0, Obj *o)
{
    if (UNLIKELY(o))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return End();
    }

    if (n0 < Begin() || n0 >= End())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return End();
    }

    _objs[n0._idx]->Release();

    _objs[n0._idx] = o;
    _objs[n0._idx]->Retain();

    return n0;
}

LLBC_Array::Iter LLBC_Array::Replace(LLBC_Array::difference_type n0, LLBC_Array::difference_type n1, const LLBC_Array &other)
{
    return Replace(Begin() + n0, Begin() + n1, other);
}

LLBC_Array::Iter LLBC_Array::Replace(LLBC_Array::Iter n0, LLBC_Array::Iter n1, const LLBC_Array &other)
{
    if (this == &other)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return End();
    }
    if (n0 < Begin() || n0 > End() ||
        n1 < Begin() || n1 > End())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return End();
    }

    if (_capacity < n0._idx + other._size)
        Recapacity(n0._idx + other._size);

    for (difference_type i = n0._idx; i < n1._idx; ++i)
    {
        _objs[i]->Release();
        _objs[i] = nullptr;
    }

    difference_type shift = other._size - (n1._idx - n0._idx);
    memmove(_objs + n1._idx + shift, _objs + n1._idx, (_size - n1._idx) * sizeof(Obj *));
    memmove(_objs + n0._idx, other._objs, other._size * sizeof(Obj *));
    for (difference_type i = n0._idx; i < n0._idx + other._size; ++i)
        _objs[i]->Retain();

    _size += shift;

    return Iter(_objs, n0._idx);
}

LLBC_Array::size_type LLBC_Array::Erase(LLBC_Array::Obj *o, bool releaseObj)
{
    size_type erasedCount = 0;
    if (UNLIKELY(o == nullptr))
        return erasedCount;

    for (difference_type i = _size - 1; i >= 0; --i)
    {
        if (_objs[i] == o)
        {
            if (releaseObj)
            {
                _objs[i]->Release();
                ++erasedCount;
            }

            memmove(_objs + i, _objs + i + 1, (_size - (i + 1)) * sizeof(Obj *));
            _objs[--_size] = nullptr;
        }
    }

    return erasedCount;
}

LLBC_Array::Iter LLBC_Array::Erase(Iter n0)
{
    if (!(n0 >= Begin() && n0 < End()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return End();
    }

    return Erase(n0, n0 + 1);
}

LLBC_Array::Iter LLBC_Array::Erase(LLBC_Array::difference_type n0)
{
    return Erase(Begin() + n0);
}

LLBC_Array::Iter LLBC_Array::Erase(LLBC_Array::Iter n0, LLBC_Array::Iter n1)
{
    if (UNLIKELY(n0 >= n1))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return End();
    }

    if (n0 < Begin() || n0 >= End() ||
        n1 < Begin() || n1 > End())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return End();
    }

    for (difference_type i = n0._idx; i < n1._idx; ++i)
        _objs[i]->Release();

    memmove(_objs + n0._idx, _objs + n1._idx, (_size - n1._idx) * sizeof(Obj *));
    memset(_objs + _size - (n1._idx - n0._idx), 0, (n1._idx - n0._idx) * sizeof(Obj *));

    _size -= (n1._idx - n0._idx);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return Iter(_objs, n0._idx);
}

LLBC_Array::Iter LLBC_Array::Erase(LLBC_Array::difference_type n0, LLBC_Array::difference_type n1)
{
    return Erase(Begin() + n0, Begin() + n1);
}

void LLBC_Array::PushFront(LLBC_Array::Obj *o)
{
    Insert(Begin(), o);
}

void LLBC_Array::PushFront(LLBC_Array::Iter it)
{
    PushFront(*it);
}

void LLBC_Array::PushBack(Obj *o)
{
    Insert(End(), o);
}

void LLBC_Array::PushBack(LLBC_Array::Iter it)
{
    PushBack(*it);
}

void LLBC_Array::PopFront()
{
    Erase(Begin());
}

void LLBC_Array::PopBack()
{
    if (!IsEmpty())
    {
        Erase(End() - 1);
    }
}

LLBC_Array::Iter LLBC_Array::Begin()
{
    return Iter(_objs, 0);
}

LLBC_Array::ConstIter LLBC_Array::Begin() const
{
    return ConstIter((ConstIter::pointer)_objs, 0);
}

LLBC_Array::Iter LLBC_Array::End()
{
    return Iter(_objs, _size);
}

LLBC_Array::ConstIter LLBC_Array::End() const
{
    return ConstIter((ConstIter::pointer)_objs, _size);
}

LLBC_Array::ReverseIter LLBC_Array::ReverseBegin()
{
    return ReverseIter(_objs, _size);
}

LLBC_Array::ConstReverseIter LLBC_Array::ReverseBegin() const
{
    return ConstReverseIter((ConstReverseIter::pointer)_objs, _size);
}

LLBC_Array::ReverseIter LLBC_Array::ReverseEnd()
{
    return ReverseIter(_objs, 0);
}

LLBC_Array::ConstReverseIter LLBC_Array::ReverseEnd() const
{
    return ConstReverseIter((ConstReverseIter::pointer)_objs, 0);
}

LLBC_Array::Obj *LLBC_Array::FirstObject()
{
    if (IsEmpty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    return *Begin();
}

LLBC_Array::ConstObj *LLBC_Array::FirstObject() const
{
    if (IsEmpty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    return *Begin();
}

LLBC_Array::Obj *LLBC_Array::LastObject()
{
    if (IsEmpty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    return *(--End());
}

LLBC_Array::ConstObj *LLBC_Array::LastObject() const
{
    if (IsEmpty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    return *(--End());
}

LLBC_Array::Obj *LLBC_Array::ObjectAtIndex(difference_type off)
{
    Iter iter = Begin() + off;
    if (iter >= Begin() && iter < End())
        return *iter;

    LLBC_SetLastError(LLBC_ERROR_ARG);
    return nullptr;
}

LLBC_Array::ConstObj *LLBC_Array::ObjectAtIndex(difference_type off) const
{
    ConstIter iter = Begin() + off;
    if (iter >= Begin() && iter < End())
        return *iter;

    LLBC_SetLastError(LLBC_ERROR_ARG);
    return nullptr;
}

LLBC_Array *LLBC_Array::ObjectsAtIndexs(const LLBC_Array::IndexSet &indexs)
{
    Obj *o = nullptr;

    int errNo = LLBC_GetLastError();
    int subErrNo = LLBC_GetSubErrorNo();

    LLBC_Array *arr = new LLBC_Array;
    IndexSet::const_iterator iter = indexs.begin();
    for (; iter != indexs.end(); ++iter)
    {
        if ((o = ObjectAtIndex(*iter)))
            arr->PushBack(o);
    }

    LLBC_SetLastError(errNo);
    LLBC_SetSubErrorNo(subErrNo);

    return arr;
}

LLBC_Array::Obj *&LLBC_Array::operator [](LLBC_Array::difference_type off)
{
    ASSERT((off >= 0 && off < _size) && 
        "LLBC_Array::operator [] method subscript invalid!");

    return _objs[off];
}

LLBC_Array::ConstObj *LLBC_Array::operator [](LLBC_Array::difference_type off) const
{
    ASSERT((off >= 0 && off < _size) && 
        "LLBC_Array::operator [] method subscript invalid!");

    return _objs[off];
}

void LLBC_Array::SetObjectFactory(LLBC_ObjectFactory *factory)
{
    LLBC_XRelease(_objFactory);

    if ((_objFactory = factory))
        _objFactory->Retain();
}

LLBC_Object *LLBC_Array::Clone() const
{
    LLBC_Array *clone = new LLBC_Array;

    // Clone object factory.
    if (_objFactory)
    {
        LLBC_ObjectFactory *cloneObjFactory = 
            static_cast<LLBC_ObjectFactory *>(_objFactory->Clone());
        clone->SetObjectFactory(cloneObjFactory);

        LLBC_XRelease(cloneObjFactory);
    }

    // Clone all array elements.
    ConstIter it = Begin(), endIt = End();
    for (; it != endIt; ++it)
    {
        LLBC_Object *cloneObj = (*it)->Clone();
        clone->PushBack(cloneObj);

        LLBC_XRelease(cloneObj);
    }

    return clone;
}

void LLBC_Array::Serialize(LLBC_Stream &s) const
{
    s <<static_cast<uint32>(_size);
    ConstIter it = Begin(), endIt = End();
    for (; it != endIt; ++it)
        s <<*it;
}

bool LLBC_Array::DeSerialize(LLBC_Stream &s)
{
    if (UNLIKELY(!_objFactory))
        return false;

    LLBC_STREAM_BEGIN_READ(s, bool, false);

    uint32 size = 0;
    LLBC_STREAM_READ(size);

    Clear();
    if (size == 0)
        return true;

    for (uint32 i = 0; i < size; ++i)
    {
        LLBC_Object *o = _objFactory->CreateObject();
        if (!s.Read(*o))
        {
            o->Release();
            Clear();

            return false;
        }

        PushBack(o);
        o->Release();
    }

    LLBC_STREAM_END_READ_RET(true);
}

void LLBC_Array::Recapacity(size_type cap)
{
    if (cap <= _capacity)
        return;

    _objs = reinterpret_cast<Obj **>(realloc(_objs, cap * sizeof(Obj *)));
    memset(_objs + _capacity, 0, (cap - _capacity) * sizeof(Obj *));

    _capacity = cap;
}

__LLBC_NS_END
