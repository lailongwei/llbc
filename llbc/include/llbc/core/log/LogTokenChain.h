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
 * Pre-declare some classes.
 */
struct LLBC_LogData;
class LLBC_ILogToken;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The log token chain class encapsulation.
 */
class LLBC_EXPORT LLBC_LogTokenChain
{
public:
    LLBC_LogTokenChain();
    ~LLBC_LogTokenChain();

public:
    /**
     * Build token chain by given pattern string.
     * @param[in] pattern - the log pattern.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int Build(const LLBC_String &pattern);

    /**
     * Format specific log data.
     * @param[in] data - log data.
     * @param[out] formattedData - location for formatted log message.
     */
    void Format(const LLBC_LogData &data, LLBC_String &formattedData) const;

    /**
     * Clean the log token chain.
     */
    void Cleanup();

private:
    /**
     * Append log token.
     * @param[in] token - the log token.
     */
    void AppendToken(LLBC_ILogToken *token);

private:
    LLBC_ILogToken *_head;
};

__LLBC_NS_END
