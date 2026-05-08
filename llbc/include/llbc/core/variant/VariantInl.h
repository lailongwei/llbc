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
#define __LLBC_INL_Var_PureType(_Type) typedef std::remove_cvref_t<_Type> _PureType
#else
#define __LLBC_INL_Var_PureType(_Type) typedef std::remove_cv_t<std::remove_reference_t<_Type>> _PureTy
#endif
__LLBC_NS_BEGIN

template <typename _Ty>
constexpr bool LLBC_VariantType::IsConvertable()
{
    typedef std::remove_cv_t<std::remove_reference_t<_Ty>> _PureTy;

    if constexpr (std::is_same_v<_PureTy, void> ||
                  std::is_integral_v<_PureTy> ||
                  std::is_enum_v<_PureTy> ||
                  std::is_floating_point_v<_PureTy> ||
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
        typedef std::remove_cv_t<std::remove_extent_t<_PureTy>> _PtrElemTy;
        if constexpr (std::is_same_v<_PtrElemTy, char>)
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
: _type(LLBC_VariantType::DeduceType<_Ty>())
{
    if constexpr (std::is_integral_v<_Ty>)
    {
        if constexpr (std::is_signed_v<_Ty>)
            _data.i64() = raw;
        else
            _data.ui64() = raw;
    }
    else if constexpr (std::is_floating_point_v<_Ty>)
    {
        _data.dbl() = raw;
    }
    else if constexpr (std::is_null_pointer_v<_Ty>)
    {
        _data.ui64() = 0;
    }
    else if constexpr (std::is_pointer_v<_Ty>)
    {
        _data.ui64() = reinterpret_cast<std::uintptr_t>(raw);
    }
    else if constexpr (std::is_enum_v<_Ty>)
    {
        if constexpr (std::is_signed_v<std::underlying_type_t<_Ty>>)
            _data.i64() = static_cast<sint64>(raw);
        else
            _data.ui64() = static_cast<uint64>(raw);
    }
    else
    {
        llbc_assert(false && "Construction from this RAW type is not supported!");
    }
}

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsStr<_Ty>(), int>>
LLBC_Variant::LLBC_Variant(_Ty &&str)
: _type(LLBC_VariantType::DeduceType<_Ty>())
{
    __LLBC_INL_Var_PureType(_Ty);

    if constexpr (std::is_pointer_v<_PureTy>)
        new (&_data.str()) Str(str);
    else if constexpr (std::is_same_v<_PureTy, std::string_view> ||
                       std::is_same_v<_PureTy, LLBC_CString>)
        new (&_data.str()) Str(str.data(), str.size());
    else
        new (&_data.str()) Str(std::forward<_Ty>(str));
}

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsSeq<_Ty>(), int> = 0>
LLBC_Variant::LLBC_Variant(_Ty &&seq)
: _type(LLBC_VariantType::DeduceType<_Ty>())
{
    __LLBC_INL_Var_PureType(_Ty);

    if constexpr (std::is_same_v<_PureTy, Seq>)
    {
        new (&_data.seq()) Seq(std::forward<_Ty>(seq));
    }
    else if constexpr (LLBC_IsTemplSpec<_PureTy, std::pair>::value)
    {
        new (&_data.seq()) Seq();
    
        _data.seq().reserve(2);
        if constexpr (std::is_rvalue_reference_v<_Ty &&>)
        {
            _data.seq().emplace_back(std::move(seq.first));
            _data.seq().emplace_back(std::move(seq.second));
        }
        else
        {
            _data.seq().emplace_back(seq.first);
            _data.seq().emplace_back(seq.second);
        }
    }
    else
    {
        new (&_data.seq()) Seq();
        if (seq.empty())
            return;

        _data.seq().reserve(seq.size());
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

                auto &myQueue = reinterpret_cast<_MyMutableQueue &>(seq);
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

                const auto &myQueue = reinterpret_cast<const _MyImmutableQueue &>(seq);
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

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsDict<_Ty>(), int> = 0>
LLBC_Variant::LLBC_Variant(_Ty &&dict)
: _type(LLBC_VariantType::DeduceType<_Ty>())
{
    __LLBC_INL_Var_PureType(_Ty);

    if constexpr (std::is_same_v<_PureTy, Dict>)
    {
        new (&_data.dict()) Dict(std::forward<_Ty>(dict));
    }
    else
    {
        new (&_data.dict()) Dict();
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

inline LLBC_Variant::~LLBC_Variant()
{
    // To improve LLBC_Variant destructor performance, decide not to call Reset() function
    // (this function would perform unnecessary cleanup operations).
    ResetData();
}

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsRaw<_Ty>(), int>>
LLBC_Variant &LLBC_Variant::operator=(const _Ty &raw)
{
    Reset(LLBC_VariantType::DeduceType<_Ty>());
    if constexpr (std::is_integral_v<_Ty>)
    {
        if constexpr (std::is_signed_v<_Ty>)
            _data.i64() = raw;
        else
            _data.ui64() = raw;
    }
    else if constexpr (std::is_floating_point_v<_Ty>)
    {
        _data.dbl() = raw;
    }
    else if constexpr (std::is_null_pointer_v<_Ty>)
    {
        _data.ui64() = 0;
    }
    else if constexpr (std::is_pointer_v<_Ty>)
    {
        _data.ui64() = reinterpret_cast<std::uintptr_t>(raw);
    }
    else if constexpr (std::is_enum_v<_Ty>)
    {
        if (std::is_signed_v<std::underlying_type_t<_Ty>>)
            _data.i64() = static_cast<sint64>(raw);
        else
            _data.ui64() = static_cast<uint64>(raw);
    }
    else
    {
        llbc_assert(false && "Assignment from this RAW type is not supported!");
    }

    return *this;
}

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsStr<_Ty>(), int>>
LLBC_Variant &LLBC_Variant::operator=(_Ty &&str)
{
    __LLBC_INL_Var_PureType(_Ty);

    Become<Str>();
    if constexpr ((std::is_array_v<_PureTy> &&
                   std::is_same_v<std::remove_cv_t<std::remove_extent_t<_PureTy>>, char>) ||
                  (std::is_pointer_v<_PureTy> &&
                    std::is_same_v<std::remove_cv_t<std::remove_pointer_t<_PureTy>>, char>))
    {
        _data.str().assign(str);
    }
    else if constexpr (std::is_same_v<_PureTy, std::string_view> ||
                       std::is_same_v<_PureTy, LLBC_CString>)
    {
        _data.str().assign(str.data(), str.size());
    }
    else
    {
        _data.str() = std::forward<_Ty>(str);
    }

    return *this;
}

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsSeq<_Ty>(), int>>
LLBC_Variant &LLBC_Variant::operator=(_Ty &&seq)
{
    __LLBC_INL_Var_PureType(_Ty);

    if constexpr (std::is_same_v<_PureTy, Seq>)
    {
        Become<Seq>()._data.seq() = std::forward<_Ty>(seq);
    }
    else if constexpr (LLBC_IsTemplSpec<_PureTy, std::pair>::value)
    {
        Become<Seq>();

        _data.seq().clear();
        _data.seq().reserve(2);
        if constexpr (std::is_rvalue_reference_v<_Ty &&>)
        {
            _data.seq().emplace_back(std::move(seq.first));
            _data.seq().emplace_back(std::move(seq.second));
        }
        else
        {
            _data.seq().emplace_back(seq.first);
            _data.seq().emplace_back(seq.second);
        }
    }
    else
    {
        Become<Seq>();
        _data.seq().clear();

        if (seq.empty())
            return *this;

        if (_data.seq().capacity() < seq.size())
            _data.seq().reserve(seq.size());

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

                auto &myQueue = reinterpret_cast<_MyMutableQueue &>(seq);
                for (auto &item : myQueue)
                    _data.seq().emplace_back(std::move(item));
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

                const auto &myQueue = reinterpret_cast<const _MyImmutableQueue &>(seq);
                for (auto &item : myQueue)
                    _data.seq().emplace_back(item);
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
                for (auto &item : seq)
                    _data.seq().emplace_back(item);
            }
        }
        else
        {
            for (auto &item : seq)
            {
                if constexpr (std::is_rvalue_reference_v<_Ty &&>)
                    _data.seq().emplace_back(std::move(item));
                else
                    _data.seq().emplace_back(item);
            }

            if constexpr (std::is_rvalue_reference_v<_Ty &&>)
                seq.clear();
        }
    }

    return *this;
}

template <typename _Ty,
          std::enable_if_t<LLBC_VariantType::IsDict<_Ty>(), int>>
LLBC_Variant &LLBC_Variant::operator=(_Ty &&dict)
{
    __LLBC_INL_Var_PureType(_Ty);

    if constexpr (std::is_same_v<_PureTy, Dict>)
    {
        Become<Dict>()._data.dict() = std::forward<_Ty>(dict);
    }
    else
    {
        Become<Dict>();
        _data.dict().clear();

        if (dict.empty())
            return *this;

        if constexpr (std::is_rvalue_reference_v<_Ty &&>)
        {
            do
            {
                auto nh = dict.extract(dict.begin());
                _data.dict().emplace(std::move(nh.key()), std::move(nh.mapped()));
            } while (!dict.empty());
        }
        else
        {
            for (auto &[key, value] : dict)
                _data.dict().emplace(key, value);
        }
    }

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
        Reset(var._type);
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
        Reset(var._type);
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
    return (... || (_type == LLBC_VariantType::DeduceType<_Tys>()));
}

template <typename _Ty>
LLBC_Variant &LLBC_Variant::Become()
{
    __LLBC_INL_Var_PureType(_Ty);
    if constexpr (std::is_same_v<_PureTy, LLBC_Variant>)
        return *this;
    else
        return Become(LLBC_VariantType::DeduceType<_Ty>());
}

template <typename _Ty>
std::enable_if_t<LLBC_VariantType::IsNil<_Ty>(), const LLBC_Variant &>
LLBC_Variant::As() const
{
    __LLBC_INL_Var_PureType(_Ty);
    if constexpr (std::is_same_v<_PureTy, LLBC_Variant>)
    {
        return *this;
    }
    else
        return (GetType() == LLBC_VariantType::NIL) ? *this : nil;
}

template <typename _Ty>
std::enable_if_t<!std::is_reference_v<_Ty> && LLBC_VariantType::IsRaw<_Ty>(), _Ty>
LLBC_Variant::As() const
{
    __LLBC_INL_Var_PureType(_Ty);
    // xxx -> nullptr_t:
    if constexpr (std::is_null_pointer_v<_PureTy>)
    {
        return nullptr;
    }

    // xxx -> integral:
    else if constexpr (std::is_integral_v<_PureTy>)
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
    else if constexpr (std::is_floating_point_v<_PureTy>)
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
            
            if (_data.str().find('.') != LLBC_String::npos)
                return LLBC_Str2Num<_Ty>(_data.str().c_str());
            
            if (_data.str().find('-') != LLBC_String::npos)
                return static_cast<_Ty>(AsSignedOrUnsigned64<sint64>());
            else
                return static_cast<_Ty>(AsSignedOrUnsigned64<uint64>());
        }
        
       // - nil/seq/dict -> floating point:
        return .0;
    }

    // xxx -> pointer:
    else if constexpr (std::is_pointer_v<_PureTy>)
    {
        return reinterpret_cast<_Ty>(As<uint64>());
    }

    // xxx -> enum:
    else if constexpr (std::is_enum_v<_PureTy>)
    {
        if (std::is_signed_v<std::underlying_type_t<_PureTy>>)
            return static_cast<_Ty>(As<sint64>());
        else
            return static_cast<_Ty>(As<uint64>());
    }
    else
    {
        llbc_assert(false && "Can not convert variant to this RAW type");
    }
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
        {
            if constexpr (std::is_same_v<_PureTy, Str>)
                return _data.ui64() != 0 ? GetTrueStr() : GetFalseStr();
            else
                return _data.ui64() != 0 ? GetTrueSTLStr(): GetFalseSTLStr();
        }

        if (Is<float, double>())
            __LLBC_Inl_Variant_AsStr_Num2Str(_data.dbl(), false);

        int castedFastNum;
        if (LIKELY(_num2StrFastAccessTbl) &&
            ((IsSigned() &&
              (castedFastNum = static_cast<int>(_data.i64())) >= LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN &&
               castedFastNum <= LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_END) ||
             (IsUnsigned() &&
              !Is<void *>() &&
              (castedFastNum = static_cast<int>(_data.ui64())) >= LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN &&
               castedFastNum <= LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_END)))
        {
            return *_num2StrFastAccessTbl[castedFastNum - LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN];
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
        {
            if constexpr (std::is_same_v<_PureTy, Str>)
                return GetEmptySeqStr();
            else
                return GetEmptySTLSeqStr();
        }

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
        {
            if constexpr (std::is_same_v<_PureTy, Str>)
                return GetEmptyDictStr();
            else
                return GetEmptySTLDictStr();
        }

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

    if constexpr (std::is_same_v<_PureTy, Str>)
        return GetEmptyStr();
    else
        return GetEmptySTLStr();
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
            const size_t seqSize = _data.seq().size();
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
                    retSeq.emplace_back(item);
            }

            return retSeq;
        }
        else if constexpr (LLBC_IsTemplSpec<_Ty, std::queue>::value ||
                           LLBC_IsTemplSpec<_Ty, std::set>::value ||
                           LLBC_IsTemplSpec<_Ty, std::unordered_set>::value)
        {
            _PureTy retSeq;
            if (Is<Seq>())
            {
                for (auto &item : _data.seq())
                    retSeq.emplace(item);
            }

            return retSeq;
        }
        else
        {
            llbc_assert(false && "Unconvertable SEQ type");
        }
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
            retDict.emplace(key, value);
    }

    return retDict;
}

