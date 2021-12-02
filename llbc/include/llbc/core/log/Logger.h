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

#include "llbc/core/thread/SpinLock.h"
#include "llbc/core/thread/MessageBlock.h"
#include "llbc/core/objectpool/ExportedObjectPoolTypes.h"

#include "llbc/core/log/LogLevel.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
struct LLBC_LogData;
class LLBC_LoggerConfigInfo;
class LLBC_ILogAppender;
class LLBC_LogRunnable;

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
     * @param[in] config            - logger config info.
     * @param[in] sharedLogRunnable - the shared log runnable.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Initialize(const LLBC_LoggerConfigInfo *config, LLBC_LogRunnable *sharedLogRunnable);

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

    /**
     * Get asynchronous mode switch.
     * @return bool - asynchronous mode switch.
     */
    bool IsAsyncMode() const;

public:
    /**
     * Install logger hook.
     * @param[in] level     - the log level.
     * @param[in] hookDeleg - the hook delegate.
     * @return int - return 0 if success, otherwise return -1.
     */
    int InstallHook(int level, const std::function<void(const LLBC_LogData *)> &hookDeleg);

    /**
     * Uninstall error hook.
     */
    void UninstallHook(int level);

public:
    /**
     * Output specific level message.
     * @param[in] tag      - log tag, can set to nullptr.
     * @param[in] file     - log file name.
     * @param[in] line     - log file line.
     * @param[in] fmt      - format control string.
     * @param[in] ...      - optional arguments.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Debug(const char *tag, const char *file, int line, const char *fmt, ...) LLBC_STRING_FORMAT_CHECK(5, 6);
    int Info(const char *tag, const char *file, int line, const char *fmt, ...) LLBC_STRING_FORMAT_CHECK(5, 6);
    int Warn(const char *tag, const char *file, int line, const char *fmt, ...) LLBC_STRING_FORMAT_CHECK(5, 6);
    int Error(const char *tag, const char *file, int line, const char *fmt, ...) LLBC_STRING_FORMAT_CHECK(5, 6);
    int Fatal(const char *tag, const char *file, int line, const char *fmt, ...) LLBC_STRING_FORMAT_CHECK(5, 6);

    /**
     * Output fmt using given level.
     * @param[in] level    - log level.
     * @param[in] tag      - log tag, can set to nullptr.
     * @param[in] file     - log file name.
     * @param[in] line     - log file line.
     * @param[in] fmt      - format control string.
     * @param[in] ...      - optional arguments.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Output(int level, const char *tag, const char *file, int line, const char *fmt, ...) LLBC_STRING_FORMAT_CHECK(6, 7);

    /**
     * Like Output() method, but message is non-format message, use to improve performance.
     * @param[in] level      - log level.
     * @param[in] tag        - log tag, can set to nullptr.
     * @param[in] file       - log file name.
     * @param[in] line       - log file line.
     * @param[in] message    - message string, non-format.
     * @param[in] messageLen - message string length, if -1, will auto calculate.
     * @return int - return 0 if success, otherwise return -1.
     */
    int OutputNonFormat(int level, const char *tag, const char *file, int line, const char *message, size_t messageLen = -1);

    /**
     * Like OutputNonFormat(), but this output method will steal 'message'.
     * @param[in] level      - log level.
     * @param[in] tag        - log tag, can set to nullptr.
     * @param[in] file       - log file name.
     * @param[in] line       - log file line.
     * @param[in] message    - message string, non-format.
     * @return int - return 0 if success, otherwise return -1.
     */
    int OutputNonFormat2(int level, const char *tag, const char *file, int line, char *message, size_t messageLen = -1);

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
    /**
     * Friend classs: LLBC_LogRunnable.
     * Asset method/data-members:
     * - OutputLogData(const LLBC_LogData &data):int
     * - Flush(bool force, sint64 now):void
     */
    friend class LLBC_LogRunnable;

    /**
     * Add log appender.
     * @param[in] appender - log appender.
     */
    void AddAppender(LLBC_ILogAppender *appender);

    /**
     * Output log data.
     * @param[in] data - log data.
     */
    int OutputLogData(const LLBC_LogData &data);

    /**
    * Flush logger(for now, just only need flush all appenders).
    * @param[in] force - force or not.
    * @param[in] now   - now time in milli-seconds.
    */
    void Flush(bool force, sint64 now);

    /**
     * Flush appenders.
     * @param[in] force - force flush or not, default is false.
     */
    void FlushAppenders(bool force);

private:
    LLBC_String _name;
    LLBC_SpinLock _lock;

    int _logLevel;
    const LLBC_LoggerConfigInfo *_config;

    LLBC_LogRunnable *_logRunnable;

    sint64 _lastFlushTime;
    sint64 _flushInterval;
    LLBC_ILogAppender *_appenders;

    LLBC_SafetyObjectPool _objPool;
    LLBC_ObjectPoolInst<LLBC_MessageBlock> &_msgBlockPoolInst;
    LLBC_ObjectPoolInst<LLBC_LogData> &_logDataPoolInst;
    std::function<void(const LLBC_LogData *)> _hookDelegs[LLBC_LogLevel::End];
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOGGER_H__

