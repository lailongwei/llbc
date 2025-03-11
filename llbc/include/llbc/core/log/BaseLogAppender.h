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
class LLBC_LogTokenChain;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * The log appender type class encapsulation.
 */
class LLBC_LogAppenderType
{
public:
    /**
     * The type enumerations.
     */
    enum
    {
        Begin,

        Console = Begin,        // console type appender.
        File,                   // file type appender.
        Network,                // network type appender.

        End
    };

    /**
     * Check givin appender type is valid or not.
     * @return bool - reutrn true if valid, otherwise return false.
     */
    LLBC_NO_DISCARD static
    constexpr bool IsValid(int appenderType) { return appenderType >= Begin && appenderType < End; }
};

/**
 * The log apender init information structure encapsulation.
 */
struct LLBC_LogAppenderInitInfo
{
    int logLevel;                   // log level.
    LLBC_String pattern;            // output pattern.
    bool colourfulOutput;           // colourful output flag.

    LLBC_String filePath;           // log file path, used in File type appender.
    LLBC_String fileSuffix;         // log file suffix name, used in File type appender.
    int fileRollingMode;            // file rolling mode flag, used in File type appender.
    sint64 maxFileSize;             // max log file size, int bytes, used in File type appender.
    int maxBackupIndex;             // max backup index, used in File type appender.
    int fileBufferSize;             // file buffer size, used in File type appender.
    bool lazyCreateLogFile;         // logfile create option, used in File type appender

    LLBC_String ip;                 // Ip address, used in Network type appender.
    uint16 port;                    // port, used in Network type appender.
};

/**
 * \brief The basic log appender class encapsulation.
 */
class LLBC_HIDDEN LLBC_BaseLogAppender
{
public:
    LLBC_BaseLogAppender();
    virtual ~LLBC_BaseLogAppender();

public:
    /**
     * Get log appender type, see LLBC_LogAppenderType.
     * @return int - log appender type.
     */
    virtual int GetType() const = 0;

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
    virtual int Output(const LLBC_LogData &data) = 0;

protected:
    /**
     * Get log level.
     * @return int - the log level.
     */
    int GetLogLevel() const;

    /**
     * Set log level.
     * @param[in] logLevel - the log level, End means disable appender output.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetLogLevel(int logLevel);

    /**
     * Get current appender's token chain.
     * @return LLBC_LogTokenChain * - the log token chain.
     */
    LLBC_LogTokenChain *GetTokenChain() const;

    /**
     * Get next appender.
     * @return LLBC_BaseLogAppender * - next log appender.
     */
    LLBC_BaseLogAppender *GetAppenderNext() const;

    /**
     * Set next appender.
     * @param[in] appender - next log appender.
     */
    void SetAppenderNext(LLBC_BaseLogAppender *appender);

    /**
     * Get log formatter buffer.
     * @return LLBC_String & - log format buffer.
     */
    LLBC_String &GetLogFormatBuf();

protected:
    /**
     * Flush method.
     */
    virtual void Flush();

private:
    /**
     * Friend class: LLBC_Logger.
     * Call method(s):
     * - GetAppenderNext()
     * - SetAppenderNext()
     */
    friend class LLBC_Logger;

private:
    volatile int _logLevel;
    LLBC_LogTokenChain *_chain;
    LLBC_BaseLogAppender *_next;

    LLBC_String _logFmtBuf;
};

__LLBC_NS_END

#include "llbc/core/log/BaseLogAppenderInl.h"


