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

#include "llbc/core/os/OS_Console.h"
#include "llbc/core/utils/Util_Debug.h"

#include "llbc/core/objbase/Object.h"
#include "llbc/core/objbase/KeyHashAlgorithm.h"
#include "llbc/core/objbase/DictionaryElem.h"

__LLBC_INTERNAL_NS_BEGIN

static LLBC_NS LLBC_String __emptyStrKey;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_DictionaryElem::LLBC_DictionaryElem(int key, LLBC_Object *o)
: _intKey(key)
, _strKey(nullptr)
, _hash(0)

, _obj(o)

, _bucket(nullptr)
, _bucketSize(0)

, _prev(nullptr)
, _next(nullptr)

, _bucketPrev(nullptr)
, _bucketNext(nullptr)
{
    o->Retain();
}

LLBC_DictionaryElem::LLBC_DictionaryElem(const LLBC_String &key, LLBC_Object *o)
: _intKey(0)
, _strKey(new LLBC_String(key))
, _hash(0)

, _obj(o)

, _bucket(nullptr)
, _bucketSize(0)

, _prev(nullptr)
, _next(nullptr)

, _bucketPrev(nullptr)
, _bucketNext(nullptr)
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

const int &LLBC_DictionaryElem::GetIntKey() const
{
    return _intKey;
}

const LLBC_String &LLBC_DictionaryElem::GetStrKey() const
{
    return _strKey ? *_strKey : LLBC_INL_NS __emptyStrKey;
}

uint32 LLBC_DictionaryElem::GetHashValue() const
{
    return _hash;
}

LLBC_Object *&LLBC_DictionaryElem::GetObject()
{
    return _obj;
}

LLBC_Object * const &LLBC_DictionaryElem::GetObject() const
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
        _hash = LLBC_Hash(_strKey->c_str(), 
            _strKey->size()) % static_cast<uint32>(_bucketSize);
    }

    // Link to hash bucket.
    SetBucketElemPrev(nullptr);
    LLBC_DictionaryElem *&hashed = _bucket[_hash];
    if(!hashed)
    {
        SetBucketElemNext(nullptr);
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
        for(; countElem != nullptr; countElem = countElem->GetBucketElemNext())
        {
            confictCount += 1;
        }

        trace("Dictionary(addr:%p), key confict!, bucket:%u, count:%d\n", this, _hash, confictCount);
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

LLBC_Object * const &LLBC_DictionaryElem::operator *() const
{
    return _obj;
}

__LLBC_NS_END
