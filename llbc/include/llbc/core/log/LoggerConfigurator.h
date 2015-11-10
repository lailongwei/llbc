/**
 * @file    LoggerConfigurator.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2103/06/12
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_LOGGER_CONFIGURATOR_H__
#define __LLBC_CORE_LOG_LOGGER_CONFIGURATOR_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_Logger;
class LLBC_LoggerConfigInfo;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * The logger configurator class encapsulation.
 */
class LLBC_LoggerConfigurator
{
public:
    LLBC_LoggerConfigurator();
    ~LLBC_LoggerConfigurator();

public:
    /**
     * Initialize logger configurator by cfg file.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Initialize(const LLBC_String &cfgFile);

    /**
     * Config given logger.
     * @param[in] name   - logger name.
     * @param[in] logger - will config logger.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Config(const LLBC_String &name, LLBC_Logger *logger) const;

public:
    /**
     * Get all config infos.
     * @return const std::map<LLBC_String ,LLBC_LoggerConfigInfo *> & - all config info map.
     */
    const std::map<LLBC_String, LLBC_LoggerConfigInfo *> &GetAllConfigInfos() const;

private:
    std::map<LLBC_String, LLBC_LoggerConfigInfo *> _configs;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOGGER_CONFIGURATOR_H__
