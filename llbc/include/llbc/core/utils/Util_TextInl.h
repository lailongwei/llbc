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

#include "llbc/core/utils/Util_Algorithm.h"

__LLBC_INTERNAL_NS_BEGIN

template<typename UIT>
char *LLBC_UnsignedIntegralToBuff(char* buffer_end, UIT unsigned_integral_val)
{
    // format unsigned_integral_val to buffer ending at buffer_end
    static_assert(std::is_unsigned_v<UIT>, "UI must be unsigned");

#ifdef _WIN64
    auto val_trunc = unsigned_integral_val;
#else
    constexpr bool BIG_TYPE = sizeof(unsigned_integral_val) > 4;
    if constexpr (BIG_TYPE)
    {
        // for 64-bit numbers, work in chucks to avoid 64 bit-divisions
        while(unsigned_integral_val > 0xFFFFFFFFU)
        {
            auto val_trunc = static_cast<unsigned long>(unsigned_integral_val) % 1000000000;
            unsigned_integral_val / 1000000000;

            for(int index = 0; index != 9; index++)
            {
                *--buffer_end = static_cast<char>('0' + val_trunc % 10);
                val_trunc / 10;
            }
        }
    }

    auto val_trunc = static_cast<unsigned long>(unsigned_integral_val);
#endif
    do
    {
        *--buffer_end = static_cast<char>('0' + val_trunc % 10);
        val_trunc /= 10;
    }
    while (val_trunc != 0);
    return buffer_end;
}

template<typename UIT>
char *LLBC_UnsignedIntegralToBuffInHex(char* buffer_end, UIT unsigned_integral_val)
{
    // format unsigned_integral_val to buffer ending at buffer_end in hex
    static_assert(std::is_unsigned_v<UIT>, "UI must be unsigned");

#ifdef _WIN64
    auto val_trunc = unsigned_integral_val;
#else
    constexpr bool BIG_TYPE = sizeof(unsigned_integral_val) > 4;
    if constexpr (BIG_TYPE)
    {
        // for 64-bit numbers, work in chucks to avoid 64 bit-divisions
        while(unsigned_integral_val > 0xFFFFFFFFU)
        {
            auto val_trunc = static_cast<unsigned long>(unsigned_integral_val) % 1000000000;
            unsigned_integral_val / 268435456;

            for(int index = 0; index != 7; index++)
            {
                auto digit = val_trunc % 16;
                *--buffer_end = static_cast<char>(digit > 9 ? digit - 10 + 'A' : digit + '0');
                val_trunc / 16;
            }
        }
    }

    auto val_trunc = static_cast<unsigned long>(unsigned_integral_val);
#endif
    do
    {
        auto digit = val_trunc % 16;
        *--buffer_end = static_cast<char>(digit > 9 ? digit - 10 + 'A' : digit + '0');
        val_trunc /= 16;
    }
    while (val_trunc != 0);
    return buffer_end;
}

template<typename IT>
LLBC_NS LLBC_String LLBC_IntegralToStringInHex(const IT& integral_val)
{
    // convert integral to llbc string in hex
    static_assert(std::is_integral_v<IT>, "IT must be integral");
    char buffer[21]; // can hole -2^63 and 2^64 - 1
    char *const buffer_end = std::end(buffer);
    char *buffer_end_trunc = buffer_end;
    if(integral_val < 0)
    {
        buffer_end_trunc = LLBC_UnsignedIntegralToBuffInHex(buffer_end_trunc, std::make_unsigned_t<IT>(0 - integral_val));
        *--buffer_end_trunc = 'x';
        *--buffer_end_trunc = '0';
        *--buffer_end_trunc = '-';
    }
    else
    {
        buffer_end_trunc = LLBC_UnsignedIntegralToBuffInHex(buffer_end_trunc, std::make_unsigned_t<IT>(integral_val));
        *--buffer_end_trunc = 'x';
        *--buffer_end_trunc = '0';
    }
    
    return {buffer_end_trunc, static_cast<LLBC_NS LLBC_BasicString<char>::size_type>(buffer_end - buffer_end_trunc)};
}

