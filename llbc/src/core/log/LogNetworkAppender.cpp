/**
 * @file    LogNetworkAppender.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/11
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/log/LogNetworkAppender.h"

__LLBC_NS_BEGIN

LLBC_LogNetworkAppender::LLBC_LogNetworkAppender()
: _ip("127.0.0.1")
, _port(0)
{
}

LLBC_LogNetworkAppender::~LLBC_LogNetworkAppender()
{
    Finalize();
}

int LLBC_LogNetworkAppender::GetType() const
{
    return LLBC_LogAppenderType::Network;
}

int LLBC_LogNetworkAppender::Initialize(const LLBC_LogAppenderInitInfo &initInfo)
{
    _ip = "127.0.0.1";
    _port = 0;

    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
}

void LLBC_LogNetworkAppender::Finalize()
{
    _Base::Finalize();
}

int LLBC_LogNetworkAppender::Output(const LLBC_LogData &data)
{
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
