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

#include "llbc/core/os/OS_Console.h"

#include "llbc/core/helper/STLHelper.h"

#include "llbc/core/thread/Guard.h"

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/Logger.h"
#include "llbc/core/log/LoggerConfigurator.h"
#include "llbc/core/log/LoggerManager.h"
#include "llbc/core/log/LogRunnable.h"

__LLBC_NS_BEGIN

LLBC_String LLBC_LoggerManager::_rootLoggerName = LLBC_CFG_LOG_ROOT_LOGGER_NAME;

LLBC_LoggerManager::LLBC_LoggerManager()
: _configurator(nullptr)
, _sharedLogRunnable(nullptr)

, _rootLogger(nullptr)
{
}

LLBC_LoggerManager::~LLBC_LoggerManager()
{
    Finalize();
}

int LLBC_LoggerManager::Initialize(const LLBC_String &cfgFile)
{
    LLBC_LockGuard guard(_lock);

    if (_rootLogger)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    _configurator = LLBC_New(LLBC_LoggerConfigurator);
    if (_configurator->Initialize(cfgFile) != LLBC_OK)
    {
        LLBC_XDelete(_configurator);
        return LLBC_FAILED;
    }

    // Create shared log runnable.
    if (_configurator->HasSharedAsyncLoggerConfigs())
        _sharedLogRunnable = LLBC_New(LLBC_LogRunnable);

    // Config root logger.
    _rootLogger = LLBC_New(LLBC_Logger);
    if (_configurator->Config(_rootLoggerName, _sharedLogRunnable, _rootLogger) != LLBC_OK)
    {
        LLBC_XDelete(_rootLogger);
        LLBC_XDelete(_configurator);

        return LLBC_FAILED;
    }

    _loggers.insert(std::make_pair(_rootLoggerName, _rootLogger));
    _loggers2.insert(std::make_pair(_rootLoggerName.c_str(), _rootLogger));

    // Config other loggers.
    const std::map<LLBC_String, LLBC_LoggerConfigInfo *> &configs = _configurator->GetAllConfigInfos();
    std::map<LLBC_String, LLBC_LoggerConfigInfo *>::const_iterator cfgIter = configs.begin();
    for (; cfgIter != configs.end(); ++cfgIter)
    {
        if (cfgIter->first == _rootLoggerName)
            continue;

        LLBC_Logger *logger = LLBC_New(LLBC_Logger);
        if (_configurator->Config(cfgIter->first, _sharedLogRunnable, logger) != LLBC_OK)
        {
            LLBC_Delete(logger);
            Finalize();
            return LLBC_FAILED;
        }

        _loggers.insert(std::make_pair(cfgIter->first, logger));
        _loggers2.insert(std::make_pair(cfgIter->first.c_str(), logger));
    }

    // Startup shared log runnable.
    if (_sharedLogRunnable)
        _sharedLogRunnable->Activate(1, LLBC_ThreadFlag::Joinable, LLBC_ThreadPriority::BelowNormal);

    return LLBC_OK;
}

bool LLBC_LoggerManager::IsInited() const
{
    return _rootLogger != nullptr;
}

void LLBC_LoggerManager::Finalize()
{
    LLBC_LockGuard guard(_lock);

    if (_rootLogger == nullptr)
        return;

    if (_sharedLogRunnable)
    {
        _sharedLogRunnable->Stop();
        LLBC_Delete(_sharedLogRunnable);
        _sharedLogRunnable = nullptr;
    }

    // Delete all loggers and set _rootLogger logger to nullptr.
    _loggers2.clear();
    LLBC_STLHelper::DeleteContainer(_loggers);
    _rootLogger = nullptr;

    // Delete logger configurator.
    LLBC_XDelete(_configurator);
}

LLBC_Logger *LLBC_LoggerManager::GetRootLogger() const
{
    if (UNLIKELY(!_rootLogger))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return nullptr;
    }

    return _rootLogger;
}

LLBC_Logger *LLBC_LoggerManager::GetLogger(const char *name) const
{
    if (UNLIKELY(!name))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return nullptr;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(!_rootLogger))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return nullptr;
    }

    auto iter = _loggers2.find(name);
    if (iter == _loggers2.end())
    {
        if (_rootLogger->IsTakeOver())
            return _rootLogger;

        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    return iter->second;
}

LLBC_Logger *LLBC_LoggerManager::GetLogger(const LLBC_String &name) const
{
    if (UNLIKELY(name.empty()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return nullptr;
    }

    LLBC_LockGuard guard(_lock);
    if (UNLIKELY(!_rootLogger))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return nullptr;
    }

    auto iter = _loggers.find(name);
    if (iter == _loggers.end())
    {
        if (_rootLogger->IsTakeOver())
            return _rootLogger;

        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    return iter->second;
}

void LLBC_LoggerManager::UnInitOutput(int logLv, const char *msg)
{
    FILE *to = logLv >= LLBC_LogLevel::Warn ? stderr : stdout;
    const LLBC_String &lvDesc = LLBC_LogLevel::GetLevelDesc(logLv);
    LLBC_FilePrint(to, "[Log][%s] %s\n", lvDesc.c_str(), msg);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
