/**
 * @file    LogConfigInfo.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/11
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_LOG_CONFIG_INFO_H__
#define __LLBC_CORE_LOG_LOG_CONFIG_INFO_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_Property;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The logger config info class encapsulation.
 */
class LLBC_LoggerConfigInfo
{
public:
    LLBC_LoggerConfigInfo();
    ~LLBC_LoggerConfigInfo();

public:
    /**
     * Initialize logger config.
     * @param[in] cfg - config property.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Initialize(const LLBC_Property &cfg);

public:
    /**
     * Get log level.
     * @return int - log level.
     */
    int GetLogLevel() const;

    /**
     * Get asynchronous mode switch.
     * @return bool - asynchronous mode switch.
     */
    bool IsAsyncMode() const;

    /**
     * Get file refresh interval.
     * @return int - the file refresh interval.
     */
    int GetFlushInterval() const;

public:
    /**
     * Get log to console switch.
     * @return bool - log to console switch.
     */
    bool IsLogToConsole() const;

    /**
     * Get console log level.
     * @return int - console log level
     */
    int GetConsoleLogLevel() const;

    /**
     * Get console log pattern.
     * @return const LLBC_String & - console log pattern.
     */
    const LLBC_String &GetConsolePattern() const;

    /**
     * Get colorful output flag(only available in WIN platform).
     * @return bool - the colorful output flag.
     */
    bool IsColourfulOutput() const;

public:
    /**
     * Get log to file switch.
     * @return bool - log to file switch.
     */
    bool IsLogToFile() const;

    /**
     * Get file log level.
     * @return int - file log level.
     */
    int GetFileLogLevel() const;

    /**
     * Get file log pattern.
     * @return const LLBC_String & - file log pattern.
     */
    const LLBC_String &GetFilePattern() const;

    /**
     * Get log file name.
     * @return const LLBC_String & - log file name.
     */
    const LLBC_String &GetLogFile() const;

    /**
     * Get force application log path flag
     * @return const bool - log path flag
     */
    const bool GetForceAppLogPath() const;

    /**
     * Get daily rolling mode switch(available in file appender).
     * @return bool - daily rolling mode switch.
     */
    bool IsDailyRollingMode() const;

    /**
     * Get max log file size.
     * @return long - max log file size.
     */
    long GetMaxFileSize() const;

    /**
     * Get max backup log file index.
     * @return int - max backup log file index.
     */
    int GetMaxBackupIndex() const;

    /**
     * Get file buffer size.
     * @return int - the file buffer size.
     */
    int GetFileBufferSize() const;

public:
    /**
     * Get take over option.
     * @return bool - take over option.
     */
    bool IsTakeOver() const;

private:
    /**
     * Normalize the log file name.
     */
    void NormalizeLogFileName();

    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_LoggerConfigInfo);

private:
    int _logLevel;
    bool _asyncMode;
    int _flushInterval;

    bool _logToConsole;
    int _consoleLogLevel;
    LLBC_String _consolePattern;
    bool _colourfulOutput;

    bool _logToFile;
    int _fileLogLevel;
    LLBC_String _logFile;
    bool _forceAppLogPath;
    LLBC_String _filePattern;
    bool _dailyMode;
    long _maxFileSize;
    int _maxBackupIndex;
    int _fileBufferSize;

    bool _takeOver;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOG_CONFIG_INFO_H__
