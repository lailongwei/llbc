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

#include "llbc/core/time/Time.h"

#include "llbc/core/os/OS_Console.h"

#include "llbc/core/helper/STLHelper.h"

#include "llbc/core/thread/Guard.h"

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/Logger.h"
#include "llbc/core/log/LoggerConfigurator.h"
#include "llbc/core/log/LoggerMgr.h"
#include "llbc/core/log/LogRunnable.h"

__LLBC_NS_BEGIN

LLBC_FastLock LLBC_LoggerMgr::_uninitColorfulOutputLock;

LLBC_LoggerMgr::LLBC_LoggerMgr()
: _sharedLogRunnable(nullptr)

, _rootLogger(nullptr)
{
}

LLBC_LoggerMgr::~LLBC_LoggerMgr()
{
    Finalize();
}

int LLBC_LoggerMgr::Initialize(const LLBC_String &cfgFilePath)
{
    LLBC_LockGuard guard(_lock);

    // Reinit check.
    if (_rootLogger)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    // Initialize logger configurator.
    LLBC_LoggerConfigurator *logConfigurator = new LLBC_LoggerConfigurator;
    LLBC_Defer(delete logConfigurator);
    if (logConfigurator->Initialize(cfgFilePath) != LLBC_OK)
        return LLBC_FAILED;

    // Create shared log runnable.
    if (logConfigurator->HasSharedAsyncLoggerConfigs())
        _sharedLogRunnable = new LLBC_LogRunnable;

    // Config root logger.
    _rootLogger = new LLBC_Logger;
    const LLBC_CString rootLoggerName(LLBC_CFG_LOG_ROOT_LOGGER_NAME);
    if (logConfigurator->Config(rootLoggerName, _sharedLogRunnable, _rootLogger) != LLBC_OK)
    {
        LLBC_XDelete(_rootLogger);
        LLBC_XDelete(_sharedLogRunnable);

        return LLBC_FAILED;
    }

    _loggerList.emplace_back(rootLoggerName, _rootLogger);
    _cstr2Loggers.insert(std::make_pair(rootLoggerName, _rootLogger));

    // Save config file path.
    _cfgFilePath.assign(cfgFilePath.c_str(), cfgFilePath.size());

    // Config other loggers.
    const std::map<LLBC_String, LLBC_LoggerConfigInfo *> &configs = logConfigurator->GetAllConfigInfos();
    for (auto cfgIter = configs.begin(); cfgIter != configs.end(); ++cfgIter)
    {
        if (cfgIter->first == rootLoggerName)
            continue;

        LLBC_Logger *logger = new LLBC_Logger;
        if (logConfigurator->Config(cfgIter->first, _sharedLogRunnable, logger) != LLBC_OK)
        {
            delete logger;
            Finalize();

            return LLBC_FAILED;
        }

        const LLBC_String &cfgLoggerName = cfgIter->first;
        const size_t loggerNameLen = cfgLoggerName.size();
        LLBC_CString loggerName(LLBC_Malloc(char, loggerNameLen + 1), loggerNameLen);
        memcpy(const_cast<char *>(loggerName.c_str()), cfgLoggerName.c_str(), cfgLoggerName.size());
        const_cast<char *>(loggerName.c_str())[loggerNameLen] = '\0';

        _loggerList.emplace_back(loggerName, logger);
        _cstr2Loggers.insert(std::make_pair(loggerName, logger));
    }

    _cstr2LoggersEnd = _cstr2Loggers.end();

    // Startup shared log runnable.
    if (_sharedLogRunnable)
        _sharedLogRunnable->Activate(1, LLBC_ThreadPriority::BelowNormal);

    return LLBC_OK;
}

int LLBC_LoggerMgr::Reload(const LLBC_String &newCfgFilePath)
{
    LLBC_LockGuard guard(_lock);

    // Not init check.
    if (!_rootLogger)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    // Load config file(use temporary LoggerConfigurator).
    auto logConfigurator = new LLBC_LoggerConfigurator;
    LLBC_Defer(delete logConfigurator);
    if (logConfigurator->Initialize(
            !newCfgFilePath.empty() ? newCfgFilePath : _cfgFilePath) != LLBC_OK)
        return LLBC_FAILED;

    // Re-Config root logger.
    if (logConfigurator->ReConfig(_rootLogger) != LLBC_OK)
        return LLBC_FAILED;
    // Re-Config other loggers.
    for (auto &loggerItem : _cstr2Loggers)
    {
        if (logConfigurator->ReConfig(loggerItem.second) != LLBC_OK)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}

void LLBC_LoggerMgr::Finalize()
{
    LLBC_LockGuard guard(_lock);
    if (_rootLogger == nullptr)
        return;

    // Delete shared log runnable.
    if (_sharedLogRunnable)
    {
        _sharedLogRunnable->Stop();
        delete _sharedLogRunnable;
        _sharedLogRunnable = nullptr;
    }

    // Delete all loggers and set _rootLogger logger to nullptr.
    _cstr2Loggers.clear();
    _cstr2LoggersEnd = _cstr2Loggers.end();
    for (auto rit = _loggerList.rbegin(); rit != _loggerList.rend(); ++rit)
    {
        if (rit->first != LLBC_CFG_LOG_ROOT_LOGGER_NAME)
            free(const_cast<char *>(rit->first.c_str()));
        delete rit->second;
    }
    _loggerList.clear();

    _rootLogger = nullptr;

    // Clear _cfgFilePath.
    _cfgFilePath.clear();
}

LLBC_Logger *LLBC_LoggerMgr::GetRootLogger() const
{
    if (UNLIKELY(_rootLogger == nullptr))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return nullptr;
    }

    return _rootLogger;
}

