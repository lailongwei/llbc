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

#include "llbc/core/os/OS_Console.h"
#include "llbc/core/file/Directory.h"
#include "llbc/core/helper/STLHelper.h"
#include "llbc/core/config/Properties.h"
#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/log/Logger.h"
#include "llbc/core/log/LogConfigLoader.h"
#include "llbc/core/log/LoggerConfigInfo.h"
#include "llbc/core/log/BaseLogAppender.h"
#include "llbc/core/log/LoggerConfigurator.h"

__LLBC_NS_BEGIN

LLBC_LoggerConfigurator::LLBC_LoggerConfigurator()
: _rootConfig(nullptr)
, _logTimeAccessor(nullptr)
{
}

LLBC_LoggerConfigurator::~LLBC_LoggerConfigurator()
{
    LLBC_STLHelper::DeleteContainer(_configs);
}

int LLBC_LoggerConfigurator::Initialize(const LLBC_String &cfgFilePath,
                                        const LLBC_LogTimeAccessor &logTimeAccessor)
{
    // Load config content.
    LLBC_Variant cfg;
    const LLBC_String &ext = LLBC_Directory::SplitExt(cfgFilePath)[1].tolower();
    if (ext == ".cfg")
    {
        if (LLBC_Properties::LoadFromFile(cfgFilePath, cfg) != LLBC_OK)
            return LLBC_FAILED;
    }
    else if (ext == ".xml")
    {
        // XML -> CIR (plain map/seq/scalar). LogConfigLoader returns the
        // full document CIR keyed by top-level tag(s); log configs always
        // wrap loggers under <Log>, so strip that wrapper to align with the
        // Properties-loaded shape: Dict<loggerName, loggerCfg>.
        LLBC_Variant docCir;
        if (LLBC_LogConfigLoader::LoadXmlFromFile(cfgFilePath, docCir) != LLBC_OK)
            return LLBC_FAILED;
        cfg = std::move(docCir["Log"]);
    }
    else
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_SUPPORT);
        return LLBC_FAILED;
    }

    if (ext == ".cfg")
    {
        for (auto &loggerCfgItem : cfg.As<LLBC_Variant::Dict>())
        {
            const auto &loggerCfg = loggerCfgItem.second;
            if (!loggerCfg.Is<LLBC_Variant::Dict>() || !loggerCfg["logControls"])
                continue;

            // `logControls` is XML-only (its structure is XML-tree shaped).    
            LLBC_String customErrStr;
            customErrStr.format(
                "cfg key `%s.logControls` is not supported -- log control "
                "items must be configured via XML (<logControls> nested "
                "element block); see doc/log_control.md.",
                loggerCfgItem.first.As<LLBC_Variant::Str>().c_str());
            LLBC_SetLastError(LLBC_ERROR_NOT_SUPPORT, customErrStr.c_str());
            return LLBC_FAILED;
        }
    }

    // Save config file path.
    _cfgFilePath = cfgFilePath;
    // Save log time accessor.
    _logTimeAccessor = &logTimeAccessor;

    // Create root logger config info.
    const LLBC_Variant &rootCfg = cfg[LLBC_CFG_LOG_ROOT_LOGGER_NAME];
    if (UNLIKELY(!rootCfg.Is<LLBC_Variant::Dict>()))
    {
        _cfgFilePath.clear();
        _logTimeAccessor = nullptr;

        LLBC_SetLastError(LLBC_ERROR_INVALID);

        return LLBC_FAILED;
    }

    // Create root logger config info.
    _rootConfig = new LLBC_LoggerConfigInfo;
    if (_rootConfig->Initialize(LLBC_CFG_LOG_ROOT_LOGGER_NAME, rootCfg, nullptr, logTimeAccessor) != LLBC_OK)
    {
        LLBC_XDelete(_rootConfig);

        _cfgFilePath.clear();
        _logTimeAccessor = nullptr;

        return LLBC_FAILED;
    }

    _configs.insert(std::make_pair(LLBC_CFG_LOG_ROOT_LOGGER_NAME, _rootConfig));

    // Create all non-root logger config infos.
    for (auto &cfgItem : cfg.As<LLBC_Variant::Dict>())
    {
        const LLBC_String loggerName = cfgItem.first;
        if (loggerName == LLBC_CFG_LOG_ROOT_LOGGER_NAME)
            continue;

        LLBC_LoggerConfigInfo *info = new LLBC_LoggerConfigInfo;
        if (info->Initialize(loggerName, cfgItem.second, _rootConfig, logTimeAccessor) != LLBC_OK)
        {
            delete info;
            LLBC_STLHelper::DeleteContainer(_configs);

            _cfgFilePath.clear();
            _logTimeAccessor = nullptr;

            return LLBC_FAILED;
        }

        _configs.insert(std::make_pair(loggerName, info));
    }

    return LLBC_OK;
}

