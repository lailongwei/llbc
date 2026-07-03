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

#pragma once

#include "llbc/core/algo/Hash.h"
#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/variant/VariantTraits.h"

// Define: Variant pure type macro.
// Note: std::remove_cvref supported since C++20, llbc compatible C++17.
#if __cplusplus >= 202002L
#define __LLBC_INL_Var_PureType(_Type) typedef std::remove_cvref_t<_Type> _PureTy
#else
#define __LLBC_INL_Var_PureType(_Type) typedef std::remove_cv_t<std::remove_reference_t<_Type>> _PureTy
#endif
__LLBC_NS_BEGIN

template <typename _Ty>
constexpr bool LLBC_VariantType::IsConvertable()
{
    typedef std::remove_cv_t<std::remove_reference_t<_Ty>> _PureTy;

    if constexpr (std::is_same_v<_PureTy, void> ||
                  IsSupportedIntegralType<_PureTy>() ||
                  IsSupportedEnumType<_PureTy>() ||
                  IsSupportedFloatingPointType<_PureTy>() ||
                  std::is_null_pointer_v<_PureTy> ||
                  std::is_pointer_v<_PureTy> ||
                  std::is_array_v<_PureTy>)
    {
        return true;
    }
    else if constexpr (std::is_same_v<_PureTy, std::string> ||
                       std::is_same_v<_PureTy, std::string_view> ||
                       std::is_same_v<_PureTy, LLBC_String> ||
                       std::is_same_v<_PureTy, LLBC_CString>)
    {
        return true;
    }
    else if constexpr (LLBC_IsTemplSpec<_PureTy, std::pair>::value)
    {
        return IsConvertable<typename _PureTy::first_type>() &&
                IsConvertable<typename _PureTy::second_type>();
    }
    else if constexpr (LLBC_IsTemplSpec<_PureTy, std::vector>::value ||
                       LLBC_IsTemplSpec<_PureTy, std::list>::value ||
                       LLBC_IsTemplSpec<_PureTy, std::queue>::value ||
                       LLBC_IsTemplSpec<_PureTy, std::deque>::value ||
                       LLBC_IsTemplSpec<_PureTy, std::set>::value ||
                       LLBC_IsTemplSpec<_PureTy, std::unordered_set>::value)
    {
        return IsConvertable<typename _PureTy::value_type>();
    }
    else if constexpr (LLBC_IsTemplSpec<_PureTy, std::map>::value ||
                       LLBC_IsTemplSpec<_PureTy, std::unordered_map>::value)
    {
        return IsConvertable<typename _PureTy::key_type>() &&
                IsConvertable<typename _PureTy::mapped_type>();
    }
    else if constexpr (std::is_same_v<_PureTy, LLBC_Variant>)
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <typename _Ty>
constexpr LLBC_VariantType::ENUM LLBC_VariantType::DeduceType()
{
    typedef std::remove_cv_t<std::remove_reference_t<_Ty>> _PureTy;

    if constexpr (std::is_same_v<_PureTy, void>) // nil
    {
        return NIL;
    }
    else if constexpr (std::is_same_v<_PureTy, bool>) // bool
    {
        return RAW_BOOL;
    }
    else if constexpr (std::is_same_v<_PureTy, sint8>) // sint8
    {
        return RAW_SINT8;
    }
    else if constexpr (std::is_same_v<_PureTy, uint8>) // uint8
    {
        return RAW_UINT8;
    }
    else if constexpr (std::is_same_v<_PureTy, sint16>) // sint16
    {
        return RAW_SINT16;
    }
    else if constexpr (std::is_same_v<_PureTy, uint16>) // uint16
    {
        return RAW_UINT16;
    }
    else if constexpr (std::is_same_v<_PureTy, sint32>) // sint32
    {
        return RAW_SINT32;
    }
    else if constexpr (std::is_same_v<_PureTy, uint32>) // uint32
    {
        return RAW_UINT32;
    }
    else if constexpr (std::is_same_v<_PureTy, long>) // long
    {
        return RAW_LONG;
    }
    else if constexpr (std::is_same_v<_PureTy, ulong>) // ulong
    {
        return RAW_ULONG;
    }
    else if constexpr (std::is_null_pointer_v<_PureTy>) // nullptr_t
    {
        return RAW_PTR;
    }
    else if constexpr (std::is_array_v<_PureTy>) // string array/non-string array:
    {
        typedef std::remove_cv_t<std::remove_extent_t<_PureTy>> _ArrayElemTy;
        if constexpr (std::is_same_v<_ArrayElemTy, char>)
            return STR_DFT;
        else
            return RAW_PTR;
    }
    else if constexpr (std::is_pointer_v<_PureTy>) // string ptr/non-string ptr:
    {
        typedef std::remove_cv_t<std::remove_pointer_t<_PureTy>> _PtrElemTy;
        if constexpr (std::is_same_v<_PtrElemTy, char>)
            return STR_DFT;
        else
            return RAW_PTR;
    }
    else if constexpr (std::is_same_v<_PureTy, sint64>) // sint64
    {
        return RAW_SINT64;
    }
    else if constexpr (std::is_same_v<_PureTy, uint64>) // uint64
    {
        return RAW_UINT64;
    }
    else if constexpr (std::is_enum_v<_PureTy>) // enum
    {
        return DeduceType<std::underlying_type_t<_PureTy>>();
    }
    else if constexpr (std::is_same_v<_PureTy, float>) // float
    {
        return RAW_FLOAT;
    }
    else if constexpr (std::is_same_v<_PureTy, double>) // double
    {
        return RAW_DOUBLE;
    }
    else if constexpr (std::is_same_v<_PureTy, std::string> ||
                       std::is_same_v<_PureTy, std::string_view> ||
                       std::is_same_v<_PureTy, LLBC_String> ||
                       std::is_same_v<_PureTy, LLBC_CString>) // str
    {
        return STR_DFT;
    }
    else if constexpr ((LLBC_IsTemplSpec<_PureTy, std::pair>::value ||
                        LLBC_IsTemplSpec<_PureTy, std::vector>::value ||
                        LLBC_IsTemplSpec<_PureTy, std::list>::value ||
                        LLBC_IsTemplSpec<_PureTy, std::queue>::value ||
                        LLBC_IsTemplSpec<_PureTy, std::deque>::value ||
                        LLBC_IsTemplSpec<_PureTy, std::set>::value ||
                        LLBC_IsTemplSpec<_PureTy, std::unordered_set>::value) &&
                       IsConvertable<_Ty>()) // seq
    {
        return SEQ_DFT;
    }
    else if constexpr ((LLBC_IsTemplSpec<_PureTy, std::map>::value ||
                        LLBC_IsTemplSpec<_PureTy, std::unordered_map>::value) &&
                       IsConvertable<_Ty>()) // dict
    {
        return DICT_DFT;
    }
    else // !!! Unsupported type, return NIL.
    {
        return NIL;
    }
}

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::NIL>
{
    typedef void Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_BOOL>
{
    typedef bool Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_SINT8>
{
    typedef sint8 Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_UINT8>
{
    typedef uint8 Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_SINT16>
{
    typedef sint16 Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_UINT16>
{
    typedef uint16 Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_SINT32>
{
    typedef sint32 Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_UINT32>
{
    typedef uint32 Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_LONG>
{
    typedef long Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_ULONG>
{
    typedef ulong Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_PTR>
{
    typedef void * Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_SINT64>
{
    typedef sint64 Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_UINT64>
{
    typedef uint64 Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_FLOAT>
{
    typedef float Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_DOUBLE>
{
    typedef double Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::STR_DFT>
{
    typedef typename LLBC_Variant::Str Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::SEQ_DFT>
{
    typedef typename LLBC_Variant::Seq Class;
};

template <>
struct LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::DICT_DFT>
{
    typedef typename LLBC_Variant::Dict Class;
};

inline constexpr bool LLBC_VariantType::IsValid(int type)
{
    for (auto &varType : ALL_TYPES)
    {
        if (type == varType)
            return true;
    }

    return false;
}

template <typename _Ty>
constexpr bool LLBC_VariantType::IsNil()
{
    return DeduceType<_Ty>() == NIL;
}

template <typename _Ty>
constexpr bool LLBC_VariantType::IsRaw()
{
    return IsRaw(DeduceType<_Ty>());
}

template <typename _Ty>
constexpr bool LLBC_VariantType::IsSigned()
{
    return IsSigned(DeduceType<_Ty>());
}

template <typename _Ty>
constexpr bool LLBC_VariantType::IsUnsigned()
{
    return IsUnsigned(DeduceType<_Ty>());
}

template <typename _Ty>
constexpr bool LLBC_VariantType::IsStr()
{
    return IsStr(DeduceType<_Ty>());
}

template <typename _Ty>
constexpr bool LLBC_VariantType::IsSeq()
{
    return IsSeq(DeduceType<_Ty>());
}

template <typename _Ty>
constexpr bool LLBC_VariantType::IsDict()
{
    return IsDict(DeduceType<_Ty>());
}

template<typename _Ty>
constexpr bool LLBC_VariantType::IsSupportedIntegralType()
{
    // Unsupported integral types:
    // - signed char
    // - wchar_t
    // - char16_t
    // - char32_t

    typedef std::remove_cv_t<std::remove_reference_t<_Ty>> _PureTy;
    if constexpr (!std::is_integral_v<_PureTy>)
        return false;

    return std::is_same_v<_PureTy, bool> ||
        std::is_same_v<_PureTy, sint8> ||
        std::is_same_v<_PureTy, uint8> ||
        std::is_same_v<_PureTy, sint16> ||
        std::is_same_v<_PureTy, uint16> ||
        std::is_same_v<_PureTy, sint32> ||
        std::is_same_v<_PureTy, uint32> ||
        std::is_same_v<_PureTy, long> ||
        std::is_same_v<_PureTy, ulong> ||
        std::is_same_v<_PureTy, sint64> ||
        std::is_same_v<_PureTy, uint64>;
}

template<typename _Ty>
constexpr bool LLBC_VariantType::IsSupportedEnumType()
{
    typedef std::remove_cv_t<std::remove_reference_t<_Ty>> _PureTy;
    if constexpr (!std::is_enum_v<_PureTy>)
    {
        return false;
    }
    else
    {
        typedef std::underlying_type_t<_PureTy> _UnderlyingTy;
        return IsSupportedIntegralType<_UnderlyingTy>();
    }
}

template <typename _Ty>
constexpr bool LLBC_VariantType::IsSupportedFloatingPointType()
{
    // Unsupported floating point types:
    // - long double 

    typedef std::remove_cv_t<std::remove_reference_t<_Ty>> _PureTy;
    if constexpr (!std::is_floating_point_v<_PureTy>)
        return false;

    return std::is_same_v<_PureTy, float> ||
        std::is_same_v<_PureTy, double>;
}

template <typename _Ty>
const LLBC_String &LLBC_VariantType::Type2Str()
{
    return Type2Str(DeduceType<_Ty>());
}

inline LLBC_Variant::LLBC_Variant()
: _type(LLBC_VariantType::NIL)
{
    _data.i64() = 0;
}

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsRaw<_Ty>(), int>>
LLBC_Variant::LLBC_Variant(const _Ty &raw)
{
    ConstructOrAssignFromRaw<_Ty, true>(raw);
}

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsStr<_Ty>(), int>>
LLBC_Variant::LLBC_Variant(_Ty &&str)
{
    ConstructOrAssignFromStr<_Ty, true>(std::forward<_Ty>(str));
}

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsSeq<_Ty>(), int>>
LLBC_Variant::LLBC_Variant(_Ty &&seq)
{
    ConstructOrAssignFromSeq<_Ty, true>(std::forward<_Ty>(seq));
}

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsDict<_Ty>(), int>>
LLBC_Variant::LLBC_Variant(_Ty &&dict)
{
    ConstructOrAssignFromDict<_Ty, true>(std::forward<_Ty>(dict));
}

inline LLBC_Variant::LLBC_Variant(const LLBC_Variant &var)
: _type(var._type)
{
    if (_type == LLBC_VariantType::STR_DFT)
        new (&_data.str()) Str(var._data.str());
    else if (_type == LLBC_VariantType::SEQ_DFT)
        new (&_data.seq()) Seq(var._data.seq());
    else if (_type == LLBC_VariantType::DICT_DFT)
        new (&_data.dict()) Dict(var._data.dict());
    else // Raw/Nil.
        _data.ui64() = var._data.ui64();
}

inline LLBC_Variant::LLBC_Variant(LLBC_Variant &&var) noexcept
: _type(var._type)
{
    if (_type == LLBC_VariantType::STR_DFT)
        new (&_data.str()) Str(std::move(var._data.str()));
    else if (_type == LLBC_VariantType::SEQ_DFT)
        new (&_data.seq()) Seq(std::move(var._data.seq()));
    else if (_type == LLBC_VariantType::DICT_DFT)
        new (&_data.dict()) Dict(std::move(var._data.dict()));
    else // Raw/Nil.
        _data.ui64() = var._data.ui64();
}

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsRaw<_Ty>(), int>>
LLBC_Variant &LLBC_Variant::operator=(const _Ty &raw)
{
    ConstructOrAssignFromRaw<_Ty, false>(raw);
    return *this;
}

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsStr<_Ty>(), int>>
LLBC_Variant &LLBC_Variant::operator=(_Ty &&str)
{
    ConstructOrAssignFromStr<_Ty, false>(std::forward<_Ty>(str));
    return *this;
}

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsSeq<_Ty>(), int>>
LLBC_Variant &LLBC_Variant::operator=(_Ty &&seq)
{
    ConstructOrAssignFromSeq<_Ty, false>(std::forward<_Ty>(seq));
    return *this;
}

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsDict<_Ty>(), int>>
LLBC_Variant &LLBC_Variant::operator=(_Ty &&dict)
{
    ConstructOrAssignFromDict<_Ty, false>(std::forward<_Ty>(dict));
    return *this;
}

inline LLBC_Variant &LLBC_Variant::operator=(const LLBC_Variant &var)
{
    if (UNLIKELY(this == &var))
        return *this;

    if (_type == var._type)
    {
        if (Is<Str>())
            _data.str() = var._data.str();
        else if (Is<Seq>())
            _data.seq() = var._data.seq();
        else if (Is<Dict>())
            _data.dict() = var._data.dict();
        else
            _data.ui64() = var._data.ui64();
    }
    else
    {
        ReleaseDataAndOnlySetType<false>(var._type);
        if (Is<Str>())
            new (&_data.str()) Str(var._data.str());
        else if (Is<Seq>())
            new (&_data.seq()) Seq(var._data.seq());
        else if (Is<Dict>())
            new (&_data.dict()) Dict(var._data.dict());
        else
            _data.ui64() = var._data.ui64();
    }

    return *this;
}

inline LLBC_Variant &LLBC_Variant::operator=(LLBC_Variant &&var) noexcept
{
    if (UNLIKELY(this == &var))
        return *this;

    if (_type == var._type)
    {
        if (Is<Str>())
            _data.str() = std::move(var._data.str());
        else if (Is<Seq>())
            _data.seq() = std::move(var._data.seq());
        else if (Is<Dict>())
            _data.dict() = std::move(var._data.dict());
        else
            _data.ui64() = var._data.ui64();
    }
    else
    {
        ReleaseDataAndOnlySetType<false>(var._type);
        if (Is<Str>())
            new (&_data.str()) Str(std::move(var._data.str()));
        else if (Is<Seq>())
            new (&_data.seq()) Seq(std::move(var._data.seq()));
        else if (Is<Dict>())
            new (&_data.dict()) Dict(std::move(var._data.dict()));
        else
            _data.ui64() = var._data.ui64();
    }

    return *this;
}

template <typename... _Tys>
bool LLBC_Variant::Is() const
{
    if constexpr (sizeof...(_Tys) == 0)
        return false;
    else
        return (... || (_type == LLBC_VariantType::DeduceType<_Tys>()));
}

template <typename _Ty>
std::enable_if_t<LLBC_VariantType::IsConvertable<_Ty>(), LLBC_Variant &>
LLBC_Variant::Become()
{
    __LLBC_INL_Var_PureType(_Ty);
    if constexpr (std::is_same_v<_PureTy, LLBC_Variant>)
        return *this;
    else
        return Become(LLBC_VariantType::DeduceType<_Ty>());
}

template <typename _Ty>
std::enable_if_t<!std::is_reference_v<_Ty> &&
                    LLBC_VariantType::IsRaw<_Ty>() &&
                    !std::is_array_v<_Ty>,
                 _Ty>
LLBC_Variant::As() const
{
    __LLBC_INL_Var_PureType(_Ty);

    // xxx -> nullptr_t:
    if constexpr (std::is_null_pointer_v<_PureTy>)
        return nullptr;

    // xxx -> integral:
    if constexpr (LLBC_VariantType::IsSupportedIntegralType<_PureTy>())
    {
        // - xxx -> bool:
        if constexpr (std::is_same_v<_PureTy, bool>)
        {
            if (IsRaw())
                return Is<float, double>() ? !LLBC_IsFloatZero(As<double>()) : _data.ui64() != 0;
            else if (Is<Str>())
                return !_data.str().empty();
            else if (Is<Seq>())
                return !_data.seq().empty();
            else if (Is<Dict>())
                return !_data.dict().empty();
            else // Nil
                return false;
        }

        // - xxx -> signed integral:
        if constexpr (std::is_signed_v<_PureTy>)
            return static_cast<_PureTy>(AsSignedOrUnsigned64<sint64>());
        else // - xxx -> unsigned integral:
            return static_cast<_PureTy>(AsSignedOrUnsigned64<uint64>());
    }

    // xxx -> floating point:
    if constexpr (LLBC_VariantType::IsSupportedFloatingPointType<_PureTy>())
    {
        // - raw -> floating point:
        if (IsRaw())
        {
            if (Is<float, double>())
                return static_cast<_Ty>(_data.dbl());
            else if (IsSigned())
                return static_cast<_Ty>(_data.i64());
            else
                return static_cast<_Ty>(_data.ui64());
        }
 
        // str -> floating point:
        if (Is<Str>())
        {
            if (_data.str().empty())
                return .0;
            
            // TODO: Will optimize(erase str().strip() call) after LLBC_Num2Str<>() support "xxx  " format convert.
            if (_data.str().find('.') != LLBC_String::npos ||
                _data.str().find('e') != LLBC_String::npos ||
                _data.str().find('E') != LLBC_String::npos)
                return LLBC_Str2Num<_Ty>(_data.str().strip().c_str());
            
            if (_data.str().find('-') != LLBC_String::npos)
                return static_cast<_Ty>(AsSignedOrUnsigned64<sint64>());
            else
                return static_cast<_Ty>(AsSignedOrUnsigned64<uint64>());
        }
        
       // - nil/seq/dict -> floating point:
        return 0.0;
    }

    // xxx -> pointer:
    if constexpr (std::is_pointer_v<_PureTy>)
        return reinterpret_cast<_Ty>(As<uint64>());

    // xxx -> enum:
    if constexpr (LLBC_VariantType::IsSupportedEnumType<_PureTy>())
    {
        if (std::is_signed_v<std::underlying_type_t<_PureTy>>)
            return static_cast<_Ty>(As<sint64>());
        else
            return static_cast<_Ty>(As<uint64>());
    }

    llbc_assert(false && "Can not convert variant to this RAW type");

    return _Ty();
}

// Define internal macro: Num -> Str.
#define __LLBC_Inl_Variant_AsStr_Num2Str(num, toHexStr)           \
    {                                                             \
        if constexpr (std::is_same_v<_PureTy, Str>) {             \
            return LLBC_Num2Str<                                  \
                std::remove_cv_t<                                 \
                    std::remove_reference_t<                      \
                        decltype(num)>>, toHexStr>(num);          \
        } else {                                                  \
            _PureTy str;                                          \
            size_t strLen;                                        \
            const auto num2StrRet = LLBC_Num2Str2<                \
                std::remove_cv_t<                                 \
                    std::remove_reference_t<                      \
                        decltype(num)>>, toHexStr>(num, &strLen); \
            if (LIKELY(num2StrRet))                               \
                str.assign(num2StrRet, strLen);                   \
            return str;                                           \
        }                                                         \
    }                                                             \

template <typename _Ty>
std::enable_if_t<!std::is_reference_v<_Ty> &&
                    (std::is_same_v<std::remove_cv_t<_Ty>, std::string> ||
                     std::is_same_v<std::remove_cv_t<_Ty>, LLBC_String>),
                 _Ty>
LLBC_Variant::As() const
{
    __LLBC_INL_Var_PureType(_Ty);

    if (IsRaw())
    {
        if (Is<bool>())
            return _data.ui64() != 0 ? GetTrueStr<_PureTy>() : GetFalseStr<_PureTy>();

        if (Is<float, double>())
            __LLBC_Inl_Variant_AsStr_Num2Str(_data.dbl(), false);

        static_assert(LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN < 0 &&
                          LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_END > 0,
                      "LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN and "
                      "LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_END must be negative and positive");

        if (LIKELY(_num2StrFastAccessTbl) &&
                   ((IsSigned() &&
                     (_data.i64() >= static_cast<sint64>(LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN) &&
                      _data.i64() <= static_cast<sint64>(LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_END))) ||
                    (IsUnsigned() &&
                     !Is<void *>() &&
                     (_data.ui64() >= 0ull &&
                      _data.ui64() <= static_cast<uint64>(LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_END)))))
        {
            if (IsSigned())
                return *_num2StrFastAccessTbl[
                    static_cast<int>(_data.i64()) - LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN];
            else
                return *_num2StrFastAccessTbl[
                    static_cast<int>(_data.ui64()) - LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN];
        }

        if (IsSigned())
        {
            __LLBC_Inl_Variant_AsStr_Num2Str(_data.i64(), false);
        }
        else
        {
            if (Is<void *>())
            {
                __LLBC_Inl_Variant_AsStr_Num2Str(_data.ui64(), true)
            }
            else
            {
                __LLBC_Inl_Variant_AsStr_Num2Str(_data.ui64(), false)
            }
        }
    }

    if (Is<Str>())
        return _data.str();

    if (Is<Seq>())
    {
        if (_data.seq().empty())
            return GetEmptySeqStr<_PureTy>();

        _PureTy content;
        content.reserve(16);
        content.append(1, '[');
        for (auto it = _data.seq().begin(); it != _data.seq().end(); )
        {
            if (it->Is<Str>())
                content.append(it->_data.str().data(), it->_data.str().size());
            else
                content.append(it->As<_PureTy>());

            if (++it != _data.seq().end())
                content.append(1, ',');
        }

        content.append(1, ']');
        return content;
    }

    if (Is<Dict>())
    {
        if (_data.dict().empty())
            return GetEmptyDictStr<_PureTy>();

        _PureTy content;
        content.reserve(32);
        content.append(1, '{');
        for (auto it = _data.dict().begin(); it != _data.dict().end(); )
        {
            if (it->first.Is<Str>())
                content.append(it->first._data.str().data(), it->first._data.str().size());
            else
                content.append(it->first.As<_PureTy>());

            content.append(1, ':');

            if (it->second.Is<Str>())
                content.append(it->second._data.str().data(), it->second._data.str().size());
            else
                content.append(it->second.As<_PureTy>());

            if (++it != _data.dict().end())
                content.append(1, ',');
        }

        content.append(1, '}');
        return content;
    }

    return GetEmptyStr<_PureTy>();
}

// Undefine Num -> Str internal macro.
#undef __LLBC_Inl_Variant_AsStr_Num2Str

template <typename _Ty>
std::enable_if_t<!std::is_reference_v<_Ty> &&
                    (std::is_same_v<std::remove_cv_t<_Ty>, std::string_view> ||
                     std::is_same_v<std::remove_cv_t<_Ty>, LLBC_CString>),
                  _Ty>
LLBC_Variant::As() const
{
    __LLBC_INL_Var_PureType(_Ty);

    // Str: Convert to _PureTy.
    if (Is<Str>())
        return _PureTy(_data.str().data(), _data.str().size());

    // Nil: return empty _PureTy.
    if (Is<void>())
        return _PureTy();

    // Raw/Seq/Dict: As<Str>() && Convert to _PureTy.
    static thread_local Str strRepr;
    strRepr = As<Str>();
    return _PureTy(strRepr.data(), strRepr.size());
}

template <typename _Ty>
std::enable_if_t<!std::is_reference_v<_Ty> &&
                    LLBC_VariantType::IsStr<_Ty>() &&
                    (std::is_pointer_v<_Ty> &&
                     std::is_same_v<std::remove_cv_t<std::remove_pointer_t<_Ty>>, char>),
                 _Ty>
LLBC_Variant::As(size_t *strLen) const
{
    const Str *strPtr;
    if (Is<Str>())
    {
        strPtr = &_data.str();
    }
    else
    {
        static thread_local Str strRepr;
        strRepr = As<Str>();
        strPtr = &strRepr;
    } 

    if (strLen)
        *strLen = strPtr->size();

    if constexpr (std::is_const_v<std::remove_pointer_t<_Ty>>)
        return strPtr->c_str();
    else
        return const_cast<char *>(strPtr->c_str());
}

template <typename _Ty>
std::enable_if_t<!std::is_reference_v<_Ty> &&
                    !std::is_same_v<std::remove_cv_t<_Ty>, LLBC_Variant::Seq> &&
                    LLBC_VariantType::IsSeq<_Ty>(),
                  _Ty>
LLBC_Variant::As() const
{
    __LLBC_INL_Var_PureType(_Ty);

    if constexpr (LLBC_IsTemplSpec<_Ty, std::pair>::value)
    {
        if (Is<Seq>() && !_data.seq().empty())
        {
            if (_data.seq().size() >= 2)
                return {_data.seq()[0], _data.seq()[1]};
            else
                return {_data.seq()[0], typename _PureTy::second_type{}};
        }
        else
        {
            return {typename _PureTy::first_type(), typename _PureTy::second_type()};
        }
    }
    else
    {
        if constexpr (LLBC_IsTemplSpec<_PureTy, std::vector>::value ||
                      LLBC_IsTemplSpec<_PureTy, std::list>::value ||
                      LLBC_IsTemplSpec<_PureTy, std::deque>::value)
        {
            _PureTy retSeq;
            if (Is<Seq>() && !_data.seq().empty())
            {
                if constexpr (LLBC_IsTemplSpec<_PureTy, std::vector>::value)
                    retSeq.reserve(_data.seq().size());

                for (auto &item : _data.seq())
                    retSeq.emplace_back(item.As<typename _PureTy::value_type>());
            }

            return retSeq;
        }
        else if constexpr (LLBC_IsTemplSpec<_PureTy, std::queue>::value ||
                           LLBC_IsTemplSpec<_PureTy, std::set>::value ||
                           LLBC_IsTemplSpec<_PureTy, std::unordered_set>::value)
        {
            _PureTy retSeq;
            if (Is<Seq>())
            {
                for (auto &item : _data.seq())
                    retSeq.emplace(item.As<typename _PureTy::value_type>());
            }

            return retSeq;
        }

        llbc_assert(false && "Unconvertable SEQ type");
        return _Ty();
    }
}

template <typename _Ty>
std::enable_if_t<!std::is_reference_v<_Ty> &&
                    !std::is_same_v<std::remove_cv_t<_Ty>, LLBC_Variant::Dict> &&
                    LLBC_VariantType::IsDict<_Ty>(),
                 _Ty>
LLBC_Variant::As() const
{
    __LLBC_INL_Var_PureType(_Ty);

    _PureTy retDict;
    if (Is<Dict>())
    {
        for (auto &[key, value] : _data.dict())
            retDict.emplace(key.template As<typename _PureTy::key_type>(),
                            value.template As<typename _PureTy::mapped_type>());
    }

    return retDict;
}

inline bool LLBC_Variant::AsLooseBool() const
{
    return (Is<Str>() && !_data.str().empty()) ? LLBC_Str2LooseBool(_data.str().c_str(), 10, true) : As<bool>();
}

inline void LLBC_Variant::StrReserve(Str::size_type newCap)
{
    Become<Str>()._data.str().reserve(newCap);
}

inline LLBC_Variant::SeqIter LLBC_Variant::SeqBegin()
{
    return Become<Seq>()._data.seq().begin();
}

inline LLBC_Variant::SeqIter LLBC_Variant::SeqEnd()
{
    return Become<Seq>()._data.seq().end();
}

inline LLBC_Variant::SeqReverseIter LLBC_Variant::SeqReverseBegin()
{
    return Become<Seq>()._data.seq().rbegin();
}

inline LLBC_Variant::SeqReverseIter LLBC_Variant::SeqReverseEnd()
{
    return Become<Seq>()._data.seq().rend();
}

inline LLBC_Variant::Seq::reference LLBC_Variant::SeqFront()
{
    return Become<Seq>()._data.seq().front();
}

inline LLBC_Variant::Seq::reference LLBC_Variant::SeqBack()
{
    return Become<Seq>()._data.seq().back();
}

inline void LLBC_Variant::StrResize(Str::size_type newSize, Str::value_type ch)
{
    Str &str = Become<Str>()._data.str();
    const Str::size_type oldSize = str.size();

    str.resize(newSize);

    if (newSize > oldSize && ch != Str::value_type())
        std::fill(str.begin() + oldSize, str.end(), ch);
}

template <typename _Ty>
inline LLBC_Variant::SeqIter LLBC_Variant::SeqInsert(SeqIter it, _Ty &&val)
{
    if (UNLIKELY(!Is<Seq>()))
        return Become<Seq>()._data.seq().end();

    return _data.seq().emplace(it, std::forward<_Ty>(val));
}

template <typename _Ty>
LLBC_Variant::SeqIter LLBC_Variant::SeqInsert(SeqIter it, Seq::size_type n, const _Ty &val)
{
    if (UNLIKELY(!Is<Seq>()))
        return Become<Seq>()._data.seq().end();

    if constexpr (std::is_same_v<_Ty, LLBC_Variant>)
        return _data.seq().insert(it, n, val);
    else
        return _data.seq().insert(it, n, LLBC_Variant(val));
}

inline LLBC_Variant::SeqIter LLBC_Variant::SeqInsert(SeqIter it, SeqConstIter first, SeqConstIter last)
{
    if (UNLIKELY(!Is<Seq>()))
        return Become<Seq>()._data.seq().end();

    return _data.seq().insert(it, first, last);
}

template <typename... _Tys>
LLBC_Variant::SeqIter LLBC_Variant::SeqBatchInsert(SeqIter it, _Tys &&... vals)
{
    if (UNLIKELY(!Is<Seq>()))
        return Become<Seq>()._data.seq().end();

    if constexpr (sizeof...(vals) == 0)
    {
        return it;
    }
    else
    {
        const auto beginInsertPos = std::distance(_data.seq().begin(), it);
        auto curInsertPos = beginInsertPos;
        (SeqInsert(_data.seq().begin() + curInsertPos++, std::forward<_Tys>(vals)), ...);

        return _data.seq().begin() + beginInsertPos;
    }
}

template <typename _Ty>
void LLBC_Variant::SeqPushBack(_Ty &&val)
{
    Become<Seq>()._data.seq().emplace_back(std::forward<_Ty>(val));
}

inline void LLBC_Variant::SeqPopBack()
{
    Become<Seq>();
    if (!_data.seq().empty())
        _data.seq().pop_back();
}

template <typename... _Tys>
void LLBC_Variant::SeqBatchPushBack(_Tys &&... vals)
{
    Become<Seq>();
    (_data.seq().emplace_back(std::forward<_Tys>(vals)), ...);
}

template <typename _Ty>
void LLBC_Variant::SeqResize(Seq::size_type newSize, const _Ty &val)
{
    if constexpr (std::is_same_v<_Ty, LLBC_Variant>)
        Become<Seq>()._data.seq().resize(newSize, val);
    else
        SeqResize(newSize, LLBC_Variant(val));
}

inline void LLBC_Variant::SeqReserve(Seq::size_type newCap)
{
    Become<Seq>()._data.seq().reserve(newCap);
}

inline LLBC_Variant::SeqIter LLBC_Variant::SeqErase(SeqIter it)
{
    if (!Is<Seq>())
        return Become<Seq>()._data.seq().end();
    return _data.seq().erase(it);
}

inline LLBC_Variant::SeqIter LLBC_Variant::SeqErase(SeqConstIter it)
{
    if (!Is<Seq>())
        return Become<Seq>()._data.seq().end();
    return _data.seq().erase(it);
}

inline LLBC_Variant::SeqIter LLBC_Variant::SeqErase(SeqIter first, SeqIter last)
{
    if (!Is<Seq>())
        return Become<Seq>()._data.seq().end();
    return _data.seq().erase(first, last);
}

inline LLBC_Variant::SeqIter LLBC_Variant::SeqErase(SeqConstIter first, SeqConstIter last)
{
    if (!Is<Seq>())
        return Become<Seq>()._data.seq().end();
    return _data.seq().erase(first, last);
}

template <typename _Ty>
size_t LLBC_Variant::SeqErase(const _Ty &val,
                              size_t eraseCount,
                              bool fromBegin,
                              bool stableErase)
{
    return SeqEraseIf([&val](const Seq::value_type &elem) { return elem == val; },
                      eraseCount,
                      fromBegin,
                      stableErase);
}

template <typename _UnaryPred>
size_t LLBC_Variant::SeqEraseIf(const _UnaryPred &pred,
                                size_t eraseCount,
                                bool fromBegin,
                                bool stableErase)
{
    if (!Is<Seq>())
    {
        Become<Seq>();
        return 0;
    }

    if (_data.seq().empty() || UNLIKELY(eraseCount == 0))
        return 0;

    size_t erasedCount = 0;
    Seq &seq = _data.seq();
    sint64 pos, endPos, step;
    if (fromBegin)
    {
        pos = 0;
        endPos = static_cast<sint64>(seq.size());
        step = 1;
    }
    else
    {
        pos = static_cast<sint64>(seq.size()) - 1;
        endPos = -1;
        step = -1;
    }

    constexpr size_t infEraseCount = static_cast<size_t>(LLBC_INFINITE);
    for (;
         pos != endPos && (eraseCount == infEraseCount || erasedCount < eraseCount);
         )
    {
        if (!pred(seq[pos]))
        {
            pos += step;
            continue;
        }

        if (stableErase)
        {
            seq.erase(seq.begin() + pos);
        }
        else
        {
            if (static_cast<size_t>(pos) != seq.size() - 1)
                seq[pos] = std::move(seq[seq.size() - 1]);
            seq.erase(--seq.end());
        }

        ++erasedCount;
        if (fromBegin)
            --endPos;
        else
            --pos;
    }

    return erasedCount;
}

inline LLBC_Variant::DictIter LLBC_Variant::DictBegin()
{
    return Become<Dict>()._data.dict().begin();
}

inline LLBC_Variant::DictIter LLBC_Variant::DictEnd()
{
    return Become<Dict>()._data.dict().end();
}

inline LLBC_Variant::DictReverseIter LLBC_Variant::DictReverseBegin()
{
    return Become<Dict>()._data.dict().rbegin();
}

inline LLBC_Variant::DictReverseIter LLBC_Variant::DictReverseEnd()
{
    return Become<Dict>()._data.dict().rend();
}

inline std::pair<LLBC_Variant::DictIter, bool> LLBC_Variant::DictInsert(const Dict::value_type &val)
{
    return Become<Dict>()._data.dict().insert(val);
}

inline std::pair<LLBC_Variant::DictIter, bool> LLBC_Variant::DictInsert(Dict::value_type &&val)
{
    return Become<Dict>()._data.dict().insert(std::move(val));
}

template <typename _Key, typename _Val>
std::pair<LLBC_Variant::DictIter, bool> LLBC_Variant::DictInsert(_Key &&key, _Val &&val)
{
    return Become<Dict>()._data.dict().emplace(std::forward<_Key>(key), std::forward<_Val>(val));
}

template <typename _Key>
LLBC_Variant::DictIter LLBC_Variant::DictFind(const _Key &key)
{
    if constexpr (std::is_same_v<_Key, Dict::key_type>)
        return Become<Dict>()._data.dict().find(key);
    else
        return DictFind(Dict::key_type(key));
}

template <typename _Key>
LLBC_Variant::DictConstIter LLBC_Variant::DictFind(const _Key &key) const
{
    if constexpr (std::is_same_v<_Key, Dict::key_type>)
        return As<Dict>().find(key);
    else
        return DictFind(Dict::key_type(key));
}

inline LLBC_Variant::DictIter LLBC_Variant::DictErase(DictIter it)
{
    if (UNLIKELY(!Is<Dict>()))
        return Become<Dict>()._data.dict().end();

    return _data.dict().erase(it);
}

inline LLBC_Variant::DictIter LLBC_Variant::DictErase(DictConstIter it)
{
    if (UNLIKELY(!Is<Dict>()))
        return Become<Dict>()._data.dict().end();

    return _data.dict().erase(it);
}

inline LLBC_Variant::DictIter LLBC_Variant::DictErase(DictConstIter first, DictConstIter last)
{
    if (UNLIKELY(!Is<Dict>()))
        return Become<Dict>()._data.dict().end();

    return _data.dict().erase(first, last);
}

inline LLBC_Variant::DictIter LLBC_Variant::DictErase(DictIter first, DictIter last)
{
    if (UNLIKELY(!Is<Dict>()))
        return Become<Dict>()._data.dict().end();

    return _data.dict().erase(first, last);
}

template <typename... _Keys>
size_t LLBC_Variant::DictErase(const _Keys &... keys)
{
    if constexpr (sizeof...(_Keys) == 0)
    {
        return 0;
    }
    else
    {
        if (UNLIKELY(!Is<Dict>()))
        {
            Become<Dict>();
            return 0;
        }

        auto eraseOne = [this](const auto &key) -> size_t {
            typedef std::remove_cv_t<std::remove_reference_t<decltype(key)>> _KeyTy;
            if constexpr (std::is_same_v<_KeyTy, Dict::key_type>)
                return _data.dict().erase(key);
            else
                return _data.dict().erase(Dict::key_type(key));
        };

        return (... + eraseOne(keys));
    }
}

template <typename _Key>
LLBC_Variant &LLBC_Variant::operator[](const _Key &key)
{
    if constexpr (std::is_same_v<_Key, LLBC_Variant>)
    {
        if (Is<Seq>())
        {
            const size_t idx = key;
            if (UNLIKELY(idx >= _data.seq().size()))
                _data.seq().resize(idx + 1);

            return _data.seq()[idx];
        }

        return Become<Dict>()._data.dict()[key];
    }
    else
    {
        return this->operator[](LLBC_Variant(key));
    }
}

template <typename _Key>
const LLBC_Variant &LLBC_Variant::operator[](const _Key &key) const
{
    if constexpr (std::is_same_v<_Key, LLBC_Variant>)
    {
        if (Is<Seq>())
        {
            const size_t idx = key;
            return idx < _data.seq().size() ? _data.seq()[idx] : nil;
        }

        if (Is<Dict>())
        {
            const auto it = _data.dict().find(key);
            return it != _data.dict().end() ? it->second : nil;
        }

        return nil;
    }
    else
    {
        return this->operator[](LLBC_Variant(key));
    }
}

template <typename _T>
bool LLBC_Variant::operator==(const _T &another) const
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
        return LLBC_VariantTraits::eq(*this, another);
    else
        return operator==(LLBC_Variant(another));
}

template <typename _T>
bool LLBC_Variant::operator!=(const _T &another) const
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
        return LLBC_VariantTraits::ne(*this, another);
    else
        return operator!=(LLBC_Variant(another));
}

template <typename _T>
bool LLBC_Variant::operator<(const _T &another) const
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
        return LLBC_VariantTraits::lt(*this, another);
    else
        return operator<(LLBC_Variant(another));
}

template <typename _T>
bool LLBC_Variant::operator>(const _T &another) const
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
        return LLBC_VariantTraits::gt(*this, another);
    else
        return operator>(LLBC_Variant(another));
}

template <typename _T>
bool LLBC_Variant::operator<=(const _T &another) const
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
        return LLBC_VariantTraits::le(*this, another);
    else
        return operator<=(LLBC_Variant(another));
}

template <typename _T>
bool LLBC_Variant::operator>=(const _T &another) const
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
        return LLBC_VariantTraits::ge(*this, another);
    else
        return operator>=(LLBC_Variant(another));
}

template <typename _T>
LLBC_Variant LLBC_Variant::operator+(const _T &another) const
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
        return LLBC_VariantTraits::add(*this, another);
    else
        return operator+(LLBC_Variant(another));
}

template <typename _T>
LLBC_Variant LLBC_Variant::operator-(const _T &another) const
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
        return LLBC_VariantTraits::sub(*this, another);
    else
        return operator-(LLBC_Variant(another));
}

template <typename _T>
LLBC_Variant LLBC_Variant::operator*(const _T &another) const
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
        return LLBC_VariantTraits::mul(*this, another);
    else
        return operator*(LLBC_Variant(another));
}

template <typename _T>
LLBC_Variant LLBC_Variant::operator/(const _T &another) const
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
        return LLBC_VariantTraits::div(*this, another);
    else
        return operator/(LLBC_Variant(another));
}

template <typename _T>
LLBC_Variant LLBC_Variant::operator%(const _T &another) const
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
        return LLBC_VariantTraits::mod(*this, another);
    else
        return operator%(LLBC_Variant(another));
}

template <typename _T>
LLBC_Variant &LLBC_Variant::operator+=(const _T &another)
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
    {
        LLBC_VariantTraits::add_equal(*this, another);
        return *this;
    }
    else
    {
        return operator+=(LLBC_Variant(another));
    }
}

template <typename _T>
LLBC_Variant & LLBC_Variant::operator-=(const _T &another)
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
    {
        LLBC_VariantTraits::sub_equal(*this, another);
        return *this;
    }
    else
    {
        return operator-=(LLBC_Variant(another));
    }
}

template <typename _T>
LLBC_Variant &LLBC_Variant::operator*=(const _T &another)
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
    {
        LLBC_VariantTraits::mul_equal(*this, another);
        return *this;
    }
    else
    {
        return operator*=(LLBC_Variant(another));
    }
}

template <typename _T>
LLBC_Variant &LLBC_Variant::operator/=(const _T &another)
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
    {
        LLBC_VariantTraits::div_equal(*this, another);
        return *this;
    }
    else
    {
        return operator/=(LLBC_Variant(another));
    }
}

template <typename _T>
LLBC_Variant &LLBC_Variant::operator%=(const _T &another)
{
    if constexpr (std::is_same_v<_T, LLBC_Variant>)
    {
        LLBC_VariantTraits::mod_equal(*this, another);
        return *this;
    }
    else
    {
        return operator%=(LLBC_Variant(another));
    }
}

inline LLBC_String LLBC_Variant::ToString() const
{
    LLBC_String strRepr;
    strRepr.reserve(32);
    strRepr.append("Variant(", 8);

    strRepr.append(TypeToString());
    strRepr.append(1, '|');
    strRepr.append(ValueToString());

    strRepr.append(1, ')');

    return strRepr;
}

template <bool _ZeroDataAfterRelease>
LLBC_FORCE_INLINE
void LLBC_Variant::ReleaseDataAndOnlySetType(LLBC_VariantType::ENUM newType)
{
    // Release data.
    if (_type == LLBC_VariantType::STR_DFT)
        _data.str().~Str();
    else if (_type == LLBC_VariantType::SEQ_DFT)
        _data.seq().~Seq();
    else if (_type == LLBC_VariantType::DICT_DFT)
        _data.dict().~Dict();

    // Zero data, if specified.
    if constexpr (_ZeroDataAfterRelease)
        _data.ui64() = 0;

    // (Only) Set type.
    _type = newType;
}

template <typename _64Ty>
_64Ty LLBC_Variant::AsSignedOrUnsigned64() const
{
    // Nil/Seq/Dict: return 0.
    if (Is<void, Seq, Dict>())
        return 0;

    // Str: Try convert to _64Ty.
    if (Is<Str>())
    {
        const Str &str = _data.str();
        if (str.empty())
            return 0;

        int base = 10;
        if (str.size() > 2)
        {
            size_t hexadecimalBegPos = str.find("0x");
            if (hexadecimalBegPos == LLBC_String::npos)
                hexadecimalBegPos = str.find("0X");

            if (hexadecimalBegPos != LLBC_String::npos)
                base = 16;
        }

        // TODO: Will optimize(erase str().strip() call) after LLBC_Num2Str<>() support "xxx  " format convert.
        return LLBC_Str2Num<_64Ty>(str.strip().c_str(), base);
    }

    // Raw: Execute static_cast.
    if (IsRaw())
    {
        if (Is<float, double>())
        {
            if (LLBC_IsFloatZero(_data.dbl()))
                return 0;

            return static_cast<_64Ty>(_data.dbl());
        }

        if constexpr (std::is_unsigned_v<_64Ty>)
            return _data.ui64();
        else
            return _data.i64();
    }

    llbc_assert(false && "Unknown vaariant type");
    return 0; 
}

template <typename _Ty, bool IsConstruct>
LLBC_FORCE_INLINE void LLBC_Variant::ConstructOrAssignFromRaw(const _Ty &raw)
{
    if constexpr (IsConstruct)
        _type = LLBC_VariantType::DeduceType<_Ty>();
    else
        ReleaseDataAndOnlySetType<false>(LLBC_VariantType::DeduceType<_Ty>());

    if constexpr (LLBC_VariantType::IsSupportedIntegralType<_Ty>())
    {
        if constexpr (std::is_signed_v<_Ty>)
            _data.i64() = raw;
        else
            _data.ui64() = raw;
    }
    else if constexpr (LLBC_VariantType::IsSupportedFloatingPointType<_Ty>())
    {
        _data.dbl() = raw;
    }
    else if constexpr (std::is_null_pointer_v<_Ty>)
    {
        _data.ui64() = 0;
    }
    else if constexpr (std::is_pointer_v<_Ty> || std::is_array_v<_Ty>)
    {
        _data.ui64() = reinterpret_cast<std::uintptr_t>(raw);
    }
    else if constexpr (LLBC_VariantType::IsSupportedEnumType<_Ty>())
    {
        if constexpr (std::is_signed_v<std::underlying_type_t<_Ty>>)
            _data.i64() = static_cast<sint64>(raw);
        else
            _data.ui64() = static_cast<uint64>(raw);
    }
    else
    {
        _data.ui64() = 0;
        llbc_assert(false && "Set value from this RAW type is not supported!");
    }
}

template <typename _Ty, bool IsConstruct>
void LLBC_Variant::ConstructOrAssignFromStr(_Ty &&str)
{
    __LLBC_INL_Var_PureType(_Ty);

    if constexpr (IsConstruct)
        _type = LLBC_VariantType::DeduceType<_Ty>();
    else
        Become<Str>();

    if constexpr ((std::is_array_v<_PureTy> &&
                   std::is_same_v<std::remove_cv_t<std::remove_extent_t<_PureTy>>, char>) ||
                  (std::is_pointer_v<_PureTy> &&
                   std::is_same_v<std::remove_cv_t<std::remove_pointer_t<_PureTy>>, char>))
    {
        if constexpr (IsConstruct)
            new (&_data.str()) Str(str);
        else
            _data.str().assign(str);
    }
    else if constexpr (std::is_same_v<_PureTy, std::string_view> ||
                       std::is_same_v<_PureTy, LLBC_CString>)
    {
        if constexpr (IsConstruct)
            new (&_data.str()) Str(str.data(), str.size());
        else
            _data.str().assign(str.data(), str.size());
    }
    else
    {
        if constexpr (IsConstruct)
        {
            new (&_data.str()) Str(std::forward<_Ty>(str));
        }
        else
        {
            if constexpr (std::is_same_v<_PureTy, Str>)
            {
                if (UNLIKELY(&str == &_data.str()))
                    return;
            }

            _data.str() = std::forward<_Ty>(str);
        }
    }
}

template <typename _Ty, bool IsConstruct>
LLBC_FORCE_INLINE void LLBC_Variant::ConstructOrAssignFromSeq(_Ty &&seq)
{
    __LLBC_INL_Var_PureType(_Ty);

    if constexpr (IsConstruct)
        _type = LLBC_VariantType::DeduceType<_Ty>();

    if constexpr (std::is_same_v<_PureTy, Seq>)
    {
        if constexpr (IsConstruct)
        {
            new (&_data.seq()) Seq(std::forward<_Ty>(seq));
        }
        else
        {
            Become<Seq>();
            if (UNLIKELY(&seq == &_data.seq()))
                return;

            _data.seq() = std::forward<_Ty>(seq);
        }
    }
    else if constexpr (LLBC_IsTemplSpec<_PureTy, std::pair>::value)
    {
        if constexpr (IsConstruct)
        {
            new (&_data.seq()) Seq();
            _data.seq().reserve(2);
        }
        else
        {
            Become<Seq>()._data.seq().resize(2);
        }
    
        if constexpr (std::is_rvalue_reference_v<_Ty &&>)
        {
            if constexpr (IsConstruct)
            {
                _data.seq().emplace_back(std::move(seq.first));
                _data.seq().emplace_back(std::move(seq.second));
            }
            else
            {
                _data.seq()[0] = std::move(seq.first);
                _data.seq()[1] = std::move(seq.second);
            }
        }
        else
        {
            if constexpr (IsConstruct)
            {
                _data.seq().emplace_back(seq.first);
                _data.seq().emplace_back(seq.second);
            }
            else
            {
                _data.seq()[0] = seq.first;
                _data.seq()[1] = seq.second;
            }
        }
    }
    else
    {
        if constexpr (IsConstruct)
            new (&_data.seq()) Seq();
        else
            Become<Seq>()._data.seq().clear();

        if (seq.empty())
            return;

        if constexpr (IsConstruct)
        {
            _data.seq().reserve(seq.size());
        }
        else
        {
            if (_data.seq().capacity() < seq.size())
                _data.seq().reserve(seq.size());
        }

        if constexpr (LLBC_IsTemplSpec<_PureTy, std::queue>::value)
        {
            if constexpr (std::is_rvalue_reference_v<_Ty &&>)
            {
                class _MyMutableQueue : public _PureTy
                {
                public:
                    auto begin() { return this->c.begin(); }
                    auto end() { return this->c.end(); }

                    void clear() { this->c.clear(); }
                };

                auto &myQueue = static_cast<_MyMutableQueue &>(seq);
                for (auto it = myQueue.begin(); it != myQueue.end(); ++it)
                    _data.seq().emplace_back(std::move(*it));
                myQueue.clear();
            }
            else
            {
                class _MyImmutableQueue : public _PureTy
                {
                public:
                    auto begin() const { return this->c.begin(); }
                    auto end() const { return this->c.end(); }
                };

                const auto &myQueue = static_cast<const _MyImmutableQueue &>(seq);
                for (auto it = myQueue.begin(); it != myQueue.end(); ++it)
                    _data.seq().emplace_back(*it);
            }
        }
        else if constexpr (LLBC_IsTemplSpec<_PureTy, std::set>::value ||
                           LLBC_IsTemplSpec<_PureTy, std::unordered_set>::value)
        {
            if constexpr (std::is_rvalue_reference_v<_Ty &&>)
            {
                do
                {
                    _data.seq().emplace_back(std::move(seq.extract(seq.begin()).value()));
                } while (!seq.empty());
            }
            else
            {
                for (const auto &elem : seq)
                    _data.seq().emplace_back(elem);
            }
        }
        else
        {
            if constexpr (std::is_rvalue_reference_v<_Ty &&>)
            {
                for (auto &&elem : seq)
                    _data.seq().emplace_back(std::move(elem));
                seq.clear();
            }
            else
            {
                for (const auto &elem : seq)
                    _data.seq().emplace_back(elem);
            }
        }
    }
}

template <typename _Ty, bool IsConstruct>
LLBC_FORCE_INLINE void LLBC_Variant::ConstructOrAssignFromDict(_Ty &&dict)
{
    __LLBC_INL_Var_PureType(_Ty);

    if constexpr (IsConstruct)
        _type = LLBC_VariantType::DeduceType<_Ty>();

    if constexpr (std::is_same_v<_PureTy, Dict>)
    {
        if constexpr (IsConstruct)
        {
            new (&_data.dict()) Dict(std::forward<_Ty>(dict));
        }
        else
        {
            Become<Dict>();
            if (UNLIKELY(&dict == &_data.dict()))
                return;

            _data.dict() = std::forward<_Ty>(dict);
        }
    }
    else
    {
        if constexpr (IsConstruct)
            new (&_data.dict()) Dict();
        else
            Become<Dict>()._data.dict().clear();

        if constexpr (std::is_rvalue_reference_v<_Ty &&>)
        {
            while (!dict.empty())
            {
                auto nh = dict.extract(dict.begin());
                _data.dict().emplace(std::move(nh.key()), std::move(nh.mapped()));
            }
        }
        else
        {
            for (auto &[key, value] : dict)
                _data.dict().emplace(key, value);
        }
    }
}

template <typename _Key>
size_t LLBC_Variant::CountImpl(const _Key &key, bool returnIfFound) const
{
    // Nil/Raw: always return 0.
    if (Is<void>() || IsRaw())
        return 0;

    // Is empty, return 0.
    if (IsEmpty())
        return 0;

    // Str:
    if (Is<Str>())
    {
        if constexpr (std::is_same_v<_Key, LLBC_Variant>)
            return CountImpl(key.template As<Str>(), returnIfFound);
        else if constexpr (!LLBC_VariantType::IsStr<_Key>())
            return CountImpl(LLBC_Variant(key).As<Str>(), returnIfFound);

        size_t subStrSize = 0;
        const char *subStrPtr = nullptr;
        if constexpr (std::is_same_v<_Key, LLBC_String> ||
                      std::is_same_v<_Key, std::string> ||
                      std::is_same_v<_Key, LLBC_CString> ||
                      std::is_same_v<_Key, std::string_view>)
        {
            subStrPtr = key.data();
            subStrSize = key.size();
        }
        else if constexpr (std::is_pointer_v<_Key> &&
                           std::is_same_v<std::remove_cv_t<std::remove_pointer_t<_Key>>, char>)
        {
            if (key)
            {
                subStrPtr = key;
                subStrSize = strlen(subStrPtr);
            }
        }
        else if constexpr (std::is_array_v<_Key> &&
                           std::is_same_v<std::remove_cv_t<std::remove_extent_t<_Key>>, char>)
        {
            subStrPtr = key;
            subStrSize = strlen(subStrPtr);
        }
        else
        {
            llbc_assert(false && "LLBC_Variant::CountImpl() implement internal error");
        }

        size_t count = 0;
        if (subStrPtr && subStrSize > 0)
        {
            size_t pos = 0;
            auto const &str = _data.str();
            while (pos < str.size())
            {
                pos = str.find(subStrPtr, pos, subStrSize);
                if (pos == Str::npos)
                    break;

                ++count;
                if (returnIfFound)
                    break;

                pos += subStrSize;
            }
        }

        return count;
    }

    // Seq:
    if (Is<Seq>())
    {
        if constexpr (!std::is_same_v<_Key, Seq::value_type>)
        {
            return CountImpl(Seq::value_type(key), returnIfFound);
        }
        else
        {
            size_t count = 0;
            for (auto &elem : _data.seq())
            {
                if (elem != key)
                    continue;

                ++count;
                if (returnIfFound)
                    break;
            }

            return count;
        }
    }

    // Dict:
    if (Is<Dict>())
    {
        if constexpr (!std::is_same_v<_Key, Dict::key_type>)
            return _data.dict().count(Dict::key_type(key));
        else
            return _data.dict().count(key);
    }

    llbc_assert(Is<Dict>() && "LLBC_Variant::CountImpl() internal error");
    return 0;
}

template <typename _StrTy>
const _StrTy &LLBC_Variant::GetEmptyStr()
{
    __LLBC_INL_Var_PureType(_StrTy);
    static const _PureTy emptyStr;
    return emptyStr;
}

template <typename _StrTy>
const _StrTy &LLBC_Variant::GetTrueStr()
{
    __LLBC_INL_Var_PureType(_StrTy);
    static const _PureTy trueStr("true");
    return trueStr;
}

template <typename _StrTy>
const _StrTy &LLBC_Variant::GetFalseStr()
{
    __LLBC_INL_Var_PureType(_StrTy);
    static const _PureTy falseStr("false");
    return falseStr;
}

template <typename _StrTy>
const _StrTy &LLBC_Variant::GetEmptySeqStr()
{
    __LLBC_INL_Var_PureType(_StrTy);
    static const _PureTy emptySeqStr("[]");
    return emptySeqStr;
}

template <typename _StrTy>
const _StrTy &LLBC_Variant::GetEmptyDictStr()
{
    __LLBC_INL_Var_PureType(_StrTy);
    static const _PureTy emptyDictStr("{}");
    return emptyDictStr;
}

__LLBC_NS_END

namespace std
{

/**
 * \brief The explicit specialization of std::hash<LLBC_Variant> impl.
 * 
 */
template <>
struct hash<LLBC_NS LLBC_Variant>
{
    #if LLBC_CUR_COMP == LLBC_COMP_GCC || LLBC_CUR_COMP == LLBC_COMP_CLANG
    __attribute__((pure)) 
    #endif // Comp == Gcc or Comp == Clang
    size_t operator()(const LLBC_NS LLBC_Variant &var) const noexcept
    {
        if (var.IsRaw())
        {
            if (var.Is<float, double>())
                #if LLBC_TARGET_PLATFORM_WIN32
                return ::std::_Hash_representation(var._data.dbl());
                #else // Non-win32 platform.
                    #if LLBC_CUR_COMP == LLBC_COMP_GCC
                        return ::std::_Hash_impl::hash(var._data.dbl());
                    #else // Non-GCC comp
                        return std::hash<double>()(var._data.dbl());
                    #endif
                #endif
            else
                return static_cast<size_t>(var._data.ui64());
        }
        else if (var.Is<LLBC_NS LLBC_String>())
        {
            const LLBC_NS LLBC_Variant::Str &str = var._data.str();
            return !str.empty() ? LLBC_NS LLBC_Hash(str.data(), str.size()) : LLBC_NS LLBC_Hash(nullptr, 0);
        }
        else if (var.Is<LLBC_NS LLBC_Variant::Seq>())
        {
            size_t hashVal = 10000;
            const LLBC_NS LLBC_Variant::Seq &seq = var._data.seq();
            if (seq.empty())
                return hashVal;

            const LLBC_NS LLBC_Variant::SeqConstIter endIt = seq.end();
            for (LLBC_NS LLBC_Variant::SeqConstIter it = seq.begin();
                 it != endIt;
                 ++it)
                 hashVal += (*this)(*it);

            return hashVal;
        }
        else if (var.Is<LLBC_NS LLBC_Variant::Dict>())
        {
            size_t hashVal = 20000;
            const LLBC_NS LLBC_Variant::Dict &dict = var._data.dict();
            if (dict.empty())
                return hashVal;

            const LLBC_NS LLBC_Variant::DictConstIter endIt = dict.end();
            for (LLBC_NS LLBC_Variant::DictConstIter it = dict.begin();
                 it != endIt;
                 ++it)
                hashVal += (*this)(it->first);

            return hashVal;
        }
        else // Nil
        {
            return 0;
        }
    }
};

}
// Undef: Variant pure type define macro.
#undef __LLBC_INL_Var_PureType
