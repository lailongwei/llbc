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

#if LLBC_CFG_OS_IMPL_SYMBOL

/**
 * Initialize symbol.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_InitSymbol();

/**
 * Cleanup symbol.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_CleanupSymbol();

/**
 * Capture stack back trace.
 * @param[in]  skipFrames    - skip frames count, default 0.
 * @param[out] captureFrames - the final back traces, default is infinite.
 *                              the infinite value limitd by LLBC_CFG_OS_SYMBOL_MAX_CAPTURE_FRAMES config option.
 * @return LLBC_String - The stack back trace string representation.
 */
LLBC_EXPORT LLBC_String LLBC_CaptureStackBackTrace(size_t skipFrames = 0, size_t captureFrames = LLBC_INFINITE);

#endif // LLBC_CFG_OS_IMPL_SYMBOL

__LLBC_NS_END
