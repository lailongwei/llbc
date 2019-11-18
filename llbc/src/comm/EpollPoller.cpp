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

#include "llbc/comm/Socket.h"
#include "llbc/comm/Session.h"
#include "llbc/comm/ServiceEvent.h"
#include "llbc/comm/PollerType.h"
#include "llbc/comm/EpollPoller.h"
#include "llbc/comm/PollerMonitor.h"
#include "llbc/comm/IService.h"

#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID

namespace
{
    typedef LLBC_NS LLBC_BasePoller Base;
}

__LLBC_NS_BEGIN

LLBC_EpollPoller::LLBC_EpollPoller()
: _epoll(LLBC_INVALID_HANDLE)
, _monitor(NULL)
{
}

LLBC_EpollPoller::~LLBC_EpollPoller()
{
    Stop();
}

int LLBC_EpollPoller::Start()
{
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    if ((_epoll = LLBC_EpollCreate(
            LLBC_CFG_EPOLL_MAX_LISTEN_FD_SIZE)) == LLBC_INVALID_HANDLE)
        return LLBC_FAILED;

    if (StartupMonitor() != LLBC_OK)
    {
        LLBC_EpollClose(_epoll);
        _epoll = LLBC_INVALID_HANDLE;

        return LLBC_FAILED;
    }

    if (Activate(1) != LLBC_OK)
    {
        StopMonitor();
        LLBC_EpollClose(_epoll);
        _epoll = LLBC_INVALID_HANDLE;

        return LLBC_FAILED;
    }

    _started = true;
    return LLBC_OK;
}

void LLBC_EpollPoller::Svc()
{
    while (!_started)
        LLBC_Sleep(20);

    while (!_stopping)
    {
        HandleQueuedEvents(20);
    }
}

void LLBC_EpollPoller::Cleanup()
{
    StopMonitor();

    LLBC_EpollClose(_epoll);
    _epoll = LLBC_INVALID_HANDLE;

    Base::Cleanup();
}

void LLBC_EpollPoller::HandleEv_AddSock(LLBC_PollerEvent &ev)
{
    Base::HandleEv_AddSock(ev);
}

void LLBC_EpollPoller::HandleEv_AsyncConn(LLBC_PollerEvent &ev)
{
    LLBC_Socket *sock = LLBC_New(LLBC_Socket);
    const LLBC_SocketHandle handle = sock->Handle();

    sock->SetNonBlocking();
    sock->SetPollerType(LLBC_PollerType::EpollPoller);
    if (sock->Connect(ev.peerAddr) == LLBC_OK)
    {
        _svc->Push(LLBC_SvcEvUtil::
                BuildAsyncConnResultEv(ev.sessionId, true, "Success", ev.peerAddr));
        
        SetConnectedSocketDftOpts(sock);
        AddSession(CreateSession(sock, ev.sessionId));
    }
    else if (LLBC_GetLastError() == LLBC_ERROR_WBLOCK)
    {
        LLBC_AsyncConnInfo asyncInfo;
        asyncInfo.socket = sock;
        asyncInfo.peerAddr = ev.peerAddr;
        asyncInfo.sessionId = ev.sessionId;
        _connecting.insert(std::make_pair(handle, asyncInfo));

        LLBC_EpollEvent epev;
        epev.data.fd = handle;
        epev.data.u32 = ev.sessionId;
        epev.events = EPOLLOUT | EPOLLET;
        LLBC_EpollCtl(_epoll, EPOLL_CTL_ADD, handle, &epev);
    }
    else
    {
        const LLBC_String &reason = LLBC_FormatLastError();
        _svc->Push(LLBC_SvcEvUtil::BuildAsyncConnResultEv(ev.sessionId, false, reason, ev.peerAddr));

        LLBC_Delete(sock);
    }
}

void LLBC_EpollPoller::HandleEv_Send(LLBC_PollerEvent &ev)
{
    Base::HandleEv_Send(ev);
}

