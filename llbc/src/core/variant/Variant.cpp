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

#include <float.h>

#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/variant/Variant.h"

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_Variant::Str __g_nullStr;
static const LLBC_NS LLBC_Variant::Str __g_nilStr = "nil";
static const LLBC_NS LLBC_Variant::Str __g_trueStr = "true";
static const LLBC_NS LLBC_Variant::Str __g_falseStr = "false";
static const LLBC_NS LLBC_Variant::Str __g_yesStr = "yes";
static const LLBC_NS LLBC_Variant::Str __g_emptySeqStr = "[]";
static const LLBC_NS LLBC_Variant::Str __g_emptyDictStr = "{}";
static const LLBC_NS LLBC_Variant::Seq __g_emptySeq;
static const LLBC_NS LLBC_Variant::Dict __g_emptyDict;

static const LLBC_NS LLBC_Variant __g_nilVariant;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

const LLBC_Variant LLBC_Variant::nil;

std::map<LLBC_VariantType::ENUM, LLBC_String> LLBC_VariantType::_typeDescs;

const LLBC_String &LLBC_VariantType::Type2Str(int type)
{
    std::map<ENUM, LLBC_String>::const_iterator it = _typeDescs.find(static_cast<ENUM>(type));
    return (it != _typeDescs.end()) ? it->second : _typeDescs[VT_NIL];
}

