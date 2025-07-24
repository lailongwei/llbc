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

template <typename _UIT, bool _HexFormat>
LLBC_FORCE_INLINE char *__LLBC_UIntegral2StrImpl(_UIT uit, char *strEnd)
{
    do
    {
        if constexpr (_HexFormat)
        {
            auto digit = uit % 16;
            *--strEnd = static_cast<char>(digit > 9 ? digit - 10 + 'a' : digit + '0');
            uit /= 16;
        }
        else
        {
            *--strEnd = static_cast<char>(uit % 10 + '0');
            uit /= 10;
        }
    } while (uit != 0);

    if constexpr (_HexFormat)
    {
        *--strEnd = 'x';
        *--strEnd = '0';
    }

    return strEnd;
}

template <typename _IntegralTy, bool _HexFormat>
LLBC_FORCE_INLINE char *__LLBC_Integral2Str(_IntegralTy val, size_t *strLen)
{
    auto &strBuf = LLBC_NS __LLBC_GetLibTls()->commonTls.num2StrBuf;
    char *strEnd = &strBuf[sizeof(strBuf) - 1];
    *strEnd = '\0';

    char *str;
    if constexpr (std::is_unsigned_v<_IntegralTy>)
    {
        str = __LLBC_UIntegral2StrImpl<_IntegralTy, _HexFormat>(val, strEnd);
    }
    else
    {
        str = __LLBC_UIntegral2StrImpl<
            std::make_unsigned_t<_IntegralTy>, _HexFormat>(
                std::make_unsigned_t<_IntegralTy>(val < 0 ? -val : val), strEnd);
        if (val < 0)
            *--str = '-';
    }

    if (strLen)
        *strLen = strEnd - str;

    return str;
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

template <typename _NumTy, bool _HexFormat>
std::enable_if_t<std::is_arithmetic_v<_NumTy> ||
                    std::is_pointer_v<_NumTy> ||
                    std::is_enum_v<_NumTy>,
                 const char *>
LLBC_Num2Str2(_NumTy num, size_t *strLen)
{
    if constexpr (std::is_same_v<_NumTy, bool>)
    {
        if (strLen)
            *strLen = 1;
        return num ? "1" : "0";
    }
    else if constexpr (std::is_enum_v<_NumTy>)
    {
        return LLBC_INL_NS __LLBC_Integral2Str<sint64, _HexFormat>(static_cast<sint64>(num), strLen);
    }
    else if constexpr (std::is_integral_v<_NumTy>)
    {
        return LLBC_INL_NS __LLBC_Integral2Str<_NumTy, _HexFormat>(num, strLen);
    }
    else if constexpr (std::is_floating_point_v<_NumTy>)
    {
        int len;
        auto &strBuf = __LLBC_GetLibTls()->commonTls.num2StrBuf;
        if constexpr (std::is_same_v<_NumTy, ldouble>)
            len = snprintf(strBuf, sizeof(strBuf), "%Lf", num);
        else
            len = snprintf(strBuf, sizeof(strBuf), "%f", num);
        if (UNLIKELY(len <= 0))
        {
            strBuf[0] = '0';
            strBuf[1] = '\0';
            if (strLen)
                *strLen = 1;

            return strBuf;
        }

        if (strLen)
        {
            // !!! Note: For win32 platform, snprintf has bug when formatting floating point. !!!
            #if LLBC_TARGET_PLATFORM_WIN32
            *strLen = strlen(strBuf);
            #else
            *strLen = static_cast<size_t>(len);
            #endif
        }

        return strBuf;
    }
    else if constexpr (std::is_pointer_v<_NumTy>)
    {
        uint64 ptrVal = 0;
        memcpy(&ptrVal, &num, std::min(sizeof(ptrVal), sizeof(num)));
        return LLBC_INL_NS __LLBC_Integral2Str<uint64, _HexFormat>(ptrVal, strLen);
    }
    else
    {
        llbc_assert(false && "Unsupported _NumTy");
        if (strLen)
            *strLen = 0;

        return "";
    }
}

template <typename _NumTy, bool _HexFormat>
std::enable_if_t<std::is_arithmetic_v<_NumTy> ||
                    std::is_pointer_v<_NumTy> ||
                    std::is_enum_v<_NumTy>,
                 LLBC_String>
LLBC_Num2Str(_NumTy num)
{
    size_t strLen;
    const char *str = LLBC_Num2Str2<_NumTy, _HexFormat>(num, &strLen);
    return {str, strLen};
}

#define __LLBC_InlMacro_Num2StrProcessErr()           \
    if (errno != 0) {                                 \
        LLBC_SetLastError(LLBC_ERROR_CLIB);           \
        return _NumTy();                              \
    }                                                 \
    else if (strEnd == str) {                         \
        LLBC_SetLastError(LLBC_ERROR_INVALID);        \
        return _NumTy();                              \
    }                                                 \
    else if (*strEnd != '\0') {                       \
        LLBC_SetLastError(LLBC_ERROR_PARTIAL_PARSED); \
        return _NumTy();                              \
    }                                                 \
                                                      \
    if constexpr (std::is_floating_point_v<_NumTy>) { \
        if (llbc::LLBC_IsFloatZero(num))              \
            LLBC_SetLastError(LLBC_ERROR_SUCCESS);    \
    }                                                 \
    else {                                            \
        if (num == _NumTy())                          \
            LLBC_SetLastError(LLBC_ERROR_SUCCESS);    \
    }                                                 \

template <typename _NumTy>
std::enable_if_t<std::is_same_v<_NumTy, bool>, bool>
LLBC_Str2Num(const char *str, int base = 10)
{
    return LLBC_Str2LooseBool(str, base, false);
}

template <typename _NumTy>
std::enable_if_t<std::is_same_v<_NumTy, sint8> ||
                    std::is_same_v<_NumTy, uint8> ||
                    std::is_same_v<_NumTy, sint16> ||
                    std::is_same_v<_NumTy, uint16> ||
                    std::is_same_v<_NumTy, sint32> ||
                    std::is_same_v<_NumTy, uint32>,
                 _NumTy>
LLBC_Str2Num(const char *str, int base)
{
    if constexpr (!std::is_unsigned_v<_NumTy>)
    {
        long longNum = LLBC_Str2Num<long>(str, base);
        if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS &&
            (longNum < static_cast<long>(std::numeric_limits<_NumTy>::min()) ||
             longNum > static_cast<long>(std::numeric_limits<_NumTy>::max())))
        {
            errno = ERANGE;
            LLBC_SetLastError(LLBC_ERROR_CLIB);
            return _NumTy();
        }

        return static_cast<_NumTy>(longNum);
    }
    else
    {
        const ulong ulongNum = LLBC_Str2Num<ulong>(str, base);
        if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS &&
            (ulongNum < static_cast<ulong>(std::numeric_limits<_NumTy>::min()) ||
             ulongNum > static_cast<ulong>(std::numeric_limits<_NumTy>::max())))
        {
            errno = ERANGE;
            LLBC_SetLastError(LLBC_ERROR_CLIB);
            return _NumTy();
        }

        return static_cast<_NumTy>(ulongNum);
    }
}

