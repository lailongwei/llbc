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

#include "llbc/core/log/ILogToken.h"

__LLBC_NS_BEGIN

/**
 * \brief The basic log token class encapsulation.
 */
class LLBC_HIDDEN LLBC_BaseLogToken : public LLBC_ILogToken
{
public:
    LLBC_BaseLogToken();
    virtual ~LLBC_BaseLogToken();

protected:
    /**
     * Get token log formatter.
     * @return LLBC_LogFormattingInfo * - log formatter.
     */
    virtual LLBC_LogFormattingInfo *GetFormatter() const;

    /**
     * Set token log formatter.
     * @param[in] formatter - log formatter.
     */
    virtual void SetFormatter(LLBC_LogFormattingInfo *formatter);

protected:
    friend class LLBC_LogTokenChain;

    /**
     * Get next token.
     * @return LLBC_ILogToken * - next log token.
     */
    virtual LLBC_ILogToken *GetTokenNext() const;

    /**
     * Set next token.
     * @param[in] next - next log token.
     */
    virtual void SetTokenNext(LLBC_ILogToken *next);

private:
    LLBC_LogFormattingInfo *_formatter;
    LLBC_ILogToken *_nextToken;
};

__LLBC_NS_END
