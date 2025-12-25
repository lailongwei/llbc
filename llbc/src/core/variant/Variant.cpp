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
static const LLBC_NS LLBC_Variant::Str __g_nilStr;
static const LLBC_NS LLBC_Variant::Str __g_trueStr = "true";
static const LLBC_NS LLBC_Variant::Str __g_falseStr = "false";
static const LLBC_NS LLBC_Variant::Str __g_emptySeqStr = "[]";
static const LLBC_NS LLBC_Variant::Str __g_emptyDictStr = "{}";
static const LLBC_NS LLBC_Variant::Seq __g_emptySeq;
static const LLBC_NS LLBC_Variant::Dict __g_emptyDict;

static const LLBC_NS LLBC_Variant __g_nilVariant;

static std::map<LLBC_NS LLBC_VariantType::ENUM, LLBC_NS LLBC_String> __g_typeDescs {
    {LLBC_NS LLBC_VariantType::NIL, "Nil"},

    {LLBC_NS LLBC_VariantType::RAW_BOOL, "bool"},
    {LLBC_NS LLBC_VariantType::RAW_SINT8, "int8"},
    {LLBC_NS LLBC_VariantType::RAW_UINT8, "uint8"},
    {LLBC_NS LLBC_VariantType::RAW_SINT16, "sint16"},
    {LLBC_NS LLBC_VariantType::RAW_UINT16, "uint16"},
    {LLBC_NS LLBC_VariantType::RAW_SINT32, "int32"},
    {LLBC_NS LLBC_VariantType::RAW_UINT32, "uint32"},
    {LLBC_NS LLBC_VariantType::RAW_LONG, "long"},
    {LLBC_NS LLBC_VariantType::RAW_ULONG, "ulong"},
    {LLBC_NS LLBC_VariantType::RAW_PTR, "ptr"},
    {LLBC_NS LLBC_VariantType::RAW_SINT64, "int64"},
    {LLBC_NS LLBC_VariantType::RAW_UINT64, "uint64"},
    {LLBC_NS LLBC_VariantType::RAW_FLOAT, "float"},
    {LLBC_NS LLBC_VariantType::RAW_DOUBLE, "double"},

    {LLBC_NS LLBC_VariantType::STR_DFT, "string"},

    {LLBC_NS LLBC_VariantType::SEQ_DFT, "sequence"},

    {LLBC_NS LLBC_VariantType::DICT_DFT, "dictionary"},
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

const LLBC_Variant LLBC_Variant::nil;

const LLBC_String &LLBC_VariantType::Type2Str(int type)
{
    std::map<ENUM, LLBC_String>::const_iterator it =
        LLBC_INL_NS __g_typeDescs.find(static_cast<ENUM>(type));
    return (it != LLBC_INL_NS __g_typeDescs.end()) ? it->second : LLBC_INL_NS __g_typeDescs[NIL];
}

__LLBC_NS_END

std::ostream &operator<<(std::ostream &o, const LLBC_NS LLBC_Variant &variant)
{
    LLBC_NS LLBC_String str = variant.ToString();
    o.write(str.c_str(), str.length());
    return o;
}

__LLBC_NS_BEGIN

LLBC_Variant::Holder::Holder(const Holder& other) : type(other.type)
{
    if (type == LLBC_VariantType::STR_DFT)
        new (&data.obj.str) Str(other.data.obj.str);
    else if (type == LLBC_VariantType::SEQ_DFT)
        new (&data.obj.seq) Seq(other.data.obj.seq);
    else if (type == LLBC_VariantType::DICT_DFT)
        new (&data.obj.dict) Dict(other.data.obj.dict);
    else
        data.raw.uint64Val = other.data.raw.uint64Val;
}

LLBC_Variant::Holder& LLBC_Variant::Holder::operator=(const Holder& other)
{
    if (this != &other)
        ClearData();

    type = other.type;
    if (type == LLBC_VariantType::STR_DFT)
        new (&data.obj.str) Str(other.data.obj.str);
    else if (type == LLBC_VariantType::SEQ_DFT)
        new (&data.obj.seq) Seq(other.data.obj.seq);
    else if (type == LLBC_VariantType::DICT_DFT)
        new (&data.obj.dict) Dict(other.data.obj.dict);
    else
        data.raw.uint64Val = other.data.raw.uint64Val;

    return *this;
}

LLBC_Variant::Holder::Holder(Holder &&other) noexcept : type(other.type)
{
    if (type == LLBC_VariantType::STR_DFT)
        new (&data.obj.str) Str(std::move(other.data.obj.str));
    else if (type == LLBC_VariantType::SEQ_DFT)
        new (&data.obj.seq) Seq(std::move(other.data.obj.seq));
    else if (type == LLBC_VariantType::DICT_DFT)
        new (&data.obj.dict) Dict(std::move(other.data.obj.dict));
    else
        data.raw.uint64Val = other.data.raw.uint64Val;

    other.type = LLBC_VariantType::NIL;
    other.data.raw.uint64Val = 0;
}

LLBC_Variant::Holder& LLBC_Variant::Holder::operator=(Holder &&other) noexcept
{
    if (this != &other)
    {
        ClearData();

        type = other.type;
        if (type == LLBC_VariantType::STR_DFT)
            new (&data.obj.str) Str(std::move(other.data.obj.str));
        else if (type == LLBC_VariantType::SEQ_DFT)
            new (&data.obj.seq) Seq(std::move(other.data.obj.seq));
        else if (type == LLBC_VariantType::DICT_DFT)
            new (&data.obj.dict) Dict(std::move(other.data.obj.dict));
        else
            data.raw.uint64Val = other.data.raw.uint64Val;

        other.type = LLBC_VariantType::NIL;
        other.data.raw.uint64Val = 0;
    }

    return *this;
}

LLBC_VariantType::ENUM LLBC_Variant::Holder::GetFirstType() const
{
    return static_cast<LLBC_VariantType::ENUM>(type & LLBC_VariantType::MASK_FIRST_TYPE);
}

void LLBC_Variant::Holder::Clear()
{
    ClearData();
    type = LLBC_VariantType::NIL;
}

void LLBC_Variant::Holder::ClearData()
{
    if (data.raw.int64Val == 0)
        return;

    if (type == LLBC_VariantType::STR_DFT)
        data.obj.str.~Str();
    else if (type == LLBC_VariantType::SEQ_DFT)
        data.obj.seq.~Seq();
    else if (type == LLBC_VariantType::DICT_DFT)
        data.obj.dict.~Dict();

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
    BecomeStr();
    _holder.data.obj.str = str;
}

LLBC_Variant::LLBC_Variant(const LLBC_Variant &var)
{
    LLBC_VariantTraits::assign(*this, var);
}

LLBC_Variant::LLBC_Variant(LLBC_Variant &&var) noexcept
: _holder(var._holder)
{
    var._holder.type = LLBC_VariantType::NIL;
    var._holder.data.raw.uint64Val = 0;
}

bool LLBC_Variant::AsBool() const
{
    const LLBC_VariantType::ENUM firstType = GetFirstType();
    if (firstType == LLBC_VariantType::RAW)
    {
        if (IsFloat() || IsDouble())
            return std::fabs(AsDouble()) >= DBL_EPSILON;

        return _holder.data.raw.uint64Val != 0;
    }
    if (firstType == LLBC_VariantType::STR)
    {
        return !_holder.data.obj.str.empty();
    }
    if (firstType == LLBC_VariantType::SEQ)
    {
        return !_holder.data.obj.seq.empty();
    }
    if (firstType == LLBC_VariantType::DICT)
    {
        return !_holder.data.obj.dict.empty();
    }

    return false;
}

bool LLBC_Variant::AsLooseBool() const
{
    if (GetFirstType() == LLBC_VariantType::STR)
    {
        const Str &str = _holder.data.obj.str;
        if (str.empty())
            return false;

        return LLBC_Str2LooseBool(str.c_str(), 10, true);
    }

    return AsBool();
}

double LLBC_Variant::AsDouble() const
{
    const LLBC_VariantType::ENUM firstType = GetFirstType();
    if (firstType == LLBC_VariantType::NIL ||
        firstType == LLBC_VariantType::SEQ ||
        firstType == LLBC_VariantType::DICT)
        return 0.0;

    if (firstType == LLBC_VariantType::STR)
    {
        const auto &str = _holder.data.obj.str;
        if (str.empty())
            return 0.0;

        if (str.find('.') != LLBC_String::npos)
            return LLBC_Str2Num<double>(str.c_str());

        if (str.find('-') != LLBC_String::npos)
            return static_cast<double>(AsSignedOrUnsigned64<sint64>());
        else
            return static_cast<double>(AsSignedOrUnsigned64<uint64>());
    }

    if (firstType == LLBC_VariantType::RAW)
    {
        if (_holder.type == LLBC_VariantType::RAW_FLOAT ||
            _holder.type == LLBC_VariantType::RAW_DOUBLE)
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
                return LLBC_Num2Str<uint64, true>(_holder.data.raw.uint64Val);
            else
                return LLBC_Num2Str(_holder.data.raw.uint64Val);
        }
    }

    if (IsStr())
    {
        return _holder.data.obj.str;
    }

    if (IsSeq())
    {
        if (_holder.data.obj.seq.empty())
            return LLBC_INL_NS __g_emptySeqStr;

        LLBC_String content;
        content.reserve(64);
        content.append(1, '[');
        for (SeqConstIter it = _holder.data.obj.seq.begin();
             it != _holder.data.obj.seq.end();
             )
        {
            content.append(it->AsStr());
            if (++it != _holder.data.obj.seq.end())
                content.append(1, ',');
        }

        content.append(1, ']');
        return content;
    }

    if (IsDict())
    {
        if (_holder.data.obj.dict.empty())
            return LLBC_INL_NS __g_emptyDictStr;

        LLBC_String content;
        content.reserve(64);
        content.append(1, '{');

        for (DictConstIter it = _holder.data.obj.dict.begin();
             it != _holder.data.obj.dict.end();
             )
        {
            content.append(it->first.AsStr());
            content.append(1, ':');
            content.append(it->second.AsStr());

            if (++it != _holder.data.obj.dict.end())
                content.append(1, ',');
        }

        content.append(1, '}');
        return content;
    }

    return LLBC_INL_NS __g_nilStr;
}

