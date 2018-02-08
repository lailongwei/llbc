/**
 * @file    IProtocol.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/01/10
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/Session.h"
#include "llbc/comm/protocol/IProtocol.h"
#include "llbc/comm/protocol/ProtocolStack.h"

__LLBC_NS_BEGIN

LLBC_IProtocol::LLBC_IProtocol()
: _sessionId(0)
, _stack(NULL)
, _filter(NULL)
, _coders(NULL)
{
}

LLBC_IProtocol::~LLBC_IProtocol()
{
}

void LLBC_IProtocol::SetStack(LLBC_ProtocolStack *stack)
{
    _stack = stack;
}

void LLBC_IProtocol::SetSession(LLBC_Session *session)
{
    _session = session;
    _sessionId = session->GetId();
}

void LLBC_IProtocol::SetFilter(LLBC_IProtocolFilter *filter)
{
    _filter = filter;
}

int LLBC_IProtocol::SetCoders(const Coders *coders)
{
    if (GetLayer() != LLBC_ProtocolLayer::CodecLayer)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    _coders = coders;
    return LLBC_OK;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
