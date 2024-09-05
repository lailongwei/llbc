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
 * \brief Log formatting information class encapsulation.
 */
class LLBC_EXPORT LLBC_LogFormattingInfo
{
public:
    /**
     * Constructor & Destructor.
     */
    LLBC_LogFormattingInfo();

public:
    /**
     * Reset formatting info.
     */
    void Reset();

    /**
     * Format the given data, start by field start index.
     * @param[in/out] data   - log data.
     * @param[in] fieldStart - begin formatting position.
     */
    void Format(LLBC_String &data, int fieldStart) const;

public:
    bool leftAlign;

    int minLen;
    int maxLen;
    char fillCharacter;

    LLBC_String addiParam;
};

__LLBC_NS_END
