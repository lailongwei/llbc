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

#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/variant/Variant.h"

__LLBC_NS_BEGIN

const LLBC_Variant LLBC_Variant::nil;

const LLBC_String &LLBC_VariantType::Type2Str(int type)
{
    static const std::array<LLBC_String, ALL_TYPES.size()> typeStrs {
        "Nil",

        "bool",
        "int8",
        "uint8",
        "int16",
        "uint16",
        "int32",
        "uint32",
        "long",
        "ulong",
        "pointer",
        "int64",
        "uint64",
        "float",
        "double",

        "string",

        "sequence",

        "dictionary",
    };

    switch (type)
    {
    case NIL:
        return typeStrs[0];
    
    case RAW_BOOL:
        return typeStrs[1];
    case RAW_SINT8:
        return typeStrs[2];
    case RAW_UINT8:
        return typeStrs[3];
    case RAW_SINT16:
        return typeStrs[4];
    case RAW_UINT16:
        return typeStrs[5];
    case RAW_SINT32:
        return typeStrs[6];
    case RAW_UINT32:
        return typeStrs[7];
    case RAW_LONG:
        return typeStrs[8];
    case RAW_ULONG:
        return typeStrs[9];
    case RAW_PTR:
        return typeStrs[10];
    case RAW_SINT64:
        return typeStrs[11];
    case RAW_UINT64:
        return typeStrs[12];
    case RAW_FLOAT:
        return typeStrs[13];
    case RAW_DOUBLE:
        return typeStrs[14];

    case STR_DFT:
        return typeStrs[15];

    case SEQ_DFT:
        return typeStrs[16];

    case DICT_DFT:
        return typeStrs[17];

    default:
        return typeStrs[0];
    }
}

__LLBC_NS_END

std::ostream &operator<<(std::ostream &o, const LLBC_NS LLBC_Variant &variant)
{
    LLBC_NS LLBC_String str = variant.ToString();
    o.write(str.c_str(), str.length());
    return o;
}

__LLBC_NS_BEGIN

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
    
    free(_num2StrFastAccessTbl);
    _num2StrFastAccessTbl = nullptr;
}

LLBC_Variant &LLBC_Variant::Become(LLBC_VariantType::ENUM type)
{
    if (_type != type)
    {
        Reset(type);
        if (type == LLBC_VariantType::STR_DFT)
            new (&_data.str()) Str;
        else if (type == LLBC_VariantType::SEQ_DFT)
            new (&_data.seq()) Seq;
        else if (type == LLBC_VariantType::DICT_DFT)
            new (&_data.dict()) Dict;
        else // Raw/Nil.
            _data.ui64() = 0;
    }

    return *this;
}

void LLBC_Variant::Clear()
{
    if (Is<Str>())
        _data.str().clear();
    else if (Is<Seq>())
        _data.seq().clear();
    else if (Is<Dict>())
        _data.dict().clear();
    else
        _data.i64() = 0;
}

bool LLBC_Variant::IsEmpty() const
{
    if (Is<Str>())
        return _data.str().empty();
    if (Is<Seq>())
        return _data.seq().empty();
    if (Is<Dict>())
        return _data.dict().empty();

    return true;
}

size_t LLBC_Variant::Size() const
{
    if (Is<Str>())
        return _data.str().size();
    if (Is<Seq>())
        return _data.seq().size();
    if (Is<Dict>())
        return _data.dict().size();

    return 0;
}

size_t LLBC_Variant::Capacity() const
{
    if (Is<Str>())
        return _data.str().capacity();
    if (Is<Seq>())
        return _data.seq().capacity();
    if (Is<Dict>())
        return _data.dict().size();

    return 0;
}

