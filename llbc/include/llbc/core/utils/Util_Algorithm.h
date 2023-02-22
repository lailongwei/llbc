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
 * Flow type enumeration.
 */
class LLBC_EXPORT LLBC_FlowType
{
public:
    enum 
    {
        NoFlow      = 0,   // no flow
        UnderFlow   = 1,   // underflow
        OverFlow    = 2,   // overflow

        Unknown            // sentinel
    };

    /**
     * Convert flow type to string format.
     */
    static const char *Type2Str(int type);

    /**
     * Convert flow string format to flow type.
     */
    static int Str2Type(const char *type);

    LLBC_DISABLE_ASSIGNMENT(LLBC_FlowType);
};

/**
 * Flow check when use add operations.
 * @param[in] val    - will check value.
 * @param[in] incVal - addent.
 *
 * @return int - flow type.
 */
template <typename T>
int LLBC_CheckFlowUseAdd(T value, T addent);

/**
 * Flow check when using subtraction operation.
 * @param[in] val - will check value.
 * @param[in] sub - subtrahend.
 *
 * @return int - flow type.
 */
template <typename T>
int LLBC_CheckFlowUseSub(T value, T subtrahend);

/**
 * Convert an number to a string(Just use to adapt _itoa(), _i64toa(), _ui64toa() Platform relationship APIS).
 * @param[in] value - number to be convert.
 * @param[in] radix - base of value, must be in the range[2, 36].
 *
 * @return LLBC_String - string result.
 */
LLBC_EXPORT LLBC_String LLBC_ItoA(sint32 value, int radix);
LLBC_EXPORT LLBC_String LLBC_UItoA(uint32 value, int radix);
LLBC_EXPORT LLBC_String LLBC_I64toA(sint64 value, int radix);
LLBC_EXPORT LLBC_String LLBC_UI64toA(uint64 value, int radix);

/**
 * Escape LLBC_String.
 * @param[in] escapeString    - string to escape.
 * @param[in] willEscapeChars - char set of need to escape.
 * @param[in] escapeChar      - char of escape.
 *
 * @return LLBC_String - string result (same with input str).
 */
LLBC_EXPORT LLBC_String &LLBC_StringEscape(LLBC_String &escapeString,
                                           const LLBC_String &willEscapeChars,
                                           char escapeChar);

/**
 * UnEscape LLBC_String.
 * @param[in] escapeString - string to unescape.
 * @param[in] escapeChar   - char of escape.
 *
 * @return LLBC_String - string result (same with input str).
 */
LLBC_EXPORT LLBC_String &LLBC_StringUnEscape(LLBC_String &escapeString, char escapeChar);

__LLBC_NS_END

/**
 * Convert an number to a string.
 * Note: Non-Win32 platform specify, compatible for Win NT, Win95,98.
 * @param[in]  value  - number to be converted.
 * @param[out] string - string result.
 * @param[in]  radix  - base of value, must be in the range [2, 36].
 *
 * @return char * - each of these functions returns a pointer to string. there is no error return.
 */
#if LLBC_TARGET_PLATFORM_NON_WIN32

 #if LLBC_CFG_CORE_UTILS_IMPL__ITOA
  LLBC_EXPORT char *_itoa(int value, char *string, int radix);
 #endif

 #if LLBC_CFG_CORE_UTILS_IMPL__I64TOA
  LLBC_EXPORT char *_i64toa(long long value, char *string, int radix);
 #endif

 #if LLBC_CFG_CORE_UTILS_IMPL__UI64TOA
  LLBC_EXPORT char *_ui64toa(unsigned long long value, char *string, int radix);
 #endif

#endif // LLBC_TARGET_PLATFORM_NON_WIN32

#include "llbc/core/utils/Util_AlgorithmInl.h"


