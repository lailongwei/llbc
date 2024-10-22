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

#include "llbc/core/file/Directory.h"
#include "llbc/core/helper/STLHelper.h"
#include "llbc/core/config/Property.h"
#include "llbc/core/tinyxml2/tinyxml2.h"
#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/utils/Util_Variant.h"

#include "llbc/core/log/Logger.h"
#include "llbc/core/log/LoggerConfigInfo.h"
#include "llbc/core/log/LoggerConfigurator.h"

__LLBC_NS_BEGIN

LLBC_LoggerConfigurator::LLBC_LoggerConfigurator()
: _rootConfig(nullptr)
{
}

LLBC_LoggerConfigurator::~LLBC_LoggerConfigurator()
{
    LLBC_STLHelper::DeleteContainer(_configs);
}

int LLBC_LoggerConfigurator::Initialize(const LLBC_String &cfgFile)
{
    // Load config content.
    LLBC_Variant cfg;
    const LLBC_String &ext = LLBC_Directory::SplitExt(cfgFile)[1].tolower();
    if (ext == ".cfg")
    {
        LLBC_Property propCfg;
        if (propCfg.LoadFromFile(cfgFile) != LLBC_OK)
            return LLBC_FAILED;

        LLBC_VariantUtil::Property2Variant(propCfg, cfg);
    }
    else if (ext == ".xml")
    {
        // Load xml file.
        ::llbc::tinyxml2::XMLDocument xmlDoc;
        const auto xmlLoadRet = xmlDoc.LoadFile(cfgFile.c_str());
        if (xmlLoadRet != ::llbc::tinyxml2::XML_SUCCESS)
        {
            LLBC_String customErrStr;
            customErrStr.format("load log config file failed(xml format), "
                                "file:%s, errno(tinyxml2:%d), error str:%s",
                                cfgFile.c_str(), xmlLoadRet, xmlDoc.ErrorStr());
            LLBC_SetLastError(LLBC_ERROR_FORMAT, customErrStr.c_str());
            return LLBC_FAILED;
        }

        // Convert to variant object(brief format).
        LLBC_Variant detailCfg;
        LLBC_VariantUtil::Xml2Variant(xmlDoc, detailCfg);
        for (auto &loggerXmlCfg : detailCfg["Log"][LLBC_XMLKeys::Children].AsSeq())
        {
            LLBC_Variant &loggerCfg = cfg[loggerXmlCfg[LLBC_XMLKeys::Name].AsStr()];
            for (auto &logCfgItem : loggerXmlCfg[LLBC_XMLKeys::Children].AsSeq())
                loggerCfg[logCfgItem[LLBC_XMLKeys::Name]] = logCfgItem[LLBC_XMLKeys::Value];
        }
    }
    else
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_SUPPORT);
        return LLBC_FAILED;
    }

    // Create root logger config info.
    const LLBC_Variant &rootCfg = cfg[LLBC_CFG_LOG_ROOT_LOGGER_NAME];
    if (UNLIKELY(!rootCfg.IsDict()))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    // Create root logger config info.
    _rootConfig = new LLBC_LoggerConfigInfo;
    if (_rootConfig->Initialize(LLBC_CFG_LOG_ROOT_LOGGER_NAME, rootCfg, nullptr) != LLBC_OK)
    {
        LLBC_XDelete(_rootConfig);
        return LLBC_FAILED;
    }

    _configs.insert(std::make_pair(LLBC_CFG_LOG_ROOT_LOGGER_NAME, _rootConfig));

    // Create all non-root logger config infos.
    for (auto &cfgItem : cfg.AsDict())
    {
        const LLBC_String loggerName = cfgItem.first;
        if (loggerName == LLBC_CFG_LOG_ROOT_LOGGER_NAME)
            continue;

        LLBC_LoggerConfigInfo *info = new LLBC_LoggerConfigInfo;
        if (info->Initialize(loggerName, cfgItem.second, _rootConfig) != LLBC_OK)
        {
            delete info;
            LLBC_STLHelper::DeleteContainer(_configs);
            return LLBC_FAILED;
        }

        _configs.insert(std::make_pair(loggerName, info));
    }

    return LLBC_OK;
}

bool LLBC_LoggerConfigurator::HasSharedAsyncLoggerConfigs() const
{
    std::map<LLBC_String, LLBC_LoggerConfigInfo *>::const_iterator iter = _configs.begin();
    for (; iter != _configs.end(); ++iter)
    {
        if (iter->second->IsAsyncMode() && 
            !iter->second->IsIndependentThread())
            return true;
    }

    return false;
}

int LLBC_LoggerConfigurator::Config(const LLBC_String &name, LLBC_LogRunnable *sharedLogRunnable, LLBC_Logger *logger) const
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
        LLBC_LoggerConfigInfo *info = new LLBC_LoggerConfigInfo;
        if (info->Initialize(name, LLBC_Variant::nil, _rootConfig) != LLBC_OK)
        {
            delete info;
            return LLBC_FAILED;
        }

        LLBC_LoggerConfigurator *nonConstThis = const_cast<LLBC_LoggerConfigurator *>(this);
        iter = nonConstThis->_configs.insert(std::make_pair(name, info)).first;
    }

    return logger->Initialize(iter->second, sharedLogRunnable);
}

const std::map<LLBC_String, LLBC_LoggerConfigInfo *> &LLBC_LoggerConfigurator::GetAllConfigInfos() const
{
    return _configs;
}

__LLBC_NS_END
