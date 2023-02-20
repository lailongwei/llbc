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

#include "llbc/common/PFConfig.h"
#include "llbc/common/OSHeader.h"
#include "llbc/common/Macro.h"

__LLBC_NS_BEGIN

/**
 * \brief Endian class encapsulation.
 */
class LLBC_EXPORT LLBC_Endian
{
public:
    enum
    {
        // Visible format 0x01020304 -> memory store format 0x04030201.
        BigEndian     = 0,

        // Network endian.
        NetEndian     = BigEndian,

        // Visible format 0x01020304 -> memory store format 0x01020304
        LittleEndian  = 1,

        UnknownEndian
    };

    /**
     * Check given endian type validate or not.
     * @return bool - return true if validate, otherwise return false.
     */
    static bool IsValid(int type);

    /**
     * Endian type/type describe convert support.
     */
    static const char *Type2Str(int type);
    static int Str2Type(const char *str);
};

/**
 * The endian type constants.
 * Note: Now library support user through this constants to fetch machine
 *       endian type, and do not use LLBC_GetMachineEndianType() API, 
 *       because at sometimes in the future, LLBC_GetMachineEndianType()
 *       API implement code maybe become complex.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_MachineEndian;

/**
 * The library default endian type constants:
 *  It representation the library default endian config.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_DefaultEndian;

/**
 * Get machine endian type, about the endian type, see LLBC_Endian class.
 * @return int - machine endian type.
 */
LLBC_EXPORT int LLBC_GetMachineEndianType();

/**
 * Reverse c/c++ basic data type byte order.
 * @param [in/out] val - the will reverse value, the reverse result will store here.
 */
template <typename T>
void LLBC_ReverseBytes(T &val);

/**
 * Reverse c/c++ basic data type byte order.
 * @param[in] val - the will reverse value.
 * @return T - the already reversed value.
 */
template <typename T>
T LLBC_ReverseBytes2(const T &val);

/**
 * Convert network byte order data to host byte order.
 * @param[in/out] val - the will convert's value, network byte order, and the convert
 *                      result will store in t his variable.
 */
template <typename T>
void LLBC_Net2Host(T &val);

/**
 * Convert network byte order data to host byte order.
 * @param[in] val - the will convert's value, network byte order.
 * @return T - the converted value, host byte order.
 */
template <typename T>
T LLBC_Net2Host2(const T &val);

/**
 * Convert host byte order data to network byte order.
 * @param[in/out] val - the will convert's value, host byte order, and the convert
 *                      result will store in here.
 */
template <typename T>
void LLBC_Host2Net(T &val);

/**
 * Convert network byte order to host byte order.
 * @param[in] - will convert's value, host byte order.
 * @return T - convert result, network byte order.
 */
template <typename T>
T LLBC_Host2Net2(const T &val);

__LLBC_NS_END

#include "llbc/common/EndianInl.h"

