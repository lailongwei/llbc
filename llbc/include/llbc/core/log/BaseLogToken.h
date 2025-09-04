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

#include "llbc/core/log/LogFormattingInfo.h"

__LLBC_NS_BEGIN

/**
 * \brief The log token type encapsulation.
 */
class LLBC_LogTokenType
{
public:
    /**
     * Token type enumeration.
     */
    enum
    {
        StrToken       = '\0',// ..: string type token.
        NameToken      = 'N', // %N: logger name type token.
        ExecNameToken  = 'e', // %e: executable name type token.
        TagToken       = 'g', // %g: tag type token.
        FileToken      = 'f', // %f: file name type token.
        LineToken      = 'l', // %l: file line type token.
        FunctionToken  = 'F', // %F: function token.
        ThreadIdToken  = 't', // %t: thread id type token.
        ProcessIdToken = 'p', // %p: process id type token.
        LevelToken     = 'L', // %L: level type token, like DEBUG, INFO, eg ...
        NewLineToken   = 'n', // %n: new line type.
        MsgToken       = 'm', // %m: message type token.
        TimeToken      = 'T', // %T: time token.
        RealTimeToken  = 'R', // %R: same as %T, but output real time token.
        EnvToken       = 'E', // %E: environment token.
        LogTraceToken  = 'x', // %x: log trace token.
        EscapeToken    = '%', // %%: escape token.

        NullToken      = '?', // null token.
    };
};

/**
 * Pre-declare some classes.
 */
struct LLBC_LogData;
class LLBC_LogTimeAccessor;

/**
 * \brief The basic log token class encapsulation.
 */
class LLBC_HIDDEN LLBC_BaseLogToken
{
public:
    LLBC_BaseLogToken(): _nextToken(nullptr) {  }
    virtual ~LLBC_BaseLogToken() = default;

public:
    /**
     * Initialize the log token.
     * @param[in] formatter       - log formatter.
     * @param[in] logTimeAccessor - log time accessor.
     * @param[in] str             - token append string data.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Initialize(const LLBC_LogFormattingInfo &formatter,
                           const LLBC_LogTimeAccessor &logTimeAccessor,
                           const LLBC_String &str) = 0;

    /**
     * Get token type.
     * @return int - token type.
     */
    virtual int GetType() const = 0;

    /**
     * Format the log data.
     * @param[in] data           - log data.
     * @param[out] formattedData - store location for formatted log string.
     */
    virtual void Format(const LLBC_LogData &data, LLBC_String &formattedData) const = 0;

protected:
    /**
     * Get token log formatter.
     * @return LLBC_LogFormattingInfo * - log formatter.
     */
    const LLBC_LogFormattingInfo &GetFormatter() const { return _formatter; }

    /**
     * Set token log formatter.
     * @param[in] formatter - log formatter.
     */
    void SetFormatter(const LLBC_LogFormattingInfo &formatter) { _formatter = formatter; }

protected:
    friend class LLBC_LogTokenChain;

    /**
     * Get next token.
     * @return LLBC_BaseLogToken * - next log token.
     */
    LLBC_BaseLogToken *GetTokenNext() const { return _nextToken; }

    /**
     * Set next token.
     * @param[in] next - next log token.
     */
    void SetTokenNext(LLBC_BaseLogToken *next) { _nextToken = next; }

private:
    LLBC_LogFormattingInfo _formatter;
    LLBC_BaseLogToken *_nextToken;
};

__LLBC_NS_END
