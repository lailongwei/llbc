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
     * @param[in] loggerName - the logger name.
     * @param[in] cfg        - config property.
     * @param[in] rootCfg    - the root logger config.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Initialize(const LLBC_String &loggerName,
                   const LLBC_Property &cfg,
                   const LLBC_LoggerConfigInfo *rootCfg);

public:
    /**
     * Get logger name.
     * @return const LLBC_String & - the logger name.
     */
    const LLBC_String &GetLoggerName() const;

    /**
     * Get not config use option from root or not(use library default).
     * @return bool - the use root option.
     */
    bool IsNotConfigUseRoot() const;

public:
    /**
     * Get logger log level.
     * @return int - the logger log level.
     */
    int GetLogLevel() const;

    /**
     * Get asynchronous mode switch.
     * @return bool - asynchronous mode switch.
     */
    bool IsAsyncMode() const;

    /**
     * Independent logger thread switch, only available in Async-Mode.
     * @return bool - independent logger thread switch.
     */
    bool IsIndependentThread() const;

    /**
     * Get file refresh interval.
     * @return int - the file refresh interval.
     */
    int GetFlushInterval() const;

public:
    /**
     * Add timestamp in json log.
     * @return bool - add timestamp in json log flag.
     */
    bool IsAddTimestampInJsonLog() const;

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
     * Get log directory.
     * @return const LLBC_String & - log directory.
     */
    const LLBC_String &GetLogDir() const;

    /**
     * Get log file name.
     * @return const LLBC_String & - log file name.
     */
    const LLBC_String &GetLogFile() const;

    /**
     * Get log file suffix.
     * @return const LLBC_String & - log file suffix.
     */
    const LLBC_String &GetLogFileSuffix() const;

    /**
     * Get log file path or not, when log code file name(%f).
     * @return bool - log file path flag, if true, log file path, otherwise only log file name.
     */
    bool IsLogCodeFilePath() const;

    /**
     * Get force application log path flag
     * @return const bool - log path flag
     */
    bool IsForceAppLogPath() const;

    /**
     * Get file rolling mode.
     * @return bool - file rolling mode.
     */
    int GetFileRollingMode() const;

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

public:
    /**
     * Get logfile create option.
     * @return bool - logfile create option.
     */
    bool IsLazyCreateLogFile() const;

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
    LLBC_String _loggerName;
    bool _notConfigUseRoot;

    int _logLevel;
    bool _asyncMode;
    bool _independentThread;
    int _flushInterval;

    bool _addTimestampInJsonLog;

    bool _logToConsole;
    int _consoleLogLevel;
    LLBC_String _consolePattern;
    bool _colourfulOutput;

    bool _logToFile;
    int _fileLogLevel;
    LLBC_String _logDir;
    LLBC_String _logFile;
    LLBC_String _logFileSuffix;
    bool _logCodeFilePath;
    bool _forceAppLogPath;
    LLBC_String _filePattern;
    int _fileRollingMode;
    long _maxFileSize;
    int _maxBackupIndex;
    int _fileBufferSize;
    bool _lazyCreateLogFile;

    bool _takeOver;
};

__LLBC_NS_END

#include "llbc/core/log/LoggerConfigInfoInl.h"