void LLBC_Variant::Serialize(LLBC_Stream &stream) const
{
    stream.Write(_type);

    if (IsRaw())
    {
        stream.Write(_data.ui64());
    }
    else if (Is<Str>())
    {
        stream.Write(_data.str());
    }
    else if (Is<Seq>())
    {
        stream.Write(static_cast<uint32>(_data.seq().size()));
        for (auto &item : _data.seq())
            stream.Write(item);
    }
    else if (Is<Dict>())
    {
        stream.Write(static_cast<uint32>(_data.dict().size()));
        for (auto &[key, value] : _data.dict())
        {
            stream.Write(key);
            stream.Write(value);
        }
    }
}

bool LLBC_Variant::Deserialize(LLBC_Stream &stream)
{
    LLBC_VariantType::ENUM streamType;
    if (!stream.Read(streamType))
        return false;

    if (_type != streamType)
        Become(streamType);

    if (Is<void>())
        return true;

    if (IsRaw())
    {
        if (UNLIKELY(!stream.Read(_data.ui64())))
        {
            Become<void>();
            return false;
        }

        return true;
    }
    else if (Is<Str>())
    {
        _data.str().clear();
        if (UNLIKELY(!stream.Read(_data.str())))
        {
            Become<void>();
            return false;
        }

        return true;
    }
    else if (Is<Seq>())
    {
        uint32 count = 0;
        if (UNLIKELY(!stream.Read(count)))
        {
            Become<void>();
            return false;
        }

        _data.seq().clear();
        if (count == 0)
            return true;

        for (uint32 i = 0; i < count; ++i)
        {
            LLBC_Variant val;
            if (UNLIKELY(!stream.Read(val)))
            {
                Become<void>();
                return false;
            }

            _data.seq().emplace_back(std::move(val));
        }

        return true;
    }
    else if (Is<Dict>())
    {
        uint32 count = 0;
        if (UNLIKELY(!stream.Read(count)))
        {
            Become<void>();
            return false;
        }

        _data.dict().clear();
        if (count == 0)
            return true;

        for (uint32 i = 0; i < count; ++i)
        {
            LLBC_Variant key;
            LLBC_Variant val;
            if (UNLIKELY(!stream.Read(key) || !stream.Read(val)))
            {
                Become<void>();
                return false;
            }

            _data.dict().emplace(std::move(key), std::move(val));
        }

        return true;
    }
    
    return false;
}

const LLBC_Variant::Str &LLBC_Variant::GetEmptyStr()
{
    static const Str emptyStr;
    return emptyStr;
}

const LLBC_Variant::Str &LLBC_Variant::GetTrueStr()
{
    static const Str trueStr("true");
    return trueStr;
}

const LLBC_Variant::Str &LLBC_Variant::GetFalseStr()
{
    static const Str falseStr("false");
    return falseStr;
}

const LLBC_Variant::Str &LLBC_Variant::GetEmptySeqStr()
{
    static const Str emptySeqStr("[]");
    return emptySeqStr;
}

const LLBC_Variant::Str &LLBC_Variant::GetEmptyDictStr()
{
    static const Str emptyDictStr("{}");
    return emptyDictStr;
}

const std::string &LLBC_Variant::GetEmptySTLStr()
{
    static const std::string emptySTLStr;
    return emptySTLStr;
}

const std::string &LLBC_Variant::GetTrueSTLStr()
{
    static const std::string trueSTLStr("true");
    return trueSTLStr;
}

const std::string &LLBC_Variant::GetFalseSTLStr()
{
    static const std::string falseSTLStr("false");
    return falseSTLStr;
}

const std::string &LLBC_Variant::GetEmptySTLSeqStr()
{
    static const std::string emptySTLSeqStr("[]");
    return emptySTLSeqStr;
}
const std::string &LLBC_Variant::GetEmptySTLDictStr()
{
    static const std::string emptySTLDictStr("{}");
    return emptySTLDictStr;
}

const LLBC_Variant::Seq &LLBC_Variant::GetEmptySeq()
{
    static const Seq emptySeq;
    return emptySeq;
}

const LLBC_Variant::Dict &LLBC_Variant::GetEmptyDict()
{
    static const Dict emptyDict;
    return emptyDict;
}

__LLBC_NS_END
