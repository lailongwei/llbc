/**
 * @file    LoggerConfigInfo.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/11
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/config/Property.h"

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LoggerConfigInfo.h"

__LLBC_NS_BEGIN

LLBC_LoggerConfigInfo::LLBC_LoggerConfigInfo()
: _logLevel(LLBC_LogLevel::End)
, _asyncMode(false)
, _flushInterval(0)

, _logToConsole(true)
, _consolePattern()
, _colourfulOutput(true)

, _logToFile(false)
, _logFile()
, _filePattern()
, _dailyMode(true)
, _maxFileSize(INT_MAX)
, _maxBackupIndex(0)
, _fileBufferSize(0)
{
}

LLBC_LoggerConfigInfo::~LLBC_LoggerConfigInfo()
{
}

int LLBC_LoggerConfigInfo::Initialize(const LLBC_Property &cfg)
{
	// Common log configs.
	_logLevel = (cfg.HasProperty("level") ? LLBC_LogLevel::Str2Levevl(cfg.GetValue("level").AsCStr()) : LLBC_CFG_LOG_DEFAULT_LEVEL);
    _asyncMode = (cfg.HasProperty("asynchronous") ? cfg.GetValue("asynchronous").AsBool() : LLBC_CFG_LOG_DEFAULT_ASYNC_MODE);
    _flushInterval= (cfg.HasProperty("flushInterval") ? cfg.GetValue("flushInterval").AsInt32() : LLBC_CFG_LOG_DEFAULT_LOG_FLUSH_INTERVAL);

	// Console log configs.
    _logToConsole = (cfg.HasProperty("logToConsole") ? cfg.GetValue("logToConsole").AsBool() : LLBC_CFG_LOG_DEFAULT_LOG_TO_CONSOLE);
    _consolePattern = (cfg.HasProperty("consolePattern") ? cfg.GetValue("consolePattern").AsStr() : LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN);
    _colourfulOutput = (cfg.HasProperty("colourfulOutput") ? cfg.GetValue("colourfulOutput").AsBool() : LLBC_CFG_LOG_DEFAULT_ENABLED_COLOURFUL_OUTPUT);

	// File log configs.
    _logToFile = (cfg.HasProperty("logToFile") ? cfg.GetValue("logToFile").AsBool() : LLBC_CFG_LOG_DEFAULT_LOG_TO_FILE);
    _logFile = (cfg.HasProperty("logFile") ? cfg.GetValue("logFile").AsStr() : LLBC_CFG_LOG_DEFAULT_LOG_FILE_NAME);
    _filePattern = (cfg.HasProperty("filePattern") ? cfg.GetValue("filePattern").AsStr() : LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN);
    _dailyMode = (cfg.HasProperty("dailyRollingMode") ? cfg.GetValue("dailyRollingMode").AsBool() : LLBC_CFG_LOG_DEFAULT_DAILY_MODE);
    _maxFileSize = (cfg.HasProperty("maxFileSize") ? cfg.GetValue("maxFileSize").AsLong() : LLBC_CFG_LOG_MAX_FILE_SIZE);
    _maxBackupIndex = (cfg.HasProperty("maxBackupIndex") ? cfg.GetValue("maxBackupIndex").AsInt32() : LLBC_CFG_LOG_MAX_BACKUP_INDEX);

    if (_asyncMode)
        _fileBufferSize = (cfg.HasProperty("fileBufferSize") ? 
                cfg.GetValue("fileBufferSize").AsInt32() : LLBC_CFG_LOG_DEFAULT_LOG_FILE_BUFFER_SIZE);
    else
        _fileBufferSize = 0;

	// Check configs.
	if (!(_logLevel >= LLBC_LogLevel::Begin && _logLevel < LLBC_LogLevel::End))
		_logLevel = LLBC_CFG_LOG_DEFAULT_LEVEL;

    _maxFileSize = MAX(1, _maxFileSize);
    _maxBackupIndex = MAX(0, _maxBackupIndex);
    _flushInterval = MIN(MAX(0, _flushInterval), LLBC_CFG_LOG_MAX_LOG_FLUSH_INTERVAL);
    
#if LLBC_TARGET_PLATFORM_IPHONE
    if (_logToFile &&
        !_logFile.empty() &&
        _logFile[0] != LLBC_SLASH_A)
        _logFile = LLBC_GetTemporaryDirectory(true) + _logFile;
#endif // LLBC_TARGET_PLATFORM_IPHONE

    return LLBC_OK;
}

int LLBC_LoggerConfigInfo::GetLogLevel() const
{
    return _logLevel;
}

bool LLBC_LoggerConfigInfo::IsAsyncMode() const
{
    return _asyncMode;
}

int LLBC_LoggerConfigInfo::GetFlushInterval() const
{
    return _flushInterval;
}

bool LLBC_LoggerConfigInfo::IsLogToConsole() const
{
    return _logToConsole;
}

const LLBC_String &LLBC_LoggerConfigInfo::GetConsolePattern() const
{
    return _consolePattern;
}

bool LLBC_LoggerConfigInfo::IsColourfulOutput() const
{
    return _colourfulOutput;
}

bool LLBC_LoggerConfigInfo::IsLogToFile() const
{
    return _logToFile;
}

const LLBC_String &LLBC_LoggerConfigInfo::GetFilePattern() const
{
    return _filePattern;
}

const LLBC_String &LLBC_LoggerConfigInfo::GetLogFile() const
{
    return _logFile;
}

bool LLBC_LoggerConfigInfo::IsDailyRollingMode() const
{
    return _dailyMode;
}

long LLBC_LoggerConfigInfo::GetMaxFileSize() const
{
    return _maxFileSize;
}

int LLBC_LoggerConfigInfo::GetMaxBackupIndex() const
{
    return _maxBackupIndex;
}

int LLBC_LoggerConfigInfo::GetFileBufferSize() const
{
    return _fileBufferSize;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
