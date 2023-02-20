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

#include "llbc/common/Macro.h"

__LLBC_NS_BEGIN

/**
 * Get llbc library last error.
 * @return int - last error no.
 */
LLBC_EXPORT int LLBC_GetLastError();

/**
 * Set llbc library last error.
 * @param[in] no           - error no.
 * @param[in] customErrStr - custom error string.
 */
LLBC_EXPORT void LLBC_SetLastError(int no, const char *customErrStr = nullptr);

/**
 * Set llbc last error(enum type).
 * @param[in] no           - error no.
 * @param[in] customErrStr - custom error string.
 */
template <typename EnumTy>
typename std::enable_if<std::is_enum<EnumTy>::value, void>::type
LLBC_SetLastError(EnumTy no, const char *customErrStr = nullptr);

/**
 * Format error message.
 * @param[in] no - error number.
 * @return const char *error describe.
 */
LLBC_EXPORT const char *LLBC_StrError(int no);

/**
 * Format error message(extend version).
 * @param[in] no        - error number.
 * @param[in] subErrrno - the sub error number.
 */
LLBC_EXPORT const char *LLBC_StrErrorEx(int no, int subErrno);

/**
 * Check given error no has sub error no or not.
 * @param[in] no = error number.
 * @return bool - return true if has sub error number, otherwise return false.
 */
LLBC_EXPORT bool LLBC_HasSubErrorNo(int no);

/**
 * Format last error.
 * @return const char *last error describe.
 */
LLBC_EXPORT const char *LLBC_FormatLastError();

/**
 * Get Sub error no.
 * @return int - sub error no.
 */
LLBC_EXPORT int LLBC_GetSubErrorNo();

/**
 * Set sub-error no.
 * @param[in] subErrorNo - sub error no.
 */
LLBC_EXPORT void LLBC_SetSubErrorNo(int subErrorNo);

/**
 * Add custom error number, and error describe.
 * @param[in] no   - custom error number.
 * @param[in] desc - custom error describe.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXPORT int LLBC_AddCustomErrno(int no, const char *desc);

/**
 * Remove custom error number.
 * @param[in] no - custom error number.
 * @return int - return 0 if successed, otherwise return -1.
 */
LLBC_EXPORT int LLBC_RemoveCustomErrno(int no);

/**
 * Remove all custom error numbers.
 */
LLBC_EXPORT int LLBC_RemoveAllCustomErrnos();

/**
 * Initialize errors part.
 */
LLBC_HIDDEN void __LLBC_InitErrors();

/**
 * Destroy errors part.
 */
LLBC_HIDDEN void __LLBC_DestroyErrors();

__LLBC_NS_END

#include <llbc/common/ErrorsInl.h>



