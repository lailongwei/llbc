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

#include "llbc/core/log/ILogAppender.h"

__LLBC_NS_BEGIN

/**
 * \brief The basic log appender class encapsulation.
 */
class LLBC_BaseLogAppender : public LLBC_ILogAppender
{
public:
    LLBC_BaseLogAppender();
    virtual ~LLBC_BaseLogAppender();

public:
    /**
     * Initialize the log appender.
     * @param[in] initInfo - log appender initialize info structure.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Initialize(const LLBC_LogAppenderInitInfo &initInfo);

    /**
     * Finalize the appender.
     */
    virtual void Finalize();

protected:
    /**
     * Get log level.
     * @return int - the log level.
     */
    virtual int GetLogLevel() const;

    /**
     * Get current appender's token chain.
     * @return LLBC_LogTokenChain * - the log token chain.
     */
    virtual LLBC_LogTokenChain *GetTokenChain() const;

    /**
     * Get next appender.
     * @return LLBC_ILogAppender * - next log appender.
     */
    virtual LLBC_ILogAppender *GetAppenderNext() const;

    /**
     * Set next appender.
     * @param[in] appender - next log appender.
     */
    virtual void SetAppenderNext(LLBC_ILogAppender *appender);

    /**
     * Get log formatter buffer.
     * @return LLBC_String & - log format buffer.
     */
    virtual LLBC_String &GetLogFormatBuf();

protected:
    /**
     * Flush method.
     */
    virtual void Flush();

private:
    int _level;
    LLBC_LogTokenChain *_chain;
    LLBC_ILogAppender *_next;

    LLBC_String _logFmtBuf;
};

__LLBC_NS_END

#include "llbc/core/log/BaseLogAppenderInl.h"