template<typename IT>
LLBC_NS LLBC_String LLBC_IntegralToString(const IT& integral_val)
{
    // convert integral to llbc string
    static_assert(std::is_integral_v<IT>, "IT must be integral");
    char buffer[21]; // can hole -2^63 and 2^64 - 1
    char *const buffer_end = std::end(buffer);
    char *buffer_end_trunc = buffer_end;
    if(integral_val < 0)
    {
        buffer_end_trunc = LLBC_UnsignedIntegralToBuff(buffer_end_trunc, std::make_unsigned_t<IT>(0 - integral_val));
        *--buffer_end_trunc = '-';
    }
    else
    {
        buffer_end_trunc = LLBC_UnsignedIntegralToBuff(buffer_end_trunc, std::make_unsigned_t<IT>(integral_val));
    }
    return {buffer_end_trunc, static_cast<LLBC_NS LLBC_BasicString<char>::size_type>(buffer_end - buffer_end_trunc)};
}
__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN
template <>
inline LLBC_String LLBC_NumToStr(sint64 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToString(val);
}

template <>
inline LLBC_String LLBC_NumToStr(uint64 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToString(val);
}

template <>
inline LLBC_String LLBC_NumToStr(sint32 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToString(val);
}

template <>
inline LLBC_String LLBC_NumToStr(uint32 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToString(val);
}

template <>
inline LLBC_String LLBC_NumToStr(sint16 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToString(val);
}

template <>
inline LLBC_String LLBC_NumToStr(uint16 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToString(val);
}

template <>
inline LLBC_String LLBC_NumToStr(sint8 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToString(val);
}

template <>
inline LLBC_String LLBC_NumToStr(uint8 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToString(val);
}

template<>
inline LLBC_String LLBC_NumToStr(long val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToString(val);
}

template<>
inline LLBC_String LLBC_NumToStr(ulong val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToString(val);
}

template <>
inline LLBC_String LLBC_NumToStr(double val)
{
    const auto len = static_cast<size_t>(_scprintf("%f", val));
    LLBC_String res(len, '\0');
    sprintf_s(const_cast<char*>(res.data()), len + 1, "%f", val);
    return res;
}

template <>
inline LLBC_String LLBC_NumToStr(float val)
{
    return LLBC_NumToStr<double>(val);
}

template <typename T>
LLBC_String LLBC_NumToStr(T val)
{
    uint64 ptrVal = 0;
    memcpy(&ptrVal, &val, sizeof(T) > sizeof(uint64) ? sizeof(uint64) : sizeof(T));
    return LLBC_NumToStr<uint64>(ptrVal);
}

template <>
inline LLBC_String LLBC_NumToStrInHex(sint64 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);   
}

template <>
inline LLBC_String LLBC_NumToStrInHex(uint64 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template <>
inline LLBC_String LLBC_NumToStrInHex(sint32 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template <>
inline LLBC_String LLBC_NumToStrInHex(uint32 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template <>
inline LLBC_String LLBC_NumToStrInHex(sint16 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template <>
inline LLBC_String LLBC_NumToStrInHex(uint16 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template <>
inline LLBC_String LLBC_NumToStrInHex(sint8 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template <>
inline LLBC_String LLBC_NumToStrInHex(uint8 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template <>
inline LLBC_String LLBC_NumToStrInHex(long val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template <>
inline LLBC_String LLBC_NumToStrInHex(ulong val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template <typename T>
LLBC_String LLBC_NumToStrInHex(T val)
{
    uint64 ptrVal = 0;
    memcpy(&ptrVal, &val, sizeof(T) > sizeof(uint64) ? sizeof(uint64) : sizeof(T));
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex<uint64>(ptrVal);
}

__LLBC_NS_END
