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

    typedef LLBC_NS LLBC_Variant::Dict Dict;
    typedef LLBC_NS LLBC_Variant::DictIter DictIter;
    typedef LLBC_NS LLBC_Variant::DictConstIter DictConstIter;
    typedef LLBC_NS LLBC_Variant::DictReverseIter DictReverseIter;
    typedef LLBC_NS LLBC_Variant::DictConstReverseIter DictConstReverseIter;
}

__LLBC_INTERNAL_NS_BEGIN

static Str __g_nullStr;
static Str __g_trueStr = "true";
static Dict __g_nullDict;

static void BecomeAndAllocDict(LLBC_NS LLBC_Variant &var, LLBC_NS LLBC_Variant::Holder &holder)
{
    if (!var.IsDict())
        var.BecomeDict();
    if (!holder.obj.dict)
        holder.obj.dict = LLBC_New0(Dict);
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

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
        _typeDescs.insert(std::make_pair(VT_RAW_SINT64, "int64"));
        _typeDescs.insert(std::make_pair(VT_RAW_UINT64, "uint64"));
        _typeDescs.insert(std::make_pair(VT_RAW_FLOAT, "float"));
        _typeDescs.insert(std::make_pair(VT_RAW_DOUBLE, "double"));

        _typeDescs.insert(std::make_pair(VT_STR_DFT, "string"));

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
        LLBC_XDelete(obj.str);
    else if (type == LLBC_VariantType::VT_DICT_DFT)
        LLBC_XDelete(obj.dict);
}

LLBC_Variant::LLBC_Variant()
{
}

LLBC_Variant::LLBC_Variant(const bool &boolVal)
{
    _holder.type = LLBC_VariantType::VT_RAW_BOOL;
    _holder.raw.int64Val = boolVal ? 1 : 0;
}

LLBC_Variant::LLBC_Variant(const sint8 &sint8Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_SINT8;
    _holder.raw.int64Val = sint8Val;
}

LLBC_Variant::LLBC_Variant(const uint8 &uint8Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_UINT8;
    _holder.raw.uint64Val = uint8Val;
}

LLBC_Variant::LLBC_Variant(const sint16 &sint16Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_SINT16;
    _holder.raw.int64Val = sint16Val;
}

LLBC_Variant::LLBC_Variant(const uint16 &uint16Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_UINT16;
    _holder.raw.uint64Val = uint16Val;
}

LLBC_Variant::LLBC_Variant(const sint32 &sint32Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_SINT32;
    _holder.raw.int64Val = sint32Val;
}

LLBC_Variant::LLBC_Variant(const uint32 &uint32Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_UINT32;
    _holder.raw.uint64Val = uint32Val;
}

LLBC_Variant::LLBC_Variant(const long &longVal)
{
    _holder.type = LLBC_VariantType::VT_RAW_LONG;
    _holder.raw.int64Val = longVal;
}

LLBC_Variant::LLBC_Variant(const ulong &ulongVal)
{
    _holder.type = LLBC_VariantType::VT_RAW_ULONG;
    _holder.raw.uint64Val = ulongVal;
}

LLBC_Variant::LLBC_Variant(const sint64 &int64Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_SINT64;
    _holder.raw.int64Val = int64Val;
}

LLBC_Variant::LLBC_Variant(const uint64 &uint64Val)
{
    _holder.type = LLBC_VariantType::VT_RAW_UINT64;
    _holder.raw.uint64Val = uint64Val;
}

LLBC_Variant::LLBC_Variant(const float &floatVal)
{
    _holder.type = LLBC_VariantType::VT_RAW_FLOAT;
    _holder.raw.doubleVal = floatVal;
}

LLBC_Variant::LLBC_Variant(const double &doubleVal)
{
    _holder.type = LLBC_VariantType::VT_RAW_DOUBLE;
    _holder.raw.doubleVal = doubleVal;
}

LLBC_Variant::LLBC_Variant(const char *cstrVal)
{
    _holder.type = LLBC_VariantType::VT_STR_DFT;
    if (cstrVal != NULL)
    {
        size_t strLen = LLBC_StrLenA(cstrVal);
        if (strLen != 0)
            _holder.obj.str = LLBC_New2(LLBC_String, cstrVal, strLen);
    }
}

LLBC_Variant::LLBC_Variant(const std::string &strVal)
{
    _holder.type = LLBC_VariantType::VT_STR_DFT;
    if (!strVal.empty())
        _holder.obj.str = LLBC_New2(LLBC_String, strVal.data(), strVal.size());
}

LLBC_Variant::LLBC_Variant(const LLBC_String &strVal)
{
    _holder.type = LLBC_VariantType::VT_STR_DFT;
    if (!strVal.empty())
        _holder.obj.str = LLBC_New1(LLBC_String, strVal);
}

