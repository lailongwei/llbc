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

#include "llbc/comm/protocol/ProtocolStack.h"

#include "llbc/comm/Packet.h"
#include "llbc/comm/Socket.h"
#include "llbc/comm/Session.h"
#include "llbc/comm/BasePoller.h"
#include "llbc/comm/PollerType.h"
#include "llbc/comm/ServiceEvent.h"
#include "llbc/comm/Service.h"

__LLBC_NS_BEGIN

LLBC_SessionCloseInfo::LLBC_SessionCloseInfo()
: _fromSvc(false)
{
    // Get reason.
    _reason = LLBC_FormatLastError();

    // Fill errNo & subErrNo.
    _errNo = LLBC_GetLastError();
    if (LLBC_HasSubErrorNo(_errNo))
        _subErrNo = LLBC_GetSubErrorNo();
    else
        _subErrNo = LLBC_ERROR_SUCCESS;
}

LLBC_SessionCloseInfo::LLBC_SessionCloseInfo(char *reason)
: _fromSvc(true)
, _reason(reason)

, _errNo(LLBC_ERROR_SUCCESS)
, _subErrNo(LLBC_ERROR_SUCCESS)
{
}

LLBC_SessionCloseInfo::LLBC_SessionCloseInfo(int errNo, int subErrNo)
: _fromSvc(false)
{
    // Format error.
    if (!LLBC_ERROR_TYPE_IS_LIBRARY(errNo))
    {
        _errNo = errNo;
        _subErrNo = subErrNo;
        _reason = LLBC_StrErrorEx(errNo, subErrNo);
    }
    else
    {
        _errNo = errNo;
        _subErrNo = LLBC_ERROR_SUCCESS;
        _reason = LLBC_StrError(errNo);
    }
}

LLBC_SessionCloseInfo::~LLBC_SessionCloseInfo()
{
}

LLBC_Session::LLBC_Session(const LLBC_SessionOpts &sessionOpts)
: _id(0)
, _acceptId(0)

, _sessionOpts(sessionOpts)

, _socket(nullptr)
, _sockHandle(LLBC_INVALID_SOCKET_HANDLE)

, _fullStack(false)
, _svc(nullptr)
, _poller(nullptr)

, _protoStack(nullptr)

, _pollerType(LLBC_PollerType::End)
{
}

LLBC_Session::~LLBC_Session()
{
    LLBC_XDelete(_socket);
    LLBC_XDelete(_protoStack);
}

bool LLBC_Session::IsListen() const
{
    return _socket->IsListen();
}

void LLBC_Session::SetSocket(LLBC_Socket *socket)
{
    _socket = socket;
    _sockHandle = _socket->Handle();
    _pollerType = socket->GetPollerType();
}

void LLBC_Session::SetService(LLBC_Service *svc)
{
    // Hold svc.
    _svc = svc;

    // Create protocol stack.
    if ((_fullStack = svc->IsFullStack()))
        _protoStack = _svc->CreateFullStack(_id, _acceptId);
    else
        _protoStack = _svc->CreatePackStack(_id, _acceptId);

    // Set session to protocol stack.
    _protoStack->SetSession(this);

    // Set socket message block object-pool(if enabled).
    #if LLBC_CFG_COMM_SESSION_RECV_BUF_USE_OBJ_POOL
    _socket->SetMsgBlockPoolInst(&_svc->GetMsgBlockObjectPool());
    #endif // LLBC_CFG_COMM_SESSION_RECV_BUF_USE_OBJ_POOL
}

void LLBC_Session::SetProtocolStack(LLBC_ProtocolStack *protoStack)
{
    LLBC_XDelete(_protoStack);
    _protoStack = protoStack;
}

int LLBC_Session::Send(LLBC_Packet *packet)
{
    // Serialize packet to block(throw protocol stack).
    int sendRet;
    bool removeSession;
    LLBC_MessageBlock *block;
    if (_fullStack)
        sendRet = _protoStack->Send(packet, block, removeSession);
    else
        sendRet = _protoStack->SendRaw(packet, block, removeSession);

    if (UNLIKELY(sendRet != LLBC_OK))
        return removeSession ? LLBC_FAILED : LLBC_OK;

    if (block == nullptr)
        return LLBC_OK;

    return Send(block);
}

