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

__LLBC_NS_BEGIN

inline const LLBC_String &LLBC_LoggerConfigInfo::GetLoggerName() const
{
    return _loggerName;
}

inline bool LLBC_LoggerConfigInfo::IsNotConfigUseRoot() const
{
    return _notConfigUseRoot;
}

inline int LLBC_LoggerConfigInfo::GetLogLevel() const
{
    return _logLevel;
}

inline bool LLBC_LoggerConfigInfo::IsAsyncMode() const
{
    return _asyncMode;
}

inline bool LLBC_LoggerConfigInfo::IsIndependentThread() const
{
    return _independentThread;
}

inline int LLBC_LoggerConfigInfo::GetFlushInterval() const
{
    return _flushInterval;
}

inline bool LLBC_LoggerConfigInfo::IsAddTimestampInJsonLog() const
{
    return _addTimestampInJsonLog;
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

inline const LLBC_String &LLBC_LoggerConfigInfo::GetLogDir() const
{
    return _logDir;
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

inline int LLBC_LoggerConfigInfo::GetFileRollingMode() const
{
    return _fileRollingMode;
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

inline bool LLBC_LoggerConfigInfo::IsLazyCreateLogFile() const
{
    return _lazyCreateLogFile;
}

__LLBC_NS_END
