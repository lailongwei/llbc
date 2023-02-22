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
 * Split string using specific separator.
 * @param[in]  str            - the source string.
 * @param[in]  separator      - separator string.
 * @param[out] destStrList    - sestination string list.
 * @param[in]  justSplitFirst - split first flag, if true, when split one time, will stop.
 * @param[in]  escapeChar     - escape character, default is '\0'.
 */
LLBC_EXPORT void LLBC_SplitString(const LLBC_String &str,
                                  const LLBC_String &separator,
                                  std::vector<LLBC_String> &destStrList,
                                  bool justSplitFirst = false,
                                  char escapeChar = '\0');

/**
 * Filter out specific string in given string.
 * @param[in] str       - the sill filter out's string.
 * @param[in] filterStr - filter out string.
 * @return LLBC_String - the already filter out's string.
 */
LLBC_EXPORT LLBC_String LLBC_FilterOutString(const LLBC_String &str, const LLBC_String &filterStr);

/**
 * Convert lower case character to upper case.
 * @param[in] str - will convert string.
 * @param[in] LLBC_String - the converted string.
 */
LLBC_EXPORT LLBC_String LLBC_ToUpper(const char *str);

/**
 * Convert upper case character to lower case.
 * @param[in] str - will convert string.
 * @return LLBC_String - the converted string.
 */
LLBC_EXPORT LLBC_String LLBC_ToLower(const char *str);

/**
 * Convert string to number(signed/unsigned)(int, long, long long, pointer, double) type.
 * @param[in] str - the string value.
 * @return number - the converted vavlue.
 */
LLBC_EXPORT sint32 LLBC_Str2Int32(const char *str);
LLBC_EXPORT uint32 LLBC_Str2UInt32(const char *str);
LLBC_EXPORT long LLBC_Str2Long(const char *str);
LLBC_EXPORT ulong LLBC_Str2ULong(const char *str);
LLBC_EXPORT sint64 LLBC_Str2Int64(const char *str);
LLBC_EXPORT uint64 LLBC_Str2UInt64(const char *str);
LLBC_EXPORT void *LLBC_Str2Ptr(const char *str);
LLBC_EXPORT double LLBC_Str2Double(const char *str);

/**
 * Convert number(signed/unsigned) type to string format.
 * @param[in] val   - number value.
 * @param[in] radix - base of value, must be in range[2, 36](specially, void * type 
 *                    instantiate function just support 10, 16 base value).
 * @return LLBC_String - the converted string.
 */
template <typename T>
LLBC_String LLBC_Num2Str(T val, int radix = 10);

/**
 * Hash specific string.
 * @param[in] str    - the string pointer.
 * @param[in] strLen - the string length.
 * @return int - the string hash code.
 */
LLBC_EXPORT int LLBC_HashString(const char *str, size_t strLen = -1);

/**
* Hash specific string.
* @param[in] str    - the string.
* @return int - the string hash code.
*/
LLBC_EXPORT int LLBC_HashString(const LLBC_String &str);

__LLBC_NS_END

#include "llbc/core/utils/Util_TextInl.h"