void LLBC_EpollPoller::HandleEv_Close(LLBC_PollerEvent &ev)
{
    Base::HandleEv_Close(ev);
}

void LLBC_EpollPoller::HandleEv_Monitor(LLBC_PollerEvent &ev)
{
    const int count = *reinterpret_cast<int *>(ev.un.monitorEv);
    LLBC_EpollEvent *evs = 
        reinterpret_cast<LLBC_EpollEvent *>(ev.un.monitorEv + sizeof(int));

    for (int i = 0; i < count; ++i)
    {
        const LLBC_EpollEvent &ev = evs[i];
        if (HandleConnecting(ev.data.fd, ev.events))
            continue;

        const int &sessionId = ev.data.u32;
        _Sessions::iterator it = _sessions.find(sessionId);
        if (UNLIKELY(it == _sessions.end()))
        {
            // TODO: For test
            Log.e2<LLBC_EpollPoller>("Session ev triggered, session not found, sid:%d, handle:%d", sessionId, ev.data.fd);
            continue;
        }

        LLBC_Session *session = it->second;
        if (ev.events & (EPOLLHUP | EPOLLERR))
        {
            LLBC_Socket *sock = session->GetSocket();

            int sockErr;
            LLBC_SessionCloseInfo *closeInfo;
            if (sock->GetPendingError(sockErr) != LLBC_OK)
            {
                // TODO: For test
                Log.e2<LLBC_EpollPoller>("time(micro second):%llu, Session[%d] encountered EPOLLHUP(%s) or EPOLLERR(%s), try get pending error, but failed, error:%d, errno:%d",
                        LLBC_GetMicroSeconds(), 
                        session->GetId(), 
                        ev.events & EPOLLHUP ? "True" : "False",  
                        ev.events & EPOLLERR ? "True" : "False",  
                        LLBC_Errno, 
                        LLBC_GetSubErrorNo());
                closeInfo = LLBC_New0(LLBC_SessionCloseInfo);
            }
            else
            {
                // TODO: For test
                Log.e2<LLBC_EpollPoller>("time(micro second):%llu, Session[%d] encountered EPOLLHUP(%s) or EPOLLERR(%s), pending error ret:%d",
                        LLBC_GetMicroSeconds(), 
                        session->GetId(), 
                        ev.events & EPOLLHUP ? "True" : "False",  
                        ev.events & EPOLLERR ? "True" : "False",  
                        sockErr);

                //! Linux platform will encounter this situation, file descriptor will reuse(by poller thread Accept()) 
                //  when epoller monitor thread received(by epoll_wait) old socket EPOLLHUP/EPOLLERR ev.
                if (UNLIKELY(sockErr == 0))
                    continue;

                closeInfo = LLBC_New2(LLBC_SessionCloseInfo, LLBC_ERROR_CLIB, sockErr);
            }

            session->OnClose(closeInfo);
        }
        else
        {
            if (ev.events & EPOLLIN)
            {
                if (session->IsListen())
                {
                    Accept(session);
                    continue;
                }
                else
                {
                    session->OnRecv();
                }
            }
            if (ev.events & EPOLLOUT)
            {
                // Maybe in session removed while calling OnRecv() method.
                if ((ev.events & EPOLLIN) && 
                        UNLIKELY(_sessions.find(sessionId) == _sessions.end()))
                    continue;

                session->OnSend();
            }
       }
    }

    LLBC_Free(ev.un.monitorEv);
}

void LLBC_EpollPoller::HandleEv_TakeOverSession(LLBC_PollerEvent &ev)
{
    Base::HandleEv_TakeOverSession(ev);
}

