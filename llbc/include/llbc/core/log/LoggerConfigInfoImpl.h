/**
 * @file    LoggerConfigInfoImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017/11/07
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_CORE_LOG_LOGGER_CONFIG_INFO_H__

__LLBC_NS_BEGIN

inline int LLBC_LoggerConfigInfo::GetLogLevel() const
{
    return _logLevel;
}

inline bool LLBC_LoggerConfigInfo::IsAsyncMode() const
{
    return _asyncMode;
}

inline int LLBC_LoggerConfigInfo::GetFlushInterval() const
{
    return _flushInterval;
}

inline bool LLBC_LoggerConfigInfo::IsLogToConsole() const
{
    return _logToConsole;
}

inline int LLBC_LoggerConfigInfo::GetConsoleLogLevel() const
{
    return _consoleLogLevel;
}

inline const LLBC_String &LLBC_LoggerConfigInfo::GetConsolePattern() const
{
    return _consolePattern;
}

inline bool LLBC_LoggerConfigInfo::IsColourfulOutput() const
{
    return _colourfulOutput;
}

inline bool LLBC_LoggerConfigInfo::IsLogToFile() const
{
    return _logToFile;
}

inline int LLBC_LoggerConfigInfo::GetFileLogLevel() const
{
    return _fileLogLevel;
}

inline const LLBC_String &LLBC_LoggerConfigInfo::GetFilePattern() const
{
    return _filePattern;
}

inline const LLBC_String &LLBC_LoggerConfigInfo::GetLogFile() const
{
    return _logFile;
}

inline const LLBC_String &LLBC_LoggerConfigInfo::GetLogFileSuffix() const
{
    return _logFileSuffix;
}

inline bool LLBC_LoggerConfigInfo::IsLogCodeFilePath() const
{
    return _logCodeFilePath;
}

inline bool LLBC_LoggerConfigInfo::IsForceAppLogPath() const
{
    return _forceAppLogPath;
}

inline bool LLBC_LoggerConfigInfo::IsDailyRollingMode() const
{
    return _dailyMode;
}

inline long LLBC_LoggerConfigInfo::GetMaxFileSize() const
{
    return _maxFileSize;
}

inline int LLBC_LoggerConfigInfo::GetMaxBackupIndex() const
{
    return _maxBackupIndex;
}

inline int LLBC_LoggerConfigInfo::GetFileBufferSize() const
{
    return _fileBufferSize;
}

inline bool LLBC_LoggerConfigInfo::IsTakeOver() const
{
    return _takeOver;
}

__LLBC_NS_END

#endif // __LLBC_CORE_LOG_LOGGER_CONFIG_INFO_H__
