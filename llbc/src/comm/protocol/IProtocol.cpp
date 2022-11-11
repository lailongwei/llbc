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

#include "llbc/comm/Service.h"
#include "llbc/comm/Session.h"
#include "llbc/comm/protocol/IProtocol.h"
#include "llbc/comm/protocol/ProtocolStack.h"

__LLBC_NS_BEGIN

LLBC_IProtocol::LLBC_IProtocol()
: _sessionId(0)
, _acceptSessionId(0)
, _session(nullptr)
, _stack(nullptr)
, _svc(nullptr)
, _filter(nullptr)
, _coders(nullptr)
, _pktPoolInst(nullptr)
{
}

LLBC_IProtocol::~LLBC_IProtocol()
{
}

int LLBC_IProtocol::GetSessionId() const
{
    return _sessionId;
}

int LLBC_IProtocol::GetAcceptSessionId() const
{
    return _acceptSessionId;
}

LLBC_ProtocolStack *LLBC_IProtocol::GetStack()
{
    return _stack;
}

LLBC_Service *LLBC_IProtocol::GetService()
{
    return _svc;
}

const LLBC_IProtocol::Coders *LLBC_IProtocol::GetCoders() const
{
    return _coders;
}

bool LLBC_IProtocol::Ctrl(int cmd, const LLBC_Variant &ctrlData, bool &removeSession)
{
    return true;
}

void LLBC_IProtocol::SetSession(LLBC_Session *session)
{
    _session = session;

    _sessionId = session->GetId();
    _acceptSessionId = session->GetAcceptId();
}

void LLBC_IProtocol::SetStack(LLBC_ProtocolStack *stack)
{
    _stack = stack;
    _svc = _stack->GetService();
    _pktPoolInst = &_svc->GetPacketObjectPool();
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
