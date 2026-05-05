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

__LLBC_INTERNAL_NS_BEGIN

static std::unordered_map<LLBC_NS LLBC_VariantType::ENUM, LLBC_NS LLBC_String> __g_typeDescs {
    {LLBC_NS LLBC_VariantType::NIL, "Nil"},

    {LLBC_NS LLBC_VariantType::RAW_BOOL, "bool"},
    {LLBC_NS LLBC_VariantType::RAW_SINT8, "int8"},
    {LLBC_NS LLBC_VariantType::RAW_UINT8, "uint8"},
    {LLBC_NS LLBC_VariantType::RAW_SINT16, "int16"},
    {LLBC_NS LLBC_VariantType::RAW_UINT16, "uint16"},
    {LLBC_NS LLBC_VariantType::RAW_SINT32, "int32"},
    {LLBC_NS LLBC_VariantType::RAW_UINT32, "uint32"},
    {LLBC_NS LLBC_VariantType::RAW_LONG, "long"},
    {LLBC_NS LLBC_VariantType::RAW_ULONG, "ulong"},
    {LLBC_NS LLBC_VariantType::RAW_PTR, "pointer"},
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

const LLBC_Variant::Str LLBC_Variant::_trueStr("true");
const LLBC_Variant::Str LLBC_Variant::_falseStr("false");
const LLBC_Variant::Str LLBC_Variant::_emptySeqStr("[]");
const LLBC_Variant::Str LLBC_Variant::_emptyDictStr("{}");
const std::string LLBC_Variant::_trueSTLStr("true");
const std::string LLBC_Variant::_falseSTLStr("false");
const std::string LLBC_Variant::_emptySTLSeqStr("[]");
const std::string LLBC_Variant::_emptySTLDictStr("{}");

const LLBC_Variant::Str LLBC_Variant::_emptyStr;
const std::string LLBC_Variant::_emptySTLStr;
const LLBC_Variant::Seq LLBC_Variant::_emptySeq;
const LLBC_Variant::Dict LLBC_Variant::_emptyDict;



const LLBC_String &LLBC_VariantType::Type2Str(int type)
{
    const auto it = LLBC_INL_NS __g_typeDescs.find(static_cast<ENUM>(type));
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

__LLBC_NS_END
