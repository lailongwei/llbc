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
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/comstring/ComString.h"

#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/variant/VariantTraits.h"
#include "llbc/core/variant/Variant.h"

namespace
{
    typedef LLBC_NS LLBC_Variant::Str Str;

    typedef LLBC_NS LLBC_Variant::Seq Seq;
    typedef LLBC_NS LLBC_Variant::SeqIter SeqIter;
    typedef LLBC_NS LLBC_Variant::SeqConstIter SeqConstIter;
    typedef LLBC_NS LLBC_Variant::SeqReverseIter SeqReverseIter;
    typedef LLBC_NS LLBC_Variant::SeqConstReverseIter SeqConstReverseIter;

    typedef LLBC_NS LLBC_Variant::Dict Dict;
    typedef LLBC_NS LLBC_Variant::DictIter DictIter;
    typedef LLBC_NS LLBC_Variant::DictConstIter DictConstIter;
    typedef LLBC_NS LLBC_Variant::DictReverseIter DictReverseIter;
    typedef LLBC_NS LLBC_Variant::DictConstReverseIter DictConstReverseIter;
}

__LLBC_INTERNAL_NS_BEGIN

static const Str __g_nullStr;
static const Str __g_trueStr = "true";
static const Seq __g_nullSeq;
static const Dict __g_nullDict;
static const LLBC_NS LLBC_Variant __g_nilVariant;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

const LLBC_Variant LLBC_Variant::nil;

std::map<int, LLBC_String> LLBC_VariantType::_typeDescs;

const LLBC_String &LLBC_VariantType::Type2Str(int type)
{
    std::map<int, LLBC_String>::const_iterator it = _typeDescs.find(type);
    return (it != _typeDescs.end()) ? it->second : _typeDescs[VT_NIL];
}

void LLBC_VariantType::InitType2StrDict()
{
    if (_typeDescs.empty())
    {
        _typeDescs.insert(std::make_pair(VT_RAW_BOOL, "bool"));

        _typeDescs.insert(std::make_pair(VT_RAW_SINT8, "int8"));
        _typeDescs.insert(std::make_pair(VT_RAW_UINT8, "uint8"));
        _typeDescs.insert(std::make_pair(VT_RAW_SINT16, "sint16"));
        _typeDescs.insert(std::make_pair(VT_RAW_UINT16, "uint16"));
        _typeDescs.insert(std::make_pair(VT_RAW_SINT32, "int32"));
        _typeDescs.insert(std::make_pair(VT_RAW_UINT32, "uint32"));
        _typeDescs.insert(std::make_pair(VT_RAW_LONG, "long"));
        _typeDescs.insert(std::make_pair(VT_RAW_ULONG, "ulong"));
        _typeDescs.insert(std::make_pair(VT_RAW_PTR, "ptr"));
        _typeDescs.insert(std::make_pair(VT_RAW_SINT64, "int64"));
        _typeDescs.insert(std::make_pair(VT_RAW_UINT64, "uint64"));
        _typeDescs.insert(std::make_pair(VT_RAW_FLOAT, "float"));
        _typeDescs.insert(std::make_pair(VT_RAW_DOUBLE, "double"));

        _typeDescs.insert(std::make_pair(VT_STR_DFT, "string"));

        _typeDescs.insert(std::make_pair(VT_SEQ_DFT, "sequence"));

        _typeDescs.insert(std::make_pair(VT_DICT_DFT, "dictionary"));

        _typeDescs.insert(std::make_pair(VT_NIL, "Nil"));
    }
}

__LLBC_NS_END

std::ostream &operator <<(std::ostream &o, const LLBC_NAMESPACE LLBC_Variant &variant)
{
    LLBC_NAMESPACE LLBC_String str = variant.ToString();
    o.write(str.c_str(), str.length());
    return o;
}

__LLBC_NS_BEGIN

LLBC_Variant::Holder::Holder()
: type(LLBC_VariantType::VT_NIL)
{
    raw.uint64Val = 0;
    obj.str = NULL;
}