LLBC_Variant::LLBC_Variant(const Dict &dictVal)
{
    _holder.type = LLBC_VariantType::VT_DICT_DFT;
    if (!dictVal.empty())
        _holder.obj.dict = LLBC_New1(Dict, dictVal);
}

LLBC_Variant::LLBC_Variant(const LLBC_Variant &varVal)
{
    LLBC_VariantTraits::assign(*this, varVal);
}

int LLBC_Variant::GetType() const
{
    return _holder.type;
}

const LLBC_Variant::Holder &LLBC_Variant::GetHolder() const
{
    return _holder;
}

bool LLBC_Variant::IsNil() const
{
    return (_holder.type  == LLBC_VariantType::VT_NIL);
}

bool LLBC_Variant::IsRaw() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW) == 
        LLBC_VariantType::VT_RAW);
}

bool LLBC_Variant::IsSignedRaw() const
{
    return (IsRaw() && 
        ((_holder.type & LLBC_VariantType::VT_MASK_RAW_SIGNED) == 
            LLBC_VariantType::VT_MASK_RAW_SIGNED));
}

bool LLBC_Variant::IsUnsignedRaw() const
{
    return (IsRaw() &&
        ((_holder.type & LLBC_VariantType::VT_MASK_RAW_SIGNED) !=
            LLBC_VariantType::VT_MASK_RAW_SIGNED));
}

bool LLBC_Variant::IsBool() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_BOOL) == 
        LLBC_VariantType::VT_RAW_BOOL);
}

bool LLBC_Variant::IsInt8() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_SINT8) ==
        LLBC_VariantType::VT_RAW_SINT8);
}

bool LLBC_Variant::IsUInt8() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_UINT8) ==
        LLBC_VariantType::VT_RAW_UINT8);
}

bool LLBC_Variant::IsInt16() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_SINT16) ==
        LLBC_VariantType::VT_RAW_SINT16);
}

bool LLBC_Variant::IsUInt16() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_UINT16) ==
        LLBC_VariantType::VT_RAW_UINT16);
}

bool LLBC_Variant::IsInt32() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_SINT32) ==
        LLBC_VariantType::VT_RAW_SINT32);
}

bool LLBC_Variant::IsUInt32() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_UINT32) ==
        LLBC_VariantType::VT_RAW_UINT32);
}

bool LLBC_Variant::IsLong() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_LONG) ==
        LLBC_VariantType::VT_RAW_LONG);
}

bool LLBC_Variant::IsULong() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_ULONG) ==
        LLBC_VariantType::VT_RAW_ULONG);
}

bool LLBC_Variant::IsInt64() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_SINT64) ==
        LLBC_VariantType::VT_RAW_SINT64);
}

bool LLBC_Variant::IsUInt64() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_UINT64) ==
        LLBC_VariantType::VT_RAW_UINT64);
}

bool LLBC_Variant::IsFloat() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_FLOAT) ==
        LLBC_VariantType::VT_RAW_FLOAT);
}

bool LLBC_Variant::IsDouble() const
{
    return ((_holder.type & LLBC_VariantType::VT_RAW_DOUBLE) ==
        LLBC_VariantType::VT_RAW_DOUBLE);
}

bool LLBC_Variant::IsStr() const
{
    return ((_holder.type & LLBC_VariantType::VT_STR_DFT) ==
        LLBC_VariantType::VT_STR_DFT);
}

bool LLBC_Variant::IsDict() const
{
    return ((_holder.type & LLBC_VariantType::VT_DICT_DFT) ==
        LLBC_VariantType::VT_DICT_DFT);
}