inline bool LLBC_Variant::AsLooseBool() const
{
    return (Is<Str>() && !_data.str().empty()) ? LLBC_Str2LooseBool(_data.str().c_str(), 10, true) : As<bool>();
}

inline void LLBC_Variant::StrResize(Str::size_type newSize, Str::value_type ch)
{
    Str &str = Become<Str>()._data.str();
    const Str::size_type oldSize = str.size();

    str.resize(newSize);

    if (newSize > oldSize && ch != Str::value_type())
        std::fill(str.begin() + oldSize, str.end(), ch);
}

inline void LLBC_Variant::StrReserve(Str::size_type newCap)
{
    Become<Str>()._data.str().reserve(newCap);
}

template <typename _Ty>
inline LLBC_Variant::SeqIter LLBC_Variant::SeqInsert(SeqIter it, _Ty &&val)
{
    llbc_assert(LIKELY(Is<Seq>()) && "Variant is not sequence type");
    return _data.seq().emplace(it, std::forward<_Ty>(val));
}

template <typename _Ty>
LLBC_Variant::SeqIter LLBC_Variant::SeqInsert(SeqIter it, Seq::size_type n, const _Ty &val)
{
    llbc_assert(LIKELY(Is<Seq>()) && "Variant is not sequence type");

    if constexpr (std::is_same_v<_Ty, LLBC_Variant>)
        return _data.seq().insert(it, n, val);
    else
        return _data.seq().insert(it, n, LLBC_Variant(val));
}