const LLBC_String &LLBC_LoggerConfigurator::GetConfigFilePath() const
{
    return _cfgFilePath;
}

bool LLBC_LoggerConfigurator::HasSharedAsyncLoggerConfigs() const
{
    for (auto &configItem : _configs)
    {
        if (configItem.second->IsAsyncMode() && 
            !configItem.second->IsIndependentThread())
            return true;
    }

    return false;
}

int LLBC_LoggerConfigurator::Config(const LLBC_String &name,
                                    LLBC_LogRunnable *sharedLogRunnable,
                                    LLBC_Logger *logger) const
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

    auto it = _configs.find(name);
    if (it == _configs.end())
    {
        LLBC_LoggerConfigInfo *info = new LLBC_LoggerConfigInfo;
        if (info->Initialize(name, LLBC_Variant::nil, _rootConfig, *_logTimeAccessor) != LLBC_OK)
        {
            delete info;
            return LLBC_FAILED;
        }

        LLBC_LoggerConfigurator *nonConstThis = const_cast<LLBC_LoggerConfigurator *>(this);
        it = nonConstThis->_configs.insert(std::make_pair(name, info)).first;
    }

    return logger->Initialize(it->second, sharedLogRunnable);
}

int LLBC_LoggerConfigurator::ReConfig(LLBC_Logger *logger) const
{
    // Find logger config.
    const auto it = _configs.find(logger->GetLoggerName());
    if (it == _configs.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    // Re-Config Update appender(s):
    const LLBC_LoggerConfigInfo *info = it->second;
    for (int appenderType = LLBC_LogAppenderType::Begin;
         appenderType != LLBC_LogAppenderType::Network; // Note: <Network> appender is not supported for now.
         ++appenderType)
    {
        // - Appender log level:
        //   Note: ignore appender not found error.
        const auto appenderLogLevel = info->GetAppenderLogLevel(appenderType);
        if (UNLIKELY(appenderLogLevel == LLBC_LogLevel::End &&
                     LLBC_GetLastError() != LLBC_ERROR_SUCCESS))
            return LLBC_FAILED;

        if (logger->SetAppenderLogLevel(appenderType, appenderLogLevel) != LLBC_OK &&
            LLBC_GetLastError() != LLBC_ERROR_NOT_FOUND)
            return LLBC_FAILED;

        // - Update color log traces.
        logger->UpdateColorLogTraces(info->GetRequireColorLogTraces());

        // - Re-Config other appender attributes.
        // ... ...
    }

    // Re-Config log output controls (logger-wide, not per-appender).
    // The logger config parsing layer (LoggerConfigInfo) has already filtered
    // out malformed entries; we still propagate any failure faithfully so the
    // caller sees it. SetLogControls is atomic: on failure the previous
    // snapshot stays effective, on success the new snapshot is published in
    // one step (no transient empty state observable to log emitters).
    if (logger->SetLogControls(info->GetLogControls()) != LLBC_OK)
        return LLBC_FAILED;

    return LLBC_OK;
}

const std::map<LLBC_String, LLBC_LoggerConfigInfo *> &LLBC_LoggerConfigurator::GetAllConfigInfos() const
{
    return _configs;
}

__LLBC_NS_END
