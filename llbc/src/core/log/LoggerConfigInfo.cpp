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

#include "llbc/core/os/OS_Process.h"
#include "llbc/core/utils/Util_Text.h"
#include  "llbc/core/file/Directory.h"
#include "llbc/core/variant/Variant.h"

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogRollingMode.h"
#include "llbc/core/log/BaseLogAppender.h"
#include "llbc/core/log/LoggerConfigInfo.h"

/*
 * Internal macros define.
 */
#define __LLBC_GetLogCfg(key, dft, rootMeth, asMeth)  __LLBC_GetLogCfg2(key, LLBC_CFG_LOG_DEFAULT_##dft, rootMeth, asMeth)

#define __LLBC_GetLogCfg2(key, dft, rootMeth, asMeth) (cfg[key] ? \
                                                           cfg[key].asMeth() : \
                                                               (_notConfigUseRoot ? rootCfg->rootMeth() : (dft))) \

__LLBC_NS_BEGIN

LLBC_LoggerConfigInfo::LLBC_LoggerConfigInfo()
: _notConfigUseRoot(false)

, _asyncMode(false)
, _independentThread(false)
, _flushInterval(0)

, _addTimestampInJsonLog(0)

, _logToConsole(true)
, _consoleLogLevel(LLBC_LogLevel::End)
, _colourfulOutput(true)