inline LLBC_Variant::SeqIter LLBC_Variant::SeqInsert(SeqIter it, SeqConstIter first, SeqConstIter last)
{
    llbc_assert(LIKELY(Is<Seq>()) && "Variant is not sequence type");
    return _data.seq().insert(it, first, last);
}

template <typename... _Tys>
LLBC_Variant::SeqIter LLBC_Variant::SeqBatchInsert(SeqIter it, _Tys &&... vals)
{
    llbc_assert(LIKELY(Is<Seq>()) && "Variant is not sequence type");
    if constexpr (sizeof...(vals) == 0)
    {
        return it;
    }
    else
    {
        const auto dist = std::distance(_data.seq().begin(), it);
        (SeqInsert(it++, std::forward<_Tys>(vals)), ...);

        return _data.seq().begin() + dist;
    }
}

inline void LLBC_Variant::SeqPopBack()
{
    Become<Seq>();
    if (LIKELY(!_data.seq().empty()))
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

inline LLBC_Variant::SeqIter LLBC_Variant::SeqErase(SeqIter it)
{
    llbc_assert(LIKELY(Is<Seq>()) && "Variant is not sequence type");
    return _data.seq().erase(it);
}

inline LLBC_Variant::SeqIter LLBC_Variant::SeqErase(SeqConstIter it)
{
    llbc_assert(LIKELY(Is<Seq>()) && "Variant is not sequence type");
    return _data.seq().erase(it);
}

inline LLBC_Variant::SeqIter LLBC_Variant::SeqErase(SeqIter first, SeqIter last)
{
    llbc_assert(LIKELY(Is<Seq>()) && "Variant is not sequence type");
    return _data.seq().erase(first, last);
}

inline LLBC_Variant::SeqIter LLBC_Variant::SeqErase(SeqConstIter first, SeqConstIter last)
{
    llbc_assert(LIKELY(Is<Seq>()) && "Variant is not sequence type");
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

    if (_data.seq().empty() || eraseCount == 0)
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
    if (UNLIKELY(!Is<Dict>()))
    {
        Become<Dict>();
        return 0;
    }

    Become<Dict>();
    return (... +
            (std::is_same_v<_Keys, Dict::key_type> ?
                _data.dict().erase(keys) :
                    _data.dict().erase(Dict::key_type(keys))));
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
            while (true)
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
    llbc_assert(Is<Dict>() && "LLBC_Variant::CountImpl() internal error");
    if constexpr (!std::is_same_v<_Key, Dict::mapped_type>)
        return _data.dict().count(Dict::mapped_type(key));
    else
        return _data.dict().count(key);
}

LLBC_FORCE_INLINE void LLBC_Variant::Reset(LLBC_VariantType::ENUM newType)
{
    ResetData();
    _type = newType;
}

LLBC_FORCE_INLINE void LLBC_Variant::ResetData()
{
    if (_type == LLBC_VariantType::STR_DFT)
        _data.obj.str.~Str();
    else if (_type == LLBC_VariantType::SEQ_DFT)
        _data.obj.seq.~Seq();
    else if (_type == LLBC_VariantType::DICT_DFT)
        _data.obj.dict.~Dict();
}

template <typename _64Ty>
_64Ty LLBC_Variant::AsSignedOrUnsigned64() const
{
    if (Is<void, Seq, Dict>())
        return 0;

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

        return LLBC_Str2Num<_64Ty>(str.c_str(), base);
    }

    if (Is<float, double>())
    {
        if (LLBC_IsFloatZero(_data.raw.dbl))
            return 0;

        return static_cast<_64Ty>(_data.dbl());
    }

    if constexpr (std::is_unsigned_v<_64Ty>)
        return _data.ui64();
    else
        return _data.i64();
}

template <typename _Key>
LLBC_Variant::Dict::size_type LLBC_Variant::DictEraseOne(const _Key &key)
{
    if constexpr (std::is_same_v<_Key, LLBC_Variant>)
        return Become<Dict>()._data.dict().erase(key);
    else
        return DictEraseOne(LLBC_Variant(key));
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