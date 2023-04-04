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

#include "llbc/comm/Packet.h"
#include "llbc/comm/Socket.h"
#include "llbc/comm/PollerType.h"
#include "llbc/comm/PollerEvent.h"
#include "llbc/comm/BasePoller.h"
#include "llbc/comm/PollerMgr.h"
#include "llbc/comm/ServiceImpl.h"

namespace
{
    typedef LLBC_NS LLBC_PollerMgr This;
}

__LLBC_INTERNAL_NS_BEGIN

static LLBC_NS LLBC_Socket *__CreateSocket(int type)
{
    LLBC_NS LLBC_SocketHandle handle = LLBC_INVALID_SOCKET_HANDLE;
#if LLBC_TARGET_PLATFORM_WIN32
    if (type == LLBC_NS LLBC_PollerType::IocpPoller)
        if (UNLIKELY((handle = LLBC_NS LLBC_CreateTcpSocketEx()) == LLBC_INVALID_SOCKET_HANDLE))
            return nullptr;
#endif

    LLBC_NS LLBC_Socket *sock = 
        new LLBC_NS LLBC_Socket(handle);
    sock->SetPollerType(type);

    return sock;
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_PollerMgr::LLBC_PollerMgr()
: _type(LLBC_PollerType::End)
, _svc(nullptr)

, _pollerCount(0)
, _pollers(nullptr)
, _pollerLock()

, _maxSessionId(1)

, _pendingAddSocks()
, _pendingAsyncConns()
{
}

LLBC_PollerMgr::~LLBC_PollerMgr()
{
    Stop();
}

void LLBC_PollerMgr::SetPollerType(int type)
{
    _type = type;
}

void LLBC_PollerMgr::SetService(LLBC_Service *svc)
{
    _svc = svc;
}

int LLBC_PollerMgr::Init(int count)
{
    if (count <= 0)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (_pollers)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    _pollerCount = count;
    _pollers = LLBC_Calloc(LLBC_BasePoller *, sizeof(LLBC_BasePoller *) * count);

    // Create pollers.
    for (int i = 0; i < count; ++i)
    {
        LLBC_BasePoller *poller = LLBC_BasePoller::Create(_type);
        poller->SetPollerId(i);
        poller->SetService(_svc);
        poller->SetPollerMgr(this);
        poller->SetBrothersCount(count);

        _pollers[i] = poller;
    }

    return LLBC_OK;
}

int LLBC_PollerMgr::Start()
{
    // Uninit check.
    if (!_pollers)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    // Startup all pollers.
    for (int i = 0; i < _pollerCount; ++i)
        _pollers[i]->Start();

    // Process pending sockets.
    for (_PendingAddSocks::iterator it = _pendingAddSocks.begin();
         it != _pendingAddSocks.end();
         ++it)
         _pollers[it->first % _pollerCount]->Push(
                LLBC_PollerEvUtil::BuildAddSockEv(it->second.first, it->first, it->second.second));
    _pendingAddSocks.clear();

    // Process Async-connections.
    for (_PendingAsyncConns::iterator it = _pendingAsyncConns.begin();
         it != _pendingAsyncConns.end();
         ++it)
    {
        _pollers[it->first % _pollerCount]->Push(
            LLBC_PollerEvUtil::BuildAsyncConnEv(it->first, it->second.second, it->second.first));
    }

    _pendingAsyncConns.clear();

    return LLBC_OK;
}

void LLBC_PollerMgr::Stop()
{
    LLBC_ReturnIf(!_pollers, void());

    // Stop all pollers.
    for (int i = _pollerCount - 1; i >= 0; --i)
        _pollers[i]->Stop();
}

void LLBC_PollerMgr::Finalize()
{
    // Always cleanup pending add-sock container.
    for (_PendingAddSocks::iterator it = _pendingAddSocks.begin();
         it != _pendingAddSocks.end();
         ++it)
        delete it->second.first;
    _pendingAddSocks.clear();

    // Always cleanup pending async-conn container.
    _pendingAsyncConns.clear();

    // Delete all pollers.
    if (_pollers)
    {
        for (int i = _pollerCount - 1; i >= 0; --i)
            LLBC_XDelete(_pollers[i]);

        LLBC_XFree(_pollers);
        _pollerCount = 0;
    }

    // Reset max sessionId.
    _maxSessionId = 1;
}

int LLBC_PollerMgr::Listen(const char *ip, uint16 port, LLBC_IProtocolFactory *protoFactory, const LLBC_SessionOpts &sessionOpts)
{
    LLBC_SockAddr_IN local;
    if (This::GetAddr(ip, port, local) != LLBC_OK)
        return 0;

    // Create socket and listen.
    LLBC_Socket *sock;
    if (!(sock = LLBC_INL_NS __CreateSocket(_type)))
    {
        return 0;
    }
    else if (sock->SetNonBlocking() != LLBC_OK ||
             sock->EnableAddressReusable() != LLBC_OK ||
             sock->BindTo(local) != LLBC_OK ||
             sock->SetNoDelay(sessionOpts.IsNoDelay()) ||
             (sessionOpts.GetSockSendBufSize() != 0 && sock->SetSendBufSize(sessionOpts.GetSockSendBufSize()) != LLBC_OK) ||
             (sessionOpts.GetSockRecvBufSize() != 0 && sock->SetRecvBufSize(sessionOpts.GetSockRecvBufSize()) != LLBC_OK) ||
             sock->Listen() != LLBC_OK ||
             sock->SetMaxPacketSize(sessionOpts.GetMaxPacketSize()) != LLBC_OK)
    {
        delete sock;
        return 0;
    }

    // Allocate sessionId and add proto factory to service(is exist).
    const int sessionId = AllocSessionId();
    if (protoFactory)
        _svc->AddSessionProtocolFactory(sessionId, protoFactory);

    // Add to poller or pending.
    if (LIKELY(_pollers))
        _pollers[sessionId % _pollerCount]->Push(
                LLBC_PollerEvUtil::BuildAddSockEv(sock, sessionId, sessionOpts));
    else
        _pendingAddSocks.insert(std::make_pair(sessionId, std::make_pair(sock, sessionOpts)));

    return sessionId;
}

int LLBC_PollerMgr::Connect(const char *ip, uint16 port, LLBC_IProtocolFactory *protoFactory, const LLBC_SessionOpts &sessionOpts)
{
    LLBC_SockAddr_IN peer;
    if (This::GetAddr(ip, port, peer) != LLBC_OK)
        return 0;

    // Create socket and connect.
    LLBC_Socket *sock;
    if (!(sock = LLBC_INL_NS __CreateSocket(_type)))
    {
        return 0;
    }
    else if ((sessionOpts.GetSockSendBufSize() != 0 && sock->SetSendBufSize(sessionOpts.GetSockSendBufSize()) != LLBC_OK) ||
             (sessionOpts.GetSockRecvBufSize() != 0 && sock->SetRecvBufSize(sessionOpts.GetSockRecvBufSize()) != LLBC_OK) ||
             sock->SetNoDelay(sessionOpts.IsNoDelay()) != LLBC_OK ||
             sock->Connect(peer) != LLBC_OK ||
             sock->SetNonBlocking() != LLBC_OK)
    {
        delete sock;
        return 0;
    }

    // Allocate session and add protoFactory to service(if exist).
    const int sessionId = AllocSessionId();
    if (protoFactory)
        _svc->AddSessionProtocolFactory(sessionId, protoFactory);

    // Add to poller or pending.
    if (LIKELY(_pollers))
        _pollers[sessionId % _pollerCount]->Push(
                LLBC_PollerEvUtil::BuildAddSockEv(sock, sessionId, sessionOpts));
    else
        _pendingAddSocks.insert(std::make_pair(sessionId, std::make_pair(sock, sessionOpts)));

    return sessionId;
}

int LLBC_PollerMgr::AsyncConn(const char *ip,
                              uint16 port,
                              int &pendingSessionId,
                              LLBC_IProtocolFactory *protoFactory,
                              const LLBC_SessionOpts &sessionOpts)
{
    LLBC_SockAddr_IN peer;
    if (This::GetAddr(ip, port, peer) != LLBC_OK)
    {
        pendingSessionId = 0;
        return LLBC_FAILED;
    }

    const int sessionId = AllocSessionId();
    if (protoFactory)
        _svc->AddSessionProtocolFactory(sessionId, protoFactory);

    if (LIKELY(_pollers))
        _pollers[sessionId % _pollerCount]->Push(
                LLBC_PollerEvUtil::BuildAsyncConnEv(sessionId, sessionOpts, peer));
    else
        _pendingAsyncConns.insert(std::make_pair(sessionId, std::make_pair(peer, sessionOpts)));

    pendingSessionId = sessionId;

    return LLBC_OK;
}

int LLBC_PollerMgr::Send(LLBC_Packet *packet)
{
    _pollers[packet->GetSessionId() % 
        _pollerCount]->Push(LLBC_PollerEvUtil::BuildSendEv(packet));
    return LLBC_OK;
}

void LLBC_PollerMgr::Close(int sessionId, const char *reason)
{
    _pollers[sessionId % _pollerCount]->Push(LLBC_PollerEvUtil::BuildCloseEv(sessionId, reason));
}

void LLBC_PollerMgr::CtrlProtocolStack(int sessionId,
                                       int ctrlCmd,
                                       const LLBC_Variant &ctrlData)
{
    _pollers[sessionId % _pollerCount]->Push(
        LLBC_PollerEvUtil::BuildCtrlProtocolStackEv(sessionId, ctrlCmd, ctrlData));
}

int LLBC_PollerMgr::AllocSessionId()
{
    return LLBC_AtomicFetchAndAdd(&_maxSessionId, 1);
}

int LLBC_PollerMgr::PushMsgToPoller(int id, LLBC_MessageBlock *block)
{
    LLBC_LockGuard guard(_pollerLock);
    LLBC_BasePoller *poller = _pollers[id];
    if (LIKELY(poller))
    {
        poller->Push(block);
        return LLBC_OK;
    }
    
    return LLBC_FAILED;
}

void LLBC_PollerMgr::OnPollerStop(int id)
{
    _pollerLock.Lock();
    _pollers[id] = nullptr;

    _pollerLock.Unlock();
}

int LLBC_PollerMgr::GetAddr(const char *ip, uint16 port, LLBC_SockAddr_IN &addr)
{
    if (LLBC_IsIPv4Addr(ip))
    {
        addr.SetIp(ip);
        addr.SetPort(port);

        return LLBC_OK;
    }

    // Use LLBC_GetAddrInfo to fetch sockaddr.
    addrinfo hints;
    hints.ai_flags = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_addrlen = 0;
    hints.ai_addr = nullptr;
    hints.ai_canonname = nullptr;
    hints.ai_next = nullptr;

    struct addrinfo *ai;
    if (LLBC_GetAddrInfo(ip,                         // servname
                         LLBC_Num2Str(port).c_str(), // nodename
                         &hints,                     // hints
                         &ai) != LLBC_OK)            // result
        return LLBC_FAILED;

    addr.FromOSDataType(reinterpret_cast<
            struct sockaddr_in *>(ai->ai_addr));
    LLBC_FreeAddrInfo(ai);
    
    return LLBC_OK;
}

__LLBC_NS_END
