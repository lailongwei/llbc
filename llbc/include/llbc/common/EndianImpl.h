/**
 * @file    EndianImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/12/13
 * @version 1.0
 *
 * @brief
 */
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
