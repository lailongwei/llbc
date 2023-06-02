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
char *LLBC_UnsignedIntegralToBuff(char* bufferEnd, UIT unsignedIntegralVal)
{
    // format unsignedIntegralVal to buffer ending at bufferEnd
    static_assert(std::is_unsigned<UIT>(), "UI must be unsigned");

    auto valTrunc = unsignedIntegralVal;
    do
    {
        *--bufferEnd = static_cast<char>('0' + valTrunc % 10);
        valTrunc /= 10;
    } while (valTrunc != 0);
    return bufferEnd;
}

template<typename UIT>
char *LLBC_UnsignedIntegralToBuffInHex(char* bufferEnd, UIT unsignedIntegralVal)
{
    // format unsignedIntegralVal to buffer ending at bufferEnd in hex
    static_assert(std::is_unsigned<UIT>(), "UI must be unsigned");

    do
    {
        auto digit = unsignedIntegralVal % 16;
        *--bufferEnd = static_cast<char>(digit > 9 ? digit - 10 + 'A' : digit + '0');
        unsignedIntegralVal /= 16;
    }
    while (unsignedIntegralVal != 0);
    return bufferEnd;
}

template<typename IT>
LLBC_NS LLBC_String LLBC_IntegralToStringInHex(const IT& integralVal)
{
    // convert integral to llbc string in hex
    static_assert(std::is_integral<IT>(), "IT must be integral");
    char buffer[21]; // can hole -2^63 and 2^64 - 1
    char *const bufferEnd = std::end(buffer);
    char *bufferEndTrunc = bufferEnd;
    if(integralVal < 0)
    {
        bufferEndTrunc = LLBC_UnsignedIntegralToBuffInHex(bufferEndTrunc,
                            typename std::make_unsigned<IT>::type(0 - integralVal));
        *--bufferEndTrunc = 'x';
        *--bufferEndTrunc = '0';
        *--bufferEndTrunc = '-';
    }
    else
    {
        bufferEndTrunc = LLBC_UnsignedIntegralToBuffInHex(bufferEndTrunc,
                            typename std::make_unsigned<IT>::type(integralVal));
        *--bufferEndTrunc = 'x';
        *--bufferEndTrunc = '0';
    }
    
    return {bufferEndTrunc, static_cast<size_t>(bufferEnd - bufferEndTrunc)};
}

template<typename IT>
LLBC_NS LLBC_String LLBC_IntegralToString(const IT& integralVal)
{
    // convert integral to llbc string
    static_assert(std::is_integral<IT>(), "IT must be integral");

    char buffer[21]; // can hole -2^63 and 2^64 - 1
    char *const bufferEnd = std::end(buffer);
    char *bufferEndTrunc = bufferEnd;
    if(integralVal < 0)
    {
        bufferEndTrunc = LLBC_UnsignedIntegralToBuff(bufferEndTrunc,
                            typename std::make_unsigned<IT>::type(0 - integralVal));
        *--bufferEndTrunc = '-';
    }
    else
    {
        bufferEndTrunc = LLBC_UnsignedIntegralToBuff(bufferEndTrunc,
                            typename std::make_unsigned<IT>::type(integralVal));
    }

    return {bufferEndTrunc, static_cast<size_t>(bufferEnd - bufferEndTrunc)};
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


template <>
inline LLBC_String LLBC_NumToStr(long double val)
{
    char buf[64] = {0};
    snprintf(buf, sizeof(buf), "%f", static_cast<double>(val));
    return buf;
}

template <>
inline LLBC_String LLBC_NumToStr(double val)
{
    char buf[64] = {0};
    snprintf(buf, sizeof(buf), "%f", val);
    return buf;
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

template<>
inline LLBC_String LLBC_NumToStrInHex(sint64 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);   
}

template<>
inline LLBC_String LLBC_NumToStrInHex(uint64 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template<>
inline LLBC_String LLBC_NumToStrInHex(long val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template<>
inline LLBC_String LLBC_NumToStrInHex(ulong val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template<>
inline LLBC_String LLBC_NumToStrInHex(sint32 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template<>
inline LLBC_String LLBC_NumToStrInHex(uint32 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template<>
inline LLBC_String LLBC_NumToStrInHex(sint16 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template<>
inline LLBC_String LLBC_NumToStrInHex(uint16 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template<>
inline LLBC_String LLBC_NumToStrInHex(sint8 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}

template<>
inline LLBC_String LLBC_NumToStrInHex(uint8 val)
{
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex(val);
}


template<>
inline LLBC_String LLBC_NumToStrInHex(void *val)
{
    uint64 ptrVal = 0;
    memcpy(&ptrVal, &val, sizeof(uint64));
    return LLBC_INTERNAL_NS LLBC_IntegralToStringInHex<uint64>(ptrVal);
}
__LLBC_NS_END
