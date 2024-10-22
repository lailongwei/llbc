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

#include "llbc/comm/Socket.h"
#include "llbc/comm/Session.h"
#include "llbc/comm/ServiceEvent.h"
#include "llbc/comm/PollerType.h"
#include "llbc/comm/EpollPoller.h"
#include "llbc/comm/PollerMonitor.h"
#include "llbc/comm/Service.h"

#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID

namespace
{
    typedef LLBC_NS LLBC_BasePoller Base;
}

__LLBC_NS_BEGIN

LLBC_EpollPoller::LLBC_EpollPoller()
: _epoll(LLBC_INVALID_HANDLE)
, _monitor(nullptr)
{
}

LLBC_EpollPoller::~LLBC_EpollPoller()
{
    Stop();
}

int LLBC_EpollPoller::Start()
{
    if (GetTaskState() != LLBC_TaskState::NotActivated)
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

    return LLBC_OK;
}

void LLBC_EpollPoller::Stop()
{
    if (!IsActivated() || _stopping)
        return;

    StopMonitor();

    LLBC_BasePoller::Stop();
}

void LLBC_EpollPoller::Svc()
{
    while (!_stopping)
        HandleQueuedEvents(20);
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
    LLBC_Socket *sock = new LLBC_Socket;
    const LLBC_SocketHandle handle = sock->Handle();

    sock->SetNonBlocking();
    sock->SetPollerType(LLBC_PollerType::EpollPoller);
    if (sock->Connect(ev.peerAddr) == LLBC_OK)
    {
        _svc->Push(LLBC_SvcEvUtil::
                BuildAsyncConnResultEv(ev.sessionId, true, "Success", ev.peerAddr));

        SetConnectedSocketOpts(sock, *ev.sessionOpts);
        AddSession(CreateSession(sock, ev.sessionId, *ev.sessionOpts, nullptr));

        LLBC_XDelete(ev.sessionOpts);
    }
    else if (LLBC_GetLastError() == LLBC_ERROR_WBLOCK)
    {
        LLBC_AsyncConnInfo asyncInfo;
        asyncInfo.socket = sock;
        asyncInfo.peerAddr = ev.peerAddr;
        asyncInfo.sessionId = ev.sessionId;
        asyncInfo.sessionOpts = *ev.sessionOpts;
        _connecting.insert(std::make_pair(handle, asyncInfo));

        LLBC_EpollEvent epev;
        epev.events = EPOLLOUT | EPOLLET;
        epev.data.u64 = (static_cast<uint64>(ev.sessionId) << 32) | static_cast<uint64>(handle);
        LLBC_EpollCtl(_epoll, EPOLL_CTL_ADD, handle, &epev);
        
        LLBC_XDelete(ev.sessionOpts);
    }
    else
    {
        const LLBC_String &reason = LLBC_FormatLastError();
        _svc->Push(LLBC_SvcEvUtil::BuildAsyncConnResultEv(ev.sessionId, false, reason, ev.peerAddr));

        delete sock;
        LLBC_XDelete(ev.sessionOpts);
    }
}

void LLBC_EpollPoller::HandleEv_Send(LLBC_PollerEvent &ev)
{
    const int sessionId = ev.un.packet->GetSessionId();

    Base::HandleEv_Send(ev);

    // In LINUX or ANDROID platform, if use EPOLL ET mode, we must force call OnSend() one time.
    _Sessions::iterator it = _sessions.find(sessionId);
    if (it == _sessions.end())
        return;

    LLBC_Session *&session = it->second;
    session->OnSend();
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
        const LLBC_EpollEvent &epev = evs[i];
        const LLBC_SocketHandle handle = static_cast<int>(epev.data.u64 & 0xffffffffull);
        if (HandleConnecting(handle, epev.events))
            continue;

        const int sessionId = static_cast<int>(epev.data.u64 >> 32);
        _Sessions::iterator it = _sessions.find(sessionId);
        if (UNLIKELY(it == _sessions.end()))
            continue;

        LLBC_Session *session = it->second;
        if (epev.events & (EPOLLHUP | EPOLLERR))
        {
            LLBC_Socket *sock = session->GetSocket();

            int sockErr;
            LLBC_SessionCloseInfo *closeInfo;
            if (sock->GetPendingError(sockErr) != LLBC_OK)
            {
                closeInfo = new LLBC_SessionCloseInfo;
            }
            else
            {
                closeInfo = new LLBC_SessionCloseInfo(LLBC_ERROR_CLIB, sockErr);
            }

            session->OnClose(closeInfo);
        }
        else
        {
            if (epev.events & EPOLLIN)
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
            if (epev.events & EPOLLOUT)
            {
                // Maybe in session removed while calling OnRecv() method.
                if ((epev.events & EPOLLIN) &&
                        UNLIKELY(_sessions.find(sessionId) == _sessions.end()))
                    continue;

                session->OnSend();
            }
       }
    }

    free(ev.un.monitorEv);
}

void LLBC_EpollPoller::HandleEv_TakeOverSession(LLBC_PollerEvent &ev)
{
    Base::HandleEv_TakeOverSession(ev);
}

void LLBC_EpollPoller::HandleEv_CtrlProtocolStack(LLBC_PollerEvent &ev)
{
    // Store sessionId first.
    const int sessionId = ev.sessionId;

    // Do protocol stack control.
    Base::HandleEv_CtrlProtocolStack(ev);

    // Find session and force trigger OnSend() operation on Epoll trigger mode.
    // TODO: Can be optimized.
    _Sessions::iterator it = _sessions.find(sessionId);
    if (it == _sessions.end())
        return;

    LLBC_Session *&session = it->second;
    session->OnSend();
}

void LLBC_EpollPoller::AddSession(LLBC_Session *session)
{
    Base::AddSession(session);

    LLBC_Socket *sock = session->GetSocket();
    const LLBC_SocketHandle handle = sock->Handle();

    LLBC_EpollEvent epev;
    epev.events = EPOLLIN | EPOLLET | EPOLLHUP | EPOLLERR;
    epev.data.u64 = (static_cast<uint64>(session->GetId()) << 32) | static_cast<uint64>(handle);
    if (!sock->IsListen())
        epev.events |= EPOLLOUT;

    LLBC_EpollCtl(_epoll, EPOLL_CTL_ADD, handle, &epev);
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
    const LLBC_Delegate<void()> deleg(this, &LLBC_EpollPoller::MonitorSvc);
    _monitor = new LLBC_PollerMonitor(deleg);
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
        SetConnectedSocketOpts(sock, asyncInfo.sessionOpts);
        AddSession(CreateSession(sock, asyncInfo.sessionId, asyncInfo.sessionOpts, nullptr));
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

        SetConnectedSocketOpts(newSock, session->GetSessionOpts());
        AddToPoller(CreateSession(newSock, 0, session->GetSessionOpts(), session));
    }
}

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
