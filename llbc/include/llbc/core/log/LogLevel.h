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
 * \brief The log level class encapsulation.
 */
class LLBC_EXPORT LLBC_LogLevel
{
public:
    /**
     * Level value enumeration.
     */
    enum
    {
        Begin,

        Trace = Begin,
        Debug,
        Info,
        Warn,
        Error,
        Fatal,

        End
    };

public:
    /**
     * Get specific log level string representation.
     * @param[in] level - log level.
     * @return const LLBC_String & - level representation.
     */
    static const LLBC_CString &GetLevelStr(int level);

public:
    /**
     * Get specific log level by level string representation.
     * @param[in] levelStr - log level representation.
     * @return int - log level.
     */
    static int Str2Level(const LLBC_CString &levelStr);

    /**
     * Check giving log level is legal or not.
     * @param[in] level - the given log level.
     * @return bool - return if log level is legal, otherwise return false.
     */
    static bool IsLegal(int level);
};

__LLBC_NS_END
