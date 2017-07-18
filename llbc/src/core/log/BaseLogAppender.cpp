/**
 * @file    BaseLogAppender.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/10
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogTokenChain.h"
#include "llbc/core/log/BaseLogAppender.h"

__LLBC_NS_BEGIN

LLBC_BaseLogAppender::LLBC_BaseLogAppender()
: _level(LLBC_LogLevel::End)

, _chain(NULL)
, _next(NULL)
{
}

LLBC_BaseLogAppender::~LLBC_BaseLogAppender()
{
    Finalize();
}

int LLBC_BaseLogAppender::Initialize(const LLBC_LogAppenderInitInfo &initInfo)
{
    if (_chain)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    _level = initInfo.level;

    _chain = new LLBC_LogTokenChain;
    if (_chain->Build(initInfo.pattern) != LLBC_OK)
    {
        LLBC_XDelete(_chain);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

void LLBC_BaseLogAppender::Finalize()
{
    LLBC_XDelete(_chain);
    _level = LLBC_LogLevel::End;
}

void LLBC_BaseLogAppender::Flush()
{
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