LLBC_Variant::Holder::~Holder()
{
    if (type == LLBC_VariantType::VT_STR_DFT)
        LLBC_Delete(obj.str);
    else if (type == LLBC_VariantType::VT_DICT_DFT)
        LLBC_Delete(obj.dict);
    else if (type == LLBC_VariantType::VT_SEQ_DFT)
        LLBC_Delete(obj.seq);
}

LLBC_Variant::LLBC_Variant(const char *cstrVal)
{
    _holder.type = LLBC_VariantType::VT_STR_DFT;
    if (cstrVal != NULL)
    {
        size_t strLen = LLBC_StrLenA(cstrVal);
        if (strLen != 0)
            _holder.obj.str = LLBC_New(LLBC_String, cstrVal, strLen);
    }
}

LLBC_Variant::LLBC_Variant(const LLBC_Variant &varVal)
{
    LLBC_VariantTraits::assign(*this, varVal);
}

bool LLBC_Variant::AsBool() const
{
    if (IsNil())
    {
        return false;
    }
    else if (IsDict())
    {
        return false;
    }
    else if (IsSeq())
    {
        return false;
    }
    else if (IsStr())
    {
        const Str *str= _holder.obj.str;
        if (!str || str->empty())
            return false;

        const Str trimedData = str->strip();
        if (trimedData.size() == 4 && trimedData.isalpha())
            return trimedData.tolower() == LLBC_INL_NS __g_trueStr;
        else
            return AsUInt64() != 0;
    }

    return _holder.raw.uint64Val != 0;
}

sint64 LLBC_Variant::AsInt64() const
{
    if (IsNil())
        return 0;
    else if (IsDict())
        return 0;
    else if (IsSeq())
        return 0;
    else if (IsStr())
        return (_holder.obj.str && !_holder.obj.str->empty()) ? LLBC_Str2Int64(_holder.obj.str->c_str()) : 0;

    if (IsDouble() || IsFloat())
        return static_cast<sint64>(_holder.raw.doubleVal);

    return _holder.raw.int64Val;
}

uint64 LLBC_Variant::AsUInt64() const
{
    if (IsNil())
        return 0;
    else if (IsDict())
        return 0;
    else if (IsSeq())
        return 0;
    else if (IsStr())
        return (_holder.obj.str && !_holder.obj.str->empty()) ? LLBC_Str2UInt64(_holder.obj.str->c_str()) : 0;

    if (IsDouble() || IsFloat())
        return static_cast<uint64>(_holder.raw.doubleVal);

    return _holder.raw.uint64Val;
}

double LLBC_Variant::AsDouble() const
{
    if (IsNil())
        return 0.0;
    else if (IsDict())
        return 0.0;
    else if (IsSeq())
        return 0.0;
    else if (IsStr())
        return (_holder.obj.str && !_holder.obj.str->empty()) ? LLBC_Str2Double(_holder.obj.str->c_str()) : 0;

    if (IsDouble() || IsFloat())
        return _holder.raw.doubleVal;

    if (IsSignedRaw())
        return static_cast<double>(_holder.raw.int64Val);

    return static_cast<double>(_holder.raw.uint64Val);
}

LLBC_String LLBC_Variant::AsStr() const
{
    if (IsNil())
    {
        return LLBC_INL_NS __g_nullStr;
    }
    else if (IsRaw())
    {
        if (IsBool())
            return _holder.raw.uint64Val ? "true" : "false";
        else if (IsFloat() || IsDouble())
            return LLBC_Num2Str(_holder.raw.doubleVal);
        else if (IsSignedRaw())
            return LLBC_Num2Str(_holder.raw.int64Val);
        else
            return LLBC_Num2Str(_holder.raw.uint64Val);
    }
    else if (IsStr())
    {
        return _holder.obj.str ? *_holder.obj.str : LLBC_INL_NS __g_nullStr;
    }
    else
    {
        return LLBC_INL_NS __g_nullStr;
    }
}

const Seq &LLBC_Variant::AsSeq() const
{
    if (IsSeq())
        return *_holder.obj.seq;

    return LLBC_INL_NS __g_nullSeq;
}

const Dict &LLBC_Variant::AsDict() const
{
    if (IsDict())
        return *_holder.obj.dict;

    return LLBC_INL_NS __g_nullDict;
}

