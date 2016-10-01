/**
 * @file    DictionaryElem.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/27
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/common/Config.h"

#if LLBC_CFG_OBJBASE_ENABLED

#include "llbc/objbase/Object.h"
#include "llbc/objbase/KeyHashAlgorithm.h"
#include "llbc/objbase/DictionaryElem.h"

__LLBC_NS_BEGIN

LLBC_DictionaryElem::LLBC_DictionaryElem(int key, LLBC_Object *o)
: _intKey(key)
, _strKey(NULL)
, _hash(0)

, _obj(o)

, _bucket(NULL)
, _bucketSize(0)

, _prev(NULL)
, _next(NULL)

, _bucketPrev(NULL)
, _bucketNext(NULL)

, _hashFun(*LLBC_KeyHashAlgorithmSingleton->GetAlgorithm(LLBC_CFG_OBJBASE_DICT_KEY_HASH_ALGO))
{
    o->Retain();
}

LLBC_DictionaryElem::LLBC_DictionaryElem(const LLBC_String &key, LLBC_Object *o)
: _intKey(0)
, _strKey(new LLBC_String(key))
, _hash(0)

, _obj(o)

, _bucket(NULL)
, _bucketSize(0)

, _prev(NULL)
, _next(NULL)

, _bucketPrev(NULL)
, _bucketNext(NULL)

, _hashFun(*LLBC_KeyHashAlgorithmSingleton->GetAlgorithm(LLBC_CFG_OBJBASE_DICT_KEY_HASH_ALGO))
{
    o->Retain();
}

LLBC_DictionaryElem::~LLBC_DictionaryElem()
{
    LLBC_XDelete(_strKey);
    _obj->Release();
}

bool LLBC_DictionaryElem::IsIntKey() const
{
    return !_strKey;
}

bool LLBC_DictionaryElem::IsStrKey() const
{
    return !!_strKey;
}

int LLBC_DictionaryElem::GetIntKey() const
{
    return _intKey;
}

const LLBC_String *LLBC_DictionaryElem::GetStrKey() const
{
    return _strKey;
}

uint32 LLBC_DictionaryElem::GetHashValue() const
{
    return _hash;
}

LLBC_Object *&LLBC_DictionaryElem::GetObject()
{
    return _obj;
}

const LLBC_Object *LLBC_DictionaryElem::GetObject() const
{
    return _obj;
}

void LLBC_DictionaryElem::Hash(LLBC_DictionaryElem **bucket, size_t bucketSize)
{
    _bucket = bucket;
    _bucketSize = bucketSize;

    // Generate hash key.
    if(IsIntKey())
    {
        _hash = _intKey % _bucketSize;
    }
    else
    {
        _hash = _hashFun(_strKey->c_str(), 
            _strKey->size()) % static_cast<uint32>(_bucketSize);
    }

    // Link to hash bucket.
    SetBucketElemPrev(NULL);
    LLBC_DictionaryElem *&hashed = _bucket[_hash];
    if(!hashed)
    {
        SetBucketElemNext(NULL);
        hashed = this;
    }
    else
    {
        hashed->SetBucketElemPrev(this);
        SetBucketElemNext(hashed);
        hashed = this;

#ifdef LLBC_DEBUG
        int confictCount = 0;
        LLBC_DictionaryElem *countElem = hashed;
        for(; countElem != NULL; countElem = countElem->GetBucketElemNext())
        {
            confictCount += 1;
        }

        trace("Dictionary(addr: %x), key confict!, bucket: %d, count: %d\n", this, _hash, confictCount);
#endif
    }
}

void LLBC_DictionaryElem::CancelHash()
{
    if(_bucket[_hash] == this)
    {
        _bucket[_hash] = GetBucketElemNext();
    }

    if(GetBucketElemPrev())
    {
        GetBucketElemPrev()->
            SetBucketElemNext(GetBucketElemNext());
    }
    if(GetBucketElemNext())
    {
        GetBucketElemNext()->
            SetBucketElemPrev(GetBucketElemPrev());
    }
}

LLBC_DictionaryElem **LLBC_DictionaryElem::GetBucket()
{
    return _bucket;
}

size_t LLBC_DictionaryElem::GetBucketSize() const
{
    return _bucketSize;
}

LLBC_DictionaryElem *LLBC_DictionaryElem::GetElemPrev()
{
    return _prev;
}

const LLBC_DictionaryElem *LLBC_DictionaryElem::GetElemPrev() const
{
    return _prev;
}

void LLBC_DictionaryElem::SetElemPrev(LLBC_DictionaryElem *prev)
{
    _prev = prev;
}

LLBC_DictionaryElem *LLBC_DictionaryElem::GetElemNext()
{
    return _next;
}

const LLBC_DictionaryElem *LLBC_DictionaryElem::GetElemNext() const
{
    return _next;
}

void LLBC_DictionaryElem::SetElemNext(LLBC_DictionaryElem *next)
{
    _next = next;
}

LLBC_DictionaryElem *LLBC_DictionaryElem::GetBucketElemPrev()
{
    return _bucketPrev;
}

const LLBC_DictionaryElem *LLBC_DictionaryElem::GetBucketElemPrev() const
{
    return _bucketPrev;
}

void LLBC_DictionaryElem::SetBucketElemPrev(LLBC_DictionaryElem *prev)
{
    _bucketPrev = prev;
}

LLBC_DictionaryElem *LLBC_DictionaryElem::GetBucketElemNext()
{
    return _bucketNext;
}

const LLBC_DictionaryElem *LLBC_DictionaryElem::GetBucketElemNext() const
{
    return _bucketNext;
}

void LLBC_DictionaryElem::SetBucketElemNext(LLBC_DictionaryElem *next)
{
    _bucketNext = next;
}

LLBC_Object *&LLBC_DictionaryElem::operator *()
{
    return _obj;
}

const LLBC_Object *LLBC_DictionaryElem::operator *() const
{
    return _obj;
}

__LLBC_NS_END

#endif // LLBC_CFG_OBJBASE_ENABLED

#include "llbc/common/AfterIncl.h"
