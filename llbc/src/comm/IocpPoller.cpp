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
#include "llbc/comm/IocpPoller.h"
#include "llbc/comm/PollerMonitor.h"
#include "llbc/comm/Service.h"

#if LLBC_TARGET_PLATFORM_WIN32

namespace
{
    typedef LLBC_NS LLBC_BasePoller Base;
}

__LLBC_NS_BEGIN

LLBC_IocpPoller::LLBC_IocpPoller()
: _iocp(LLBC_INVALID_IOCP_HANDLE)
, _monitor(nullptr)
{
}

LLBC_IocpPoller::~LLBC_IocpPoller()
{
    Stop();
}

int LLBC_IocpPoller::Start()
{
    if (GetTaskState() != LLBC_TaskState::NotActivated)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    if ((_iocp = LLBC_CreateIocp()) == LLBC_INVALID_IOCP_HANDLE)
        return LLBC_FAILED;

    if (StartupMonitor() != LLBC_OK)
    {
        LLBC_CloseIocp(_iocp);
        _iocp = LLBC_INVALID_IOCP_HANDLE;

        return LLBC_FAILED;
    }

    if (Activate() != LLBC_OK)
    {
        StopMonitor();

        LLBC_CloseIocp(_iocp);
        _iocp = LLBC_INVALID_IOCP_HANDLE;

        return LLBC_FAILED;
    }

    return LLBC_OK;
}

void LLBC_IocpPoller::Stop()
{
    if (!IsActivated() || _stopping)
        return;

    StopMonitor();

    LLBC_BasePoller::Stop();
}

void LLBC_IocpPoller::Svc()
{
    while (!_stopping)
        HandleQueuedEvents(20);
}

void LLBC_IocpPoller::Cleanup()
{
    LLBC_CloseIocp(_iocp);
    _iocp = LLBC_INVALID_IOCP_HANDLE;

    Base::Cleanup();
}

void LLBC_IocpPoller::HandleEv_AddSock(LLBC_PollerEvent &ev)
{
    Base::HandleEv_AddSock(ev);
}

void LLBC_IocpPoller::HandleEv_AsyncConn(LLBC_PollerEvent &ev)
{
    bool succeed = true;
    LLBC_String reason = "Success";
    do {
        LLBC_SocketHandle handle = LLBC_CreateTcpSocketEx();
        if (handle == LLBC_INVALID_SOCKET_HANDLE)
        {
            succeed = false;
            reason = LLBC_FormatLastError();
            break;
        }

        LLBC_Socket *socket = new LLBC_Socket(handle);

        socket->SetNonBlocking();
        socket->SetPollerType(LLBC_PollerType::IocpPoller);
        if (socket->AttachToIocp(_iocp) != LLBC_OK)
        {
            delete socket;

            succeed = false;
            reason = LLBC_FormatLastError();
            break;
        }

        LLBC_POverlapped ol = new LLBC_Overlapped;
        ol->opcode = LLBC_OverlappedOpcode::Connect;
        ol->sock = handle;
        if (socket->ConnectEx(ev.peerAddr, ol) != LLBC_OK &&
                LLBC_GetLastError() != LLBC_ERROR_PENDING)
        {
            delete ol;
            delete socket;

            succeed = false;
            reason = LLBC_FormatLastError();
            break;
        }

        socket->InsertOverlapped(ol);

        LLBC_AsyncConnInfo asyncInfo;
        asyncInfo.socket = socket;
        asyncInfo.peerAddr = ev.peerAddr;
        asyncInfo.sessionId = ev.sessionId;
        asyncInfo.sessionOpts = *ev.sessionOpts;

        _connecting.insert(std::make_pair(handle, asyncInfo));
    } while (false);

    if (!succeed)
        _svc->Push(LLBC_SvcEvUtil::
                BuildAsyncConnResultEv(ev.sessionId, succeed, reason, ev.peerAddr));

    LLBC_XDelete(ev.sessionOpts);
}

void LLBC_IocpPoller::HandleEv_Send(LLBC_PollerEvent &ev)
{
    Base::HandleEv_Send(ev);
}

void LLBC_IocpPoller::HandleEv_Close(LLBC_PollerEvent &ev)
{
    Base::HandleEv_Close(ev);
}

void LLBC_IocpPoller::HandleEv_Monitor(LLBC_PollerEvent &ev)
{
    // Wait return value.
    const int waitRet = *reinterpret_cast<int *>(ev.un.monitorEv);

    // Overlapped pointer.
    int off = sizeof(int);
    LLBC_POverlapped ol = *reinterpret_cast<LLBC_POverlapped *>(ev.un.monitorEv + off);
    // Error No.
    off += sizeof(LLBC_POverlapped);
    int errNo = *reinterpret_cast<int *>(ev.un.monitorEv + off);
    // Sub-Error No.
    off += sizeof(int);
    int subErrNo = *reinterpret_cast<int *>(ev.un.monitorEv + off);

    free(ev.un.monitorEv);

    if (HandleConnecting(waitRet, ol, errNo, subErrNo))
        return;

    _Sockets::iterator it  = _sockets.find(ol->sock);
    if (UNLIKELY(it == _sockets.end()))
    {
        if (ol->acceptSock != LLBC_INVALID_SOCKET_HANDLE)
            LLBC_CloseSocket(ol->acceptSock);
        if (ol->data)
            LLBC_Recycle(reinterpret_cast<LLBC_MessageBlock *>(ol->data));
        delete ol;

        return;
    }

    LLBC_Session *session = it->second;
    if (waitRet == LLBC_FAILED)
    {
        session->OnClose(ol, new LLBC_SessionCloseInfo(errNo, subErrNo));
    }
    else
    {
        if (session->IsListen())
            Accept(session, ol);
        else if (ol->opcode == LLBC_OverlappedOpcode::Send)
            session->OnSend(ol);
        else if (ol->opcode == LLBC_OverlappedOpcode::Receive)
            session->OnRecv(ol);
    }
}