int LLBC_Session::Send(LLBC_MessageBlock *block)
{
    // Check session send buffer size limit.
    size_t sessionSndBufUsed = _socket->GetWillSendBuffer().GetSize();
    #if LLBC_TARGET_PLATFORM_WIN32
    if (_pollerType == LLBC_PollerType::IocpPoller)
        sessionSndBufUsed += _socket->GetIocpSendingDataSize();
    #endif

    if (_sessionOpts.GetSessionSendBufSize() != static_cast<size_t>(LLBC_INFINITE) &&
        (sessionSndBufUsed + block->GetReadableSize()) >= _sessionOpts.GetSessionSendBufSize())
    {
        LLBC_Recycle(block);
        LLBC_SetLastError(LLBC_ERROR_SESSION_SND_BUF_LIMIT);

        return LLBC_FAILED;
    }

    // Send.
    if (_socket->AsyncSend(block) != LLBC_OK)
        return LLBC_FAILED;

    return LLBC_OK;
}

#if LLBC_TARGET_PLATFORM_WIN32
void LLBC_Session::OnSend(LLBC_POverlapped ol)
{
    _socket->OnSend(ol);
}
#else // LLBC_TARGET_PLATFORM_NON_WIN32
void LLBC_Session::OnSend()
{
    _socket->OnSend();
}
#endif // LLBC_TARGET_PLATFORM_WIN32

#if LLBC_TARGET_PLATFORM_WIN32
void LLBC_Session::OnRecv(LLBC_POverlapped ol)
{
    _socket->OnRecv(ol);
}
#else // LLBC_TARGET_PLATFORM_NON_WIN32
void LLBC_Session::OnRecv()
{
    _socket->OnRecv();
}
#endif // LLBC_TARGET_PLATFORM_WIN32

#if LLBC_TARGET_PLATFORM_WIN32
void LLBC_Session::OnClose(LLBC_POverlapped ol, LLBC_SessionCloseInfo *closeInfo)
#else
void LLBC_Session::OnClose(LLBC_SessionCloseInfo *closeInfo)
#endif // LLBC_TARGET_PLATFORM_WIN32
{
    if (closeInfo == nullptr)
        closeInfo = new LLBC_SessionCloseInfo;

    // Notify socket session closed.
    const LLBC_SocketHandle sockHandle = _socket->Handle();
    #if LLBC_TARGET_PLATFORM_WIN32
    _socket->OnClose(ol);
    #else
    _socket->OnClose();
    #endif // LLBC_TARGET_PLATFORM_WIN32

    // Build session-destroy event and push to service.
    _svc->Push(LLBC_SvcEvUtil::BuildSessionDestroyEv(_socket->GetLocalAddress(),
                                                     _socket->GetPeerAddress(),
                                                     _socket->IsListen(),
                                                     _id,
                                                     _acceptId,
                                                     sockHandle,
                                                     closeInfo));

    // Let poller remove self.
    _poller->RemoveSession(this);
}

void LLBC_Session::OnSent(size_t len)
{
    // TODO: For support sampler, do stuff here.
    // ... ...
}

bool LLBC_Session::OnRecved(LLBC_MessageBlock *block, bool &sessionRemoved)
{
    int recvRet;
    bool &removeSession = sessionRemoved;

    removeSession = false;

    _recvedPackets.clear();
    if (_fullStack)
        recvRet = _protoStack->Recv(block, _recvedPackets, removeSession);
    else
        recvRet = _protoStack->RecvRaw(block, _recvedPackets, removeSession);

    if (UNLIKELY(recvRet != LLBC_OK))
    {
        if (removeSession)
            OnClose();

        return false;
    }

    LLBC_Packet *packet;
    for (size_t i = 0; i < _recvedPackets.size(); ++i)
    {
        packet = _recvedPackets[i];
        packet->SetSessionId(_id);
        packet->SetLocalAddr(_socket->GetLocalAddress());
        packet->SetPeerAddr(_socket->GetPeerAddress());

        _svc->Push(LLBC_SvcEvUtil::BuildDataArrivalEv(packet));
    }

    return true;
}

void LLBC_Session::CtrlProtocolStack(int cmd, const LLBC_Variant &ctrlData, bool &removeSession)
{
    if (_fullStack)
        (void)_protoStack->CtrlStack(cmd, ctrlData, removeSession);
    else
        (void)_protoStack->CtrlStackRaw(cmd, ctrlData, removeSession);
}

__LLBC_NS_END
