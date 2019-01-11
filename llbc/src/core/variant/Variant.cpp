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
        _typeDescs.insert(std::make_pair(VT_RAW_PTR, "ptr"));
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
