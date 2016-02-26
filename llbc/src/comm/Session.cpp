/**
 * @file    Session.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/11
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/protocol/ProtocolStack.h"

#include "llbc/comm/Packet.h"
#include "llbc/comm/Socket.h"
#include "llbc/comm/Session.h"
#include "llbc/comm/BasePoller.h"
#include "llbc/comm/PollerType.h"
#include "llbc/comm/ServiceEvent.h"
#include "llbc/comm/IService.h"


__LLBC_NS_BEGIN

LLBC_Session::LLBC_Session()
: _id(0)
, _socket(NULL)
, _sockHandle(LLBC_INVALID_SOCKET_HANDLE)

, _svc(NULL)
, _poller(NULL)

, _protoStack(NULL)
{
}

LLBC_Session::~LLBC_Session()
{
    LLBC_XDelete(_socket);
    LLBC_XDelete(_protoStack);
}

int LLBC_Session::GetId() const
{
    return _id;
}

void LLBC_Session::SetId(int id)
{
    _id = id;
}

LLBC_SocketHandle LLBC_Session::GetSocketHandle() const
{
    return _sockHandle;
}

bool LLBC_Session::IsListen() const
{
    return _socket->IsListen();
}

LLBC_Socket *LLBC_Session::GetSocket()
{
    return _socket;
}

const LLBC_Socket *LLBC_Session::GetSocket() const
{
    return _socket;
}

void LLBC_Session::SetSocket(LLBC_Socket *socket)
{
    _socket = socket;
    _sockHandle = _socket->Handle();
    _pollerType = socket->GetPollerType();
}

LLBC_IService *LLBC_Session::GetService()
{
    return _svc;
}

void LLBC_Session::SetService(LLBC_IService *svc)
{
    _svc = svc;
#if LLBC_CFG_COMM_USE_FULL_STACK
    _protoStack = _svc->CreateFullStack();
#else
    _protoStack = _svc->CreateRawStack();
#endif
    _protoStack->SetSession(this);
}

LLBC_BasePoller *LLBC_Session::GetPoller()
{
    return _poller;
}

void LLBC_Session::SetPoller(LLBC_BasePoller *poller)
{
    _poller = poller;
}

int LLBC_Session::Send(LLBC_Packet *packet)
{
    LLBC_MessageBlock *block;
#if LLBC_CFG_COMM_USE_FULL_STACK
    if (_protoStack->Send(packet, block) != LLBC_OK)
#else
    if (_protoStack->SendRaw(packet, block) != LLBC_OK)
#endif
        return LLBC_FAILED;

    if (Send(block) != LLBC_OK)
    {
        LLBC_Delete(block);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_Session::Send(LLBC_MessageBlock *block)
{
    if (_socket->AsyncSend(block) != LLBC_OK)
        return LLBC_FAILED;

    // In LINUX or ANDROID platform, if use EPOLL ET mode, we must force call OnSend() one time.
#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
    if (_pollerType == LLBC_PollerType::EpollPoller)
        OnSend();
#endif

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
void LLBC_Session::OnClose(LLBC_POverlapped ol)
#else
void LLBC_Session::OnClose()
#endif // LLBC_TARGET_PLATFORM_WIN32
{
    // Notify socket session closed.
#if LLBC_TARGET_PLATFORM_WIN32
    _socket->OnClose(ol);
#else
    _socket->OnClose();
#endif // LLBC_TARGET_PLATFORM_WIN32

    // Build session-destroy event and push to service.
    _svc->Push(LLBC_SvcEvUtil::BuildSessionDestroyEv(_id));

    // Let poller remove self.
    _poller->RemoveSession(this);
}

void LLBC_Session::OnSent(size_t len)
{
    // TODO: For support sampler, do stuff here.
    // ... ...
}

bool LLBC_Session::OnRecved(LLBC_MessageBlock *block)
{
    std::vector<LLBC_Packet *> packets;
#if LLBC_CFG_COMM_USE_FULL_STACK
    if (_protoStack->Recv(block, packets) != LLBC_OK)
#else
    if (_protoStack->RecvRaw(block, packets) != LLBC_OK)
#endif
    {
        OnClose();
        return false;
    }

    LLBC_Packet *packet;
    for (size_t i = 0; i < packets.size(); i++)
    {
        packet = packets[i];
        packet->SetSessionId(_id);
        packet->SetLocalAddr(_socket->GetLocalAddress());
        packet->SetPeerAddr(_socket->GetPeerAddress());

        _svc->Push(LLBC_SvcEvUtil::BuildDataArrivalEv(packet));
    }

    return true;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
