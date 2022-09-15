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

#if LLBC_TARGET_PLATFORM_NON_WIN32
 #include <libgen.h>
#endif // Non-Win32

#include "llbc/core/utils/Util_Text.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

__LLBC_NS_BEGIN

void LLBC_SplitString(const LLBC_String &str,
                      const LLBC_String &separator,
                      std::vector<LLBC_String> &destStrList,
                      bool justSplitFirst,
                      char escapeChar)
{
    if (UNLIKELY(str.empty()))
        return;

    if (UNLIKELY(separator.empty()))
    {
        destStrList.push_back(str);
        return;
    }

    LLBC_String::size_type curPos = 0;
    LLBC_String::size_type prevPos = 0;

    LLBC_String strInternal = str;
    while ((curPos = strInternal.find(separator, curPos)) != LLBC_String::npos)
    {
        if (curPos != 0 && strInternal[curPos - 1] == escapeChar)
        {
            strInternal.erase(-- curPos, 1);
            curPos += separator.size();
            continue;
        }

        LLBC_String temp = strInternal.substr(prevPos, curPos - prevPos);
        destStrList.push_back(temp);

        if (justSplitFirst)
        {
            destStrList.push_back(strInternal.substr(curPos + separator.size()));
            return;
        }

        curPos += separator.size();
        prevPos = curPos;
    }

    LLBC_String temp = strInternal.substr(prevPos);
    if (!temp.empty())
        destStrList.push_back(temp);
}

LLBC_String LLBC_FilterOutString(const LLBC_String &str, const LLBC_String &filterStr)
{
    std::vector<LLBC_String> strings;
    LLBC_SplitString(str, filterStr, strings);

    LLBC_String retStr;
    for (size_t i = 0; i < strings.size(); ++i)
        retStr += strings[i];

    return retStr;
}

LLBC_String LLBC_ToUpper(const char *str)
{
    LLBC_String convertedStr(str);

    char ch;
    const LLBC_String::size_type length = convertedStr.size();
    for (LLBC_String::size_type i = 0; i < length; ++i)
    {
        ch = convertedStr[i];
        if (ch >= 'a' && ch <= 'z')
            convertedStr[i] -= 32;
    }

    return convertedStr;
}

LLBC_String LLBC_ToLower(const char *str)
{
    LLBC_String convertedStr(str);

    char ch;
    const LLBC_String::size_type length = convertedStr.size();
    for (LLBC_String::size_type i = 0; i < length; ++i)
    {
        ch = convertedStr[i];
        if (ch >= 'A' && ch <= 'Z')
            convertedStr[i] += 32;
    }

    return convertedStr;
}

sint32 LLBC_Str2Int32(const char *str)
{
    return atoi(str);
}

uint32 LLBC_Str2UInt32(const char *str)
{
    return static_cast<uint32>(LLBC_Str2Int32(str));
}

long LLBC_Str2Long(const char *str)
{
    return atol(str);
}

ulong LLBC_Str2ULong(const char *str)
{
    return static_cast<ulong>(LLBC_Str2Long(str));
}

sint64 LLBC_Str2Int64(const char *str)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return atoll(str);
#else
    return _atoi64(str);
#endif
}

uint64 LLBC_Str2UInt64(const char *str)
{
    return static_cast<uint64>(LLBC_Str2Int64(str));
}

void *LLBC_Str2Ptr(const char *str)
{
    if (UNLIKELY(!str))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return nullptr;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    bool hexFormat = false;
    LLBC_String lowerStr = LLBC_ToLower(str).strip();
    if (lowerStr.size() >= 2 && (lowerStr[0] == '0' && lowerStr[1] == 'x'))
    {
        hexFormat = true;
        lowerStr = lowerStr.substr(2);
    }

    if (lowerStr.empty())
        return nullptr;

    for (LLBC_String::size_type i = 0; i < lowerStr.size(); ++i)
    {
        if (hexFormat)
        {
            if (!((lowerStr[i] >= '0' && lowerStr[i] <= '9') ||
                (lowerStr[i] >= 'a' && lowerStr[i] <= 'f')))
            {
                LLBC_SetLastError(LLBC_ERROR_ARG);
                return nullptr;
            }
        }
        else
        {
            if (lowerStr[i] < '0' || lowerStr[i] > '9')
            {
                LLBC_SetLastError(LLBC_ERROR_ARG);
                return nullptr;
            }
        }
    }

    uint64 ptrVal = 0;
    uint64 baseVal = hexFormat ? 16 : 10;
    for (LLBC_String::size_type i = 0; i < lowerStr.size(); ++i)
    {
        ptrVal *= baseVal;
        if (lowerStr[i] >= '0' && lowerStr[i] <= '9')
            ptrVal += (uint8)(lowerStr[i] - '0');
        else
            ptrVal += (uint8)(lowerStr[i] - 'a' + (char)10);
    }

    void *ptr;
    memcpy(&ptr, &ptrVal, sizeof(void *));

    return ptr;
}

double LLBC_Str2Double(const char *str)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return atof(str);
#else
    return atof(str);
#endif
}

int LLBC_HashString(const LLBC_String &str)
{
    return LLBC_HashString(str.c_str(), str.length());
}

int LLBC_HashString(const char *str, size_t strLen)
{
    if (UNLIKELY(str == nullptr || strLen == 0))
        return 0;

    if (strLen == static_cast<size_t>(-1))
        strLen = strlen(str);

    int hashVal = 0;
    for (size_t i = 0; i < strLen; ++i)
    {
#if LLBC_64BIT_PROCESSOR
        hashVal += static_cast<int>(i * 0x31 * str[i]);
#else
        hashVal += i * 0x31 * str[i];
#endif
    }

    hashVal += static_cast<int>(strLen) * str[0];
    hashVal *= str[strLen - 1];

    return hashVal;
}

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif

__LLBC_NS_END