const LLBC_Variant::Seq &LLBC_Variant::AsSeq() const
{
    return IsSeq() ? _holder.data.obj.seq : LLBC_INL_NS __g_emptySeq;
}

const LLBC_Variant::Dict &LLBC_Variant::AsDict() const
{
    return IsDict() ? _holder.data.obj.dict : LLBC_INL_NS __g_emptyDict;
}

bool LLBC_Variant::IsEmpty() const
{
    if (_holder.type == LLBC_VariantType::STR_DFT)
        return _holder.data.obj.str.empty();
    if (_holder.type == LLBC_VariantType::SEQ_DFT)
        return _holder.data.obj.seq.empty();
    if (_holder.type == LLBC_VariantType::DICT_DFT)
        return _holder.data.obj.dict.empty();

    return true;
}

size_t LLBC_Variant::Size() const
{
    if (_holder.type == LLBC_VariantType::STR_DFT)
        return _holder.data.obj.str.size();
    if (_holder.type == LLBC_VariantType::SEQ_DFT)
        return _holder.data.obj.seq.size();
    if (_holder.type == LLBC_VariantType::DICT_DFT)
        return _holder.data.obj.dict.size();

    return 0;
}

size_t LLBC_Variant::Capacity() const
{
    if (_holder.type == LLBC_VariantType::STR_DFT)
        return _holder.data.obj.str.capacity();
    if (_holder.type == LLBC_VariantType::SEQ_DFT)
        return _holder.data.obj.seq.capacity();
    if (_holder.type == LLBC_VariantType::DICT_DFT)
        return _holder.data.obj.dict.size();

    return 0;
}

