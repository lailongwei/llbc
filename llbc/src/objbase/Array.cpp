/**
 * @file    Array.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/19
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/objbase/Object.h"
#include "llbc/objbase/ObjectFactory.h"
#include "llbc/objbase/ObjectMacro.h"
#include "llbc/objbase/Array.h"

__LLBC_NS_BEGIN

LLBC_Array::LLBC_Array()
: _size(0)
, _capacity(0) 
, _objs(NULL)

, _objFactory(NULL)
{
    this->Recapacity(1);
}

LLBC_Array::~LLBC_Array()
{
    this->Erase(this->Begin(), this->End());
    LLBC_XFree(_objs);

    LLBC_SAFE_RELEASE(_objFactory);
}

void LLBC_Array::Clear()
{
    this->Erase(this->Begin(), this->End());
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
    return this->Insert(this->Begin() + n0, o);
}

LLBC_Array::Iter LLBC_Array::Insert(LLBC_Array::Iter n0, LLBC_Array::Obj *o)
{
    if (UNLIKELY(!o))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return this->End();
    }
    if (!(n0 >= this->Begin() && n0 <= this->End()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return this->End();
    }

    if (_size == _capacity)
    {
        this->Recapacity(MAX(1, _size * 2));
    }

    LLBC_MemCpy(_objs + n0._idx + 1, 
        _objs + n0._idx, (_size - n0._idx) * sizeof(Obj *));

    _objs[n0._idx] = o;
    _objs[n0._idx]->Retain();

    _size += 1;

    return Iter(_objs, n0._idx);
}

LLBC_Array::Iter LLBC_Array::Insert(LLBC_Array::difference_type n0, const LLBC_Array &other)
{
    return this->Replace(n0, n0, other);
}

LLBC_Array::Iter LLBC_Array::Insert(LLBC_Array::Iter n0, const LLBC_Array &other)
{
    return this->Replace(n0, n0, other);
}

LLBC_Array::Iter LLBC_Array::Replace(LLBC_Array::difference_type n0, Obj *o)
{
    return this->Replace(this->Begin() + n0, o);
}

LLBC_Array::Iter LLBC_Array::Replace(LLBC_Array::Iter n0, Obj *o)
{
    if (UNLIKELY(o))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return this->End();
    }
    if (!(n0 >= this->Begin() && n0 < this->End()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return this->End();
    }

    _objs[n0._idx]->Release();

    _objs[n0._idx] = o;
    _objs[n0._idx]->Retain();

    return n0;
}

LLBC_Array::Iter LLBC_Array::Replace(LLBC_Array::difference_type n0, LLBC_Array::difference_type n1, const LLBC_Array &other)
{
    return this->Replace(this->Begin() + n0, this->Begin() + n1, other);
}

LLBC_Array::Iter LLBC_Array::Replace(LLBC_Array::Iter n0, LLBC_Array::Iter n1, const LLBC_Array &other)
{
    if (this == &other)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return this->End();
    }
    if (!(n0 >= this->Begin() && n0 <= this->End()) ||
        !(n1 >= this->Begin() && n1 <= this->End()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return this->End();
    }

    if (_capacity < n0._idx + other._size)
    {
        this->Recapacity(n0._idx + other._size);
    }

    for (difference_type i = n0._idx; i < n1._idx; i++)
    {
        _objs[i]->Release();
        _objs[i] = NULL;
    }

    difference_type shift = other._size - (n1._idx - n0._idx);
    LLBC_MemCpy(_objs + n1._idx + shift, _objs + n1._idx, (_size - n1._idx) * sizeof(Obj *));
    LLBC_MemCpy(_objs + n0._idx, other._objs, other._size * sizeof(Obj *));
    for (difference_type i = n0._idx; i < n0._idx + other._size; i++)
    {
        _objs[i]->Retain();
    }

    _size += shift;

    return Iter(_objs, n0._idx);
}

LLBC_Array::size_type LLBC_Array::Erase(LLBC_Array::Obj *o, bool releaseObj)
{
    size_type erasedCount = 0;
    if (UNLIKELY(o == NULL))
    {
        return erasedCount;
    }

    for (difference_type i = _size - 1; i >= 0; i --)
    {
        if (_objs[i] == o)
        {
            if (releaseObj)
            {
                _objs[i]->Release();
                ++erasedCount;
            }

            LLBC_MemCpy(_objs + i, _objs + i + 1, _size - (i + 1) * sizeof(Obj *));
            _objs[-- _size] = NULL;
        }
    }

    return erasedCount;
}

LLBC_Array::Iter LLBC_Array::Erase(Iter n0)
{
    if (!(n0 >= this->Begin() && n0 < this->End()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return this->End();
    }

    return this->Erase(n0, n0 + 1);
}

LLBC_Array::Iter LLBC_Array::Erase(LLBC_Array::difference_type n0)
{
    return this->Erase(this->Begin() + n0);
}

LLBC_Array::Iter LLBC_Array::Erase(LLBC_Array::Iter n0, LLBC_Array::Iter n1)
{
    if (UNLIKELY(n0 >= n1))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return this->End();
    }
    if (!(n0 >= this->Begin() && n0 < this->End()) ||
        !(n1 >= this->Begin() && n1 <= this->End()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return this->End();
    }

    for (difference_type i = n0._idx; i < n1._idx; i++)
    {
        _objs[i]->Release();
    }

    LLBC_MemCpy(_objs + n0._idx, _objs + n1._idx, (_size - n1._idx) * sizeof(Obj *));
    LLBC_MemSet(_objs + _size - (n1._idx - n0._idx), 0, (n1._idx - n0._idx) * sizeof(Obj *));

    _size -= (n1._idx - n0._idx);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return Iter(_objs, n0._idx);
}

LLBC_Array::Iter LLBC_Array::Erase(LLBC_Array::difference_type n0, LLBC_Array::difference_type n1)
{
    return this->Erase(this->Begin() + n0, this->Begin() + n1);
}

void LLBC_Array::PushFront(LLBC_Array::Obj *o)
{
    this->Insert(this->Begin(), o);
}

void LLBC_Array::PushFront(LLBC_Array::Iter it)
{
    this->PushFront(*it);
}

void LLBC_Array::PushBack(Obj *o)
{
    this->Insert(this->End(), o);
}

void LLBC_Array::PushBack(LLBC_Array::Iter it)
{
    this->PushBack(*it);
}

void LLBC_Array::PopFront()
{
    this->Erase(this->Begin());
}

void LLBC_Array::PopBack()
{
    if (!this->IsEmpty())
    {
        this->Erase(this->End() - 1);
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
    if (this->IsEmpty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    return *this->Begin();
}

LLBC_Array::ConstObj *LLBC_Array::FirstObject() const
{
    if (this->IsEmpty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    return *this->Begin();
}

LLBC_Array::Obj *LLBC_Array::LastObject()
{
    if (this->IsEmpty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    return *(-- this->End());
}

LLBC_Array::ConstObj *LLBC_Array::LastObject() const
{
    if (this->IsEmpty())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    return *(-- this->End());
}

LLBC_Array::Obj *LLBC_Array::ObjectAtIndex(difference_type off)
{
    Iter iter = this->Begin() + off;
    if (iter >= this->Begin() && iter < this->End())
    {
        return *iter;
    }

    LLBC_SetLastError(LLBC_ERROR_ARG);
    return NULL;
}

LLBC_Array::ConstObj *LLBC_Array::ObjectAtIndex(difference_type off) const
{
    ConstIter iter = this->Begin() + off;
    if (iter >= this->Begin() && iter < this->End())
    {
        return *iter;
    }

    LLBC_SetLastError(LLBC_ERROR_ARG);
    return NULL;
}

LLBC_Array *LLBC_Array::ObjectsAtIndexs(const LLBC_Array::IndexSet &indexs)
{
    Obj *o = NULL;

    int errNo = LLBC_GetLastError();
    int subErrNo = LLBC_GetSubErrorNo();

    LLBC_Array *arr = new LLBC_Array;
    IndexSet::const_iterator iter = indexs.begin();
    for (; iter != indexs.end(); iter++)
    {
        if ((o = this->ObjectAtIndex(*iter)))
        {
            arr->PushBack(o);
        }
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

LLBC_Array &LLBC_Array::operator =(const LLBC_Array &rhs)
{
    if (this == &rhs)
    {
        return *this;
    }

    this->Clear();
    if (_capacity < rhs._size)
    {
        this->Recapacity(rhs._size);
    }

    LLBC_MemCpy(_objs, rhs._objs, rhs._size * sizeof(Obj *));
    for (size_type i = 0; i < rhs._size; i++)
    {
        _objs[i]->Retain();
    }

    _size = rhs._size;

    this->SetObjectFactory(rhs._objFactory);

    return *this;
}

void LLBC_Array::SetObjectFactory(LLBC_ObjectFactory *factory)
{
    LLBC_SAFE_RELEASE(_objFactory);

    if ((_objFactory = factory))
    {
        _objFactory->Retain();
    }
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

        LLBC_SAFE_RELEASE(cloneObjFactory);
    }

    // Clone all array elements.
    ConstIter it = this->Begin(), endIt = this->End();
    for (; it != endIt; it++)
    {
        LLBC_Object *cloneObj = (*it)->Clone();
        clone->PushBack(cloneObj);

        LLBC_SAFE_RELEASE(cloneObj);
    }

    return clone;
}

void LLBC_Array::Serialize(LLBC_Stream &s) const
{
    this->SerializeInl(s, false);
}

bool LLBC_Array::DeSerialize(LLBC_Stream &s)
{
    return this->DeSerializeInl(s, false);
}

void LLBC_Array::SerializeEx(LLBC_Stream &s) const
{
    this->SerializeInl(s, true);
}

bool LLBC_Array::DeSerializeEx(LLBC_Stream &s)
{
    return this->DeSerializeInl(s, true);
}

void LLBC_Array::Recapacity(size_type cap)
{
    if (cap <= _capacity)
    {
        return;
    }

    _objs = reinterpret_cast<Obj **>(realloc(_objs, cap * sizeof(Obj *)));
    LLBC_MemSet(_objs + _capacity, 0, (cap - _capacity) * sizeof(Obj *));

    _capacity = cap;
}

void LLBC_Array::SerializeInl(LLBC_Stream &s, bool extended) const
{
    LLBC_STREAM_BEGIN_WRITE(s);

    LLBC_STREAM_WRITE(static_cast<uint64>(_size));

    ConstIter it = this->Begin(), endIt = this->End();
    for (; it != endIt; it++)
    {
        if (!extended)
        {
            LLBC_STREAM_WRITE(*it);
        }
        else
        {
            LLBC_STREAM_WRITE_EX(*it);
        }
    }

    LLBC_STREAM_END_WRITE();
}

bool LLBC_Array::DeSerializeInl(LLBC_Stream &s, bool extended)
{
    if (UNLIKELY(!_objFactory))
    {
        return false;
    }

    this->Clear();

    LLBC_STREAM_BEGIN_READ(s, bool, false);

    uint64 size = 0;
    LLBC_STREAM_READ(size);

    for (uint64 i = 0; i < size; i++)
    {
        LLBC_Object *o = _objFactory->CreateObject();
        if (!(!extended ? s.Read(*o) : s.ReadEx(*o)))
        {
            o->Release();
            this->Clear();

            return false;
        }

        this->PushBack(o);
        o->Release();
    }

    LLBC_STREAM_END_READ_RET(true);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