void LLBC_Variant::Clear()
{
    if (IsRaw())
        _holder.raw.uint64Val = 0;
    else if (IsStr() && _holder.obj.str)
        _holder.obj.str->clear();
    else if (IsSeq())
        _holder.obj.seq->clear();
    else if (IsDict())
        _holder.obj.dict->clear();
}

bool LLBC_Variant::IsEmpty() const
{
    if (IsStr())
        return _holder.obj.str ? _holder.obj.str->empty() : true;
    else if (IsSeq())
        return _holder.obj.seq->empty();
    else if (IsDict())
        return _holder.obj.dict->empty();
    else
        return true;
}

size_t LLBC_Variant::Size() const
{
    if (IsStr())
        return _holder.obj.str ? _holder.obj.str->size() : 0;
    else if (IsSeq())
        return _holder.obj.seq->size();
    else if (IsDict())
        return _holder.obj.dict->size();
    else
        return 0;

}

size_t LLBC_Variant::Capacity() const
{
    if (IsStr())
        return _holder.obj.str ? _holder.obj.str->capacity() : 0;
    else if (IsSeq())
        return _holder.obj.seq->capacity();
    else
        return 0;
}

SeqIter LLBC_Variant::SeqBegin()
{
    BecomeSeq();
    return _holder.obj.seq->begin();
}

SeqIter LLBC_Variant::SeqEnd()
{
    BecomeSeq();
    return _holder.obj.seq->end();
}

SeqConstIter LLBC_Variant::SeqBegin() const
{
    const_cast<LLBC_Variant *>(this)->BecomeSeq();
    return _holder.obj.seq->begin();
}

SeqConstIter LLBC_Variant::SeqEnd() const
{
    const_cast<LLBC_Variant *>(this)->BecomeSeq();
    return _holder.obj.seq->end();
}

SeqReverseIter LLBC_Variant::SeqReverseBegin()
{
    BecomeSeq();
    return _holder.obj.seq->rbegin();
}

SeqReverseIter LLBC_Variant::SeqReverseEnd()
{
    BecomeSeq();
    return _holder.obj.seq->rend();
}

SeqConstReverseIter LLBC_Variant::SeqReverseBegin() const
{
    const_cast<LLBC_Variant *>(this)->BecomeSeq();
    return _holder.obj.seq->rbegin();
}

SeqConstReverseIter LLBC_Variant::SeqReverseEnd() const
{
    const_cast<LLBC_Variant *>(this)->BecomeSeq();
    return _holder.obj.seq->rend();
}

Seq::reference LLBC_Variant::SeqFront()
{
    BecomeSeq();
    return _holder.obj.seq->front();
}

Seq::const_reference LLBC_Variant::SeqFront() const
{
    const_cast<LLBC_Variant *>(this)->BecomeSeq();
    return _holder.obj.seq->front();
}

Seq::reference LLBC_Variant::SeqBack()
{
    BecomeSeq();
    return _holder.obj.seq->back();
}

Seq::const_reference LLBC_Variant::SeqBack() const
{
    const_cast<LLBC_Variant *>(this)->BecomeSeq();
    return _holder.obj.seq->back();
}

SeqIter LLBC_Variant::SeqInsert(SeqIter it, const Seq::value_type &val)
{
    BecomeSeq();
    return _holder.obj.seq->insert(it, val);
}

void LLBC_Variant::SeqInsert(SeqIter it, Seq::size_type n, const Seq::value_type &val)
{
    BecomeSeq();
    _holder.obj.seq->insert(it, n, val);
}

void LLBC_Variant::SeqInsert(SeqIter it, SeqConstIter first, SeqConstIter last)
{
    BecomeSeq();
    _holder.obj.seq->insert(it, first, last);
}

void LLBC_Variant::SeqPushBack(const Seq::value_type &val)
{
    BecomeSeq();
    _holder.obj.seq->push_back(val);
}

void LLBC_Variant::SeqPopBack()
{
    BecomeSeq();
    if (!_holder.obj.seq->empty())
        _holder.obj.seq->pop_back();
}

void LLBC_Variant::SeqResize(Seq::size_type n, const Seq::value_type &val)
{
    BecomeSeq();
    _holder.obj.seq->resize(n, val);
}

