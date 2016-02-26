/**
 * @file    LoggerManager.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/12
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/helper/STLHelper.h"

#include "llbc/core/thread/Guard.h"

#include "llbc/core/log/Logger.h"
#include "llbc/core/log/LoggerConfigurator.h"
#include "llbc/core/log/LoggerManager.h"

__LLBC_NS_BEGIN

LLBC_LoggerManager::LLBC_LoggerManager()
: _lock()
, _root(NULL)
, _loggers()
, _configurator(NULL)
{
}

LLBC_LoggerManager::~LLBC_LoggerManager()
{
    this->Finalize();
}

int LLBC_LoggerManager::Initialize(const LLBC_String &cfgFile)
{
    LLBC_Guard guard(_lock);

    if (_root)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    _configurator = new LLBC_LoggerConfigurator;
    if (_configurator->Initialize(cfgFile) != LLBC_OK)
    {
        LLBC_XDelete(_configurator);
        return LLBC_FAILED;
    }

    // First, config root logger.
    _root = new LLBC_Logger;
    if (_configurator->Config("root", _root) != LLBC_OK)
    {
        LLBC_XDelete(_root);
        LLBC_XDelete(_configurator);

        return LLBC_FAILED;
    }

    // Config other loggers.
    const std::map<LLBC_String, LLBC_LoggerConfigInfo *> &configs = _configurator->GetAllConfigInfos();
    std::map<LLBC_String, LLBC_LoggerConfigInfo *>::const_iterator iter = configs.begin();
    for (; iter != configs.end(); iter++)
    {
        if (iter->first == "root")
        {
            continue;
        }

        LLBC_Logger *logger = new LLBC_Logger;
        if (_configurator->Config(iter->first, logger) != LLBC_OK)
        {
            delete logger;
            this->Finalize();
            return LLBC_FAILED;
        }

        _loggers.insert(std::make_pair(iter->first, logger));
    }

    return LLBC_OK;
}

void LLBC_LoggerManager::Finalize()
{
    LLBC_Guard guard(_lock);

    LLBC_XDelete(_root);
    LLBC_STLHelper::DeleteContainer(_loggers);

    LLBC_XDelete(_configurator);
}

LLBC_Logger *LLBC_LoggerManager::GetRootLogger() const
{
    LLBC_LoggerManager *nonConstThis = const_cast<LLBC_LoggerManager *>(this);
    LLBC_Guard guard(nonConstThis->_lock);
    if (UNLIKELY(!_root))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return NULL;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return _root;
}

LLBC_Logger *LLBC_LoggerManager::GetLogger(const LLBC_String &name) const
{
    if (UNLIKELY(name.empty()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return NULL;
    }

    LLBC_LoggerManager *nonConstThis = const_cast<LLBC_LoggerManager *>(this);
    LLBC_Guard guard(nonConstThis->_lock);
    if (UNLIKELY(!_root))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return NULL;
    }

    std::map<LLBC_String, LLBC_Logger *>::const_iterator iter = _loggers.find(name);
    if (iter == _loggers.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return iter->second;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
