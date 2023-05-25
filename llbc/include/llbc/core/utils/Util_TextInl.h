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
inline LLBC_String LLBC_NumToStr(sint64 val)
{
    return std::to_string(val);
}

template <>
inline LLBC_String LLBC_NumToStr(uint64 val)
{
    return std::to_string(val);
}

template <>
inline LLBC_String LLBC_NumToStr(sint32 val)
{
    return std::to_string(val);
}

template <>
inline LLBC_String LLBC_NumToStr(uint32 val)
{
    return std::to_string(val);
}

template <>
inline LLBC_String LLBC_NumToStr(sint16 val)
{
    return std::to_string(val);
}

template <>
inline LLBC_String LLBC_NumToStr(uint16 val)
{
    return std::to_string(val);
}

template <>
inline LLBC_String LLBC_NumToStr(sint8 val)
{
    return std::to_string(val);
}

template <>
inline LLBC_String LLBC_NumToStr(uint8 val)
{
    return std::to_string(val);
}

template <>
inline LLBC_String LLBC_NumToStr(double val)
{
    return std::to_string(val);
}

template <>
inline LLBC_String LLBC_NumToStr(float val)
{
    return std::to_string(val);
}

template <typename T>
LLBC_String LLBC_NumToStr(T val)
{
    uint64 ptrVal = 0;
    memcpy(&ptrVal, &val, sizeof(T) > sizeof(uint64) ? sizeof(uint64) : sizeof(T));
    return LLBC_NumToStr<uint64>(ptrVal);
}

template <>
inline LLBC_String LLBC_NumToStrHex(sint64 val)
{
    std::stringstream stream;
    stream << "0x"
           << std::setfill ('0') << std::setw(sizeof(val) * 2)
           << std::hex << val;
    return stream.str();
}

template <>
inline LLBC_String LLBC_NumToStrHex(uint64 val)
{
    std::stringstream stream;
    stream << "0x"
           << std::setfill ('0') << std::setw(sizeof(val) * 2)
           << std::hex << val;
    return stream.str();
}

template <>
inline LLBC_String LLBC_NumToStrHex(sint32 val)
{
    std::stringstream stream;
    stream << "0x"
           << std::setfill ('0') << std::setw(sizeof(val) * 2)
           << std::hex << val;
    return stream.str();
}

template <>
inline LLBC_String LLBC_NumToStrHex(uint32 val)
{
    std::stringstream stream;
    stream << "0x"
           << std::setfill ('0') << std::setw(sizeof(val) * 2)
           << std::hex << val;
    return stream.str();
}

template <>
inline LLBC_String LLBC_NumToStrHex(sint16 val)
{
    std::stringstream stream;
    stream << "0x"
           << std::setfill ('0') << std::setw(sizeof(val) * 2)
           << std::hex << val;
    return stream.str();
}

template <>
inline LLBC_String LLBC_NumToStrHex(uint16 val)
{
    std::stringstream stream;
    stream << "0x"
           << std::setfill ('0') << std::setw(sizeof(val) * 2)
           << std::hex << val;
    return stream.str();
}

template <>
inline LLBC_String LLBC_NumToStrHex(sint8 val)
{
    std::stringstream stream;
    stream << "0x"
           << std::setfill ('0') << std::setw(sizeof(val) * 2)
           << std::hex << val;
    return stream.str();
}

template <>
inline LLBC_String LLBC_NumToStrHex(uint8 val)
{
    std::stringstream stream;
    stream << "0x"
           << std::setfill ('0') << std::setw(sizeof(val) * 2)
           << std::hex << val;
    return stream.str();
}

template <typename T>
LLBC_String LLBC_NumToStrHex(T val)
{
    uint64 ptrVal = 0;
    memcpy(&ptrVal, &val, sizeof(T) > sizeof(uint64) ? sizeof(uint64) : sizeof(T));
    return LLBC_NumToStrHex<uint64>(ptrVal);
}

__LLBC_NS_END