LLBC_Variant::SeqIter LLBC_Variant::SeqBegin()
{
    BecomeSeq();
    return _holder.data.obj.seq.begin();
}

LLBC_Variant::SeqIter LLBC_Variant::SeqEnd()
{
    BecomeSeq();
    return _holder.data.obj.seq.end();
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
    BecomeSeq();
    return _holder.data.obj.seq.rbegin();
}

LLBC_Variant::SeqReverseIter LLBC_Variant::SeqReverseEnd()
{
    BecomeSeq();
    return _holder.data.obj.seq.rend();
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
    BecomeSeq();
    return _holder.data.obj.seq.front();
}

LLBC_Variant::Seq::reference LLBC_Variant::SeqBack()
{
    BecomeSeq();
    return _holder.data.obj.seq.back();
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
    BecomeSeq();
    return _holder.data.obj.seq.insert(it, val);
}

void LLBC_Variant::SeqInsert(SeqIter it, Seq::size_type n, const Seq::value_type &val)
{
    BecomeSeq();
    _holder.data.obj.seq.insert(it, n, val);
}

void LLBC_Variant::SeqInsert(SeqIter it, SeqConstIter first, SeqConstIter last)
{
    BecomeSeq();
    _holder.data.obj.seq.insert(it, first, last);
}

void LLBC_Variant::SeqPopBack()
{
    BecomeSeq();
    if (!_holder.data.obj.seq.empty())
        _holder.data.obj.seq.pop_back();
}

void LLBC_Variant::SeqResize(Seq::size_type n, const Seq::value_type &val)
{
    BecomeSeq();
    _holder.data.obj.seq.resize(n, val);
}

