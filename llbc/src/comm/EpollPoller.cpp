/**
 * @file    EpollPoller.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/15
 * @version 1.0
 *
 * @brief
 */

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
    this->Stop();
}

int LLBC_EpollPoller::Start()
{
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_RTN_FAILED;
    }

    if ((_epoll = LLBC_EpollCreate(
            LLBC_CFG_EPOLL_MAX_LISTEN_FD_SIZE)) == LLBC_INVALID_HANDLE)
        return LLBC_RTN_FAILED;

    if (this->StartupMonitor() != LLBC_RTN_OK)
    {
        LLBC_EpollClose(_epoll);
        _epoll = LLBC_INVALID_HANDLE;

        return LLBC_RTN_FAILED;
    }

    if (this->Activate(1) != LLBC_RTN_OK)
    {
        this->StopMonitor();
        LLBC_EpollClose(_epoll);
        _epoll = LLBC_INVALID_HANDLE;

        return LLBC_RTN_FAILED;
    }

    _started = true;
    return LLBC_RTN_OK;
}

void LLBC_EpollPoller::Svc()
{
    while (!_started)
        LLBC_Sleep(20);

    while (!_stopping)
    {
        this->HandleQueuedEvents(20);
    }
}

void LLBC_EpollPoller::Cleanup()
{
    this->StopMonitor();

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
    if (sock->Connect(ev.peerAddr) == LLBC_RTN_OK)
    {
        _svc->Push(LLBC_SvcEvUtil::
                BuildAsyncConnResultEv(true, "Success", ev.peerAddr));
        
        this->SetConnectedSocketDftOpts(sock);
        this->AddSession(this->CreateSession(sock, ev.sessionId));
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
        epev.events = EPOLLOUT | EPOLLET;
        LLBC_EpollCtl(_epoll, EPOLL_CTL_ADD, handle, &epev);
    }
    else
    {
        const LLBC_String &reason = LLBC_FormatLastError();
        _svc->Push(LLBC_SvcEvUtil::BuildAsyncConnResultEv(false, reason, ev.peerAddr));

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

    for (int i = 0; i < count; i++)
    {
        const LLBC_EpollEvent &ev = evs[i];
        if (this->HandleConnecting(ev.data.fd, ev.events))
            continue;

        _Sockets::iterator it = _sockets.find(ev.data.fd);
        if (UNLIKELY(it == _sockets.end()))
            continue;

        LLBC_Session *session = it->second;
        if (ev.events & (EPOLLHUP|EPOLLERR))
        {
            session->OnClose();
        }
        else
        {
            if (ev.events & EPOLLIN)
            {
                if (session->IsListen())
                {
                    this->Accept(session);
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
                        UNLIKELY(_sockets.find(
                            ev.data.fd) == _sockets.end()))
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
    epev.events = EPOLLIN | EPOLLET | EPOLLHUP | EPOLLERR;
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
    LLBC_IDelegate0 *deleg = new LLBC_Delegate0<
        LLBC_EpollPoller>(this, &LLBC_EpollPoller::MonitorSvc);

    _monitor = new LLBC_PollerMonitor(deleg);
    if (_monitor->Start() != LLBC_RTN_OK)
    {
        LLBC_XDelete(_monitor);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
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

    this->Push(LLBC_PollerEvUtil::BuildEpollMonitorEv(_events, ret));
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
                            &optlen) == LLBC_RTN_OK && optval == 0)
            connected = true;
    }
    
    _svc->Push(LLBC_SvcEvUtil::BuildAsyncConnResultEv(connected, 
                connected ? "Success" : LLBC_FormatLastError(), asyncInfo.peerAddr));
    if (connected)
    {
        LLBC_EpollEvent epev;
        epev.events = EPOLLOUT | EPOLLET;
        LLBC_EpollCtl(_epoll, EPOLL_CTL_DEL, handle, &epev);

        this->SetConnectedSocketDftOpts(sock);
        this->AddSession(this->CreateSession(sock, asyncInfo.sessionId));
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

        this->SetConnectedSocketDftOpts(newSock);
        this->AddToPoller(this->CreateSession(newSock));
    }
}

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID

#include "llbc/common/AfterIncl.h"
