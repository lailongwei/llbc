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

#include "llbc/core/helper/STLHelper.h"

#include "llbc/core/thread/Guard.h"

#include "llbc/core/log/Logger.h"
#include "llbc/core/log/LoggerConfigurator.h"
#include "llbc/core/log/LoggerManager.h"
#include "llbc/core/log/Log.h"
#include "llbc/core/log/LogRunnable.h"

__LLBC_NS_BEGIN

LLBC_String LLBC_LoggerManager::_rootLoggerName = LLBC_CFG_LOG_ROOT_LOGGER_NAME;

LLBC_LoggerManager::LLBC_LoggerManager()
: _configurator(nullptr)
, _sharedLogRunnable(nullptr)

, _root(nullptr)
{
}

LLBC_LoggerManager::~LLBC_LoggerManager()
{
    Finalize();
}

int LLBC_LoggerManager::Initialize(const LLBC_String &cfgFile)
{
    LLBC_LockGuard guard(_lock);

    if (_root)
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
    _root = LLBC_New(LLBC_Logger);
    if (_configurator->Config(_rootLoggerName, _sharedLogRunnable, _root) != LLBC_OK)
    {
        LLBC_XDelete(_root);
        LLBC_XDelete(_configurator);

        return LLBC_FAILED;
    }

    _loggers.insert(std::make_pair(_rootLoggerName, _root));

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
    }

    // Init Log helper class.
    LLBC_LogHelper::Initialize(this);

    // Startup shared log runnable.
    if (_sharedLogRunnable)
        _sharedLogRunnable->Activate(1, LLBC_ThreadFlag::Joinable, LLBC_ThreadPriority::BelowNormal);

    return LLBC_OK;
}

bool LLBC_LoggerManager::IsInited() const
{
    LLBC_LoggerManager *nonConstThis = const_cast<LLBC_LoggerManager *>(this);
    LLBC_LockGuard guard(nonConstThis->_lock);
    return _root != nullptr;
}

void LLBC_LoggerManager::Finalize()
{
    LLBC_LockGuard guard(_lock);

    if (_root == nullptr)
        return;

    if (_sharedLogRunnable)
    {
        _sharedLogRunnable->Stop();
        LLBC_Delete(_sharedLogRunnable);
        _sharedLogRunnable = nullptr;
    }

    // Finalize Log helper class.
    LLBC_LogHelper::Finalize();

    // Delete all loggers and set _root logger to nullptr.
    LLBC_STLHelper::DeleteContainer(_loggers);
    _root = nullptr;

    // Delete logger configurator.
    LLBC_XDelete(_configurator);
}

LLBC_Logger *LLBC_LoggerManager::GetRootLogger() const
{
    LLBC_LoggerManager *nonConstThis = const_cast<LLBC_LoggerManager *>(this);
    LLBC_LockGuard guard(nonConstThis->_lock);
    if (UNLIKELY(!_root))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return nullptr;
    }

    return _root;
}

LLBC_Logger *LLBC_LoggerManager::GetLogger(const LLBC_String &name) const
{
    if (UNLIKELY(name.empty()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return nullptr;
    }

    LLBC_LoggerManager *nonConstThis = const_cast<LLBC_LoggerManager *>(this);
    LLBC_LockGuard guard(nonConstThis->_lock);
    if (UNLIKELY(!_root))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return nullptr;
    }

    std::map<LLBC_String, LLBC_Logger *>::const_iterator iter = _loggers.find(name);
    if (iter == _loggers.end())
    {
        if (_root->IsTakeOver())
            return _root;

        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    return iter->second;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