, _logToFile(false)
, _fileLogLevel(LLBC_LogLevel::End)
, _logCodeFilePath(true)
, _forceAppLogPath(false)
, _fileRollingMode(LLBC_LogRollingMode::End)
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
                                      const LLBC_Variant &cfg,
                                      const LLBC_LoggerConfigInfo *rootCfg)
{
    // LoggerName.
    _loggerName = loggerName;

    // Not config use default/root option.
    LLBC_String notCfgUseOpt;
    if (cfg["notConfigUse"])
        notCfgUseOpt = cfg["notConfigUse"].AsStr();
    else if (rootCfg)
        notCfgUseOpt = rootCfg->IsNotConfigUseRoot() ? "root" : "default";
    else
        notCfgUseOpt = LLBC_CFG_LOG_DEFAULT_NOT_CONFIG_OPTION_USE;
    _notConfigUseRoot = notCfgUseOpt.strip().tolower() == "root" && rootCfg;

    _asyncMode = __LLBC_GetLogCfg(
        "asynchronous", ASYNC_MODE, IsAsyncMode, AsLooseBool);
    if (_asyncMode)
        _independentThread = __LLBC_GetLogCfg(
            "independentThread", INDEPENDENT_THREAD, IsIndependentThread, AsLooseBool);
     _flushInterval = __LLBC_GetLogCfg(
         "flushInterval", LOG_FLUSH_INTERVAL, GetFlushInterval, AsInt32);

    // Json styled log configs.
    _addTimestampInJsonLog = __LLBC_GetLogCfg(
        "addTimestampInJsonLog", ADD_TIMESTAMP_IN_JSON_LOG, IsAddTimestampInJsonLog, AsLooseBool);

    // Console log configs.
     _logToConsole = __LLBC_GetLogCfg("logToConsole", LOG_TO_CONSOLE, IsLogToConsole, AsLooseBool);
    if (_logToConsole)
    {
        if (cfg["consoleLogLevel"])
            _consoleLogLevel = LLBC_LogLevel::GetLevelEnum(cfg["consoleLogLevel"].AsStr().c_str());
        else
            _consoleLogLevel = _notConfigUseRoot ? rootCfg->GetConsoleLogLevel() : LLBC_CFG_LOG_DEFAULT_LEVEL;

        if (cfg["consolePattern"])
            _consolePattern = cfg["consolePattern"].AsStr();
        else
            _consolePattern = _notConfigUseRoot ? 
                rootCfg->GetConsolePattern().c_str() : LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN;
        _colourfulOutput = __LLBC_GetLogCfg(
            "colourfulOutput", COLOURFUL_OUTPUT, IsColourfulOutput, AsLooseBool);
    }

    // File log configs.
    _logToFile = __LLBC_GetLogCfg("logToFile", LOG_TO_FILE, IsLogToFile, AsLooseBool);
    if (_logToFile)
    {
        // File log level.
        if (cfg["fileLogLevel"])
            _fileLogLevel = LLBC_LogLevel::GetLevelEnum(cfg["fileLogLevel"].AsStr().c_str());
        else
            _fileLogLevel = _notConfigUseRoot ? rootCfg->GetFileLogLevel() : LLBC_CFG_LOG_DEFAULT_LEVEL;

        // Log file dir.
        if (!(_logDir = __LLBC_GetLogCfg2("logDir", "", GetLogDir, AsStr).strip()).empty())
        {
            #if LLBC_TARGET_PLATFORM_WIN32
            _logDir = _logDir.findreplace('/', '\\');
            #else
            _logDir = _logDir.findreplace('\\', '/');
            #endif
        }

        // Force application log path(log file place into application module file dir).
        _forceAppLogPath = __LLBC_GetLogCfg(
            "forceAppLogPath", FORCE_APP_LOG_PATH, IsForceAppLogPath, AsLooseBool);

        // Log file name.
        if (cfg["logFile"])
            _logFile = cfg["logFile"].AsStr().strip();

        if (_logFile.empty())
        {
            if (_loggerName != LLBC_CFG_LOG_ROOT_LOGGER_NAME &&
                _notConfigUseRoot)
                _logFile = rootCfg->GetOriginalLogFile();
            else
                _logFile = "%L";
        }

        _originalLogFile = _logFile;

        if (!_logDir.empty())
            _logFile = LLBC_Directory::Join(_logDir, _logFile);
        if (!LLBC_Directory::IsAbsPath(_logFile))
        {
            if (_forceAppLogPath)
                _logFile = LLBC_Directory::Join(LLBC_Directory::ModuleFileDir(), _logFile);
            else
                _logFile = LLBC_Directory::AbsPath(_logFile);
        }

        // Log file suffix.
        _logFileSuffix = __LLBC_GetLogCfg(
            "logFileSuffix", LOG_FILE_SUFFIX, GetLogFileSuffix, AsStr);
        // Log code file path.
        _logCodeFilePath = __LLBC_GetLogCfg(
            "logCodeFilePath", LOG_CODE_FILE_PATH, IsLogCodeFilePath, AsLooseBool);
        // Log file pattern.
        if (cfg["filePattern"])
            _filePattern = cfg["filePattern"].AsStr();
        else
            _filePattern = _notConfigUseRoot ? 
                rootCfg->GetFilePattern().c_str() : LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN;

        // File rolling mode.
        if (cfg["fileRollingMode"])
            _fileRollingMode = LLBC_LogRollingMode::Str2Mode(cfg["fileRollingMode"].AsStr());
        else
            _fileRollingMode = _notConfigUseRoot ?
                rootCfg->GetFileRollingMode() : LLBC_CFG_LOG_DEFAULT_FILE_ROLLING_MODE;
        if (!LLBC_LogRollingMode::IsValid(_fileRollingMode))
            _fileRollingMode = LLBC_CFG_LOG_DEFAULT_FILE_ROLLING_MODE;

        // Max file size.
        if (cfg["maxFileSize"])
            _maxFileSize = NormalizeLogFileSize(cfg["maxFileSize"]);
        else if (_notConfigUseRoot)
            _maxFileSize = rootCfg->GetMaxFileSize();
        else
            _maxFileSize = LLBC_CFG_LOG_DEFAULT_MAX_FILE_SIZE;
        // Max backup index.
        _maxBackupIndex = __LLBC_GetLogCfg2(
            "maxBackupIndex", LLBC_CFG_LOG_MAX_BACKUP_INDEX, GetMaxBackupIndex, AsInt32);
        // Lazy create log file.
        _lazyCreateLogFile = __LLBC_GetLogCfg2(
            "lazyCreateLogFile", LLBC_CFG_LOG_LAZY_CREATE_LOG_FILE, IsLazyCreateLogFile, AsLooseBool);

        // File buffer size.
        if (_asyncMode)
            _fileBufferSize = __LLBC_GetLogCfg(
                "fileBufferSize", LOG_FILE_BUFFER_SIZE, GetFileBufferSize, AsInt32);
    }

    // Misc configs.
    if (!rootCfg)
        _takeOver = __LLBC_GetLogCfg2(
            "takeOver", LLBC_CFG_LOG_ROOT_LOGGER_TAKE_OVER_UNCONFIGED, IsTakeOver, AsLooseBool);

    // Check configs.
    _maxBackupIndex = MAX(0, _maxBackupIndex);
    _flushInterval = MIN(MAX(0, _flushInterval), LLBC_CFG_LOG_MAX_LOG_FLUSH_INTERVAL);

    // Normalize log file name.
    NormalizeLogFileName();

    // Rejudge root logger 'notConfigUse' option, if is root logger config.
    if (loggerName == LLBC_CFG_LOG_ROOT_LOGGER_NAME)
        _notConfigUseRoot = notCfgUseOpt.strip().tolower() == "root";

    return LLBC_OK;
}

int LLBC_LoggerConfigInfo::GetAppenderLogLevel(int appenderType) const
{
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    if (appenderType == LLBC_LogAppenderType::Console)
    {
        return GetConsoleLogLevel();
    }
    else if (appenderType == LLBC_LogAppenderType::File)
    {
        return GetFileLogLevel();
    }
    else
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_LogLevel::End;
    }
}

