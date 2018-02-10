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

#ifndef __LLBC_CORE_LOG_LOGGER_H__
#define __LLBC_CORE_LOG_LOGGER_H__

#include "llbc/common/Common.h"

#include "llbc/core/thread/RecursiveLock.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
struct LLBC_LogData;
class LLBC_LogRunnable;
class LLBC_LoggerConfigInfo;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The logger class encapsulation.
 */
class LLBC_EXPORT LLBC_Logger
{
public:
    LLBC_Logger();
    ~LLBC_Logger();

public:
    /**
     * Initialize the loggeer.
     * @param[in] name   - logger name.
     * @param[in] config - logger config info.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Initialize(const LLBC_String &name, const LLBC_LoggerConfigInfo *config);

    /**
     * Check logger initialized or not.
     * @return bool - return 0 if success, otherwise return -1.
     */
    bool IsInit() const;

    /**
     * Finalize logger.
     */
    void Finalize();

public:
    /**
     * Get the logger name.
     * @return const LLBC_String & - logger name.
     */
    const LLBC_String &GetLoggerName() const;

    /**
     * Get log level.
     * @return int - the log level.
     */
    int GetLogLevel() const;

    /**
     * Set log level.
     * @param[in] level - new log level.
     */
    void SetLogLevel(int level);

    /**
     * Get logger take over option, only available on root logger.
     * @return bool - take over option.
     */
    bool IsTakeOver() const;

public:
    /**
     * Output specific level message.
     * @param[in] tag      - log tag, can set to NULL.
     * @param[in] file     - log file name.
     * @param[in] line     - log file line.
     * @param[in] fmt      - format control string.
     * @param[in] argument - optional arguments.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Debug(const char *tag, const char *file, int line, const char *fmt, ...);
    int Info(const char *tag, const char *file, int line, const char *fmt, ...);
    int Warn(const char *tag, const char *file, int line, const char *fmt, ...);
    int Error(const char *tag, const char *file, int line, const char *fmt, ...);
    int Fatal(const char *tag, const char *file, int line, const char *fmt, ...);

    /**
     * Output fmt using given level.
     * @param[in] level    - log level.
     * @param[in] tag      - log tag, can set to NULL.
     * @param[in] file     - log file name.
     * @param[in] line     - log file line.
     * @param[in] fmt      - format control string.
     * @param[in] argument - optional arguments.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Output(int level, const char *tag, const char *file, int line, const char *fmt, ...);

    /**
     * Like Output() method, but message is non-format message, use to improve performance.
     * @param[in] level      - log level.
     * @param[in] tag        - log tag, can set to NULL.
     * @param[in] file       - log file name.
     * @param[in] line       - log file line.
     * @param[in] message    - message string, non-format.
     * @param[in] messageLen - message string length, if -1, will auto calculate.
     */
    int OutputNonFormat(int level, const char *tag, const char *file, int line, const char *message, size_t messageLen = -1);

private:
    /**
     * Direct output message using given level.
     */
    int DirectOutput(int level, const char *tag, const char *file, int line, char *message, int len);
    /**
     * Build log data.
     * @param[in] level   - log level.
     * @param[in] tag     - log tag.
     * @param[in] file    - log file name.
     * @param[in] line    - log file line.
     * @param[in] message - log format control string.
     * @param[in] len     - the message length, not include tailing character.
     * @return LLBC_LogData * - the log data.
     */
    LLBC_LogData *BuildLogData(int level,
                               const char *tag,
                               const char *file,
                               int line,
                               char *message,
                               int len);

private:
    LLBC_RecursiveLock _mutex;

    LLBC_String _name;

    int _logLevel;
    const LLBC_LoggerConfigInfo *_config;

    LLBC_LogRunnable *_logRunnable;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOGGER_H__
