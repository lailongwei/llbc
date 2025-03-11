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
 * \brief The log rolling mode enumeration.
 */
struct LLBC_EXPORT LLBC_LogRollingMode
{
public:
    enum
    {
        Begin,

        NoRolling = Begin,
        HourlyRolling,
        DailyRolling,

        End
    };

    /**
     * Get rolling mode string representation.
     * @param[in] mode - the rolling mode.
     * @return const LLBC_CString & - the rolling mode string representation.
     */
    static const LLBC_CString &GetModeStr(int mode);

    /**
     * Get rolling mode by rolling mode string.
     * @param[in] modeStr - the rolling mode string.
     * @return int - the rolling mode.
     */
    static int Str2Mode(const LLBC_CString &modeStr);

    /**
     * Check given rolling mode is validate or not.
     * @param[in] mode - the rolling mode.
     */
    static bool IsValid(int mode);
};

__LLBC_NS_END
