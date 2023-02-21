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

#include "llbc/common/BasicDataType.h"

__LLBC_NS_BEGIN

template <typename T>
void LLBC_ReverseBytes(T &val)
{
    const T copyVal = val;

    uint8 *ptr = reinterpret_cast<uint8 *>(&val);
    const uint8 *copyPtr = reinterpret_cast<const uint8 *>(&copyVal);
    for (size_t i = 0; i < sizeof(T); ++i)
        ptr[i] = copyPtr[sizeof(T) - i - 1];
}

template <typename T>
T LLBC_ReverseBytes2(const T &val)
{
    T reversedVal = val;
    LLBC_ReverseBytes(reversedVal);

    return reversedVal;
}

template <>
inline void LLBC_Net2Host<sint64>(sint64 &val)
{
    if (LLBC_MachineEndian == LLBC_Endian::NetEndian)
        return;

    uint32 part1 = static_cast<uint32>(val & 0x00000000ffffffffL);
    uint32 part2 = static_cast<uint32>(val >> 32);

    LLBC_Net2Host<uint32>(part1);
    LLBC_Net2Host<uint32>(part2);
    val = static_cast<sint64>((static_cast<uint64>(part2) << 32) | part1);
}

template <>
inline void LLBC_Net2Host<uint64>(uint64 &val)
{
    if (LLBC_MachineEndian == LLBC_Endian::NetEndian)
        return;

    uint32 part1 = static_cast<uint32>(val & 0x00000000ffffffffL);
    uint32 part2 = static_cast<uint32>(val >> 32);

    LLBC_Net2Host<uint32>(part1);
    LLBC_Net2Host<uint32>(part2);
    val = (static_cast<uint64>(part2) << 32) | part1;
}

template <typename T>
void LLBC_Net2Host(T &val)
{
    if (LLBC_MachineEndian != LLBC_Endian::NetEndian)
        LLBC_ReverseBytes(val);
}

template <>
inline sint64 LLBC_Net2Host2<sint64>(const sint64 &val)
{
    if (LLBC_MachineEndian == LLBC_Endian::NetEndian)
        return val;

    uint32 part1 = static_cast<uint32>(val & 0x00000000ffffffffL);
    uint32 part2 = static_cast<uint32>(val >> 32);

    LLBC_Net2Host<uint32>(part1);
    LLBC_Net2Host<uint32>(part2);
    return static_cast<sint64>((static_cast<uint64>(part2) << 32) | part1);
}

template <>
inline uint64 LLBC_Net2Host2<uint64>(const uint64 &val)
{
    if (LLBC_MachineEndian == LLBC_Endian::NetEndian)
        return val;

    uint32 part1 = static_cast<uint32>(val & 0x00000000ffffffffL);
    uint32 part2 = static_cast<uint32>(val >> 32);

    LLBC_Net2Host<uint32>(part1);
    LLBC_Net2Host<uint32>(part2);
    return (static_cast<uint64>(part2) << 32) | part1;
}

template <typename T>
T LLBC_Net2Host2(const T &val)
{
    if (LLBC_MachineEndian != LLBC_Endian::NetEndian)
        return LLBC_ReverseBytes2(val);

    return val;
}

template <>
inline void LLBC_Host2Net<sint64>(sint64 &val)
{
    if (LLBC_MachineEndian == LLBC_Endian::NetEndian)
        return;

    uint32 part1 = static_cast<uint32>(val & 0x00000000ffffffffL);
    uint32 part2 = static_cast<uint32>(val >> 32);

    LLBC_Net2Host<uint32>(part1);
    LLBC_Net2Host<uint32>(part2);
    val = static_cast<sint64>((static_cast<uint64>(part2) << 32) | part1);
}

template <>
inline void LLBC_Host2Net<uint64>(uint64 &val)
{
    if (LLBC_MachineEndian == LLBC_Endian::NetEndian)
        return;

    uint32 part1 = static_cast<uint32>(val & 0x00000000ffffffffL);
    uint32 part2 = static_cast<uint32>(val >> 32);

    LLBC_Net2Host<uint32>(part1);
    LLBC_Net2Host<uint32>(part2);
    val = (static_cast<uint64>(part2) << 32) | part1;
}

template <typename T>
void LLBC_Host2Net(T &val)
{
    if (LLBC_MachineEndian != LLBC_Endian::NetEndian)
        LLBC_ReverseBytes(val);
}

template <>
inline sint64 LLBC_Host2Net2<sint64>(const sint64 &val)
{
    if (LLBC_MachineEndian == LLBC_Endian::NetEndian)
        return val;

    uint32 part1 = static_cast<uint32>(val & 0x00000000ffffffffL);
    uint32 part2 = static_cast<uint32>(val >> 32);

    LLBC_Net2Host<uint32>(part1);
    LLBC_Net2Host<uint32>(part2);
    return static_cast<sint64>((static_cast<uint64>(part2) << 32) | part1);
}

template <>
inline uint64 LLBC_Host2Net2<uint64>(const uint64 &val)
{
    if (LLBC_MachineEndian == LLBC_Endian::NetEndian)
        return val;

    uint32 part1 = static_cast<uint32>(val & 0x00000000ffffffffL);
    uint32 part2 = static_cast<uint32>(val >> 32);

    LLBC_Net2Host<uint32>(part1);
    LLBC_Net2Host<uint32>(part2);
    return (static_cast<uint64>(part2) << 32) | part1;
}

template <typename T>
T LLBC_Host2Net2(const T &val)
{
    if (LLBC_MachineEndian != LLBC_Endian::NetEndian)
        return LLBC_ReverseBytes2(val);

    return val;
}

__LLBC_NS_END