void LLBC_Variant::SeqReserve(Seq::size_type n)
{
    BecomeSeq();
    _holder.data.obj.seq.reserve(n);
}

LLBC_Variant::SeqIter LLBC_Variant::SeqErase(SeqIter it)
{
    BecomeSeq();
    return _holder.data.obj.seq.erase(it);
}

LLBC_Variant::SeqIter LLBC_Variant::SeqErase(SeqIter first, SeqIter last)
{
    BecomeSeq();
    return _holder.data.obj.seq.erase(first, last);
}

void LLBC_Variant::SeqErase(const Seq::value_type &val)
{
    BecomeSeq();
    if (_holder.data.obj.seq.empty())
        return;

    SeqIter it;
    while ((it = std::find(_holder.data.obj.seq.begin(), _holder.data.obj.seq.end(), val)) != _holder.data.obj.seq.end())
        _holder.data.obj.seq.erase(it);
}

LLBC_Variant::DictIter LLBC_Variant::DictBegin()
{
    BecomeDict();
    return _holder.data.obj.dict.begin();
}

LLBC_Variant::DictIter LLBC_Variant::DictEnd()
{
    BecomeDict();
    return _holder.data.obj.dict.end();
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
    BecomeDict();
    return _holder.data.obj.dict.rbegin();
}

LLBC_Variant::DictReverseIter LLBC_Variant::DictReverseEnd()
{
    BecomeDict();
    return _holder.data.obj.dict.rend();
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
    BecomeDict();
    return _holder.data.obj.dict.emplace(key, val);
}

std::pair<LLBC_Variant::DictIter, bool> LLBC_Variant::DictInsert(const Dict::value_type &val)
{
    BecomeDict();
    return _holder.data.obj.dict.insert(val);
}

LLBC_Variant::DictIter LLBC_Variant::DictFind(const Dict::key_type &key)
{
    BecomeDict();
    return _holder.data.obj.dict.find(key);
}

LLBC_Variant::DictConstIter LLBC_Variant::DictFind(const Dict::key_type &key) const
{
    return AsDict().find(key);
}

LLBC_Variant::DictIter LLBC_Variant::DictErase(DictIter it)
{
    BecomeDict();
    return _holder.data.obj.dict.erase(it);
}

LLBC_Variant::DictIter LLBC_Variant::DictErase(DictIter first, DictIter last)
{
    BecomeDict();
    return _holder.data.obj.dict.erase(first, last);
}

LLBC_Variant &LLBC_Variant::operator[](const LLBC_Variant &key)
{
    if (_holder.type == LLBC_VariantType::SEQ_DFT)
    {
        const size_t idx = key;
        if (UNLIKELY(idx >= _holder.data.obj.seq.size()))
            _holder.data.obj.seq.resize(idx + 1);

        return _holder.data.obj.seq[idx];
    }

    BecomeDict();
    return _holder.data.obj.dict[key];
}

const LLBC_Variant &LLBC_Variant::operator[](const LLBC_Variant &key) const
{
    if (_holder.type == LLBC_VariantType::SEQ_DFT)
    {
        const size_t intKey = key;
        if (intKey >= _holder.data.obj.seq.size())
            return LLBC_INL_NS __g_nilVariant;

        return _holder.data.obj.seq[intKey];
    }

    if (_holder.type == LLBC_VariantType::DICT_DFT)
    {
        const DictConstIter it = _holder.data.obj.dict.find(key);
        return it != _holder.data.obj.dict.end() ? it->second : LLBC_INL_NS __g_nilVariant;
    }

    return LLBC_INL_NS __g_nilVariant;
}

