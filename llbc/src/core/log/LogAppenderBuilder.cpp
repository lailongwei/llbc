/**
 * @file    LogAppenderBuilder.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2103/06/11
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/log/LogConsoleAppender.h"
#include "llbc/core/log/LogFileAppender.h"
#include "llbc/core/log/LogNetworkAppender.h"

#include "llbc/core/log/LogAppenderBuilder.h"

__LLBC_NS_BEGIN

LLBC_LogAppenderBuilder::LLBC_LogAppenderBuilder()
{
}

LLBC_LogAppenderBuilder::~LLBC_LogAppenderBuilder()
{
}

LLBC_ILogAppender *LLBC_LogAppenderBuilder::BuildAppender(int type) const
{
    LLBC_ILogAppender *appender = NULL;
    switch (type)
    {
    case LLBC_LogAppenderType::Console:
        appender = new LLBC_LogConsoleAppender;
        break;

    case LLBC_LogAppenderType::File:
        appender = new LLBC_LogFileAppender;
        break;

    case LLBC_LogAppenderType::Network:
        appender = new LLBC_LogNetworkAppender;
        break;

    default:
        break;
    }

    return appender;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
