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

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

__LLBC_NS_BEGIN

bool LLBC_Str2LooseBool(const char *str, int base, bool recognizeTrueAndYes)
{
    // If str is null, return false.
    if (UNLIKELY(!str))
        return false;
    // If str length is zero, return false.
    size_t strLen = strlen(str);
    if (strLen == 0)
        return false;

    // Left-Strip str.
    const char *strEnd = str + strLen;
    while (str != strEnd && LLBC_IsSpace(*str))
    {
        ++str;
        --strLen;
    }
    if (strLen == 0)
        return false;

    // Right-Sstrip str.
    --strEnd;
    while (strEnd != str && LLBC_IsSpace(*strEnd))
    {
        --strEnd;
        --strLen;
    }

    // Check stripped str length(must be less than sizeof(num2StrBuf)).
    auto &num2StrBuf = __LLBC_GetLibTls()->commonTls.num2StrBuf;
    if (UNLIKELY(strLen >= sizeof(num2StrBuf)))
        return false;

    // Try recognize true/yes, if specific.
    num2StrBuf[strLen] = '\0';
    if (recognizeTrueAndYes &&
        str[0] != '-' &&
        (strLen == 3 || strLen == 4))
    {
        for (size_t idx = 0; idx < strLen; ++idx)
            num2StrBuf[idx] = static_cast<char>(tolower(str[idx]));

        if ((strLen == 3 && memcmp(num2StrBuf, "yes", 3) == 0) ||
            (strLen == 4 && memcmp(num2StrBuf, "true", 4) == 0))
            return true;
    }
    else
    {
        memcpy(num2StrBuf, str, strLen);
    }

    // Process floating point string.
    if (strchr(num2StrBuf, '.') != nullptr)
        return !LLBC_IsFloatZero(LLBC_Str2Num<long double>(num2StrBuf, base));

    // Normalize base: if starts with '0x'/'0X' and base != 16, normalize to 16.
    if (base != 16 &&
        strLen > 2 &&
        num2StrBuf[0] == '0' &&
        (num2StrBuf[1] == 'x' || num2StrBuf[1] == 'X'))
        base = 16;

    // If starts with '-', convert to sint64, otherwise convert to uint64.
    if (strLen > 1 && num2StrBuf[0] == '-')
        return LLBC_Str2Num<sint64>(num2StrBuf, base) != 0ll;
    else
        return LLBC_Str2Num<uint64>(num2StrBuf, base) != 0ll;
}

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif

__LLBC_NS_END
