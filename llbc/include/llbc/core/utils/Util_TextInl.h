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

__LLBC_NS_BEGIN

template <>
inline LLBC_String LLBC_Num2Str(sint64 val, int radix)
{
    return LLBC_I64toA(val, radix);
}

template <>
inline LLBC_String LLBC_Num2Str(uint64 val, int radix)
{
    return LLBC_UI64toA(val, radix);
}

template <>
inline LLBC_String LLBC_Num2Str(sint32 val, int radix)
{
    return LLBC_Num2Str<sint64>(val, radix);
}

template <>
inline LLBC_String LLBC_Num2Str(uint32 val, int radix)
{
    return LLBC_Num2Str<uint64>(val, radix);
}

template <>
inline LLBC_String LLBC_Num2Str(sint16 val, int radix)
{
    return LLBC_Num2Str<sint64>(val, radix);
}

template <>
inline LLBC_String LLBC_Num2Str(uint16 val, int radix)
{
    return LLBC_Num2Str<uint64>(val, radix);
}

template <>
inline LLBC_String LLBC_Num2Str(sint8 val, int radix)
{
    return LLBC_Num2Str<sint64>(val, radix);
}

template <>
inline LLBC_String LLBC_Num2Str(uint8 val, int radix)
{
    return LLBC_Num2Str<uint64>(val, radix);
}

template <>
inline LLBC_String LLBC_Num2Str(long val, int radix)
{
    return LLBC_Num2Str<sint64>(val, radix);
}

template <>
inline LLBC_String LLBC_Num2Str(ulong val, int radix)
{
    return LLBC_Num2Str<uint64>(val, radix);
}

template <>
inline LLBC_String LLBC_Num2Str(double val, int radix)
{
    char buf[64] = {0};
    snprintf(buf, sizeof(buf), "%f", val);

    return buf;
}

template <>
inline LLBC_String LLBC_Num2Str(float val, int radix)
{
    return LLBC_Num2Str<double>(val, radix);
}

template <typename T>
inline LLBC_String LLBC_Num2Str(T val, int radix)
{
    if (radix != 10 && radix != 16)
        radix = 10;

    LLBC_String str;
    if (radix == 16)
        str += "0x";

    uint64 ptrVal = 0;
    memcpy(&ptrVal, &val, sizeof(T) > sizeof(uint64) ? sizeof(uint64) : sizeof(T));
    return (str + LLBC_Num2Str<uint64>(ptrVal, radix));
}

__LLBC_NS_END
