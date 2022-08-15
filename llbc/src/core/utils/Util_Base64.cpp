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

#include "llbc/core/utils/Util_Base64.h"

__LLBC_INTERNAL_NS_BEGIN

const char __b64Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                             "abcdefghijklmnopqrstuvwxyz"
                             "0123456789+/";

inline void __A3ToA4(LLBC_NS uint8 *a3, LLBC_NS uint8 *a4)
{
    a4[0] = (a3[0] & 0xfc) >> 2;
    a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
    a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
    a4[3] = (a3[2] & 0x3f);
}

inline void __A4ToA3(LLBC_NS uint8 *a4, LLBC_NS uint8 *a3)
{
    a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
    a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
    a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
}

inline LLBC_NS uint8 __B64Lookup(char c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A';
    else if (c >= 'a' && c <= 'z') return c - 71;
    else if (c >= '0' && c <= '9') return c + 4;
    else if (c == '+') return 62;
    else if (c == '/') return 63;
    else return -1;
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

int LLBC_Base64::Encode(const char *input, size_t inputLen, char *output, size_t &outputLen)
{
    if (UNLIKELY(outputLen < CalcEncodeLen(inputLen)))
    {
        LLBC_SetLastError(LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    uint8 a3[3];
    uint8 a4[4];
    int i = 0, j;
    outputLen = 0;
    while (inputLen--)
    {
        a3[i++] = *(input++);
        if (i == 3)
        {
            LLBC_INL_NS __A3ToA4(a3, a4);
            for (i = 0; i < 4; ++i)
                output[outputLen++] = LLBC_INL_NS __b64Alphabet[a4[i]];

            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; ++j)
            a3[j] = '\0';

        LLBC_INL_NS __A3ToA4(a3, a4);
        for (j = 0; j < i + 1; ++j)
            output[outputLen++] = LLBC_INL_NS __b64Alphabet[a4[j]];

        while ((i++ < 3))
            output[outputLen++] = '=';
    }

    output[outputLen] = '\0';
    return LLBC_OK;
}

int LLBC_Base64::Decode(const char *input, size_t inputLen, char *output, size_t &outputLen)
{
    if (UNLIKELY(outputLen < CalcDecodedLen(input, inputLen)))
    {
        LLBC_SetLastError(LLBC_ERROR_LIMIT);
        return LLBC_OK;
    }

    int i = 0, j;
    unsigned char a3[3];
    unsigned char a4[4];

    outputLen = 0;
    while (inputLen--)
    {
        if (*input == '=')
            break;

        a4[i++] = *(input++);
        if (i == 4)
        {
            for (i = 0; i <4; ++i)
                a4[i] = LLBC_INL_NS __B64Lookup(a4[i]);

            LLBC_INL_NS __A4ToA3(a4, a3);
            for (i = 0; i < 3; ++i)
                output[outputLen++] = a3[i];

            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 4; ++j)
            a4[j] = '\0';

        for (j = 0; j <4; ++j)
            a4[j] = LLBC_INL_NS __B64Lookup(a4[j]);

        LLBC_INL_NS __A4ToA3(a4, a3);

        for (j = 0; j < i - 1; ++j)
            output[outputLen++] = a3[j];
    }

    output[outputLen] = '\0';
    return LLBC_OK;
}

size_t LLBC_Base64::CalcEncodeLen(size_t bytesLen)
{
    return (bytesLen + 2 - ((bytesLen + 2) % 3)) / 3 * 4;
}

size_t LLBC_Base64::CalcDecodedLen(const char *input, size_t inputLen)
{
    size_t placeHolderCount = 0;
    for (size_t i = inputLen - 1; input[i] == '='; --i)
        ++placeHolderCount;

    return ((6 * inputLen) / 8) - placeHolderCount;
}

__LLBC_NS_END
