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

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Process.h"
#include "llbc/core/utils/Util_Text.h"
#include  "llbc/core/file/Directory.h"
#include "llbc/core/config/Property.h"

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LoggerConfigInfo.h"

__LLBC_NS_BEGIN

LLBC_LoggerConfigInfo::LLBC_LoggerConfigInfo()
: _logLevel(LLBC_LogLevel::End)
, _asyncMode(false)
, _flushInterval(0)

, _logToConsole(true)
, _consoleLogLevel(LLBC_LogLevel::End)
, _consolePattern()
, _colourfulOutput(true)

, _logToFile(false)
, _fileLogLevel(LLBC_LogLevel::End)
, _logFile()
, _logFileSuffix()
, _logCodeFilePath(true)
, _forceAppLogPath(false)
, _filePattern()
, _dailyMode(true)
, _maxFileSize(INT_MAX)
, _maxBackupIndex(0)
, _fileBufferSize(0)
, _lazyCreateLogFile(false)

, _takeOver(false)
{
}

LLBC_LoggerConfigInfo::~LLBC_LoggerConfigInfo()
{
}

int LLBC_LoggerConfigInfo::Initialize(const LLBC_Property &cfg)
{
    // Common log configs.
    _logLevel = (cfg.HasProperty("level") ? LLBC_LogLevel::Str2Level(cfg.GetValue("level").AsCStr()) : LLBC_CFG_LOG_DEFAULT_LEVEL);
    _asyncMode = (cfg.HasProperty("asynchronous") ? cfg.GetValue("asynchronous").AsBool() : LLBC_CFG_LOG_DEFAULT_ASYNC_MODE);
    _flushInterval= (cfg.HasProperty("flushInterval") ? cfg.GetValue("flushInterval").AsInt32() : LLBC_CFG_LOG_DEFAULT_LOG_FLUSH_INTERVAL);

    // Console log configs.
    _logToConsole = (cfg.HasProperty("logToConsole") ? cfg.GetValue("logToConsole").AsBool() : LLBC_CFG_LOG_DEFAULT_LOG_TO_CONSOLE);
    _consoleLogLevel = (cfg.HasProperty("consoleLogLevel") ? LLBC_LogLevel::Str2Level(cfg.GetValue("consoleLogLevel").AsCStr()) : _logLevel);
    _consolePattern = (cfg.HasProperty("consolePattern") ? cfg.GetValue("consolePattern").AsStr() : LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN);
    _colourfulOutput = (cfg.HasProperty("colourfulOutput") ? cfg.GetValue("colourfulOutput").AsBool() : LLBC_CFG_LOG_DEFAULT_ENABLED_COLOURFUL_OUTPUT);

    // File log configs.
    _logToFile = (cfg.HasProperty("logToFile") ? cfg.GetValue("logToFile").AsBool() : LLBC_CFG_LOG_DEFAULT_LOG_TO_FILE);
    _fileLogLevel = (cfg.HasProperty("fileLogLevel") ? LLBC_LogLevel::Str2Level(cfg.GetValue("fileLogLevel").AsCStr()) : _logLevel);
    _logFile = (cfg.HasProperty("logFile") ? cfg.GetValue("logFile").AsStr() : LLBC_CFG_LOG_DEFAULT_LOG_FILE_NAME);
    _logFileSuffix = (cfg.HasProperty("logFileSuffix") ? cfg.GetValue("logFileSuffix").AsStr() : LLBC_CFG_LOG_DEFAULT_LOG_FILE_SUFFIX);
    _forceAppLogPath = (cfg.HasProperty("forceAppLogPath") ? cfg.GetValue("forceAppLogPath").AsBool() : LLBC_CFG_LOG_DEFAULT_FORCE_APP_LOG_PATH);
    _logCodeFilePath = (cfg.HasProperty("logCodeFilePath") ? cfg.GetValue("logCodeFilePath").AsBool() : LLBC_CFG_LOG_DEFAULT_LOG_CODE_FILE_PATH);
    _filePattern = (cfg.HasProperty("filePattern") ? cfg.GetValue("filePattern").AsStr() : LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN);
    _dailyMode = (cfg.HasProperty("dailyRollingMode") ? cfg.GetValue("dailyRollingMode").AsBool() : LLBC_CFG_LOG_DEFAULT_DAILY_MODE);
    _maxFileSize = (cfg.HasProperty("maxFileSize") ? cfg.GetValue("maxFileSize").AsLong() : LLBC_CFG_LOG_MAX_FILE_SIZE);
    _maxBackupIndex = (cfg.HasProperty("maxBackupIndex") ? cfg.GetValue("maxBackupIndex").AsInt32() : LLBC_CFG_LOG_MAX_BACKUP_INDEX);
    _lazyCreateLogFile = (cfg.HasProperty("lazyCreateLogFile") ? cfg.GetValue("lazyCreateLogFile").AsBool() : LLBC_CFG_LOG_LAZY_CREATE_LOG_FILE);

    // Misc configs.
    _takeOver = (cfg.HasProperty("takeOver") ? cfg.GetValue("takeOver").AsBool() : LLBC_CFG_LOG_ROOT_LOGGER_TAKE_OVER_UNCONFIGED);

    if (_asyncMode)
        _fileBufferSize = (cfg.HasProperty("fileBufferSize") ? 
                cfg.GetValue("fileBufferSize").AsInt32() : LLBC_CFG_LOG_DEFAULT_LOG_FILE_BUFFER_SIZE);
    else
        _fileBufferSize = 0;

    // Check configs.
    if (!LLBC_LogLevel::IsLegal(_logLevel))
        _logLevel = LLBC_CFG_LOG_DEFAULT_LEVEL;
    if (!LLBC_LogLevel::IsLegal(_consoleLogLevel))
        _consoleLogLevel = _logLevel;
    if (!LLBC_LogLevel::IsLegal(_fileLogLevel))
        _fileLogLevel = _logLevel;

    _maxFileSize = MAX(1, _maxFileSize);
    _maxBackupIndex = MAX(0, _maxBackupIndex);
    _flushInterval = MIN(MAX(0, _flushInterval), LLBC_CFG_LOG_MAX_LOG_FLUSH_INTERVAL);

    // Normallize log file name.
    NormalizeLogFileName();

    return LLBC_OK;
}

void LLBC_LoggerConfigInfo::NormalizeLogFileName()
{
    const LLBC_String curProcId = 
        LLBC_Num2Str(LLBC_GetCurrentProcessId());
    _logFile.findreplace("%p", curProcId); 

    const LLBC_String modFileName = 
        LLBC_Directory::BaseName(LLBC_Directory::ModuleFileName());
    _logFile.findreplace("%m", modFileName);

#if LLBC_TARGET_PLATFORM_IPHONE
    if (_logToFile &&
        !_logFile.empty() &&
        _logFile[0] != LLBC_SLASH_A)
        _logFile = LLBC_Directory::Join(LLBC_Directory::TempDir(), _logFile);
#endif // LLBC_TARGET_PLATFORM_IPHONE
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