void LLBC_IocpPoller::HandleEv_TakeOverSession(LLBC_PollerEvent &ev)
{
    Base::HandleEv_TakeOverSession(ev);
}

void LLBC_IocpPoller::AddSession(LLBC_Session *session, bool needAddToIocp)
{
    Base::AddSession(session, needAddToIocp);

    LLBC_Socket *sock = session->GetSocket();
    if (needAddToIocp)
        sock->AttachToIocp(_iocp);

    if (session->IsListen())
        sock->PostAsyncAccept();
    else
        sock->PostZeroWSARecv();
}

void LLBC_IocpPoller::RemoveSession(LLBC_Session *session)
{
    Base::RemoveSession(session);
}

int LLBC_IocpPoller::StartupMonitor()
{
    const LLBC_Delegate<void()> deleg(this, &LLBC_IocpPoller::MonitorSvc);
    _monitor = new LLBC_PollerMonitor(deleg);
    if (_monitor->Start() != LLBC_OK)
    {
        LLBC_XDelete(_monitor);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

void LLBC_IocpPoller::StopMonitor()
{
    LLBC_XDelete(_monitor);
}

void LLBC_IocpPoller::MonitorSvc()
{
    ulong bytesTrans;
    LLBC_POverlapped ol;
    LLBC_POverlappedGroup olGroup;

    int ret = LLBC_GetQueuedCompletionStatus(_iocp, 
                                             &bytesTrans, 
                                             (void **)&olGroup, 
                                             &ol, 
                                             20);

    int errNo = LLBC_ERROR_SUCCESS, subErrNo = 0;
    if (ret != LLBC_FAILED || LLBC_GetLastError() != LLBC_ERROR_TIMEOUTED)
    {
        if (ret != LLBC_OK)
        {
            errNo = LLBC_GetLastError();
            subErrNo = LLBC_GetSubErrorNo();
        }

        Push(LLBC_PollerEvUtil::BuildIocpMonitorEv(ret, ol, errNo, subErrNo));
    }
}

bool LLBC_IocpPoller::HandleConnecting(int waitRet, LLBC_POverlapped ol, int errNo, int subErrNo)
{
    if (ol->opcode != LLBC_OverlappedOpcode::Connect)
        return false;

    _Connecting::iterator it = _connecting.find(ol->sock);
    LLBC_AsyncConnInfo &asyncInfo = it->second;

    LLBC_Socket *sock = asyncInfo.socket;
    sock->DeleteOverlapped(ol);

    if (waitRet == LLBC_OK)
    {
        sock->SetOption(SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, nullptr, 0);
        SetConnectedSocketOpts(sock, asyncInfo.sessionOpts);

        _svc->Push(LLBC_SvcEvUtil::BuildAsyncConnResultEv(
            asyncInfo.sessionId, true, LLBC_StrError(LLBC_ERROR_SUCCESS), asyncInfo.peerAddr));

        AddSession(CreateSession(sock, asyncInfo.sessionId, asyncInfo.sessionOpts, nullptr), false);
    }
    else
    {
        _svc->Push(LLBC_SvcEvUtil::BuildAsyncConnResultEv(
                asyncInfo.sessionId, false, LLBC_StrErrorEx(errNo, subErrNo), asyncInfo.peerAddr));
        delete asyncInfo.socket;
    }

    _connecting.erase(it);
    return true;
}

void LLBC_IocpPoller::Accept(LLBC_Session *session, LLBC_POverlapped ol)
{
    // Create accepted socket and set some options.
    LLBC_Socket *sock = session->GetSocket();
    LLBC_Socket *newSock = new LLBC_Socket(ol->acceptSock);
    newSock->SetNonBlocking();
    newSock->SetOption(SOL_SOCKET,
                       SO_UPDATE_ACCEPT_CONTEXT,
                       &ol->sock,
                       sizeof(LLBC_SocketHandle));
    newSock->SetPollerType(LLBC_PollerType::IocpPoller);

    SetConnectedSocketOpts(newSock, session->GetSessionOpts());

    // Delete overlapped.
    ol->acceptSock = LLBC_INVALID_SOCKET_HANDLE;
    sock->DeleteOverlapped(ol);
    // Post new async-accept request.
    sock->PostAsyncAccept();

    // Create session and add to poller.
    AddToPoller(CreateSession(newSock, 0, session->GetSessionOpts(), session));
}

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_WIN32
