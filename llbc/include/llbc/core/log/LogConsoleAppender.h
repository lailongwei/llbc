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

#if LLBC_TARGET_PLATFORM_WIN32
#include "llbc/core/thread/FastLock.h"
#endif // LLBC_TARGET_PLATFORM_WIN32
#include "llbc/core/log/BaseLogAppender.h"

__LLBC_NS_BEGIN

/**
 * \brief The console type log appender class encapsulation.
 */
class LLBC_LogConsoleAppender : public LLBC_BaseLogAppender
{
    typedef LLBC_BaseLogAppender _Base;

public:
    LLBC_LogConsoleAppender();
    virtual ~LLBC_LogConsoleAppender();

public:
    /**
     * Get log appender type, see LLBC_LogAppenderType.
     * @return int - log appender type.
     */
    virtual int GetType() const;

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

    /**
     * Output log data.
     * @param[in] data - log data.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Output(const LLBC_LogData &data);

private:
    /**
     * Determine the log text output color, only available in WIN platform.
     * @param[in] logLv - the log level.
     * @return int - the output color.
     */
    int DetermineLogTextColor(int logLv);

private:
    bool _colourfulOutput;
#if LLBC_TARGET_PLATFORM_WIN32
    static LLBC_FastLock _colorLock;
#endif
};

__LLBC_NS_END
