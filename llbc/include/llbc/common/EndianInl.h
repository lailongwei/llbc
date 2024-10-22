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

LLBC_FORCE_INLINE constexpr bool LLBC_Endian::IsValid(int type)
{
    return (type >= LLBC_Endian::BigEndian &&
        type < LLBC_Endian::UnknownEndian) ? true : false;
}

template <typename T>
typename std::enable_if<(std::is_arithmetic<T>::value || std::is_enum<T>::value) &&
                            sizeof(T) == 1,
                        T>::type
LLBC_ReverseBytes(const T &val)
{
    return val;
}

template <typename T>
typename std::enable_if<(std::is_arithmetic<T>::value || std::is_enum<T>::value) &&
                            (sizeof(T) == 2 || sizeof(T) == 4),
                        T>::type
LLBC_ReverseBytes(const T &val)
{
    union
    {
        T val;
        uint8 bytes[sizeof(T)] ;
    } source, dest;

    source.val = val;
    for (size_t i = 0; i < sizeof(T); ++i)
        dest.bytes[i] = source.bytes[sizeof(T) - i - 1];

    return dest.val;
}

template <typename T>
typename std::enable_if<(std::is_arithmetic<T>::value || std::is_enum<T>::value) &&
                            sizeof(T) == 8,
                        T>::type
LLBC_ReverseBytes(const T &val)
{
    const uint32 highPart = LLBC_ReverseBytes<uint32>(static_cast<uint32>(
        *reinterpret_cast<const uint64 *>(&val) >> 32));
    const uint32 lowPart = LLBC_ReverseBytes<uint32>(
        static_cast<uint32>(*reinterpret_cast<const uint64 *>(&val) & 0xffffffffllu));

    const uint64 reversedVal = (static_cast<uint64>(lowPart) << 32) | highPart;
    return *reinterpret_cast<const T *>(&reversedVal);
}

template <typename T>
typename std::enable_if<(std::is_arithmetic<T>::value || std::is_enum<T>::value) &&
                            sizeof(T) == 10,
                        T>::type
LLBC_ReverseBytes(const T &val)
{
    #pragma pack(push, 2)
    struct 
    {
        uint64 lowPart;
        uint16 highPart;
    } u80Val;

    static_assert(sizeof(T) == sizeof(u80Val), "llbc framework internal error");
    #pragma pack(pop)

    memcpy(&u80Val, &val, sizeof(u80Val));
    u80Val.lowPart = LLBC_ReverseBytes<uint64>(u80Val.lowPart);
    u80Val.highPart = LLBC_ReverseBytes<uint16>(u80Val.highPart);

    T reversedVal;
    memcpy(&reversedVal, &u80Val, sizeof(T));

    return reversedVal;
}

template <typename T>
typename std::enable_if<(std::is_arithmetic<T>::value || std::is_enum<T>::value) &&
                            sizeof(T) == 12,
                        T>::type
LLBC_ReverseBytes(const T &val)
{
    #pragma pack(push, 4)
    struct
    {
        uint64 lowPart;
        uint32 highPart;
    } u96Val;

    static_assert(sizeof(T) == sizeof(u96Val), "llbc framework internal error");
    #pragma pack(pop)

    memcpy(&u96Val, &val, sizeof(T));
    u96Val.lowPart = LLBC_ReverseBytes<uint64>(u96Val.lowPart);
    u96Val.highPart = LLBC_ReverseBytes<uint32>(u96Val.highPart);

    T reversedVal;
    memcpy(&reversedVal, &u96Val, sizeof(T));

    return reversedVal;
}

template <typename T>
typename std::enable_if<(std::is_arithmetic<T>::value || std::is_enum<T>::value) &&
                            sizeof(T) == 16,
                        T>::type
LLBC_ReverseBytes(const T &val)
{
    #pragma pack(push, 8)
    struct
    {
        uint64 lowPart;
        uint64 highPart;
    } u128Val, reversedU128Val;

    static_assert(sizeof(T) == sizeof(u128Val), "llbc framework internal error");
    #pragma pack(pop)

    memcpy(&u128Val, &val, sizeof(u128Val));
    reversedU128Val.lowPart = LLBC_ReverseBytes<uint64>(u128Val.highPart);
    reversedU128Val.highPart = LLBC_ReverseBytes<uint64>(u128Val.lowPart);

    T reversedVal;
    memcpy(&reversedVal, &reversedU128Val, sizeof(T));

    return reversedVal;
}

template <typename T>
T LLBC_Net2Host(const T &val)
{
    if (LLBC_MachineEndian != LLBC_Endian::NetEndian)
        return LLBC_ReverseBytes(val);

    return val;
}

template <typename T>
T LLBC_Host2Net(const T &val)
{
    if (LLBC_MachineEndian != LLBC_Endian::NetEndian)
        return LLBC_ReverseBytes(val);

    return val;
}

__LLBC_NS_END