LLBC_Variant &LLBC_Variant::operator=(bool b)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::RAW_BOOL;
    _holder.data.raw.int64Val = b ? 1 : 0;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(sint8 i8)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::RAW_SINT8;
    _holder.data.raw.int64Val = static_cast<sint64>(i8);

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(uint8 ui8)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::RAW_UINT8;
    _holder.data.raw.uint64Val = ui8;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(sint16 i16)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::RAW_SINT16;
    _holder.data.raw.int64Val = i16;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(uint16 ui16)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::RAW_UINT16;
    _holder.data.raw.uint64Val = ui16;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(sint32 i32)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::RAW_SINT32;
    _holder.data.raw.int64Val = i32;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(uint32 ui32)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::RAW_UINT32;
    _holder.data.raw.uint64Val = ui32;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(long l)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::RAW_LONG;
    _holder.data.raw.int64Val = l;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(ulong ul)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::RAW_ULONG;
    _holder.data.raw.uint64Val = ul;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(const char * const &str)
{
    BecomeStr();

    const auto len = strlen(str);
    if(len == 0)
        _holder.data.obj.str.clear();
    else
        _holder.data.obj.str.assign(str, len);

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(const sint64 &i64)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::RAW_SINT64;
    _holder.data.raw.int64Val = i64;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(const uint64 &ui64)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::RAW_UINT64;
    _holder.data.raw.uint64Val = ui64;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(float f)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::RAW_FLOAT;
    _holder.data.raw.doubleVal = f;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(const double &d)
{
    _holder.ClearData();

    _holder.type = LLBC_VariantType::RAW_DOUBLE;
    _holder.data.raw.doubleVal = d;

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(const std::string &str)
{
    BecomeStr();

    if (str.empty())
    {
        _holder.data.obj.str.clear();
    }
    else
    {
        _holder.data.obj.str = str;
    }

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(const LLBC_String &str)
{
    BecomeStr();
    if (str.empty())
    {
        _holder.data.obj.str.clear();
    }
    else
    {
        _holder.data.obj.str = str;
    }

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(const LLBC_CString &str)
{
    BecomeStr();
    if (str.empty())
    {
        _holder.data.obj.str.clear();
    }
    else
    {
        _holder.data.obj.str = str;
    }

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(const Seq &seq)
{
    BecomeSeq();
    if (seq.empty())
    {
        _holder.data.obj.seq.clear();
    }
    else
    {
        _holder.data.obj.seq = seq;
    }

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(const Dict &dict)
{
    BecomeDict();
    if (dict.empty())
    {
        _holder.data.obj.dict.clear();
    }
    else
    {
        _holder.data.obj.dict = dict;
    }

    return *this;
}

LLBC_Variant &LLBC_Variant::operator=(LLBC_Variant &&var) noexcept
{
    if (this == &var)
        return *this;

    _holder.Clear();
    _holder = var._holder;

    var._holder.type = LLBC_VariantType::NIL;
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
        stream.Write(_holder.data.obj.str);
    }
    else if (IsSeq())
    {
        stream.Write(static_cast<uint32>(_holder.data.obj.seq.size()));

        const Seq::const_iterator seqEnd = _holder.data.obj.seq.end();
        for (SeqConstIter it = _holder.data.obj.seq.begin(); it != seqEnd; ++it)
            stream.Write(*it);
    }
    else if (IsDict())
    {
        stream.Write(static_cast<uint32>(_holder.data.obj.dict.size()));

        const Dict::const_iterator dictEnd = _holder.data.obj.dict.end();
        for (DictConstIter it = _holder.data.obj.dict.begin(); it != dictEnd; ++it)
        {
            stream.Write(it->first);
            stream.Write(it->second);
        }
    }
}

bool LLBC_Variant::Deserialize(LLBC_Stream &stream)
{
    LLBC_VariantType::ENUM streamType;
    if (!stream.Read(streamType))
        return false;

    if (streamType != _holder.type)
        Become(streamType);

    if (IsNil())
        return true;

    if (IsRaw())
    {
        if (!stream.Read(_holder.data.raw.uint64Val))
        {
            BecomeNil();
            return false;
        }

        return true;
    }
    else if (IsStr())
    {
        _holder.data.obj.str.clear();

        if (!stream.Read(_holder.data.obj.str))
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

        _holder.data.obj.seq.clear();

        if (count == 0)
            return true;

        for (uint32 i = 0; i < count; ++i)
        {
            LLBC_Variant val;
            if (!stream.Read(val))
            {
                BecomeNil();
                return false;
            }

            _holder.data.obj.seq.push_back(val);
        }

        return true;
    }
    else if (IsDict())
    {
        uint32 count = 0;
        if (!stream.Read(count))
        {
            _holder.type = LLBC_VariantType::NIL;
            return false;
        }

        _holder.data.obj.dict.clear();

        if (count == 0)
            return true;

        for (uint32 i = 0; i < count; ++i)
        {
            LLBC_Variant key;
            LLBC_Variant val;
            if (!stream.Read(key) || !stream.Read(val))
            {
                BecomeNil();
                return false;
            }

            _holder.data.obj.dict.insert(std::make_pair(key, val));
        }

        return true;
    }
    
    return false;
}

void LLBC_Variant::SeqPushBackElem(const Seq::value_type &val)
{
    BecomeSeq();
    _holder.data.obj.seq.push_back(val);
}

LLBC_Variant::Dict::size_type LLBC_Variant::DictEraseKey(const Dict::key_type &key)
{
    BecomeDict();
    return _holder.data.obj.dict.erase(key);
}

__LLBC_NS_END
