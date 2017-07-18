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
, _forceAppLogPath(false)
, _filePattern()
, _dailyMode(true)
, _maxFileSize(INT_MAX)
, _maxBackupIndex(0)
, _fileBufferSize(0)

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
    _forceAppLogPath = (cfg.HasProperty("forceAppLogPath") ? cfg.GetValue("forceAppLogPath").AsBool() : LLBC_CFG_LOG_DEFAULT_FORCE_APP_LOG_PATH);
    _filePattern = (cfg.HasProperty("filePattern") ? cfg.GetValue("filePattern").AsStr() : LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN);
    _dailyMode = (cfg.HasProperty("dailyRollingMode") ? cfg.GetValue("dailyRollingMode").AsBool() : LLBC_CFG_LOG_DEFAULT_DAILY_MODE);
    _maxFileSize = (cfg.HasProperty("maxFileSize") ? cfg.GetValue("maxFileSize").AsLong() : LLBC_CFG_LOG_MAX_FILE_SIZE);
    _maxBackupIndex = (cfg.HasProperty("maxBackupIndex") ? cfg.GetValue("maxBackupIndex").AsInt32() : LLBC_CFG_LOG_MAX_BACKUP_INDEX);

    // Misc configs.
    _takeOver = (cfg.HasProperty("takeOver") ? cfg.GetValue("takeOver").AsBool() : LLBC_CFG_LOG_ROOT_LOGGER_TAKE_OVER_UNCONFIGED);

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

    // Normallize log file name.
    NormalizeLogFileName();

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

int LLBC_LoggerConfigInfo::GetConsoleLogLevel() const
{
    return _consoleLogLevel;
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

int LLBC_LoggerConfigInfo::GetFileLogLevel() const
{
    return _fileLogLevel;
}

const LLBC_String &LLBC_LoggerConfigInfo::GetFilePattern() const
{
    return _filePattern;
}

const LLBC_String &LLBC_LoggerConfigInfo::GetLogFile() const
{
    return _logFile;
}

const bool LLBC_LoggerConfigInfo::GetForceAppLogPath() const
{
    return _forceAppLogPath;
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

bool LLBC_LoggerConfigInfo::IsTakeOver() const
{
    return _takeOver;
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