void LLBC_EpollPoller::AddSession(LLBC_Session *session)
{
    Base::AddSession(session);

    LLBC_Socket *sock = session->GetSocket();
    const LLBC_SocketHandle handle = sock->Handle();

    LLBC_EpollEvent epev;
    epev.data.fd = handle;
    epev.data.u32 = session->GetId();
    epev.events = EPOLLIN | EPOLLET | EPOLLHUP | EPOLLERR;
    if (!sock->IsListen())
        epev.events |= EPOLLOUT;

    // TODO: For debug
    Log.d2<LLBC_EpollPoller>("Add new session[%d], time(micro seconds):%llu",
            session->GetId(), LLBC_GetMicroSeconds());
    LLBC_EpollCtl(_epoll, EPOLL_CTL_ADD, handle, &epev);

    // TODO: For debug
    // if (!sock->IsListen())
    //     session->OnRecv();
}

void LLBC_EpollPoller::RemoveSession(LLBC_Session *session)
{
    // For compatible before 2.6.9 version kernel, we pass event point to LLBC_EpollCtl() API,
    // even through this argument is ignored.
    LLBC_EpollEvent epev;
    epev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLHUP | EPOLLERR;
    LLBC_EpollCtl(_epoll, EPOLL_CTL_DEL, session->GetSocketHandle(), &epev);

    Base::RemoveSession(session);
}

int LLBC_EpollPoller::StartupMonitor()
{
    typedef LLBC_Delegate0<void, LLBC_EpollPoller> __MonitorDeleg;

    LLBC_IDelegate0<void> *deleg = 
        LLBC_New2(__MonitorDeleg, this, &LLBC_EpollPoller::MonitorSvc);
    _monitor = LLBC_New1(LLBC_PollerMonitor, deleg);
    if (_monitor->Start() != LLBC_OK)
    {
        LLBC_XDelete(_monitor);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

void LLBC_EpollPoller::StopMonitor()
{
    LLBC_XDelete(_monitor);
}

void LLBC_EpollPoller::MonitorSvc()
{
    int ret = LLBC_EpollWait(_epoll,
                             _events,
                             LLBC_CFG_COMM_MAX_EVENT_COUNT,
                             50);
    if (ret <= 0)
        return;

    Push(LLBC_PollerEvUtil::BuildEpollMonitorEv(_events, ret));
}

bool LLBC_EpollPoller::HandleConnecting(LLBC_SocketHandle handle, int events)
{
    _Connecting::iterator it = _connecting.find(handle);
    if (it == _connecting.end())
        return false;
    
    LLBC_AsyncConnInfo &asyncInfo = it->second;
    LLBC_Socket *sock = asyncInfo.socket;

    bool connected = false;
    if (events & EPOLLOUT)
    {
        int optval;
        LLBC_SocketLen optlen = sizeof(int);
        if (sock->GetOption(SOL_SOCKET, 
                            SO_ERROR, 
                            &optval, 
                            &optlen) == LLBC_OK && optval == 0)
            connected = true;
    }
    
    _svc->Push(LLBC_SvcEvUtil::BuildAsyncConnResultEv(asyncInfo.sessionId,
                                                      connected,
                                                      connected ? "Success" : LLBC_FormatLastError(),
                                                      asyncInfo.peerAddr));

    LLBC_EpollEvent epev;
    epev.events = EPOLLOUT | EPOLLET;
    LLBC_EpollCtl(_epoll, EPOLL_CTL_DEL, handle, &epev);
    if (connected)
    {
        SetConnectedSocketDftOpts(sock);
        AddSession(CreateSession(sock, asyncInfo.sessionId));
    }
    else
    {
        LLBC_XDelete(sock);
    }

    _connecting.erase(it);
    return true;
}

void LLBC_EpollPoller::Accept(LLBC_Session *session)
{
    LLBC_Socket *newSock;
    LLBC_Socket *sock = session->GetSocket();
    for (; ;)
    {
        if (!(newSock = sock->Accept()))
            break;

        newSock->SetNonBlocking();

        SetConnectedSocketDftOpts(newSock);
        AddToPoller(CreateSession(newSock, 0, session));
    }
}

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID

#include "llbc/common/AfterIncl.h"
