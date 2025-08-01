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

#include "llbc/core/variant/Variant.h"

__LLBC_NS_BEGIN

template <size_t _TraceStrLimit>
template <typename _TraceDataTy>
LLBC_LogTrace::TraceData<_TraceStrLimit>::TraceData(const _TraceDataTy &traceData)
{
    // Convert trace key.
    if constexpr (std::is_arithmetic_v<_TraceDataTy> || std::is_enum_v<_TraceDataTy>)
    {
        size_t traceStrLen;
        const char *traceStr = LLBC_Num2Str2<_TraceDataTy, false>(traceData, &traceStrLen);
        strLen = std::min(traceStrLen, sizeof(str) - 1);
        memcpy(str, traceStr, traceStrLen + 1);
    }
    else if (std::is_null_pointer_v<_TraceDataTy>)
    {
        strLen = 0;
        str[0] = '\0';
    }
    else if constexpr (std::is_pointer_v<_TraceDataTy> &&
                       std::is_same_v<std::remove_cv_t<std::remove_pointer_t<_TraceDataTy>>, char>)
    {
        if (UNLIKELY(traceData == nullptr))
        {
            strLen = 0;
            str[0] = '\0';
        }
        else
        {
            const size_t traceStrLen = strlen(traceData);
            strLen = std::min(traceStrLen, sizeof(str) - 1);
            memcpy(str, traceData, strLen + 1);
        }
    }
    else if constexpr (std::is_array_v<_TraceDataTy> &&
                       std::is_same_v<std::remove_cv_t<std::remove_extent_t<_TraceDataTy>>, char>)
    {
        const size_t traceStrLen = strlen(traceData);
        strLen = std::min(traceStrLen, sizeof(str) - 1);
        memcpy(str, traceData, strLen + 1);
    }
    else if constexpr (LLBC_IsTemplSpec<_TraceDataTy, std::basic_string>::value ||
                       LLBC_IsTemplSpec<_TraceDataTy, std::basic_string_view>::value ||
                       LLBC_IsTemplSpec<_TraceDataTy, LLBC_BasicString>::value ||
                       LLBC_IsTemplSpec<_TraceDataTy, LLBC_BasicCString>::value)
    {
        strLen = std::min(traceData.size(), sizeof(str) - 1);
        if (strLen > 0)
            memcpy(str, traceData.c_str(), strLen);
        str[strLen] = '\0';
    }
    else if constexpr(std::is_same_v<_TraceDataTy, LLBC_Variant>)
    {
        const LLBC_String &dataStr = traceData.AsStr();
        strLen = std::min(dataStr.size(), sizeof(str) - 1);
        if (strLen > 0)
            memcpy(str, dataStr.c_str(), strLen);
        str[strLen] = '\0';
    }
    else
    {
        llbc_assert(false && "Unsupported <_TraceDataTy>");
        strLen = 0;
        str[0] = '\0';
    }
}

template <size_t _TraceStrLimit>
LLBC_LogTrace::TraceData<_TraceStrLimit> &
LLBC_LogTrace::TraceData<_TraceStrLimit>::operator=(const TraceData &other)
{
    if (UNLIKELY(&other == this))
        return *this;

    strLen = other.strLen;
    memcpy(str, other.str, strLen + 1);

    return *this;
}

template <size_t _TraceStrLimit>
bool LLBC_LogTrace::TraceData<_TraceStrLimit>::operator<(const TraceData &other) const
{
    if (strLen < other.strLen)
        return true;
    else if (strLen > other.strLen)
        return false;

    if (strLen == 0)
        return false;

    return memcmp(str, other.str, strLen) < 0;
}

template <size_t _TraceStrLimit>
bool LLBC_LogTrace::TraceData<_TraceStrLimit>::operator==(const TraceData &other) const
{
    if (strLen != other.strLen)
        return false;

    return strLen == 0 || memcmp(str, other.str, strLen) == 0;
}

template <size_t _TraceStrLimit>
LLBC_LogTrace::TraceData<_TraceStrLimit>::operator bool() const
{
    return strLen != 0;
}

template <typename _TraceKeyTy,
          typename TraceContentTy>
LLBC_LogTrace::LLBC_LogTrace(const _TraceKeyTy &traceKey, const TraceContentTy &traceContent)
: traceKey(traceKey)
, traceContent(traceContent)
{
}

inline LLBC_LogTrace::LLBC_LogTrace(const LLBC_LogTrace &other)
: traceKey(other.traceKey)
, traceContent(other.traceContent)
{
}

inline LLBC_LogTrace &LLBC_LogTrace::operator=(const LLBC_LogTrace &other)
{
    if (UNLIKELY(&other == this))
        return *this;

    traceKey = other.traceKey;
    traceContent = other.traceContent;

    return *this;
}

__LLBC_NS_END