template <typename _NumTy>
std::enable_if_t<std::is_same_v<_NumTy, long> ||
                    std::is_same_v<_NumTy, ulong> ||
                    std::is_same_v<_NumTy, sint64> ||
                    std::is_same_v<_NumTy, uint64>,
                 _NumTy>
LLBC_Str2Num(const char *str, int base)
{
    // Reset clib errno(Don't reset library errno).
    errno = 0;
    // LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    // Call strtoxx().
    _NumTy num;
    char *strEnd = nullptr;
    if constexpr (std::is_same_v<_NumTy, long>)
        num = LIKELY(str) ? strtol(str, &strEnd, base) : 0l;
    else if constexpr (std::is_same_v<_NumTy, ulong>)
        num = LIKELY(str) ? strtoul(str, &strEnd, base) : 0ul;
    else if constexpr (std::is_same_v<_NumTy, sint64>)
        num = LIKELY(str) ? strtoll(str, &strEnd, base) : 0ll;
    else // uint64
        num = LIKELY(str) ? strtoull(str, &strEnd, base) : 0llu;

    // Process error.
    // - case 1: [FAILED]  errno != 0: clib error, return _NumTy().
    // - case 2: [FAILED]  parse failed: LLBC_ERROR_INVALID, return _NumTy().
    // - case 3: [FAILED]  partial parsed: LLBC_ERROR_PARTIAL_PARSED, return num.
    // - case 4: [SUCCESS] success: return num.
    __LLBC_InlMacro_Num2StrProcessErr()

    return num;
}

template <typename _NumTy>
std::enable_if_t<std::is_enum_v<_NumTy>, _NumTy>
LLBC_Str2Num(const char *str, int base)
{
    return static_cast<_NumTy>(LLBC_Str2Num<sint64>(str, base));
}

template <typename _NumTy>
std::enable_if_t<std::is_pointer_v<_NumTy>, _NumTy>
LLBC_Str2Num(const char *str, int base)
{
    // If is null, return nullptr.
    if (UNLIKELY(!str))
        return nullptr;

    // Normalize base: if str start with '0x'/'0X' and base != 16, normalize to 16.
    const char *zeroPos;
    if (base != 16 &&
        (zeroPos = strchr(str, '0')) != nullptr)
    {
        if (*(zeroPos + 1) == 'x' ||
            *(zeroPos + 1) == 'X')
            base = 16;
    }

    // Convert to uint64 value && return.
    _NumTy ptr = nullptr;
    uint64 ptrVal = LLBC_Str2Num<uint64>(str, base);
    memcpy(&ptr, &ptrVal, std::min(sizeof(ptr), sizeof(ptrVal)));

    return ptr;
}

template <typename _NumTy>
std::enable_if_t<std::is_floating_point_v<_NumTy>, _NumTy>
LLBC_Str2Num(const char *str, int base)
{
    // Mask base param as unused.
    LLBC_UNUSED_PARAM(base);

    // Reset clib errnor(don't reset llbc framework last error).
    errno = 0;
    // LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    // Call strtoxx().
    _NumTy num = _NumTy();
    char *strEnd = nullptr;
    if constexpr (std::is_same_v<_NumTy, float>)
    {
        num = LIKELY(str) ? strtof(str, &strEnd) : .0f;
    }
    else if constexpr (std::is_same_v<_NumTy, double>)
    {
        num = LIKELY(str) ? strtod(str, &strEnd) : .0;
    }
    else if constexpr (std::is_same_v<_NumTy, long double>)
    {
        num = LIKELY(str) ? strtold(str, &strEnd) : .0;
    }
    else
    {
        llbc_assert(false && "Unsupported floating point type");
        abort();
    }

    // Process error.
    // - case 1: [FAILED]  errno != 0: clib error, return _NumTy().
    // - case 2: [FAILED]  parse failed: LLBC_ERROR_INVALID, return _NumTy().
    // - case 3: [FAILED]  partial parsed: LLBC_ERROR_PARTIAL_PARSED, return num.
    // - case 4: [SUCCESS] success: return num.
    __LLBC_InlMacro_Num2StrProcessErr()

    return num;
}

#undef __LLBC_InlMacro_Num2StrProcessErr

__LLBC_NS_END
