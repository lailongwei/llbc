/**
 * @file    LogTokenBuilder.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/10
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_LOG_TOKEN_BUILDER_H__
#define __LLBC_CORE_LOG_LOG_TOKEN_BUILDER_H__

#include "llbc/common/Common.h"

#include "llbc/core/singleton/Singleton.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_ILogToken;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The log token builder class encapsulation.
 */
class LLBC_HIDDEN LLBC_LogTokenBuilder
{
public:
    LLBC_LogTokenBuilder();
    ~LLBC_LogTokenBuilder();

public:
    LLBC_ILogToken *BuildLogToken(int type) const;
};

/**
 * Singleton macro.
 */
template class LLBC_HIDDEN LLBC_Singleton<LLBC_LogTokenBuilder>;
#define LLBC_LogTokenBuilderSingleton LLBC_Singleton<LLBC_NS LLBC_LogTokenBuilder>::Instance()

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOG_TOKEN_BUILDER_H__
