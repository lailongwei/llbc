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
#include "llbc/core/objbase/Dictionary.h"

__LLBC_NS_BEGIN

LLBC_Dictionary::LLBC_Dictionary(LLBC_Dictionary::size_type bucketSize)
: _size(0)
, _head(0)
, _tail(nullptr)

, _bucketSize(bucketSize)
, _bucket(nullptr)

, _objFactory(nullptr)
{
    _bucket = reinterpret_cast<LLBC_DictionaryElem **>(
        malloc(_bucketSize * sizeof(LLBC_DictionaryElem *)));
    memset(_bucket, 0, _bucketSize * sizeof(LLBC_DictionaryElem *));
}

LLBC_Dictionary::~LLBC_Dictionary()
{
    Clear();
    free(_bucket);

    LLBC_XRelease(_objFactory);
}

void LLBC_Dictionary::Clear()
{
    LLBC_DictionaryElem *elem = _head;
    while (elem)
    {
        LLBC_DictionaryElem *temp = elem;
        elem = elem->GetElemNext();

        delete temp;
    }

    _size = 0;
    _head = _tail = nullptr;

    memset(_bucket, 0, _bucketSize * sizeof(LLBC_DictionaryElem *));
}

LLBC_Dictionary::size_type LLBC_Dictionary::GetSize() const
{
    return _size;
}

bool LLBC_Dictionary::IsEmpty() const
{
    return _size == 0;
}

int LLBC_Dictionary::SetHashBucketSize(size_type bucketSize)
{
    if (UNLIKELY(bucketSize == 0))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    // Cancel hash.
    Iter it = Begin(), endIt = End();
    for (; it != endIt; ++it)
    {
        it.Elem()->CancelHash();
    }

    // ReAlloc bucket.
    _bucketSize = bucketSize;
    _bucket = reinterpret_cast<LLBC_DictionaryElem **>(
        realloc(_bucket, _bucketSize * sizeof(LLBC_DictionaryElem *)));
    memset(_bucket, 0, _bucketSize * sizeof(LLBC_DictionaryElem *));

    // ReHash.
    it = Begin();
    for (; it != endIt; ++it)
    {
        it.Elem()->Hash(_bucket, _bucketSize);
    }

    return LLBC_OK;
}

int LLBC_Dictionary::Insert(int key, LLBC_Dictionary::Obj *o)
{
    if (UNLIKELY(!o))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    if (Find(key) != End())
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    // Check bucket size and auto expand it.
    if (_size >= _bucketSize * 100)
    {
        SetHashBucketSize(_bucketSize * 2);
    }

    LLBC_DictionaryElem *elem = new LLBC_DictionaryElem(key, o);

    // Link to doubly-linked list.
    if (_tail)
    {
        elem->SetElemPrev(_tail);
        _tail->SetElemNext(elem);
        _tail = elem;
    }
    else
    {
        _head = _tail = elem;
    }

    // Hash to bucket.
    elem->Hash(_bucket, _bucketSize);

    _size += 1;

    return LLBC_OK;
}

int LLBC_Dictionary::Insert(const LLBC_String &key, LLBC_Dictionary::Obj *o)
{
    if (UNLIKELY(!o))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    if (Find(key) != End())
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    // Check bucket size and auto expand it.
    if (_size >= _bucketSize * 100)
    {
        SetHashBucketSize(_bucketSize * 2);
    }

    LLBC_DictionaryElem *elem = new LLBC_DictionaryElem(key, o);

    // Hash to bucket.
    elem->Hash(_bucket, _bucketSize);
    // Link to doubly-linked list.
    AddToDoublyLinkedList(elem);

    _size += 1;

    return LLBC_OK;
}

int LLBC_Dictionary::Replace(int key, LLBC_Dictionary::Obj *o)
{
    if (UNLIKELY(!o))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    Iter it = Find(key);
    if (it != End())
    {
        (*it)->Release();
        ((*it) = o)->Retain();

        return LLBC_OK;
    }

    return Insert(key, o);
}

int LLBC_Dictionary::Replace(const LLBC_String &key, LLBC_Dictionary::Obj *o)
{
    if (UNLIKELY(!o))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    Iter it = Find(key);
    if (it != End())
    {
        (*it)->Release();
        ((*it) = o)->Retain();

        return LLBC_OK;
    }

    return Insert(key, o);
}

