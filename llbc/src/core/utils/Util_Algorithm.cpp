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
#include "llbc/common/Macro.h"

#include "llbc/core/utils/Util_Algorithm.h"

__LLBC_INTERNAL_NS_BEGIN
static inline void __SetEscapeCharFlag(LLBC_NS uint8 flags[16], char c)
{
    if (c >= 0)
        flags[c >> 3] |= ((LLBC_NS uint8) 1 << (c % 8));
}

static inline bool __IsSetEscapeCharFlag(LLBC_NS uint8 flags[16], char c)
{
    if (c >= 0)
        return flags[c >> 3] & ((LLBC_NS uint8) 1 << (c % 8));

    return false;
}
__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

static const char *__flow_descs[] =
{
    "no flow",
    "underflow",
    "overflow",
    "unknown"
};

const char *LLBC_FlowType::Type2Str(int type)
{
    return ((type >= LLBC_FlowType::Unknown) ? 
        __flow_descs[LLBC_FlowType::Unknown] : __flow_descs[type]);
}

int LLBC_FlowType::Str2Type(const char *type)
{
    if (UNLIKELY(type == nullptr))
    {
        return LLBC_FlowType::Unknown;
    }

    if (strcmp(type, __flow_descs[LLBC_FlowType::NoFlow]) == 0)
    {
        return LLBC_FlowType::NoFlow;
    }
    else if (strcmp(type, __flow_descs[LLBC_FlowType::UnderFlow]) == 0)
    {
        return LLBC_FlowType::UnderFlow;
    }
    else if (strcmp(type, __flow_descs[LLBC_FlowType::OverFlow]) == 0)
    {
        return LLBC_FlowType::OverFlow;
    }

    return LLBC_FlowType::Unknown;
}

LLBC_String LLBC_ItoA(sint32 value, int radix)
{
    return LLBC_I64toA(value, radix);
}

LLBC_String LLBC_UItoA(uint32 value, int radix)
{
    return LLBC_UI64toA(value, value);
}

LLBC_String LLBC_I64toA(sint64 value, int radix)
{
    char *p;
    char *firstDigit;
    char temp;
    unsigned int digval;
    char buf[64] = {0};

    p = buf;
    firstDigit = p;

    if (value < 0)
    {
        p[0] = '-';
        firstDigit = ++p;

        value = -value;
    }

    do
    {
        digval = (unsigned int)(value % radix);
        value /= radix;

        if (digval > 9)
            *p++ = (char)(digval - 10 + 'a');
        else
            *p++ = (char)(digval + '0');
    } while (value > 0);

    *p-- = '\0';

    do 
    {
        temp = *p;
        *p = *firstDigit;
        *firstDigit = temp;

        --p;
        ++firstDigit;
    } while (firstDigit < p);

    return buf;
}

LLBC_String LLBC_UI64toA(uint64 value, int radix)
{
    char *p;
    char *firstDigit;
    char temp;
    unsigned int digval;
    char buf[64] = {0};

    p = buf;
    firstDigit = p;

    do
    {
        digval = (unsigned int)(value % radix);
        value /= radix;

        if (digval > 9)
            *p++ = (char)(digval - 10 + 'a');
        else
            *p++ = (char)(digval + '0');
    } while (value > 0);

    *p-- = '\0';

    do 
    {
        temp = *p;
        *p = *firstDigit;
        *firstDigit = temp;

        --p;
        ++firstDigit;
    } while (firstDigit < p);

    return buf;
}

LLBC_String &LLBC_StringEscape(LLBC_String &escapeString, const LLBC_String &willEscapeChars, char escapeChar)
{
    const size_t strLen = escapeString.size();
    if (strLen == 0)
        return escapeString;

    uint8 flags[16] = { 0 };
    LLBC_INTERNAL_NS __SetEscapeCharFlag(flags, escapeChar);

    const size_t escapeLen = willEscapeChars.size();
    for (size_t i = 0; i < escapeLen; ++i) 
        LLBC_INTERNAL_NS __SetEscapeCharFlag(flags, willEscapeChars[i]);

    // todo: cpp11 buffer move into str
    char *buffer = nullptr;
    size_t bufIdx = 0, copyIdx = 0;
    for (size_t i = 0; i < strLen; ++i)
    {
        const char &ch = escapeString[i];
        if (!LLBC_INTERNAL_NS __IsSetEscapeCharFlag(flags, ch))
            continue;

        if (UNLIKELY(buffer == nullptr))
            buffer = LLBC_Calloc(char, strLen * 2);

        const size_t copyLen = i - copyIdx;
        memcpy(buffer + bufIdx, &escapeString[copyIdx], copyLen);

        bufIdx += copyLen;
        buffer[bufIdx++] = escapeChar;
        buffer[bufIdx++] = ch;
        copyIdx = i + 1;
    }

    if (buffer != nullptr)
    {
        if (copyIdx < strLen)
        {
            const size_t copyLen = strLen - copyIdx;
            memcpy(buffer + bufIdx, &escapeString[copyIdx], copyLen);
            bufIdx += copyLen;
        }

        escapeString.assign(buffer, bufIdx);
        free(buffer);
    }
    return escapeString;
}

LLBC_String &LLBC_StringUnEscape(LLBC_String &escapeString, char escapeChar)
{
    return escapeString.unescape(escapeChar);
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_NON_WIN32

 #if LLBC_CFG_CORE_UTILS_IMPL__ITOA
  char *_itoa(int value, char *string, int radix)
  {
      LLBC_String result = LLBC_ItoA(value, radix);
      strcmp(string, result.c_str());

      return nullptr;
  }
 #endif

 #if LLBC_CFG_CORE_UTILS_IMPL__I64TOA
  char *_i64toa(long long value, char *string, int radix)
  {
      LLBC_String result = LLBC_I64toA(value, radix);
      strcmp(string, result.c_str());

      return nullptr;
  }
 #endif

 #if LLBC_CFG_CORE_UTILS_IMPL__UI64TOA
  char *_ui64toa(unsigned long long value, char *string, int radix)
  {
      LLBC_String result = LLBC_UI64toA(value, radix);
      strcmp(string, result.c_str());

      return nullptr;
  }
 #endif

#endif // LLBC_TARGET_PLATFORM_NON_WIN32