void LLBC_Variant::SeqReserve(Seq::size_type n)
{
    BecomeSeq();
    _holder.obj.seq->reserve(n);
}

SeqIter LLBC_Variant::SeqErase(SeqIter it)
{
    BecomeSeq();
    return _holder.obj.seq->erase(it);
}

SeqIter LLBC_Variant::SeqErase(SeqIter first, SeqIter last)
{
    BecomeSeq();
    return _holder.obj.seq->erase(first, last);
}

void LLBC_Variant::SeqErase(const Seq::value_type &val)
{
    BecomeSeq();
    if (_holder.obj.seq->empty())
        return;

    SeqIter it;
    while ((it = std::find(_holder.obj.seq->begin(), _holder.obj.seq->end(), val)) != _holder.obj.seq->end())
        _holder.obj.seq->erase(it);
}

DictIter LLBC_Variant::DictBegin()
{
    BecomeDict();
    return _holder.obj.dict->begin();
}

DictIter LLBC_Variant::DictEnd()
{
    BecomeDict();
    return _holder.obj.dict->end();
}

DictConstIter LLBC_Variant::DictBegin() const
{
    const_cast<LLBC_Variant *>(this)->BecomeDict();
    return _holder.obj.dict->begin();
}

DictConstIter LLBC_Variant::DictEnd() const
{
    const_cast<LLBC_Variant *>(this)->BecomeDict();
    return _holder.obj.dict->end();
}

DictReverseIter LLBC_Variant::DictReverseBegin()
{
    BecomeDict();
    return _holder.obj.dict->rbegin();
}

DictReverseIter LLBC_Variant::DictReverseEnd()
{
    BecomeDict();
    return _holder.obj.dict->rend();
}

DictConstReverseIter LLBC_Variant::DictReverseBegin() const
{
    const_cast<LLBC_Variant *>(this)->BecomeDict();
    return _holder.obj.dict->rbegin();
}

DictConstReverseIter LLBC_Variant::DictReverseEnd() const
{
    const_cast<LLBC_Variant *>(this)->BecomeDict();
    return _holder.obj.dict->rend();
}

std::pair<DictIter, bool> LLBC_Variant::DictInsert(const Dict::key_type &key, const Dict::mapped_type &val)
{
    return DictInsert(Dict::value_type(key, val));
}

std::pair<DictIter, bool> LLBC_Variant::DictInsert(const Dict::value_type &val)
{
    BecomeDict();
    return _holder.obj.dict->insert(val);
}

DictIter LLBC_Variant::DictFind(const Dict::key_type &key)
{
    BecomeDict();
    return _holder.obj.dict->find(key);
}

DictConstIter LLBC_Variant::DictFind(const Dict::key_type &key) const
{
    const_cast<LLBC_Variant *>(this)->BecomeDict();
    return _holder.obj.dict->find(key);
}

void LLBC_Variant::DictErase(DictIter it)
{
    BecomeDict();
    _holder.obj.dict->erase(it);
}

Dict::size_type LLBC_Variant::DictErase(const Dict::key_type &key)
{
    BecomeDict();
    return _holder.obj.dict->erase(key);
}

void LLBC_Variant::DictErase(DictIter first, DictIter last)
{
    BecomeDict();
    _holder.obj.dict->erase(first, last);
}

LLBC_Variant &LLBC_Variant::operator [](const LLBC_Variant &key)
{
    if (IsSeq())
    {
        return (*_holder.obj.seq)[key];
    }
    else
    {
        BecomeDict();
        return (*_holder.obj.dict)[key];
    }
}

const LLBC_Variant &LLBC_Variant::operator [](const LLBC_Variant &key) const
{
    if (IsSeq())
    {
        return (*_holder.obj.seq)[key];
    }
    else if (IsDict())
    {
        DictConstIter it = _holder.obj.dict->find(key);
        if (it == _holder.obj.dict->end())
            return LLBC_INL_NS __g_nilVariant;

        return it->second;
    }

    return LLBC_INL_NS __g_nilVariant;
}

