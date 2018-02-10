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

#ifdef __LLBC_COM_ENDIAN_H__

#include "llbc/common/BasicDataType.h"

__LLBC_NS_BEGIN

template <typename T>
inline void LLBC_ReverseBytes(T &val)
{
    const T copyVal = val;

    uint8 *ptr = reinterpret_cast<uint8 *>(&val);
    const uint8 *copyPtr = reinterpret_cast<const uint8 *>(&copyVal);
    for (register size_t i = 0; i < sizeof(T); i++)
        ptr[i] = copyPtr[sizeof(T) - i - 1];
}

template <typename T>
inline T LLBC_ReverseBytes2(const T &val)
{
    T reversedVal = val;
    LLBC_ReverseBytes(reversedVal);

    return reversedVal;
}

template <typename T>
inline void LLBC_Net2Host(T &val)
{
    if (LLBC_MachineEndian != LLBC_Endian::NetEndian)
        LLBC_ReverseBytes(val);
}

template <typename T>
inline T LLBC_Net2Host2(const T &val)
{
    if (LLBC_MachineEndian != LLBC_Endian::NetEndian)
        return LLBC_ReverseBytes2(val);

    return val;
}

template <typename T>
inline void LLBC_Host2Net(T &val)
{
    if (LLBC_MachineEndian != LLBC_Endian::NetEndian)
        LLBC_ReverseBytes(val);
}

template <typename T>
inline T LLBC_Host2Net2(const T &val)
{
    if (LLBC_MachineEndian != LLBC_Endian::NetEndian)
        return LLBC_ReverseBytes2(val);

    return val;
}

__LLBC_NS_END

#endif // __LLBC_COM_ENDIAN_H__
