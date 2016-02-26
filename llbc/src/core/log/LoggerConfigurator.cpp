/**
 * @file    LoggerConfigurator.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/12
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/helper/STLHelper.h"

#include "llbc/core/config/Property.h"

#include "llbc/core/log/Logger.h"
#include "llbc/core/log/LoggerConfigInfo.h"
#include "llbc/core/log/LoggerConfigurator.h"

__LLBC_NS_BEGIN

LLBC_LoggerConfigurator::LLBC_LoggerConfigurator()
{
}

LLBC_LoggerConfigurator::~LLBC_LoggerConfigurator()
{
    LLBC_STLHelper::DeleteContainer(_configs, true);
}

int LLBC_LoggerConfigurator::Initialize(const LLBC_String &cfgFile)
{
	LLBC_Property cfg;
	if (cfg.LoadFromFile(cfgFile) != LLBC_OK)
		return LLBC_FAILED;

	// Get all logger names
	const LLBC_Strings loggerNames = cfg.GetPropertyNames(false);
	for (LLBC_Strings::const_iterator it = loggerNames.begin();
		 it != loggerNames.end();
		 it++)
	{
		const LLBC_String &loggerName = *it;
        LLBC_LoggerConfigInfo *info = new LLBC_LoggerConfigInfo;
        if (info->Initialize(*cfg.GetProperty(loggerName)) != LLBC_OK)
        {
            delete info;
            LLBC_STLHelper::DeleteContainer(_configs, true);
            return LLBC_FAILED;
        }

        _configs.insert(std::make_pair(loggerName, info));
    }

    return LLBC_OK;
}

int LLBC_LoggerConfigurator::Config(const LLBC_String &name, LLBC_Logger *logger) const
{
    if (name.empty() || !logger)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if (logger->IsInit())
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    std::map<LLBC_String, LLBC_LoggerConfigInfo *>::const_iterator iter = _configs.find(name);
    if (iter == _configs.end())
    {
        LLBC_Property cfg;
        LLBC_LoggerConfigInfo *info = new LLBC_LoggerConfigInfo;
        if (info->Initialize(cfg) != LLBC_OK)
        {
            delete info;
            return LLBC_FAILED;
        }

        LLBC_LoggerConfigurator *nonConstThis = const_cast<LLBC_LoggerConfigurator *>(this);
        iter = nonConstThis->_configs.insert(std::make_pair(name, info)).first;
    }

    return logger->Initialize(name, iter->second);
}

const std::map<LLBC_String, LLBC_LoggerConfigInfo *> &LLBC_LoggerConfigurator::GetAllConfigInfos() const
{
    return _configs;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