int LLBC_Dictionary::Replace(Iter it, Obj *o)
{
    if (UNLIKELY(!o || it == End()))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    (*it)->Release();
    ((*it) = o)->Retain();

    return LLBC_OK;
}

int LLBC_Dictionary::Erase(int key)
{
    return Erase(Find(key));
}

int LLBC_Dictionary::Erase(const LLBC_String &key)
{
    return Erase(Find(key));
}

int LLBC_Dictionary::Erase(Iter it)
{
    if (it == End())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    LLBC_DictionaryElem * elem = it.Elem();

    // Remove from hash bucket.
    elem->CancelHash();
    // Remove from doubly-linked list.
    RemoveFromDoublyLinkedList(elem);

    delete elem;

    _size -= 1;

    return LLBC_OK;
}

void LLBC_Dictionary::Erase(Obj *o)
{
    if (IsEmpty())
        return;

    for (Iter it = Begin(); it != End(); ++it)
    {
        if (it.Obj() != o)
            continue;

        Erase(it);
        return;
    }
}

LLBC_Dictionary::Iter LLBC_Dictionary::Find(int key)
{
    int hash = key % _bucketSize;
    LLBC_DictionaryElem *elem = _bucket[hash];
    for (; elem != nullptr; elem = elem->GetBucketElemNext())
    {
        if (elem->IsIntKey() && elem->GetIntKey() == key)
        {
            return Iter(elem);
        }
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    return End();
}

LLBC_Dictionary::Iter LLBC_Dictionary::Find(const LLBC_String &key)
{
    uint32 hash = LLBC_Hash(key.c_str(), key.size()) % _bucketSize;
    LLBC_DictionaryElem *elem = _bucket[hash];
    for (; elem != nullptr; elem = elem->GetBucketElemNext())
    {
        if (elem->IsStrKey() && elem->GetStrKey() == key)
        {
            return Iter(elem);
        }
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    return End();
}

LLBC_Dictionary::ConstIter LLBC_Dictionary::Find(int key) const
{
    LLBC_Dictionary *nonConstThis = 
        const_cast<LLBC_Dictionary *>(this);

    LLBC_Dictionary::Iter it = nonConstThis->Find(key);
    if (it == nonConstThis->End())
    {
        return End();
    }

    return ConstIter(it.Elem());
}

LLBC_Dictionary::ConstIter LLBC_Dictionary::Find(const LLBC_String &key) const
{
    LLBC_Dictionary *nonConstThis = 
        const_cast<LLBC_Dictionary *>(this);

    LLBC_Dictionary::Iter it = nonConstThis->Find(key);
    if (it == nonConstThis->End())
    {
        return End();
    }

    return ConstIter(it.Elem());
}

LLBC_Dictionary::Iter LLBC_Dictionary::Begin()
{
    return Iter(_head);
}

LLBC_Dictionary::ConstIter LLBC_Dictionary::Begin() const
{
    return ConstIter(_head);
}

LLBC_Dictionary::Iter LLBC_Dictionary::End()
{
    return Iter(Iter::pointer(nullptr));
}

LLBC_Dictionary::ConstIter LLBC_Dictionary::End() const
{
    return ConstIter(ConstIter::cpointer(nullptr));
}

LLBC_Dictionary::ReverseIter LLBC_Dictionary::ReverseBegin()
{
    return ReverseIter(_tail);
}

LLBC_Dictionary::ConstReverseIter LLBC_Dictionary::ReverseBegin() const
{
    return ConstReverseIter(_tail);
}

LLBC_Dictionary::ReverseIter LLBC_Dictionary::ReverseEnd()
{
    return ReverseIter(Iter::pointer(nullptr));
}

LLBC_Dictionary::ConstReverseIter LLBC_Dictionary::ReverseEnd() const
{
    return ConstReverseIter(ConstIter::cpointer(nullptr));
}

LLBC_Dictionary::ConstObj *LLBC_Dictionary::operator [](int key) const
{
    ConstIter it = Find(key);
    return it != End() ? it.Obj() : nullptr;
}

LLBC_Dictionary::ConstObj *LLBC_Dictionary::operator [](const LLBC_String &key) const
{
    ConstIter it = Find(key);
    return it != End() ? it.Obj() : nullptr;
}

void LLBC_Dictionary::SetObjectFactory(LLBC_ObjectFactory *factory)
{
    LLBC_XRelease(_objFactory);

    if ((_objFactory = factory))
    {
        _objFactory->Retain();
    }
}

LLBC_Object *LLBC_Dictionary::Clone() const
{
    LLBC_Dictionary *clone = new LLBC_Dictionary;

    // Clone object factory.
    if (_objFactory)
    {
        LLBC_ObjectFactory *cloneObjFactory = 
            static_cast<LLBC_ObjectFactory *>(_objFactory->Clone());
        clone->SetObjectFactory(cloneObjFactory);

        LLBC_XRelease(cloneObjFactory);
    }

    // Clone all elements.
    ConstIter it = Begin(), endIt = End();
    for (; it != endIt; ++it)
    {
        LLBC_Object *cloneObj = ((Obj *)*it)->Clone();
        if (it.IsIntKey())
        {
            clone->Insert(it.IntKey(), cloneObj);
        }
        else
        {
            clone->Insert(it.StrKey(), cloneObj);
        }

        LLBC_XRelease(cloneObj);
    }

    return clone;
}

void LLBC_Dictionary::Serialize(LLBC_Stream &s) const
{
    s <<static_cast<uint32>(_bucketSize)
      <<static_cast<uint32>(_size);

    ConstIter it = Begin(), endIt = End();
    for (; it != endIt; ++it)
    {
        const LLBC_DictionaryElem * const elem = it.Elem();

        const uint8 intKeyFlag = elem->IsIntKey() ? 1 : 0;
        s <<intKeyFlag;

        if (intKeyFlag)
            s <<elem->GetIntKey();
        else
            s <<elem->GetStrKey();

        s <<*it;
    }
}

bool LLBC_Dictionary::DeSerialize(LLBC_Stream &s)
{
    if (!_objFactory)
        return false;

    LLBC_STREAM_BEGIN_READ(s, bool, false);

    uint32 bucketSize = 0;
    LLBC_STREAM_READ(bucketSize);
    SetHashBucketSize(static_cast<size_type>(bucketSize));

    Clear();
    if (bucketSize == 0)
        return true;

    uint32 size = 0;
    LLBC_STREAM_READ(size);
    for (uint32 i = 0; i < size; ++i)
    {
        uint8 intKeyFlag = 0;
        LLBC_STREAM_READ(intKeyFlag);

        if (intKeyFlag)
        {
            int intKey = 0;
            LLBC_STREAM_READ(intKey);

            LLBC_Object *o = _objFactory->CreateObject();
            if (!s.Read(*o))
            {
                o->Release();
                return false;
            }

            Insert(intKey, o);
            o->Release();
        }
        else
        {
            LLBC_String strKey;
            LLBC_STREAM_READ(strKey);

            LLBC_Object *o = _objFactory->CreateObject();
            if (!s.Read(*o))
            {
                o->Release();
                return false;
            }

            Insert(strKey, o);
            o->Release();
        }
    }

    LLBC_STREAM_END_READ_RET(true);
}

void LLBC_Dictionary::AddToDoublyLinkedList(LLBC_DictionaryElem *elem)
{
    if (_tail)
    {
        elem->SetElemPrev(_tail);
        _tail->SetElemNext(elem);
        _tail = elem;
    }
    else
    {
        _head = _tail = elem;
    }
}

void LLBC_Dictionary::RemoveFromDoublyLinkedList(LLBC_DictionaryElem *elem)
{
    if (elem == _head)
    {
        _head = elem->GetElemNext();
    }
    if (elem == _tail)
    {
        _tail = elem->GetElemPrev();
    }

    if (elem->GetElemPrev())
    {
        elem->GetElemPrev()->SetElemNext(elem->GetElemNext());
    }
    if (elem->GetElemNext())
    {
        elem->GetElemNext()->SetElemPrev(elem->GetElemPrev());
    }
}

__LLBC_NS_END
