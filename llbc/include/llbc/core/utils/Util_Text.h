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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Convert number to string.
 * Note: This function will never failed, and will not set last error to LLBC_ERROR_SUCCESS when called.
 * @param[in] num    - the number.
 * @param[in] strLen - number length, if not null, will set.
 * @return const char * - the number string.
 */
template <typename _NumTy, bool _HexFormat = false>
std::enable_if_t<std::is_arithmetic_v<_NumTy> ||
                    std::is_pointer_v<_NumTy> ||
                    std::is_enum_v<_NumTy>,
                 const char *>
LLBC_Num2Str2(_NumTy num, size_t *strLen = nullptr);

/**
 * Convert number to string(this function has pool performance).
 * Note: This function will never failed, and will not set last error to LLBC_ERROR_SUCCESS when called.
 * @param[in] num - the number.
 * @return LLBC_String - the number string.
 */
template <typename _NumTy, bool _HexFormat = false>
std::enable_if_t<std::is_arithmetic_v<_NumTy> ||
                    std::is_pointer_v<_NumTy> ||
                    std::is_enum_v<_NumTy>,
                 LLBC_String>
LLBC_Num2Str(_NumTy num);

/**
 * llbc framework special function, try convert string to loose bol.
 * Notes:
 * - This function can recognize true/yes, and return true.
 * - This function will never fail, It means, Dont call LLBC_GetLastError() to check error.
 * @param[in] str                 - the string.
 * @param[in] base                - base of the interpreted integer value。
 * @param[in] recognizeTrueAndYes - recognize true/false string, default is true. 
 * @return bool - the bool value.
 */
LLBC_EXPORT bool LLBC_Str2LooseBool(const char *str,
                                    int base = 10,
                                    bool recognizeTrueAndYes = true);

/**
 * Interprets an sint8/uint8/sint16/uint16/sint32/uint32 value in a byte string pointed to by str.
 * @param[in] str  - pointer to the null-terminated by string to be interpreted.
 * @param[in] base - base of the interpreted integer value.
 * @return _NumTy - An integer value corresponding to the contents of str is returned.
 *                  if return 0, you should check LLBC_GetLastError() to ensure error occurred or not.
 *                  otherwise return value is valid(no need to call LLBC_GetLastError()).
 */
template <typename _NumTy>
std::enable_if_t<std::is_same_v<_NumTy, sint8> ||
                    std::is_same_v<_NumTy, uint8> ||
                    std::is_same_v<_NumTy, sint16> ||
                    std::is_same_v<_NumTy, uint16> ||
                    std::is_same_v<_NumTy, sint32> ||
                    std::is_same_v<_NumTy, uint32>,
                 _NumTy>
LLBC_Str2Num(const char *str, int base = 10);

/**
 * Interprets an long/ulong/sint64/uint64 value in a byte string pointed to by str.
 * @param[in] str  - pointer to the null-terminated by string to be interpreted.
 * @param[in] base - base of the interpreted integer value.
 * @return _NumTy - An integer value corresponding to the contents of str is returned.
 *                  if return 0, you should check LLBC_GetLastError() to ensure error occurred or not.
 *                  otherwise return value is valid(no need to call LLBC_GetLastError()).
 */
template <typename _NumTy>
std::enable_if_t<std::is_same_v<_NumTy, long> ||
                    std::is_same_v<_NumTy, ulong> ||
                    std::is_same_v<_NumTy, sint64> ||
                    std::is_same_v<_NumTy, uint64>,
                 _NumTy>
LLBC_Str2Num(const char *str, int base = 10);

/**
 * Interprets an enum value in a byte string pointed to by str.
 * @param[in] str  - pointer to the null-terminated by string to be interpreted.
 * @param[in] base - base of the interpreted integer value.
 * @return _NumTy - An integer value corresponding to the contents of str is returned.
 *                  if return 0, you should check LLBC_GetLastError() to ensure error occurred or not.
 */
template <typename _NumTy>
std::enable_if_t<std::is_enum_v<_NumTy>, _NumTy>
LLBC_Str2Num(const char *str, int base = 10);

/**
 * Interprets an pointer value in a byte string pointed to by str.
 * Note: !!! In this function implement, will auto normalize <base> param to 16, if str starts with '0x'/'0X' !!!
 * @param[in] str  - pointer to the null-terminated by string to be interpreted.
 * @param[in] base - base of the interpreted integer value.
 * @return _NumTy - An integer value corresponding to the contents of str is returned.
 *                  if return 0, you should check LLBC_GetLastError() to ensure error occurred or not.
 */
template <typename _NumTy>
std::enable_if_t<std::is_pointer_v<_NumTy>, _NumTy>
LLBC_Str2Num(const char *str, int base = 10);

/**
 * Interprets an floating point value in a byte string pointed to by str.
 * @param[in] str  - pointer to the null-terminated by string to be interpreted.
 * @param[in] base - base of the interpreted integer value.
 * @return _NumTy - An integer value corresponding to the contents of str is returned.
 *                  if return 0, you should check LLBC_GetLastError() to ensure error occurred or not.
 */
template <typename _NumTy>
std::enable_if_t<std::is_floating_point_v<_NumTy>, _NumTy>
LLBC_Str2Num(const char *str, int base = 10);

__LLBC_NS_END

#include "llbc/core/utils/Util_TextInl.h"