void LLBC_VariantType::InitType2StrDict()
{
    if (_typeDescs.empty())
    {
        _typeDescs.insert(std::make_pair(VT_NIL, "Nil"));

        _typeDescs.emplace(VT_RAW_BOOL, "bool");
        _typeDescs.emplace(VT_RAW_SINT8, "int8");
        _typeDescs.emplace(VT_RAW_UINT8, "uint8");
        _typeDescs.emplace(VT_RAW_SINT16, "sint16");
        _typeDescs.emplace(VT_RAW_UINT16, "uint16");
        _typeDescs.emplace(VT_RAW_SINT32, "int32");
        _typeDescs.emplace(VT_RAW_UINT32, "uint32");
        _typeDescs.emplace(VT_RAW_LONG, "long");
        _typeDescs.emplace(VT_RAW_ULONG, "ulong");
        _typeDescs.emplace(VT_RAW_PTR, "ptr");
        _typeDescs.emplace(VT_RAW_SINT64, "int64");
        _typeDescs.emplace(VT_RAW_UINT64, "uint64");
        _typeDescs.emplace(VT_RAW_FLOAT, "float");
        _typeDescs.emplace(VT_RAW_DOUBLE, "double");

        _typeDescs.emplace(VT_STR_DFT, "string");

        _typeDescs.emplace(VT_SEQ_DFT, "sequence");

        _typeDescs.emplace(VT_DICT_DFT, "dictionary");
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
, data()
{
}

LLBC_Variant::Holder::~Holder()
{
    Clear();
}

LLBC_VariantType::ENUM LLBC_Variant::Holder::GetFirstType() const
{
    return static_cast<LLBC_VariantType::ENUM>(type & MASK_FIRST);
}

void LLBC_Variant::Holder::Clear()
{
    ClearData();
    type = LLBC_VariantType::VT_NIL;
}

void LLBC_Variant::Holder::ClearData()
{
    if (data.raw.int64Val == 0)
        return;

    if (type == LLBC_VariantType::VT_STR_DFT)
        delete data.obj.str;
    else if (type == LLBC_VariantType::VT_SEQ_DFT)
        delete data.obj.seq;
    else if (type == LLBC_VariantType::VT_DICT_DFT)
        delete data.obj.dict;

    data.raw.int64Val = 0;
}

LLBC_Variant::Str **LLBC_Variant::_num2StrFastAccessTbl = nullptr;

void LLBC_Variant::InitNumber2StrFastAccessTable()
{
    if (UNLIKELY(_num2StrFastAccessTbl))
        return;

    char buf[32];
    constexpr int cap =
        LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_END - LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN + 1;
    _num2StrFastAccessTbl = LLBC_Malloc(Str *, sizeof(Str *) * cap);
    for (int i = 0; i < cap; ++i)
    {
        #if LLBC_TARGET_PLATFORM_WIN32
        _itoa_s(i + LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN, buf, 10);
        #else
        snprintf(buf, sizeof(buf), "%d", i + LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN);
        #endif

        _num2StrFastAccessTbl[i] = new Str(buf);
    }
}

void LLBC_Variant::DestroyNumber2StrFastAccessTable()
{
    if (UNLIKELY(!_num2StrFastAccessTbl))
        return;

    constexpr int cap =
        LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_END - LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN + 1;
    for (int i = 0; i < cap; ++i)
        delete _num2StrFastAccessTbl[i];

    LLBC_XFree(_num2StrFastAccessTbl);
}

LLBC_Variant::LLBC_Variant(const char *str)
{
    _holder.type = LLBC_VariantType::VT_STR_DFT;
    if (str)
    {
        const size_t strLen = strlen(str);
        if (strLen != 0)
            _holder.data.obj.str = new Str(str, strLen);
    }
}

LLBC_Variant::LLBC_Variant(const LLBC_Variant &var)
{
    LLBC_VariantTraits::assign(*this, var);
}

LLBC_Variant::LLBC_Variant(LLBC_Variant &&var)
: _holder(var._holder)
{
    var._holder.type = LLBC_VariantType::VT_NIL;
    var._holder.data.raw.uint64Val = 0;
}

bool LLBC_Variant::AsBool() const
{
    const LLBC_VariantType::ENUM firstType = GetFirstType();
    if (firstType == LLBC_VariantType::VT_RAW)
    {
        if (IsFloat() || IsDouble())
            return std::fabs(AsDouble()) >= DBL_EPSILON;

        return _holder.data.raw.uint64Val != 0;
    }
    if (firstType == LLBC_VariantType::VT_STR)
    {
        return _holder.data.obj.str && !_holder.data.obj.str->empty();
    }
    if (firstType == LLBC_VariantType::VT_SEQ)
    {
        return _holder.data.obj.seq && !_holder.data.obj.seq->empty();
    }
    if (firstType == LLBC_VariantType::VT_DICT)
    {
        return _holder.data.obj.dict && !_holder.data.obj.dict->empty();
    }

    return false;
}

bool LLBC_Variant::AsLooseBool() const
{
    if (GetFirstType() == LLBC_VariantType::VT_STR)
    {
        const Str * const &str = _holder.data.obj.str;
        if (!str || str->empty())
            return false;

        const Str nmlStr = str->strip().tolower();
        if (nmlStr == LLBC_INL_NS __g_trueStr || nmlStr == LLBC_INL_NS __g_yesStr)
            return true;
        else if (nmlStr.find('.') != Str::npos)
            return std::fabs(LLBC_Str2Double(nmlStr.c_str())) > DBL_EPSILON;
        else
            return AsInt64() != 0;
    }

    return AsBool();
}

sint64 LLBC_Variant::AsInt64() const
{
    const LLBC_VariantType::ENUM firstType = GetFirstType();
    if (firstType == LLBC_VariantType::VT_NIL ||
        firstType == LLBC_VariantType::VT_SEQ ||
        firstType == LLBC_VariantType::VT_DICT)
        return 0;

    if (firstType == LLBC_VariantType::VT_STR)
    {
        const Str * const &str = _holder.data.obj.str;
        if (!str || str->empty())
            return 0;

        if (str->find('.'))
            return static_cast<sint64>(LLBC_Str2Double(str->c_str()));
        else
            return LLBC_Str2Int64(str->c_str());
    }

    if (_holder.type == LLBC_VariantType::VT_RAW_FLOAT ||
        _holder.type == LLBC_VariantType::VT_RAW_DOUBLE)
    {
        if (std::fabs(_holder.data.raw.doubleVal) <= DBL_EPSILON)
            return 0;

        return static_cast<sint64>(_holder.data.raw.doubleVal);
    }

    return _holder.data.raw.int64Val;
}

uint64 LLBC_Variant::AsUInt64() const
{
    return static_cast<uint64>(AsInt64());
}

double LLBC_Variant::AsDouble() const
{
    const LLBC_VariantType::ENUM firstType = GetFirstType();
    if (firstType == LLBC_VariantType::VT_NIL ||
        firstType == LLBC_VariantType::VT_SEQ ||
        firstType == LLBC_VariantType::VT_DICT)
        return 0.0;

    if (firstType == LLBC_VariantType::VT_STR)
        return (_holder.data.obj.str && !_holder.data.obj.str->empty()) ? 
                LLBC_Str2Double(_holder.data.obj.str->c_str()) : 0.0;

    if (firstType == LLBC_VariantType::VT_RAW)
    {
        if (_holder.type == LLBC_VariantType::VT_RAW_FLOAT ||
            _holder.type == LLBC_VariantType::VT_RAW_DOUBLE)
            return _holder.data.raw.doubleVal;

        if (IsSignedRaw())
            return static_cast<double>(_holder.data.raw.int64Val);
        else
            return static_cast<double>(_holder.data.raw.uint64Val);
    }

    return 0.0;
}

LLBC_String LLBC_Variant::AsStr() const
{
    if (IsRaw())
    {
        if (IsBool())
        {
            return _holder.data.raw.uint64Val ? LLBC_INL_NS __g_trueStr : LLBC_INL_NS __g_falseStr;
        }
        else if (IsFloat() || IsDouble())
        {
            return LLBC_Num2Str(_holder.data.raw.doubleVal);
        }
        else if (IsSignedRaw())
        {
            if (_holder.data.raw.int64Val >= LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN &&
                _holder.data.raw.int64Val <= LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_END)
                return *_num2StrFastAccessTbl[
                    static_cast<int>(_holder.data.raw.int64Val - LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN)];

            return LLBC_Num2Str(_holder.data.raw.int64Val);
        }
        else
        {
            if (IsPtr())
                return LLBC_String().format("0x%p", _holder.data.raw.uint64Val);
            else
                return LLBC_Num2Str(_holder.data.raw.uint64Val);
        }
    }

    if (IsStr())
    {
        return _holder.data.obj.str ? *_holder.data.obj.str : LLBC_INL_NS __g_nullStr;
    }

    if (IsSeq())
    {
        if (!_holder.data.obj.seq || _holder.data.obj.seq->empty())
            return LLBC_INL_NS __g_emptySeqStr;

        LLBC_String content;
        content.reserve(64);
        content.append(1, '[');
        for (SeqConstIter it = _holder.data.obj.seq->begin();
             it != _holder.data.obj.seq->end();
             )
        {
            content.append(it->AsStr());
            if (++it != _holder.data.obj.seq->end())
                content.append(1, ',');
        }

        content.append(1, ']');
        return content;
    }

    if (IsDict())
    {
        if (!_holder.data.obj.dict || _holder.data.obj.dict->empty())
            return LLBC_INL_NS __g_emptyDictStr;

        LLBC_String content;
        content.reserve(64);
        content.append(1, '{');

        for (DictConstIter it = _holder.data.obj.dict->begin();
             it != _holder.data.obj.dict->end();
             )
        {
            content.append(it->first.AsStr());
            content.append(1, ':');
            content.append(it->second.AsStr());

            if (++it != _holder.data.obj.dict->end())
                content.append(1, ',');
        }

        content.append(1, '}');
        return content;
    }

    return LLBC_INL_NS __g_nilStr;
}

const LLBC_Variant::Seq &LLBC_Variant::AsSeq() const
{
    return IsSeqX() ? *_holder.data.obj.seq : LLBC_INL_NS __g_emptySeq;
}

const LLBC_Variant::Dict &LLBC_Variant::AsDict() const
{
    return IsDictX() ? *_holder.data.obj.dict : LLBC_INL_NS __g_emptyDict;
}

bool LLBC_Variant::IsEmpty() const
{
    if (_holder.type == LLBC_VariantType::VT_STR_DFT)
        return _holder.data.obj.str ? _holder.data.obj.str->empty() : true;
    if (_holder.type == LLBC_VariantType::VT_SEQ_DFT)
        return _holder.data.obj.seq->empty();
    if (_holder.type == LLBC_VariantType::VT_DICT_DFT)
        return _holder.data.obj.dict->empty();

    return true;
}

size_t LLBC_Variant::Size() const
{
    if (_holder.type == LLBC_VariantType::VT_STR_DFT)
        return _holder.data.obj.str ? _holder.data.obj.str->size() : 0;
    if (_holder.type == LLBC_VariantType::VT_SEQ_DFT)
        return _holder.data.obj.seq ? _holder.data.obj.seq->size() : 0;
    if (_holder.type == LLBC_VariantType::VT_DICT_DFT)
        return _holder.data.obj.dict ? _holder.data.obj.dict->size() : 0;

    return 0;
}

size_t LLBC_Variant::Capacity() const
{
    if (_holder.type == LLBC_VariantType::VT_STR_DFT)
        return _holder.data.obj.str ? _holder.data.obj.str->capacity() : 0;
    if (_holder.type == LLBC_VariantType::VT_SEQ_DFT)
        return _holder.data.obj.seq->capacity();
    if (_holder.type == LLBC_VariantType::VT_DICT_DFT)
        return _holder.data.obj.dict->size();

    return 0;
}

LLBC_Variant::SeqIter LLBC_Variant::SeqBegin()
{
    BecomeSeqX();
    return _holder.data.obj.seq->begin();
}

LLBC_Variant::SeqIter LLBC_Variant::SeqEnd()
{
    BecomeSeqX();
    return _holder.data.obj.seq->end();
}

LLBC_Variant::SeqConstIter LLBC_Variant::SeqBegin() const
{
    return AsSeq().begin();
}

LLBC_Variant::SeqConstIter LLBC_Variant::SeqEnd() const
{
    return AsSeq().end();
}

LLBC_Variant::SeqReverseIter LLBC_Variant::SeqReverseBegin()
{
    BecomeSeqX();
    return _holder.data.obj.seq->rbegin();
}

LLBC_Variant::SeqReverseIter LLBC_Variant::SeqReverseEnd()
{
    BecomeSeqX();
    return _holder.data.obj.seq->rend();
}

LLBC_Variant::SeqConstReverseIter LLBC_Variant::SeqReverseBegin() const
{
    return AsSeq().rbegin();
}

LLBC_Variant::SeqConstReverseIter LLBC_Variant::SeqReverseEnd() const
{
    return AsSeq().rend();
}

LLBC_Variant::Seq::reference LLBC_Variant::SeqFront()
{
    BecomeSeqX();
    return _holder.data.obj.seq->front();
}

LLBC_Variant::Seq::reference LLBC_Variant::SeqBack()
{
    BecomeSeqX();
    return _holder.data.obj.seq->back();
}

LLBC_Variant::Seq::const_reference LLBC_Variant::SeqFront() const
{
    return AsSeq().front();
}

LLBC_Variant::Seq::const_reference LLBC_Variant::SeqBack() const
{
    return AsSeq().back();
}

LLBC_Variant::SeqIter LLBC_Variant::SeqInsert(SeqIter it, const Seq::value_type &val)
{
    BecomeSeqX();
    return _holder.data.obj.seq->insert(it, val);
}

void LLBC_Variant::SeqInsert(SeqIter it, Seq::size_type n, const Seq::value_type &val)
{
    BecomeSeqX();
    _holder.data.obj.seq->insert(it, n, val);
}

void LLBC_Variant::SeqInsert(SeqIter it, SeqConstIter first, SeqConstIter last)
{
    BecomeSeqX();
    _holder.data.obj.seq->insert(it, first, last);
}

void LLBC_Variant::SeqPopBack()
{
    BecomeSeq();
    if (_holder.data.obj.seq && !_holder.data.obj.seq->empty())
        _holder.data.obj.seq->pop_back();
}

void LLBC_Variant::SeqResize(Seq::size_type n, const Seq::value_type &val)
{
    BecomeSeqX();
    _holder.data.obj.seq->resize(n, val);
}

void LLBC_Variant::SeqReserve(Seq::size_type n)
{
    BecomeSeqX();
    _holder.data.obj.seq->reserve(n);
}

LLBC_Variant::SeqIter LLBC_Variant::SeqErase(SeqIter it)
{
    BecomeSeqX();
    return _holder.data.obj.seq->erase(it);
}

LLBC_Variant::SeqIter LLBC_Variant::SeqErase(SeqIter first, SeqIter last)
{
    BecomeSeqX();
    return _holder.data.obj.seq->erase(first, last);
}

void LLBC_Variant::SeqErase(const Seq::value_type &val)
{
    BecomeSeq();
    if (!_holder.data.obj.seq || _holder.data.obj.seq->empty())
        return;

    SeqIter it;
    while ((it = std::find(_holder.data.obj.seq->begin(), _holder.data.obj.seq->end(), val)) != _holder.data.obj.seq->end())
        _holder.data.obj.seq->erase(it);
}

LLBC_Variant::DictIter LLBC_Variant::DictBegin()
{
    BecomeDictX();
    return _holder.data.obj.dict->begin();
}

LLBC_Variant::DictIter LLBC_Variant::DictEnd()
{
    BecomeDictX();
    return _holder.data.obj.dict->end();
}

LLBC_Variant::DictConstIter LLBC_Variant::DictBegin() const
{
    return AsDict().begin();
}

LLBC_Variant::DictConstIter LLBC_Variant::DictEnd() const
{
    return AsDict().end();
}

LLBC_Variant::DictReverseIter LLBC_Variant::DictReverseBegin()
{
    BecomeDictX();
    return _holder.data.obj.dict->rbegin();
}

LLBC_Variant::DictReverseIter LLBC_Variant::DictReverseEnd()
{
    BecomeDictX();
    return _holder.data.obj.dict->rend();
}

LLBC_Variant::DictConstReverseIter LLBC_Variant::DictReverseBegin() const
{
    return AsDict().rbegin();
}

LLBC_Variant::DictConstReverseIter LLBC_Variant::DictReverseEnd() const
{
    return AsDict().rend();
}

std::pair<LLBC_Variant::DictIter, bool> LLBC_Variant::DictInsert(const Dict::key_type &key, const Dict::mapped_type &val)
{
    BecomeDictX();
    return _holder.data.obj.dict->emplace(key, val);
}

std::pair<LLBC_Variant::DictIter, bool> LLBC_Variant::DictInsert(const Dict::value_type &val)
{
    BecomeDictX();
    return _holder.data.obj.dict->insert(val);
}

LLBC_Variant::DictIter LLBC_Variant::DictFind(const Dict::key_type &key)
{
    BecomeDictX();
    return _holder.data.obj.dict->find(key);
}

LLBC_Variant::DictConstIter LLBC_Variant::DictFind(const Dict::key_type &key) const
{
    return AsDict().find(key);
}

LLBC_Variant::DictIter LLBC_Variant::DictErase(DictIter it)
{
    BecomeDictX();
    return _holder.data.obj.dict->erase(it);
}

LLBC_Variant::DictIter LLBC_Variant::DictErase(DictIter first, DictIter last)
{
    BecomeDictX();
    return _holder.data.obj.dict->erase(first, last);
}

LLBC_Variant &LLBC_Variant::operator [](const LLBC_Variant &key)
{
    if (_holder.type == LLBC_VariantType::VT_SEQ_DFT)
    {
        if (!_holder.data.obj.seq)
            _holder.data.obj.seq = new Seq;

        return (*_holder.data.obj.seq)[key];
    }

    BecomeDictX();
    return (*_holder.data.obj.dict)[key];
}

const LLBC_Variant &LLBC_Variant::operator [](const LLBC_Variant &key) const
{
    if (_holder.type == LLBC_VariantType::VT_SEQ_DFT)
    {
        const size_t intKey = key;
        if (!_holder.data.obj.seq || intKey >= _holder.data.obj.seq->size())
            return LLBC_INL_NS __g_nilVariant;

        return (*_holder.data.obj.seq)[intKey];
    }

    if (_holder.type == LLBC_VariantType::VT_DICT_DFT)
    {
        if (!_holder.data.obj.dict)
            return LLBC_INL_NS __g_nilVariant;

        const DictConstIter it = _holder.data.obj.dict->find(key);
        return it != _holder.data.obj.dict->end() ? it->second : LLBC_INL_NS __g_nilVariant;
    }

    return LLBC_INL_NS __g_nilVariant;
}

LLBC_Variant &LLBC_Variant::operator =(bool b)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::VT_RAW_BOOL;
    _holder.data.raw.int64Val = b ? 1 : 0;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(sint8 i8)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::VT_RAW_SINT8;
    _holder.data.raw.int64Val = static_cast<sint64>(i8);

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(uint8 ui8)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::VT_RAW_UINT8;
    _holder.data.raw.uint64Val = ui8;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(sint16 i16)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::VT_RAW_SINT16;
    _holder.data.raw.int64Val = i16;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(uint16 ui16)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::VT_RAW_UINT16;
    _holder.data.raw.uint64Val = ui16;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(sint32 i32)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::VT_RAW_SINT32;
    _holder.data.raw.int64Val = i32;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(uint32 ui32)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::VT_RAW_UINT32;
    _holder.data.raw.uint64Val = ui32;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(long l)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::VT_RAW_LONG;
    _holder.data.raw.int64Val = l;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(ulong ul)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::VT_RAW_ULONG;
    _holder.data.raw.uint64Val = ul;

    return *this;
}

LLBC_Variant & LLBC_Variant::operator =(const char * const &str)
{
    if (!IsStr())
    {
        _holder.ClearData();
        _holder.type = LLBC_VariantType::VT_STR_DFT;
    }

    const size_t len = str ? strlen(str) : 0;
    if (len == 0)
    {
        if (_holder.data.obj.str)
            _holder.data.obj.str->clear();
    }
    else
    {
        if (_holder.data.obj.str)
            _holder.data.obj.str->assign(str, len);
        else
            _holder.data.obj.str = new LLBC_String(str, len);
    }

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(const sint64 &i64)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::VT_RAW_SINT64;
    _holder.data.raw.int64Val = i64;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(const uint64 &ui64)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::VT_RAW_UINT64;
    _holder.data.raw.uint64Val = ui64;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(float f)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::VT_RAW_FLOAT;
    _holder.data.raw.doubleVal = f;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(const double &d)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::VT_RAW_DOUBLE;
    _holder.data.raw.doubleVal = d;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(const LLBC_String &str)
{
    BecomeStr();
    if (str.empty())
    {
        if (_holder.data.obj.str)
            _holder.data.obj.str->clear();
    }
    else
    {
        if (_holder.data.obj.str)
            *_holder.data.obj.str = str;
        else
            _holder.data.obj.str = new LLBC_String(str);
    }

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(const Seq &seq)
{
    BecomeSeq();
    if (seq.empty())
    {
        if (_holder.data.obj.seq)
            _holder.data.obj.seq->clear();
    }
    else
    {
        if (_holder.data.obj.seq)
            *_holder.data.obj.seq = seq;
        else
            _holder.data.obj.seq = new Seq(seq);
    }

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(const Dict &dict)
{
    BecomeSeq();
    if (dict.empty())
    {
        if (_holder.data.obj.dict)
            _holder.data.obj.dict->clear();
    }
    else
    {
        if (_holder.data.obj.dict)
            *_holder.data.obj.dict = dict;
        else
            _holder.data.obj.dict = new Dict(dict);
    }

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(LLBC_Variant &&var)
{
    if (this == &var)
        return *this;

    _holder.Clear();
    _holder = var._holder;

    var._holder.type = LLBC_VariantType::VT_NIL;
    var._holder.data.raw.uint64Val = 0;

    return *this;
}

void LLBC_Variant::Serialize(LLBC_Stream &stream) const
{
    stream.Write(_holder.type);

    if (IsRaw())
    {
        stream.Write(_holder.data.raw.uint64Val);
    }
    else if (IsStr())
    {
        stream.Write(_holder.data.obj.str ? *_holder.data.obj.str : LLBC_INL_NS __g_nullStr);
    }
    else if (IsSeq())
    {
        if (!_holder.data.obj.seq)
        {
            stream.Write(static_cast<uint32>(0));
            return;
        }

        stream.Write(static_cast<uint32>(_holder.data.obj.seq->size()));
        for (SeqConstIter it = _holder.data.obj.seq->begin();
             it != _holder.data.obj.seq->end();
             ++it)
        {
            stream.Write(*it);
        }
    }
    else if (IsDict())
    {
        if (!_holder.data.obj.dict)
        {
            stream.Write(static_cast<uint32>(0));
            return;
        }

        stream.Write(static_cast<uint32>(_holder.data.obj.dict->size()));
        for (DictConstIter it = _holder.data.obj.dict->begin();
             it != _holder.data.obj.dict->end();
             ++it)
        {
            stream.Write(it->first);
            stream.Write(it->second);
        }
    }
}

bool LLBC_Variant::DeSerialize(LLBC_Stream &stream)
{
    LLBC_VariantType::ENUM streamType;
    if (!stream.Read(streamType))
        return false;

    if (streamType != _holder.type)
        BecomeNil();

    _holder.type = streamType;
    if (IsNil())
        return true;

    if (IsRaw())
    {
        if (!stream.Read(_holder.data.raw.uint64Val))
        {
            _holder.type = LLBC_VariantType::VT_NIL;
            return false;
        }

        return true;
    }
    else if (IsStr())
    {
        if (_holder.data.obj.str)
            _holder.data.obj.str->clear();
        else
            _holder.data.obj.str = new LLBC_String;

        if (!stream.Read(*_holder.data.obj.str))
        {
            BecomeNil();
            return false;
        }

        return true;
    }
    else if (IsSeq())
    {
        uint32 count = 0;
        if (!stream.Read(count))
        {
            BecomeNil();
            return false;
        }

        if (_holder.data.obj.seq)
            _holder.data.obj.seq->clear();

        if (count == 0)
            return true;
       if (!_holder.data.obj.seq)
            _holder.data.obj.seq = new Seq;

        for (uint32 i = 0; i < count; ++i)
        {
            LLBC_Variant val;
            if (!stream.Read(val))
            {
                BecomeNil();
                return false;
            }

            _holder.data.obj.seq->push_back(val);
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

        if (_holder.data.obj.dict)
            _holder.data.obj.dict->clear();

        if (count == 0)
            return true;
        if (!_holder.data.obj.dict)
            _holder.data.obj.dict = new Dict;

        for (uint32 i = 0; i < count; ++i)
        {
            LLBC_Variant key;
            LLBC_Variant val;
            if (!stream.Read(key) || !stream.Read(val))
            {
                BecomeNil();
                return false;
            }

            _holder.data.obj.dict->insert(std::make_pair(key, val));
        }

        return true;
    }
    
    return false;
}

void LLBC_Variant::SeqPushBackElem(const Seq::value_type &val)
{
    BecomeSeqX();
    _holder.data.obj.seq->push_back(val);
}

LLBC_Variant::Dict::size_type LLBC_Variant::DictEraseKey(const Dict::key_type &key)
{
    BecomeDictX();
    return _holder.data.obj.dict->erase(key);
}

__LLBC_NS_END