LLBC_Variant &LLBC_Variant::operator =(bool val)
{
    CleanTypeData(_holder.type);

    _holder.type = LLBC_VariantType::VT_RAW_BOOL;
    _holder.raw.int64Val = val ? 1 : 0;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(sint8 val)
{
    CleanTypeData(_holder.type);

    _holder.type = LLBC_VariantType::VT_RAW_SINT8;
    _holder.raw.int64Val = val;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(uint8 val)
{
    CleanTypeData(_holder.type);

    _holder.type = LLBC_VariantType::VT_RAW_UINT8;
    _holder.raw.uint64Val = val;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(sint16 val)
{
    CleanTypeData(_holder.type);

    _holder.type = LLBC_VariantType::VT_RAW_SINT16;
    _holder.raw.int64Val = val;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(uint16 val)
{
    CleanTypeData(_holder.type);

    _holder.type = LLBC_VariantType::VT_RAW_UINT16;
    _holder.raw.uint64Val = val;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(sint32 val)
{
    CleanTypeData(_holder.type);

    _holder.type = LLBC_VariantType::VT_RAW_SINT32;
    _holder.raw.int64Val = val;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(uint32 val)
{
    CleanTypeData(_holder.type);

    _holder.type = LLBC_VariantType::VT_RAW_UINT32;
    _holder.raw.uint64Val = val;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(long val)
{
    CleanTypeData(_holder.type);

    _holder.type = LLBC_VariantType::VT_RAW_LONG;
    _holder.raw.int64Val = val;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(ulong val)
{
    CleanTypeData(_holder.type);

    _holder.type = LLBC_VariantType::VT_RAW_ULONG;
    _holder.raw.uint64Val = val;

    return *this;
}

LLBC_Variant & LLBC_Variant::operator=(const char * const &val)
{
    if (!IsStr())
    {
        CleanTypeData(_holder.type);
        _holder.type = LLBC_VariantType::VT_STR_DFT;
    }

    const size_t len = val ? ::strlen(val) : 0;
    if (len == 0)
    {
        if (_holder.obj.str)
            _holder.obj.str->clear();
    }
    else
    {
        if (_holder.obj.str)
            _holder.obj.str->assign(val, len);
        else
            _holder.obj.str = LLBC_New(LLBC_String, val, len);
    }

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(const sint64 &val)
{
    CleanTypeData(_holder.type);

    _holder.type = LLBC_VariantType::VT_RAW_SINT64;
    _holder.raw.int64Val = val;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(const uint64 &val)
{
    CleanTypeData(_holder.type);

    _holder.type = LLBC_VariantType::VT_RAW_UINT64;
    _holder.raw.uint64Val = val;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(float val)
{
    CleanTypeData(_holder.type);

    _holder.type = LLBC_VariantType::VT_RAW_FLOAT;
    _holder.raw.doubleVal = val;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(const double &val)
{
    CleanTypeData(_holder.type);

    _holder.type = LLBC_VariantType::VT_RAW_DOUBLE;
    _holder.raw.doubleVal = val;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(const LLBC_String &val)
{
    if (!IsStr())
    {
        CleanTypeData(_holder.type);
        _holder.type = LLBC_VariantType::VT_STR_DFT;
    }

    if (val.empty())
    {
        if (_holder.obj.str)
            _holder.obj.str->clear();
    }
    else
    {
        if (_holder.obj.str)
            *_holder.obj.str = val;
        else
            _holder.obj.str = LLBC_New(LLBC_String, val);
    }

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(const Seq &val)
{
    if (!IsSeq())
        BecomeSeq();
    else if (_holder.obj.seq == &val)
        return *this;

    if (val.empty())
        _holder.obj.seq->clear();
    else
        *_holder.obj.seq = val;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(const Dict &val)
{
    if (!IsDict())
        BecomeDict();
    else if (_holder.obj.dict == &val)
        return *this;

    if (val.empty())
        _holder.obj.dict->clear();
    else
        *_holder.obj.dict = val;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(const LLBC_Variant &val)
{
    LLBC_VariantTraits::assign(*this, val);
    return *this;
}

bool LLBC_Variant::operator ==(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::eq(*this, another);
}

bool LLBC_Variant::operator !=(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::ne(*this, another);
}

bool LLBC_Variant::operator <(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::lt(*this, another);
}

bool LLBC_Variant::operator >(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::gt(*this, another);
}

bool LLBC_Variant::operator <=(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::le(*this, another);
}

bool LLBC_Variant::operator >=(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::ge(*this, another);
}

LLBC_Variant LLBC_Variant::operator +(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::add(*this, another);
}

LLBC_Variant LLBC_Variant::operator -(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::sub(*this, another);
}

LLBC_Variant LLBC_Variant::operator *(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::mul(*this, another);
}

LLBC_Variant LLBC_Variant::operator /(const LLBC_Variant &another) const
{
    return LLBC_VariantTraits::div(*this, another);
}

LLBC_Variant &LLBC_Variant::operator +=(const LLBC_Variant &another)
{
    LLBC_VariantTraits::add_equal(*this, another);
    return *this;
}

LLBC_Variant &LLBC_Variant::operator -=(const LLBC_Variant &another)
{
    LLBC_VariantTraits::sub_equal(*this, another);
    return *this;
}

LLBC_Variant &LLBC_Variant::operator *=(const LLBC_Variant &another)
{
    LLBC_VariantTraits::mul_equal(*this, another);
    return *this;
}

LLBC_Variant &LLBC_Variant::operator /=(const LLBC_Variant &another)
{
    LLBC_VariantTraits::div_equal(*this, another);
    return *this;
}

const LLBC_String &LLBC_Variant::TypeToString() const
{
    return LLBC_VariantType::Type2Str(_holder.type);
}

LLBC_String LLBC_Variant::ValueToString() const
{
    if (IsStr())
    {
        return _holder.obj.str ? *_holder.obj.str : LLBC_INL_NS __g_nullStr;
    }
    else if (IsSeq())
    {
        LLBC_String content;
        content.append("[");

        for (SeqConstIter it = _holder.obj.seq->begin();
             it != _holder.obj.seq->end();
             )
        {
            content.append((*it).ValueToString());
            if (++it != _holder.obj.seq->end())
                content.append("|");
        }

        content.append("]");
        return content;
    }
    else if (IsDict())
    {

        LLBC_String content;
        content.append("{");

        for (DictConstIter it = _holder.obj.dict->begin();
             it != _holder.obj.dict->end();
             )
        {
            content.append(it->first.ValueToString());
            content.append(":");
            content.append(it->second.ValueToString());

            if (++it != _holder.obj.dict->end())
                content.append("|");
        }

        content.append("}");
        return content;
    }
    else if (IsNil())
    {
        return "nil";
    }

    // RAW type var data.
    switch(_holder.type)
    {
    case LLBC_VariantType::VT_RAW_BOOL:
        return _holder.raw.int64Val != 0 ? "true" : "false";

    case LLBC_VariantType::VT_RAW_SINT8:
    case LLBC_VariantType::VT_RAW_SINT16:
    case LLBC_VariantType::VT_RAW_SINT32:
    case LLBC_VariantType::VT_RAW_LONG:
    case LLBC_VariantType::VT_RAW_SINT64:
        return LLBC_Num2Str(_holder.raw.int64Val);

    case LLBC_VariantType::VT_RAW_UINT8:
    case LLBC_VariantType::VT_RAW_UINT16:
    case LLBC_VariantType::VT_RAW_UINT32:
    case LLBC_VariantType::VT_RAW_ULONG:
    case LLBC_VariantType::VT_RAW_UINT64:
        return LLBC_Num2Str(_holder.raw.uint64Val);

    case LLBC_VariantType::VT_RAW_PTR:
        return LLBC_String().format("0x%p", _holder.raw.uint64Val);

    case LLBC_VariantType::VT_RAW_FLOAT:
    case LLBC_VariantType::VT_RAW_DOUBLE:
        return LLBC_Num2Str(_holder.raw.doubleVal);

    default:
        break;
    }

    return "";
}

LLBC_String LLBC_Variant::ToString() const
{
    LLBC_String desc = "type:";
    desc += TypeToString();
    desc += ", value:";
    desc += ValueToString();
    return desc;
}

void LLBC_Variant::Serialize(LLBC_Stream &stream) const
{
    stream.Write(_holder.type);

    if (IsRaw())
    {
        stream.Write(_holder.raw.uint64Val);
    }
    else if (IsStr())
    {
        stream.WriteEx(_holder.obj.str ? *_holder.obj.str : LLBC_INL_NS __g_nullStr);
    }
    else if (IsSeq())
    {
        stream.Write(static_cast<uint32>(_holder.obj.seq->size()));
        for (SeqConstIter it = _holder.obj.seq->begin();
             it != _holder.obj.seq->end();
             ++it)
        {
            stream.Write(*it);
        }
    }
    else if (IsDict())
    {
        stream.Write(static_cast<uint32>(_holder.obj.dict->size()));
        for (DictConstIter it = _holder.obj.dict->begin();
             it != _holder.obj.dict->end();
             ++it)
        {
            stream.Write(it->first);
            stream.Write(it->second);
        }
    }
}

bool LLBC_Variant::DeSerialize(LLBC_Stream &stream)
{
    BecomeNil();

    if (!stream.Read(_holder.type))
        return false;

    if (IsNil())
        return true;

    if (IsRaw())
    {
        if (!stream.Read(_holder.raw.uint64Val))
        {
            _holder.type = LLBC_VariantType::VT_NIL;
            return false;
        }

        return true;
    }
    else if (IsStr())
    {
        if (!_holder.obj.str)
            _holder.obj.str = LLBC_New(LLBC_String);

        if (!stream.ReadEx(*_holder.obj.str))
        {
            LLBC_XDelete(_holder.obj.str);
            _holder.type = LLBC_VariantType::VT_NIL;

            return false;
        }

        return true;
    }
    else if (IsSeq())
    {
        uint32 count = 0;
        if (!stream.Read(count))
        {
            _holder.type = LLBC_VariantType::VT_NIL;
            return false;
        }

        _holder.obj.seq = LLBC_New(Seq);
        if (count == 0)
            return true;

        for (uint32 i = 0; i < count; ++i)
        {
            LLBC_Variant val;
            if (!stream.Read(val))
            {
                LLBC_XDelete(_holder.obj.seq);
                _holder.type = LLBC_VariantType::VT_NIL;
                return false;
            }

            _holder.obj.seq->push_back(val);
        }

        return true;
    }
    else if (IsDict())
    {
        uint32 count = 0;
        if (!stream.Read(count))
        {
            _holder.type = LLBC_VariantType::VT_NIL;
            return false;
        }

        _holder.obj.dict = LLBC_New(Dict);
        if (count == 0)
            return true;

        for (uint32 i = 0; i < count; ++i)
        {
            LLBC_Variant key;
            LLBC_Variant val;
            if (!stream.Read(key) || !stream.Read(val))
            {
                LLBC_XDelete(_holder.obj.dict);
                _holder.type = LLBC_VariantType::VT_NIL;
                return false;
            }

            _holder.obj.dict->insert(std::make_pair(key, val));
        }

        return true;
    }
    
    return false;
}

void LLBC_Variant::SerializeEx(LLBC_Stream &stream) const
{
    Serialize(stream);
}

bool LLBC_Variant::DeSerializeEx(LLBC_Stream &stream)
{
    return DeSerialize(stream);
}

void LLBC_Variant::SetType(int type)
{
    _holder.type = static_cast<LLBC_VariantType::ENUM>(type);
}

LLBC_Variant::Holder &LLBC_Variant::GetHolder()
{
    return _holder;
}

void LLBC_Variant::CleanRawData()
{
    _holder.raw.uint64Val = 0;
}

void LLBC_Variant::CleanStrData()
{
    LLBC_XDelete(_holder.obj.str);
}

void LLBC_Variant::CleanSeqData()
{
    LLBC_XDelete(_holder.obj.seq);
}

void LLBC_Variant::CleanDictData()
{
    LLBC_XDelete(_holder.obj.dict);
}

void LLBC_Variant::CleanTypeData(int type)
{
    typedef LLBC_VariantType _Type;

    const int topType = (type & MASK_FIRST);
    switch(topType)
    {
    case _Type::VT_RAW:
        CleanRawData();
        break;

    case _Type::VT_STR:
        CleanStrData();
        break;

    case _Type::VT_SEQ:
        CleanSeqData();
        break;

    case _Type::VT_DICT:
        CleanDictData();
        break;

    default:
        break;
    }
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