void LLBC_LoggerConfigInfo::NormalizeLogFileName()
{
    // Replace process id: %p.
    const LLBC_String curProcId = 
        LLBC_Num2Str(LLBC_GetCurrentProcessId());
    _logFile.findreplace("%p", curProcId); 

    // Replace exec name: %e/%m.
    // Note: %m pattern is deprecated.
    // TODO: Temporary support pattern additional params.
    while (true)
    {
        auto modFileNamePatternBeg = _logFile.find("%e");
        LLBC_DoIf(modFileNamePatternBeg == LLBC_String::npos, modFileNamePatternBeg = _logFile.find("%m"));
        LLBC_BreakIf(modFileNamePatternBeg == LLBC_String::npos);

        bool needPreserveExecNameLink = false;
        size_t modFileNamePatternEnd = LLBC_String::npos;

        modFileNamePatternEnd = modFileNamePatternBeg + 2;
        if (modFileNamePatternEnd != _logFile.size() && _logFile[modFileNamePatternEnd] == '{')
        {
            ++modFileNamePatternEnd;
            while (modFileNamePatternEnd < _logFile.size() &&
                   _logFile[modFileNamePatternEnd] != '}')
                ++modFileNamePatternEnd;

            if (modFileNamePatternEnd < _logFile.size())
            {
                if (_logFile.substr(
                        modFileNamePatternBeg + 3,
                        modFileNamePatternEnd - modFileNamePatternBeg - 3).strip().tolower() == "preservelink")
                    needPreserveExecNameLink = true;
                ++modFileNamePatternEnd;
            }
        }

        const LLBC_String modFileName =
            LLBC_Directory::SplitExt(LLBC_Directory::ModuleFileName(!needPreserveExecNameLink))[0];
        _logFile.erase(modFileNamePatternBeg, modFileNamePatternEnd - modFileNamePatternBeg);
        _logFile.insert(modFileNamePatternBeg, modFileName);
    };

    // Replace logger name: %l.
    _logFile.findreplace("%l", _loggerName);

    // Replace smart logger name: %L.
    // for root logger:
    //   => %L => root
    //   => xxx_%L => xxx
    //   => xxx__%L => xxx
    //   => xxx-%L => xxx
    //   => xxx--%L => xxx
    // for non-root logger: same as %l.
    if (_loggerName != LLBC_CFG_LOG_ROOT_LOGGER_NAME ||
        LLBC_Directory::BaseName(_logFile) == "%L")
    {
        _logFile.findreplace("%L", _loggerName);
    }
    else
    {
        LLBC_String::size_type smartLoggerNamePos = 0;
        while ((smartLoggerNamePos = _logFile.find("%L", smartLoggerNamePos)) != LLBC_String::npos)
        {
            while (smartLoggerNamePos > 0 &&
                (_logFile[smartLoggerNamePos - 1] == '_' ||
                    _logFile[smartLoggerNamePos - 1] == '-'))
                _logFile.erase(--smartLoggerNamePos);
            _logFile.erase(smartLoggerNamePos, 2);
        }
    }

#if LLBC_TARGET_PLATFORM_IPHONE
    if (_logToFile &&
        !_logFile.empty() &&
        _logFile[0] != '/')
        _logFile = LLBC_Directory::Join(LLBC_Directory::TempDir(), _logFile);
#endif // LLBC_TARGET_PLATFORM_IPHONE
}

sint64 LLBC_LoggerConfigInfo::NormalizeLogFileSize(const LLBC_String &logFileSize)
{
    // strip.
    const LLBC_String nmlLogFileSizeStr = logFileSize.strip();

    // If is empty, use default.
    if (nmlLogFileSizeStr.empty())
        return LLBC_CFG_LOG_DEFAULT_MAX_FILE_SIZE;

    // Find storage unit.
    LLBC_String::size_type unitPos = 0;
    for (; unitPos < nmlLogFileSizeStr.size(); ++unitPos)
    {
        if (!isdigit(nmlLogFileSizeStr[unitPos]) && nmlLogFileSizeStr[unitPos] != '.')
            break;
    }

    // normalize storage unit.
    const LLBC_String unit = nmlLogFileSizeStr.substr(unitPos).strip().tolower();
    double nmlLogFileSize = LLBC_Variant(nmlLogFileSizeStr.substr(0, unitPos));
    // - k/kb, kib
    if (unit == "k" || unit == "kb")
        nmlLogFileSize *= 1000.0;
    else if (unit == "kib")
        nmlLogFileSize *= 1024.0;
    // - m/mb, mib
    else if (unit == "m" || unit == "mb")
        nmlLogFileSize *= (1000.0 * 1000.0);
    else if (unit == "mib")
        nmlLogFileSize *= (1024.0 * 1024.0);
    // - g/gb, gib
    else if (unit == "g" || unit == "gb")
        nmlLogFileSize *= (1000.0 * 1000.0 * 1000.0);
    else if (unit == "gib")
        nmlLogFileSize *= (1024.0 * 1024.0 * 1024.0);
    // - unknown/unsupported storage unit, ignore.
    // else
    //     // ... ...

    // Clamp.
    return MIN(MAX(1024ll, static_cast<sint64>(nmlLogFileSize)),
               LLBC_CFG_LOG_MAX_FILE_SIZE_LIMIT);
}

__LLBC_NS_END

/**
 * Internal macros undef.
 */
#undef __LLBC_GetLogCfg
#undef __LLBC_GetLogCfg2
