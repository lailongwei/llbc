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
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/Packet.h"
#include "llbc/comm/Socket.h"
#include "llbc/comm/PollerType.h"
#include "llbc/comm/PollerEvent.h"
#include "llbc/comm/BasePoller.h"
#include "llbc/comm/PollerMgr.h"

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
            return NULL;
#endif

    LLBC_NS LLBC_Socket *sock = 
        LLBC_New1(LLBC_NS LLBC_Socket, handle);
    sock->SetPollerType(type);

    return sock;
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_PollerMgr::LLBC_PollerMgr()
: _type(LLBC_PollerType::End)
, _svc(NULL)

, _pollerCount(0)
, _pollers(NULL)
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

void LLBC_PollerMgr::SetService(LLBC_IService *svc)
{
    _svc = svc;
}

int LLBC_PollerMgr::Start(int count)
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
    _pollers = LLBC_Malloc(LLBC_BasePoller *, sizeof(LLBC_BasePoller *) * count);
    ::memset(_pollers, 0, sizeof(LLBC_BasePoller *) * count);

    // Create pollers.
    for (int i = 0; i < count; i++)
    {
        _pollers[i] = LLBC_BasePoller::Create(_type);
        _pollers[i]->SetPollerId(i);
        _pollers[i]->SetService(_svc);
        _pollers[i]->SetPollerMgr(this);
        _pollers[i]->SetBrothersCount(count);
    }

    // Startup all pollers.
    for (int i = 0; i < count; i++)
        _pollers[i]->Start();

    // Process pending sockets.
    for (_PendingAddSocks::iterator it = _pendingAddSocks.begin();
         it != _pendingAddSocks.end();
         it++)
         _pollers[it->first % _pollerCount]->Push(
                LLBC_PollerEvUtil::BuildAddSockEv(it->first, it->second));
    _pendingAddSocks.clear();

    // Process Async-connections.
    for (_PendingAsyncConns::iterator it = _pendingAsyncConns.begin();
         it != _pendingAsyncConns.end();
         it++)
        _pollers[it->first % _pollerCount]->Push(
                LLBC_PollerEvUtil::BuildAsyncConnEv(it->first, it->second));
    _pendingAsyncConns.clear();

    return LLBC_OK;
}

void LLBC_PollerMgr::Stop()
{
    // Always cleanup pending add-sock container.
    LLBC_STLHelper::DeleteContainer(_pendingAddSocks);
    // Always cleanup pending async-conn container.
    _pendingAsyncConns.clear();

    if (_pollers)
    {
        for (int i = 0; i < _pollerCount; i++)
            LLBC_Delete(_pollers[i]);
        LLBC_XFree(_pollers);
        _pollerCount = 0;
    }

    _maxSessionId = 1;
}

int LLBC_PollerMgr::Listen(const char *ip, uint16 port)
{
    LLBC_SockAddr_IN local;
    if (This::GetAddr(ip, port, local) != LLBC_OK)
        return 0;

    LLBC_Socket *sock;
    if (!(sock = LLBC_INL_NS __CreateSocket(_type)))
    {
        return 0;
    }
    else if (sock->SetNonBlocking() != LLBC_OK ||
            sock->EnableAddressReusable() != LLBC_OK ||
            sock->BindTo(local) != LLBC_OK ||
            sock->Listen() != LLBC_OK)
    {
        LLBC_Delete(sock);
        return 0;
    }

    const int sessionId = AllocSessionId();
    if (LIKELY(_pollers))
        _pollers[sessionId % _pollerCount]->Push(
                LLBC_PollerEvUtil::BuildAddSockEv(sessionId, sock));
    else
        _pendingAddSocks.insert(std::make_pair(sessionId, sock));

    return sessionId;
}

int LLBC_PollerMgr::Connect(const char *ip, uint16 port)
{
    LLBC_SockAddr_IN peer;
    if (This::GetAddr(ip, port, peer) != LLBC_OK)
        return 0;

    LLBC_Socket *sock;
    if (!(sock = LLBC_INL_NS __CreateSocket(_type)))
    {
        return 0;
    }
    else if (sock->Connect(peer) != LLBC_OK)
    {
        LLBC_Delete(sock);
        return 0;
    }

    sock->SetNonBlocking();

    const int sessionId = AllocSessionId();

    if (LIKELY(_pollers))
        _pollers[sessionId % _pollerCount]->Push(
                LLBC_PollerEvUtil::BuildAddSockEv(sessionId, sock));
    else
        _pendingAddSocks.insert(std::make_pair(sessionId, sock));

    return sessionId;
}

int LLBC_PollerMgr::AsyncConn(const char *ip, uint16 port, int &pendingSessionId)
{
    LLBC_SockAddr_IN peer;
    if (This::GetAddr(ip, port, peer) != LLBC_OK)
    {
        pendingSessionId = 0;
        return LLBC_FAILED;
    }

    const int sessionId = AllocSessionId();
    if (LIKELY(_pollers))
        _pollers[sessionId % _pollerCount]->Push(
                LLBC_PollerEvUtil::BuildAsyncConnEv(sessionId, peer));
    else
        _pendingAsyncConns.insert(std::make_pair(sessionId, peer));

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
    _pollers[id] = NULL;

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
    struct addrinfo hints;
    hints.ai_flags = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_addrlen = 0;
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;

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

#include "llbc/common/AfterIncl.h"
