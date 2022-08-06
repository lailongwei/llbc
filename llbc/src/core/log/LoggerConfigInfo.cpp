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

/*
 * Internal macros define.
 */
#define __LLBC_GetLogCfg(key, dft, rootMeth, asMeth)  __LLBC_GetLogCfg2(key, LLBC_CFG_LOG_DEFAULT_##dft, rootMeth, asMeth)

#define __LLBC_GetLogCfg2(key, dft, rootMeth, asMeth) (cfg.HasProperty(key) ? \
                                                       cfg.GetValue(key).asMeth() : \
                                                       (_notConfigUseRoot ? rootCfg->rootMeth() : (dft))) \

__LLBC_NS_BEGIN

LLBC_LoggerConfigInfo::LLBC_LoggerConfigInfo()
: _loggerName()
, _notConfigUseRoot(false)

, _logLevel(LLBC_LogLevel::End)
, _asyncMode(false)
, _independentThread(false)
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

int LLBC_LoggerConfigInfo::Initialize(const LLBC_String &loggerName,
                                      const LLBC_Property &cfg,
                                      const LLBC_LoggerConfigInfo *rootCfg)
{
    // LoggerName.
    _loggerName = loggerName;

    // Not config use default/root option.
    LLBC_String notCfgUseOpt;
    if (cfg.HasProperty("notConfigUse"))
        notCfgUseOpt = cfg.GetValue("notConfigUse").AsStr();
    else if (rootCfg)
        notCfgUseOpt = rootCfg->IsNotConfigUseRoot() ? "root" : "default";
    else
        notCfgUseOpt = LLBC_CFG_LOG_DEFAULT_NOT_CONFIG_OPTION_USE;
    _notConfigUseRoot = notCfgUseOpt.strip().tolower() == "root" && rootCfg;

    // Common log configs.
    if (cfg.HasProperty("level"))
        _logLevel = LLBC_LogLevel::Str2Level(cfg.GetValue("level").AsStr().c_str());
    else
        _logLevel = _notConfigUseRoot ? rootCfg->_logLevel : LLBC_CFG_LOG_DEFAULT_LEVEL;
    _asyncMode = __LLBC_GetLogCfg("asynchronous", ASYNC_MODE, IsAsyncMode, AsLooseBool);
    if (_asyncMode)
        _independentThread = __LLBC_GetLogCfg("independentThread", INDEPENDENT_THREAD, IsIndependentThread, AsLooseBool);
     _flushInterval = __LLBC_GetLogCfg("flushInterval", LOG_FLUSH_INTERVAL, GetFlushInterval, AsInt32);

    // Console log configs.
     _logToConsole = __LLBC_GetLogCfg("logToConsole", LOG_TO_CONSOLE, IsLogToConsole, AsLooseBool);
    if (_logToConsole)
    {
        if (cfg.HasProperty("consoleLogLevel"))
            _consoleLogLevel = LLBC_LogLevel::Str2Level(cfg.GetValue("consoleLogLevel").AsStr().c_str());
        else
            _consoleLogLevel = _notConfigUseRoot ? rootCfg->GetConsoleLogLevel() : _logLevel;

        if (cfg.HasProperty("consolePattern"))
            _consolePattern = cfg.GetValue("consolePattern").AsStr().c_str();
        else
            _consolePattern = _notConfigUseRoot ? 
                rootCfg->GetConsolePattern().c_str() : LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN;
        _colourfulOutput = __LLBC_GetLogCfg("colourfulOutput", COLOURFUL_OUTPUT, IsColourfulOutput, AsLooseBool);
    }

    // File log configs.
    _logToFile = __LLBC_GetLogCfg("logToFile", LOG_TO_FILE, IsLogToFile, AsLooseBool);
    if (_logToFile)
    {
        // File log level option.
        if (cfg.HasProperty("fileLogLevel"))
            _fileLogLevel = LLBC_LogLevel::Str2Level(cfg.GetValue("fileLogLevel").AsStr().c_str());
        else
            _fileLogLevel = _notConfigUseRoot ? rootCfg->GetFileLogLevel() : _logLevel;

        // Log dir option.
        if (!(_logDir = __LLBC_GetLogCfg2("logDir", "", GetLogDir, AsStr).strip()).empty())
        {
            #if LLBC_TARGET_PLATFORM_WIN32
            _logDir = _logDir.findreplace(LLBC_SLASH_A, LLBC_BACKLASH_A);
            #else
            _logDir = _logDir.findreplace(LLBC_BACKLASH_A, LLBC_SLASH_A);
            #endif
        }

        // Force application log path(log file place into application module file dir).
        _forceAppLogPath = __LLBC_GetLogCfg("forceAppLogPath", FORCE_APP_LOG_PATH, IsForceAppLogPath, AsLooseBool);

        // Log file path option.
        if (cfg.HasProperty("logFile"))
            _logFile = cfg.GetValue("logFile").AsStr().strip();
        if (_logFile.empty())
            _logFile = _loggerName;
        if (!_logDir.empty())
            _logFile = LLBC_Directory::Join(_logDir, _logFile);
        if (!LLBC_Directory::IsAbsPath(_logFile))
        {
            if (_forceAppLogPath)
                _logFile = LLBC_Directory::Join(LLBC_Directory::ModuleFileDir(), _logFile);
            else
                _logFile = LLBC_Directory::AbsPath(_logFile);
        }

        // Other file log options.
        _logFileSuffix = __LLBC_GetLogCfg("logFileSuffix", LOG_FILE_SUFFIX, GetLogFileSuffix, AsStr);
        _logCodeFilePath = __LLBC_GetLogCfg("logCodeFilePath", LOG_CODE_FILE_PATH, IsLogCodeFilePath, AsLooseBool);
        if (cfg.HasProperty("filePattern"))
            _filePattern = cfg.GetValue("filePattern").AsStr().c_str();
        else
            _filePattern = _notConfigUseRoot ? 
                rootCfg->GetFilePattern().c_str() : LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN;
        _dailyMode = __LLBC_GetLogCfg("dailyRollingMode", DAILY_MODE, IsDailyRollingMode, AsLooseBool);
        _maxFileSize = __LLBC_GetLogCfg2("maxFileSize", LLBC_CFG_LOG_MAX_FILE_SIZE, GetMaxFileSize, AsLong);
        _maxBackupIndex = __LLBC_GetLogCfg2("maxBackupIndex", LLBC_CFG_LOG_MAX_BACKUP_INDEX, GetMaxBackupIndex, AsInt32);
        _lazyCreateLogFile = __LLBC_GetLogCfg2("lazyCreateLogFile", LLBC_CFG_LOG_LAZY_CREATE_LOG_FILE, IsLazyCreateLogFile, AsLooseBool);

        if (_asyncMode)
            _fileBufferSize = __LLBC_GetLogCfg("fileBufferSize", LOG_FILE_BUFFER_SIZE, GetFileBufferSize, AsInt32);
    }

    // Misc configs.
    if (!rootCfg)
        _takeOver = __LLBC_GetLogCfg2("takeOver", LLBC_CFG_LOG_ROOT_LOGGER_TAKE_OVER_UNCONFIGED, IsTakeOver, AsLooseBool);

    // Check configs.
    if (!LLBC_LogLevel::IsLegal(_logLevel))
        _logLevel = LLBC_CFG_LOG_DEFAULT_LEVEL;
    if (!LLBC_LogLevel::IsLegal(_consoleLogLevel))
        _consoleLogLevel = _logLevel;
    if (!LLBC_LogLevel::IsLegal(_fileLogLevel))
        _fileLogLevel = _logLevel;

    _maxFileSize = MAX(1024, _maxFileSize);
    _maxBackupIndex = MAX(0, _maxBackupIndex);
    _flushInterval = MIN(MAX(0, _flushInterval), LLBC_CFG_LOG_MAX_LOG_FLUSH_INTERVAL);

    // Normalize log file name.
    NormalizeLogFileName();

    // Rejudge root logger 'notConfigUse' option, if is root logger config.
    if (loggerName == LLBC_CFG_LOG_ROOT_LOGGER_NAME)
        _notConfigUseRoot = notCfgUseOpt.strip().tolower() == "root";

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

/**
 * Internal macros undef.
 */
#undef __LLBC_GetLogCfg
#undef __LLBC_GetLogCfg2

#include "llbc/common/AfterIncl.h"
