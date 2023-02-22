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

#pragma once

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_Logger;
class LLBC_LogRunnable;
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
     * Check has any shared async loggers or not.
     * @return bool - the async loggers flag.
     */
    bool HasSharedAsyncLoggerConfigs() const;

    /**
     * Config given logger.
     * @param[in] name              - logger name.
     * @param[in] sharedLogRunnable - the shared log runnable.
     * @param[in] logger            - will config logger.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Config(const LLBC_String &name, LLBC_LogRunnable *sharedLogRunnable, LLBC_Logger *logger) const;

public:
    /**
     * Get all config infos.
     * @return const std::map<LLBC_String ,LLBC_LoggerConfigInfo *> & - all config info map.
     */
    const std::map<LLBC_String, LLBC_LoggerConfigInfo *> &GetAllConfigInfos() const;

private:
    LLBC_LoggerConfigInfo * _rootConfig;
    std::map<LLBC_String, LLBC_LoggerConfigInfo *> _configs;
};

__LLBC_NS_END
