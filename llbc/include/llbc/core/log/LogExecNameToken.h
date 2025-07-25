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

#include "llbc/core/log/BaseLogToken.h"

__LLBC_NS_BEGIN

/**
 * \brief Executable name type log token calss encapsulation.
 */
class LLBC_HIDDEN LLBC_LogExecNameToken : public LLBC_BaseLogToken
{
public:
    LLBC_LogExecNameToken();
    ~LLBC_LogExecNameToken() override = default;

public:
    /**
     * Initialize the log token.
     * @param[in] formatter - log formatter.
     * @param[in] str       - token append string data.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Initialize(const LLBC_LogFormattingInfo &formatter, const LLBC_String &str) override;

    /**
     * Get token type.
     * @return int - token type.
     */
    int GetType() const override;

    /**
     * Format the log data.
     * @param[in] data           - log data.
     * @param[out] formattedData - store location for formatted log string.
     */
    void Format(const LLBC_LogData &data, LLBC_String &formattedData) const override;

private:
    char _execName[LLBC_PATH_MAX + 1];
    size_t _execNameLen;
};

__LLBC_NS_END