LLBC_Logger *LLBC_LoggerMgr::GetLogger(const LLBC_CString &name) const
{
    _lock.Lock();
    if (UNLIKELY(_rootLogger == nullptr))
    {
        _lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);

        return nullptr;
    }

    if (name.empty())
    {
        _lock.Unlock();
        return _rootLogger;
    }

    // If logger cnt <= 25, use liner find, (note: 25 is a experience value).
    if (_loggerList.size() <= 25)
    {
        const size_t loggerCnt = _loggerList.size();
        auto loggerList = &_loggerList[0];
        for (size_t i = 0; i < loggerCnt; ++i)
        {
            if (loggerList[i].first == name)
            {
                _lock.Unlock();
                return loggerList[i].second;
            }
        }
    }
    else // Otherwise search in map.
    {
        auto iter = _cstr2Loggers.find(name);
        if (LIKELY(iter != _cstr2LoggersEnd))
        {
            _lock.Unlock();
            return iter->second;
        }
    }

    if (_rootLogger->IsTakeOver())
    {
        _lock.Unlock();
        return _rootLogger;
    }

    _lock.Unlock();
    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);

    return nullptr;
}

void LLBC_LoggerMgr::UnInitOutput(int logLv,
                                  const char *tag,
                                  const char *file,
                                  int line,
                                  const char *func,
                                  const char *fmt,
                                  ...)
{
    FILE *to = logLv >= LLBC_LogLevel::Warn ? stderr : stdout;
    const LLBC_CString &lvDesc = LLBC_LogLevel::GetLevelStr(logLv);

    // Parse to get file basename.
    if (file)
    {
        #if LLBC_TARGET_PLATFORM_WIN32
        const char *fileBaseName = strrchr(file, '\\');
        LLBC_DoIf(!fileBaseName, fileBaseName = strrchr(file, '/'));
        LLBC_DoIf(fileBaseName, file = fileBaseName + 1);
        #else
        const char *fileBaseName = strrchr(file, '/');
        LLBC_DoIf(!fileBaseName, fileBaseName = strrchr(file, '\\'));
        LLBC_DoIf(fileBaseName, file = fileBaseName + 1);
        #endif
    }

    // Get now time.
    const auto now = LLBC_Time::Now();

    // Set console color.
    int oldConsoleColor;
    if (logLv >= LLBC_LogLevel::Warn)
    {
        _uninitColorfulOutputLock.Lock();
        if (logLv == LLBC_LogLevel::Warn)
            LLBC_SetConsoleColor(to,
                                 LLBC_ConsoleColor::Bg_Default |
                                 LLBC_ConsoleColor::Fg_Yellow |
                                 LLBC_ConsoleColor::Highlight_Fg);
        else
            LLBC_SetConsoleColor(to,
                                 LLBC_ConsoleColor::Bg_Default |
                                 LLBC_ConsoleColor::Fg_Red |
                                 LLBC_ConsoleColor::Highlight_Fg);
        oldConsoleColor = LLBC_GetConsoleColor(to);
    }

    // Log
    LLBC_FilePrint(to,
                   "[Log]%s [%-5s][%s:%d %s]<%s> - ",
                   now.ToString().c_str(),
                   lvDesc.c_str(),
                   file ? file : "",
                   line,
                   func ? func : "",
                   tag ? tag : "");

    // Reset console color, if required.
    if (logLv >= LLBC_LogLevel::Warn)
    {
        LLBC_SetConsoleColor(to, oldConsoleColor);
        _uninitColorfulOutputLock.Unlock();
    }

    va_list ap;
    va_start(ap, fmt);

    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (LIKELY(libTls))
    {
        const int len = vsnprintf(libTls->coreTls.loggerFmtBuf,
                                  sizeof(libTls->coreTls.loggerFmtBuf),
                                  fmt,
                                  ap);
        LLBC_FilePrint(to, "%s\n", len < 0 ? fmt : libTls->coreTls.loggerFmtBuf);
    }
    else
    {
        char fmtBuf[512];
        const int len = vsnprintf(fmtBuf,
                                  sizeof(fmtBuf),
                                  fmt,
                                  ap);
        LLBC_FilePrint(to, "%s\n", len < 0 ? fmt : fmtBuf);
    }

    va_end(ap);

    fflush(to);
}

void LLBC_LoggerMgr::UnInitNonFormatOutput(int logLv,
                                           const char* tag,
                                           const char* file,
                                           int line,
                                           const char* func,
                                           const char* msg,
                                           size_t msgLen)
{
    FILE *to = logLv >= LLBC_LogLevel::Warn ? stderr : stdout;
    const LLBC_CString &lvDesc = LLBC_LogLevel::GetLevelStr(logLv);

    if (file)
    {
#if LLBC_TARGET_PLATFORM_WIN32
    const char *fileBaseName = strrchr(file, '\\');
    LLBC_DoIf(!fileBaseName, fileBaseName = strrchr(file, '/'));
    LLBC_DoIf(fileBaseName, file = fileBaseName + 1);
#else
    const char *fileBaseName = strrchr(file, '/');
    LLBC_DoIf(!fileBaseName, fileBaseName = strrchr(file, '\\'));
    LLBC_DoIf(fileBaseName, file = fileBaseName + 1);
#endif
    }

    const auto now = LLBC_Time::Now();
    LLBC_FilePrint(to,
                   "[Log]%s [%-5s][%s:%d %s]<%s> - ",
                   now.ToString().c_str(),
                   lvDesc.c_str(),
                   file ? file : "",
                   line,
                   func ? func : "",
                   tag ? tag : "");

    fwrite(msg, 1, msgLen, to);
    fwrite("\n", 1, 1, to);

    fflush(to);
}

__LLBC_NS_END
