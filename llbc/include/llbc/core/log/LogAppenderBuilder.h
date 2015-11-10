/**
 * @file    LogAppenderBuilder.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/11
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_LOG_APPENDER_BUILDER_H__
#define __LLBC_CORE_LOG_LOG_APPENDER_BUILDER_H__

#include "llbc/common/Common.h"

#include "llbc/core/singleton/Singleton.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_ILogAppender;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The log appender builder class encapsulation.
 */
class LLBC_HIDDEN LLBC_LogAppenderBuilder
{
public:
    LLBC_LogAppenderBuilder();
    ~LLBC_LogAppenderBuilder();

public:
    LLBC_ILogAppender *BuildAppender(int type) const;
};

/**
 * Singleton class macro.
 */
template class LLBC_HIDDEN LLBC_Singleton<LLBC_LogAppenderBuilder>;
#define LLBC_LogAppenderBuilderSingleton LLBC_Singleton<LLBC_NS LLBC_LogAppenderBuilder>::Instance()

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOG_APPENDER_BUILDER_H__
