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
#include "llbc/common/BeforeIncl.h"

#include "llbc/common/Common.h"
#include "llbc/core/utils/Util_Text.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

#if LLBC_TARGET_PLATFORM_NON_WIN32

int lstrlenA(LPCSTR lpString)
{
#if LLBC_TARGET_PLATFORM_NON_IPHONE
    return ::strlen(lpString);
#else // iPhone
    return static_cast<int>(::strlen(lpString));
#endif // LLBC_TARGET_PLATFORM_NON_IPHONE
}

int lstrlenW(LPCWSTR lpString)
{
    if (UNLIKELY(!lpString))
    {
        return 0;
    }

    int i = 0;
    for (; lpString[i] != LLBC_WTEXT('\0'); i ++);
    return i;
}

LPSTR lstrcatA(LPSTR lpString1, LPCSTR lpString2)
{
    return ::strcat(lpString1, lpString2);
}

LPWSTR lstrcatW(LPWSTR lpString1, LPCWSTR lpString2)
{
    if (UNLIKELY(!lpString2))
    {
        return lpString1;
    }

    int str1Len = ::lstrlenW(lpString1);
    int str2Len = ::lstrlenW(lpString2);
    memcpy(lpString1 + str1Len, lpString2, str2Len * sizeof(LLBC_NS wchar));

    return lpString1;
}

int lstrcmpA(LPCSTR lpString1, LPCSTR lpString2)
{
    return ::strcmp(lpString1, lpString2);
}

int lstrcmpW(LPCWSTR lpString1, LPCWSTR lpString2)
{
    int str1Len = ::lstrlenW(lpString1);
    int str2Len = ::lstrlenW(lpString2);
    int len = MIN(str1Len, str2Len);
    for (register int i = 0; i < len; i ++)
    {
        if (lpString1[i] < lpString2[i])
        {
            return -1;
        }
        else if (lpString1[i] == lpString2[i])
        {
            return 1;
        }
    }

    return (str1Len < str2Len ? -1 : 
        (str1Len == str2Len ? 0 : 1));
}

int lstrcmpiA(LPCSTR lpString1, LPCSTR lpString2)
{
    return ::lstrcmpA(lpString1,lpString2);
}

int lstrcmpiW(LPCWSTR lpString1, LPCWSTR lpString2)
{
    return ::lstrcmpW(lpString1, lpString2);
}

LPSTR lstrcpyA(LPSTR lpString1, LPCSTR lpString2)
{
    return strcpy(lpString1, lpString2);
}

LPWSTR lstrcpyW(LPWSTR lpString1, LPCWSTR lpString2)
{
    int str2Len = ::lstrlenW(lpString2);
    memcpy(lpString1, lpString2, str2Len * sizeof(LLBC_NS wchar));
    lpString1[str2Len] = LLBC_WTEXT('\0');

    return lpString1;
}

#endif // LLBC_TARGET_PLATFORM_NON_WIN32

__LLBC_NS_BEGIN