LLBC_Variant &LLBC_Variant::BecomeNil()
{
    if (IsNil())
        return *this;

    CleanTypeData(_holder.type);
    _holder.type = LLBC_VariantType::VT_NIL;

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeBool()
{
    if (!IsBool())
        *this = LLBC_Variant(AsBool());

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeInt8()
{
    if (!IsInt8())
        *this = AsInt8();

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeUInt8()
{
    if (!IsUInt8())
        *this = AsUInt8();

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeInt16()
{
    if (!IsInt16())
        *this = AsInt16();

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeUInt16()
{
    if (!IsUInt16())
        *this = AsUInt16();

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeInt32()
{
    if (!IsInt32())
        *this = AsInt32();

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeUInt32()
{
    if (!IsUInt32())
        *this = AsUInt32();

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeLong()
{
    if (!IsLong())
        *this = AsLong();

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeULong()
{
    if (!IsULong())
        *this = AsULong();

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeInt64()
{
    if (!IsInt64())
        *this = AsInt64();

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeUInt64()
{
    if (!IsUInt64())
        *this = AsUInt64();

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeFloat()
{
    if (!IsFloat())
        *this = AsFloat();

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeDouble()
{
    if (!IsDouble())
        *this = AsDouble();

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeStr()
{
    if (!IsStr())
        *this = AsStr();

    return *this;
}

LLBC_Variant &LLBC_Variant::BecomeDict()
{
    if (!IsDict())
        *this = AsDict();

    return *this;
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

sint8 LLBC_Variant::AsInt8() const
{
    return static_cast<sint8>(AsInt64());
}

uint8 LLBC_Variant::AsUInt8() const
{
    return static_cast<uint8>(AsUInt64());
}

sint16 LLBC_Variant::AsInt16() const
{
    return static_cast<sint16>(AsInt64());
}

uint16 LLBC_Variant::AsUInt16() const
{
    return static_cast<uint16>(AsUInt64());
}

sint32 LLBC_Variant::AsInt32() const
{
    return static_cast<sint32>(AsInt64());
}

uint32 LLBC_Variant::AsUInt32() const
{
    return static_cast<uint32>(AsUInt64());
}

long LLBC_Variant::AsLong() const
{
    return static_cast<long>(AsInt64());
}

unsigned long LLBC_Variant::AsULong() const
{
    return static_cast<unsigned long>(AsUInt64());
}

sint64 LLBC_Variant::AsInt64() const
{
    if (IsNil())
        return 0;
    else if (IsDict())
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
    else if (IsStr())
        return (_holder.obj.str && !_holder.obj.str->empty()) ? LLBC_Str2UInt64(_holder.obj.str->c_str()) : 0;

    if (IsDouble() || IsFloat())
        return static_cast<uint64>(_holder.raw.doubleVal);

    return _holder.raw.uint64Val;
}

float LLBC_Variant::AsFloat() const
{
    return static_cast<float>(AsDouble());
}

double LLBC_Variant::AsDouble() const
{
    if (IsNil())
        return 0.0;
    else if (IsDict())
        return 0.0;
    else if (IsStr())
        return (_holder.obj.str && !_holder.obj.str->empty()) ? LLBC_Str2Double(_holder.obj.str->c_str()) : 0;

    if (IsDouble() || IsFloat())
        return _holder.raw.doubleVal;

    if (IsSignedRaw())
        return static_cast<double>(_holder.raw.int64Val);

    return static_cast<double>(_holder.raw.uint64Val);
}

const char *LLBC_Variant::AsCStr() const
{
    return AsStr().c_str();
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

const Dict &LLBC_Variant::AsDict() const
{
    if (IsDict() && _holder.obj.dict)
        return *_holder.obj.dict;

    return LLBC_INL_NS __g_nullDict;
}

LLBC_Variant::operator bool() const
{
    return AsBool();
}

LLBC_Variant::operator sint8() const
{
    return AsInt8();
}

LLBC_Variant::operator uint8() const
{
    return AsUInt8();
}

LLBC_Variant::operator sint16() const
{
    return AsInt16();
}

LLBC_Variant::operator uint16() const
{
    return AsUInt16();
}

LLBC_Variant::operator sint32() const
{
    return AsInt32();
}

LLBC_Variant::operator uint32() const
{
    return AsUInt32();
}

LLBC_Variant::operator long() const
{
    return AsLong();
}

LLBC_Variant::operator ulong() const
{
    return AsULong();
}

LLBC_Variant::operator sint64() const
{
    return AsInt64();
}

LLBC_Variant::operator uint64() const
{
    return AsUInt64();
}

LLBC_Variant::operator float() const
{
    return AsFloat();
}

LLBC_Variant::operator double() const
{
    return AsDouble();
}

LLBC_Variant::operator const char *() const
{
    return AsCStr();
}

LLBC_Variant::operator LLBC_String () const
{
    return AsStr();
}

LLBC_Variant::operator const Dict &() const
{
    return AsDict();
}

DictIter LLBC_Variant::Begin()
{
    if (IsDict() && _holder.obj.dict)
        return _holder.obj.dict->begin();
    else
        return LLBC_INL_NS __g_nullDict.begin();
}

DictConstIter LLBC_Variant::Begin() const
{
    if (IsDict() && _holder.obj.dict)
        return _holder.obj.dict->begin();
    else
        return LLBC_INL_NS __g_nullDict.begin();
}

DictIter LLBC_Variant::End()
{
    if (IsDict() && _holder.obj.dict)
        return _holder.obj.dict->end();
    else
        return LLBC_INL_NS __g_nullDict.end();
}

DictConstIter LLBC_Variant::End() const
{
    if (IsDict() && _holder.obj.dict)
        return _holder.obj.dict->end();
    else
        return LLBC_INL_NS __g_nullDict.end();
}

DictReverseIter LLBC_Variant::ReverseBegin()
{
    if (IsDict() && _holder.obj.dict)
        return _holder.obj.dict->rbegin();
    else
        return LLBC_INL_NS __g_nullDict.rbegin();
}

DictConstReverseIter LLBC_Variant::ReverseBegin() const
{
    if (IsDict() && _holder.obj.dict)
        return _holder.obj.dict->rbegin();
    else
        return LLBC_INL_NS __g_nullDict.rbegin();
}

DictReverseIter LLBC_Variant::ReverseEnd()
{
    if (IsDict() && _holder.obj.dict)
        return _holder.obj.dict->rend();
    else
        return LLBC_INL_NS __g_nullDict.rend();
}

DictConstReverseIter LLBC_Variant::ReverseEnd() const
{
    if (IsDict() && _holder.obj.dict)
        return _holder.obj.dict->rend();
    else
        return LLBC_INL_NS __g_nullDict.rend();
}

std::pair<DictIter, bool> LLBC_Variant::Insert(const Dict::key_type &key, const Dict::mapped_type &val)
{
    return Insert(Dict::value_type(key, val));
}

std::pair<DictIter, bool> LLBC_Variant::Insert(const Dict::value_type &val)
{
    LLBC_INL_NS BecomeAndAllocDict(*this, _holder);
    return _holder.obj.dict->insert(val);
}

DictIter LLBC_Variant::Find(const Dict::key_type &key)
{
    if (IsDict() && _holder.obj.dict)
        return _holder.obj.dict->find(key);
    else
        return LLBC_INL_NS __g_nullDict.end();
}

DictConstIter LLBC_Variant::Find(const Dict::key_type &key) const
{
    if (IsDict() && _holder.obj.dict)
        return _holder.obj.dict->find(key);
    else
        return LLBC_INL_NS __g_nullDict.end();
}

void LLBC_Variant::Erase(DictIter it)
{
    if (IsDict() && _holder.obj.dict)
        _holder.obj.dict->erase(it);
}

Dict::size_type LLBC_Variant::Erase(const Dict::key_type &key)
{
    if (IsDict() && _holder.obj.dict)
        return _holder.obj.dict->erase(key);
    else
        return 0;
}

void LLBC_Variant::Erase(DictIter first, DictIter last)
{
    if (IsDict() && _holder.obj.dict)
        _holder.obj.dict->erase(first, last);
}

Dict::mapped_type &LLBC_Variant::operator [](const LLBC_Variant &key)
{
    LLBC_INL_NS BecomeAndAllocDict(*this, _holder);
    return (*_holder.obj.dict)[key];
}

const Dict::mapped_type &LLBC_Variant::operator [](const LLBC_Variant &key) const
{
    if (IsDict() && _holder.obj.dict)
        return (*_holder.obj.dict)[key];
    else
        return LLBC_INL_NS __g_nullDict[key];
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
            _holder.obj.str = LLBC_New1(LLBC_String, val);
    }

    return *this;
}

LLBC_Variant &LLBC_Variant::operator =(const Dict &val)
{
    if (!IsDict())
    {
        CleanTypeData(_holder.type);
        _holder.type = LLBC_VariantType::VT_DICT_DFT;
    }

    if (val.empty())
    {
        if (_holder.obj.dict)
            _holder.obj.dict->clear();
    }
    else
    {
        if (_holder.obj.dict)
            *_holder.obj.dict = val;
        else
            _holder.obj.dict = LLBC_New1(Dict, val);
    }

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
    else if (IsDict())
    {

        LLBC_String content;
        content.append("{");

        if (_holder.obj.dict)
        {
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
    else if (IsDict())
    {
        if (!_holder.obj.dict)
        {
            stream.Write(static_cast<uint32>(0));
        }
        else
        {
            stream.Write(static_cast<uint32>(_holder.obj.dict->size()));
            for (DictConstIter it = _holder.obj.dict->begin();
                 it != _holder.obj.dict->end();
                 it++)
            {
                stream.Write(it->first);
                stream.Write(it->second);
            }
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
            _holder.obj.str = LLBC_New0(LLBC_String);

        if (!stream.ReadEx(*_holder.obj.str))
        {
            LLBC_XDelete(_holder.obj.str);
            _holder.type = LLBC_VariantType::VT_NIL;

            return false;
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

        if (count == 0)
            return true;

        _holder.obj.dict = LLBC_New0(Dict);
        for (uint32 i = 0; i < count; i ++)
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

    case _Type::VT_DICT:
        CleanDictData();
        break;

    default:
        break;
    }
}

void LLBC_Variant::OptimizePerformance()
{
    if (IsStr())
    {
        if (_holder.obj.str && _holder.obj.str->empty())
            LLBC_XDelete(_holder.obj.str);
    }
    else if (IsDict())
    {
        if (_holder.obj.dict && _holder.obj.dict->empty())
            LLBC_XDelete(_holder.obj.dict);
    }
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
