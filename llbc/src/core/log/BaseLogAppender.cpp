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

#include "llbc/core/log/LogTokenChain.h"
#include "llbc/core/log/BaseLogAppender.h"

__LLBC_NS_BEGIN

LLBC_BaseLogAppender::LLBC_BaseLogAppender()
: _chain(NULL)
, _next(NULL)
{
}

LLBC_BaseLogAppender::~LLBC_BaseLogAppender()
{
    this->Finalize();
}

int LLBC_BaseLogAppender::Initialize(const LLBC_LogAppenderInitInfo &initInfo)
{
    if (_chain)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_RTN_FAILED;
    }

    _chain = new LLBC_LogTokenChain;
    if (_chain->Build(initInfo.pattern) != LLBC_RTN_OK)
    {
        LLBC_XDelete(_chain);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

void LLBC_BaseLogAppender::Finalize()
{
    LLBC_XDelete(_chain);
}

LLBC_LogTokenChain *LLBC_BaseLogAppender::GetTokenChain() const
{
    return _chain;
}

LLBC_ILogAppender *LLBC_BaseLogAppender::GetAppenderNext() const
{
    return _next;
}

void LLBC_BaseLogAppender::SetAppenderNext(LLBC_ILogAppender *next)
{
    _next = next;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