void LLBC_SplitString(const LLBC_String &str,
                      const LLBC_String &separator,
                      std::vector<LLBC_String> &destStrList,
                      bool justSplitFirst,
                      char escapeChar)
{
    if (UNLIKELY(str.empty()))
    {
        return;
    }

    if (UNLIKELY(separator.empty()))
    {
        destStrList.push_back(str);
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
    {   
        destStrList.push_back(temp);
    }
}

LLBC_String LLBC_FilterOutString(const LLBC_String &str, const LLBC_String &filterStr)
{
    std::vector<LLBC_String> strings;
    LLBC_SplitString(str, filterStr, strings);

    LLBC_String retStr;
    for (size_t i = 0; i < strings.size(); i ++)
    {
        retStr += strings[i];
    }

    return retStr;
}

LLBC_String LLBC_ToUpper(const char *str)
{
    LLBC_String convertedStr(str);

    char ch = '\0';
    const LLBC_String::size_type length = convertedStr.size();
    for (register LLBC_String::size_type i = 0; i < length; i ++)
    {
        ch = convertedStr[i];
        if (ch >= 'a' && ch <= 'z')
        {
            convertedStr[i] -= 32;
        }
    }

    return convertedStr;
}

LLBC_String LLBC_ToLower(const char *str)
{
    LLBC_String convertedStr(str);

    char ch = '\0';
    const LLBC_String::size_type length = convertedStr.size();
    for (register LLBC_String::size_type i = 0; i < length; i ++)
    {
        ch = convertedStr[i];
        if (ch >= 'A' && ch <= 'Z')
        {
            convertedStr[i] += 32;
        }
    }

    return convertedStr;
}

LLBC_String LLBC_Trim(const LLBC_String &str)
{
    if (UNLIKELY(str.empty()))
    {
        return LLBC_String(); 
    }

    return LLBC_TrimRight(LLBC_TrimLeft(str));
}

LLBC_String LLBC_Trim(const LLBC_String &str, char target)
{
    if (UNLIKELY(str.empty()))
    {
        return LLBC_String();
    }

    return LLBC_TrimRight(LLBC_TrimLeft(str, target), target);
}

LLBC_String LLBC_Trim(const LLBC_String &str, const char *targets)
{
    if (UNLIKELY(str.empty()))
    {
        return LLBC_String();
    }

    return LLBC_TrimRight(LLBC_TrimLeft(str, targets), targets);
}

LLBC_String LLBC_TrimLeft(const LLBC_String &str)
{
    return LLBC_TrimLeft(str, "\t ");
}

LLBC_String LLBC_TrimLeft(const LLBC_String &str, char target)
{
    if (UNLIKELY(str.empty()))
    {
        return LLBC_String();
    }

    const LLBC_String::size_type length = str.size();
    register LLBC_String::size_type leftPos = 0;
    for (; str[leftPos] == target && leftPos < length; leftPos ++);

    if (leftPos >= length)
    {
        return LLBC_String();
    }

    return str.substr(leftPos, LLBC_String::npos);
}

LLBC_String LLBC_TrimLeft(const LLBC_String &str, const char *targets)
{
    if (UNLIKELY(!targets))
    {
        return str;
    }

    LLBC_String retStr = str;
    const uint32 len = LLBC_StrLenA(targets);
    for (register uint32 i = 0; i < len; i ++)
    {
        retStr = LLBC_TrimLeft(retStr, targets[i]);
    }

    return retStr;
}

LLBC_String LLBC_TrimRight(const LLBC_String &str)
{
    return LLBC_TrimRight(str, "\t ");
}

LLBC_String LLBC_TrimRight(const LLBC_String &str, char target)
{
    if (UNLIKELY(str.empty()))
    {
        return LLBC_String();
    }

    const LLBC_String::size_type length = str.size();
    register LLBC_String::size_type rightPos = length - 1;
    for (; str[rightPos] == target && rightPos != 0; rightPos --);

    return str.substr(0, rightPos + 1);
}

LLBC_String LLBC_TrimRight(const LLBC_String &str, const char *targets)
{
    if (UNLIKELY(!targets))
    {
        return str;
    }

    LLBC_String retStr = str;
    const uint32 len = LLBC_StrLenA(targets);
    for (register uint32 i = 0; i < len; i ++)
    {
        retStr = LLBC_TrimRight(retStr, targets[i]);               
    }

    return retStr;
}

LLBC_String LLBC_DirName(const LLBC_String &path)
{
    if (UNLIKELY(path.empty()))
    {
        return LLBC_String();
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    char *buf = reinterpret_cast<char *>(::malloc(path.size() + 1));
    ::memcpy(buf,  path.data(), path.size());
    buf[path.size()] = '\0';

    ::dirname(buf);

    LLBC_String dirName = buf;
    ::free(buf);

    return dirName;
#else
    if (path[path.length() - 1] == ':')
    {
        return path;
    }

    LLBC_String::size_type slashPos = path.rfind(LLBC_SLASH_A);
    LLBC_String::size_type backlashPos = path.rfind(LLBC_BACKLASH_A);

    if (slashPos == LLBC_String::npos)
    {
        if (backlashPos == LLBC_String::npos)
        {
            return LLBC_String();
        }

        return path.substr(0, backlashPos);
    }
    else
    {
        if (backlashPos == LLBC_String::npos)
        {
            return path.substr(0, slashPos);
        }
    }

    return path.substr(0, MAX(slashPos, backlashPos));
#endif
}

LLBC_String LLBC_BaseName(const LLBC_String &path, bool incExtension)
{
    if (UNLIKELY(path.empty()))
    {
        return LLBC_String();
    }

    LLBC_String baseName;
#if LLBC_TARGET_PLATFORM_NON_WIN32
    baseName = ::basename(const_cast<char *>(path.c_str()));
#else
    LLBC_String::size_type slashPos = path.rfind(LLBC_SLASH_A);
    LLBC_String::size_type backlashPos = path.rfind(LLBC_BACKLASH_A);

    if (slashPos == LLBC_String::npos)
    {
        if (backlashPos == LLBC_String::npos)
        {
            baseName = path;
        }
        else
        {
            baseName = path.substr(backlashPos + 1);
        }
    }
    else
    {
        if (backlashPos == LLBC_String::npos)
        {
            baseName = path.substr(slashPos + 1);
        }
        else
        {
            baseName = path.substr(MAX(slashPos, backlashPos) + 1);
        }
    }
#endif

    if (!incExtension)
    {
        LLBC_String::size_type dotPos = baseName.rfind('.');
        if (dotPos != LLBC_String::npos && dotPos != 0)
        {
            baseName.erase(dotPos);
        }
    }

    return baseName;
}

LLBC_String LLBC_ExtensionName(const LLBC_String &path)
{
    LLBC_String basename = LLBC_BaseName(path);
    if (UNLIKELY(basename.empty()))
    {
        return LLBC_String();
    }

    LLBC_String::size_type pos = basename.rfind(".");
    if (pos == LLBC_String::npos)
    {
        return LLBC_String();
    }

    return basename.substr(pos + 1);
}

sint32 LLBC_Str2Int32(const char *str)
{
    return ::atoi(str);
}

uint32 LLBC_Str2UInt32(const char *str)
{
    return static_cast<uint32>(LLBC_Str2Int32(str));
}

long LLBC_Str2Long(const char *str)
{
    return ::atol(str);
}

ulong LLBC_Str2ULong(const char *str)
{
    return static_cast<ulong>(LLBC_Str2Long(str));
}

sint64 LLBC_Str2Int64(const char *str)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return ::atoll(str);
#else
    return ::_atoi64(str);
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
        return NULL;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    bool hexFormat = false;
    LLBC_String lowerStr = LLBC_ToLower(str);
    lowerStr = LLBC_Trim(lowerStr);
    if (lowerStr.size() >= 2 && (lowerStr[0] == '0' && lowerStr[1] == 'x'))
    {
        hexFormat = true;
        lowerStr = lowerStr.substr(2);
    }

    if (lowerStr.empty())
    {
        return NULL;
    }

    for (LLBC_String::size_type i = 0; i < lowerStr.size(); i ++)
    {
        if (hexFormat)
        {
            if (!((lowerStr[i] >= '0' && lowerStr[i] <= '9') ||
                (lowerStr[i] >= 'a' && lowerStr[i] <= 'f')))
            {
                LLBC_SetLastError(LLBC_ERROR_ARG);
                return NULL;
            }
        }
        else
        {
            if (lowerStr[i] < '0' || lowerStr[i] > '9')
            {
                LLBC_SetLastError(LLBC_ERROR_ARG);
                return NULL;
            }
        }
    }

    ulong ptrVal = 0;
    ulong baseVal = hexFormat ? 16 : 10;
    for (LLBC_String::size_type i = 0; i < lowerStr.size(); i ++)
    {
        ptrVal *= baseVal;
        if (lowerStr[i] >= '0' && lowerStr[i] <= '9')
        {
            ptrVal += (uint8)(lowerStr[i] - '0');
        }
        else
        {
            ptrVal += (uint8)(lowerStr[i] - 'a' + (char)10);
        }
    }

    return reinterpret_cast<void *>(ptrVal);
}

double LLBC_Str2Double(const char *str)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return ::atof(str);
#else
    return ::atof(str);
#endif
}

int LLBC_HashString(const LLBC_String &str)
{
    return LLBC_HashString(str.c_str(), str.length());
}

int LLBC_HashString(const char *str, size_t strLen)
{
    if (UNLIKELY(str == NULL || strLen == 0))
        return 0;

    if (strLen == static_cast<size_t>(-1))
        strLen = LLBC_StrLenA(str);

    int hashVal = 0;
    for (size_t i = 0; i < strLen; i++)
    {
#ifdef _M_X64
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

#include "llbc/common/AfterIncl.h"
