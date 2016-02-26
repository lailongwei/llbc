/**
 * @file    Dictionary.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/28
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/objbase/Object.h"
#include "llbc/objbase/ObjectFactory.h"
#include "llbc/objbase/ObjectMacro.h"
#include "llbc/objbase/Dictionary.h"

__LLBC_NS_BEGIN

LLBC_Dictionary::LLBC_Dictionary(LLBC_Dictionary::size_type bucketSize)
: _size(0)
, _head(0)
, _tail(NULL)

, _bucketSize(bucketSize)
, _bucket(NULL)

, _objFactory(NULL)
{
    _bucket = reinterpret_cast<LLBC_DictionaryElem **>(
        malloc(_bucketSize * sizeof(LLBC_DictionaryElem *)));
    LLBC_MemSet(_bucket, 0, _bucketSize * sizeof(LLBC_DictionaryElem *));
}

LLBC_Dictionary::~LLBC_Dictionary()
{
    Clear();
    free(_bucket);

    LLBC_SAFE_RELEASE(_objFactory);
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
    _head = _tail = NULL;

    LLBC_MemSet(_bucket, 0, _bucketSize * sizeof(LLBC_DictionaryElem *));
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
    for (; it != endIt; it++)
    {
        it.Elem()->CancelHash();
    }

    // ReAlloc bucket.
    _bucketSize = bucketSize;
    _bucket = reinterpret_cast<LLBC_DictionaryElem **>(
        realloc(_bucket, _bucketSize * sizeof(LLBC_DictionaryElem *)));
    LLBC_MemSet(_bucket, 0, _bucketSize * sizeof(LLBC_DictionaryElem *));

    // ReHash.
    it = Begin();
    for (; it != endIt; it++)
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

LLBC_Dictionary::Iter LLBC_Dictionary::Find(int key)
{
    int hash = key % _bucketSize;
    LLBC_DictionaryElem *elem = _bucket[hash];
    for (; elem != NULL; elem = elem->GetBucketElemNext())
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
    LLBC_KeyHashAlgorithm *hashAlgo = LLBC_KeyHashAlgorithmSingleton;
    const LLBC_KeyHashAlgorithm::HashBase &hashFun = 
        *hashAlgo->GetAlgorithm(LLBC_CFG_OBJBASE_DICT_KEY_HASH_ALGO);

    uint32 hash = hashFun(key.c_str(), key.size()) % _bucketSize;
    LLBC_DictionaryElem *elem = _bucket[hash];
    for (; elem != NULL; elem = elem->GetBucketElemNext())
    {
        if (elem->IsStrKey() && *elem->GetStrKey() == key)
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
    return Iter(NULL);
}

LLBC_Dictionary::ConstIter LLBC_Dictionary::End() const
{
    return ConstIter(NULL);
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
    return ReverseIter(NULL);
}

LLBC_Dictionary::ConstReverseIter LLBC_Dictionary::ReverseEnd() const
{
    return ConstReverseIter(NULL);
}

LLBC_Dictionary::ConstObj *LLBC_Dictionary::operator [](int key) const
{
    ConstIter it = Find(key);
    return it != End() ? it.Obj() : NULL;
}

LLBC_Dictionary::ConstObj *LLBC_Dictionary::operator [](const LLBC_String &key) const
{
    ConstIter it = Find(key);
    return it != End() ? it.Obj() : NULL;
}

void LLBC_Dictionary::SetObjectFactory(LLBC_ObjectFactory *factory)
{
    LLBC_SAFE_RELEASE(_objFactory);

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

        LLBC_SAFE_RELEASE(cloneObjFactory);
    }

    // Clone all elements.
    ConstIter it = Begin(), endIt = End();
    for (; it != endIt; it++)
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

        LLBC_SAFE_RELEASE(cloneObj);
    }

    return clone;
}

void LLBC_Dictionary::Serialize(LLBC_Stream &s) const
{
    SerializeInl(s, false);
}

bool LLBC_Dictionary::DeSerialize(LLBC_Stream &s)
{
    return DeSerializeInl(s, false);
}

void LLBC_Dictionary::SerializeEx(LLBC_Stream &s) const
{
    SerializeInl(s, true);
}

bool LLBC_Dictionary::DeSerializeEx(LLBC_Stream &s)
{
    return DeSerializeInl(s, true);
}

void LLBC_Dictionary::SerializeInl(LLBC_Stream &s, bool extended) const
{
    LLBC_STREAM_BEGIN_WRITE(s);

    LLBC_STREAM_WRITE(static_cast<uint64>(_bucketSize));

    LLBC_STREAM_WRITE(static_cast<uint64>(_size));

    ConstIter it = Begin(), endIt = End();
    for (; it != endIt; it++)
    {
        const LLBC_DictionaryElem *elem = it.Elem();

        const uint8 intKeyFlag = elem->IsIntKey() ? 1 : 0;
        LLBC_STREAM_WRITE(intKeyFlag);

        if (intKeyFlag)
        {
            LLBC_STREAM_WRITE(elem->GetIntKey());
        }
        else
        {
            !extended ? s.Write(
                *elem->GetStrKey()) : s.WriteEx(*elem->GetStrKey());
        }

        !extended ? s.Write(*it) : s.WriteEx(*it);
    }

    LLBC_STREAM_END_WRITE();
}

bool LLBC_Dictionary::DeSerializeInl(LLBC_Stream &s, bool extended)
{
    if (!_objFactory)
    {
        return false;
    }

    Clear();

    LLBC_STREAM_BEGIN_READ(s, bool, false);

    uint64 bucketSize = 0;
    LLBC_STREAM_READ(bucketSize);
    SetHashBucketSize(static_cast<size_type>(bucketSize));

    uint64 size = 0;
    LLBC_STREAM_READ(size);
    for (uint64 i = 0; i < size; i++)
    {
        uint8 intKeyFlag = 0;
        LLBC_STREAM_READ(intKeyFlag);

        if (intKeyFlag)
        {
            int intKey = 0;
            LLBC_STREAM_READ(intKey);

            LLBC_Object *o = _objFactory->CreateObject();
            if (!(!extended ? s.Read(*o) : s.ReadEx(*o)))
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
            if (!(!extended ? s.Read(*o) : s.ReadEx(*o)))
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

#include "llbc/common/AfterIncl.h"
