/**
 * @file    IocpPoller.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/14
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
#include "llbc/comm/IocpPoller.h"
#include "llbc/comm/PollerMonitor.h"
#include "llbc/comm/IService.h"

#if LLBC_TARGET_PLATFORM_WIN32

namespace
{
    typedef LLBC_NS LLBC_BasePoller Base;

    static void DestroyPollerEv(void *data)
    {
        LLBC_NS LLBC_PollerEvent *ev = 
            reinterpret_cast<LLBC_NS LLBC_PollerEvent *>(data);
        LLBC_NS LLBC_PollerEvUtil::DestroyEv(*ev);
    }
}

__LLBC_NS_BEGIN

LLBC_IocpPoller::LLBC_IocpPoller()
: _iocp(LLBC_INVALID_IOCP_HANDLE)
, _monitor(NULL)
{
}

LLBC_IocpPoller::~LLBC_IocpPoller()
{
    Stop();
}

int LLBC_IocpPoller::Start()
{
    if (_started)
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

    _started = true;
    return LLBC_OK;
}

void LLBC_IocpPoller::Svc()
{
    while (!_started)
        LLBC_Sleep(20);

    while (!_stopping)
        HandleQueuedEvents(20);
}

void LLBC_IocpPoller::Cleanup()
{
    StopMonitor();

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

        LLBC_Socket *socket = LLBC_New1(LLBC_Socket, handle);

        socket->SetNonBlocking();
        socket->SetPollerType(LLBC_PollerType::IocpPoller);
        if (socket->AttachToIocp(_iocp) != LLBC_OK)
        {
            LLBC_Delete(socket);

            succeed = false;
            reason = LLBC_FormatLastError();
            break;
        }

        LLBC_POverlapped ol = LLBC_New(LLBC_Overlapped);
        ol->opcode = LLBC_OverlappedOpcode::Connect;
        ol->sock = handle;
        if (socket->ConnectEx(ev.peerAddr, ol) != LLBC_OK &&
                LLBC_GetLastError() != LLBC_ERROR_PENDING)
        {
            LLBC_Delete(ol);
            LLBC_Delete(socket);

            succeed = false;
            reason = LLBC_FormatLastError();
            break;
        }

        socket->InsertOverlapped(ol);

        LLBC_AsyncConnInfo asyncInfo;
        asyncInfo.socket = socket;
        asyncInfo.peerAddr = ev.peerAddr;
        asyncInfo.sessionId = ev.sessionId;

        _connecting.insert(std::make_pair(handle, asyncInfo));
    } while (false);

    if (!succeed)
        _svc->Push(LLBC_SvcEvUtil::
                BuildAsyncConnResultEv(ev.sessionId, succeed, reason, ev.peerAddr));
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

    LLBC_Free(ev.un.monitorEv);

    if (HandleConnecting(waitRet, ol, errNo, subErrNo))
        return;

    _Sockets::iterator it  = _sockets.find(ol->sock);
    if (UNLIKELY(it == _sockets.end()))
    {
        if (ol->acceptSock != LLBC_INVALID_SOCKET_HANDLE)
            LLBC_CloseSocket(ol->acceptSock);
        if (ol->data)
            LLBC_Delete(reinterpret_cast<LLBC_MessageBlock *>(ol->data));
        LLBC_Delete(ol);

        return;
    }

    LLBC_Session *session = it->second;
    if (waitRet == LLBC_FAILED)
    {
        session->OnClose(ol, LLBC_New2(LLBC_SessionCloseInfo, errNo, subErrNo));
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
    LLBC_IDelegate0<void> *deleg = new LLBC_Delegate0<void,
        LLBC_IocpPoller>(this, &LLBC_IocpPoller::MonitorSvc);

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
    if (ret != LLBC_FAILED || LLBC_GetLastError() != LLBC_ERROR_TIMEOUT)
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
        sock->SetOption(SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
        SetConnectedSocketDftOpts(sock);

        _svc->Push(LLBC_SvcEvUtil::BuildAsyncConnResultEv(
            asyncInfo.sessionId, true, LLBC_StrError(LLBC_ERROR_SUCCESS), asyncInfo.peerAddr));

        AddSession(CreateSession(sock, asyncInfo.sessionId), false);
    }
    else
    {
        _svc->Push(LLBC_SvcEvUtil::BuildAsyncConnResultEv(
                asyncInfo.sessionId, false, LLBC_StrErrorEx(errNo, subErrNo), asyncInfo.peerAddr));
        LLBC_Delete(asyncInfo.socket);
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

    SetConnectedSocketDftOpts(newSock);

    // Delete overlapped.
    ol->acceptSock = LLBC_INVALID_SOCKET_HANDLE;
    sock->DeleteOverlapped(ol);
    // Post new async-accept request.
    sock->PostAsyncAccept();

    // Create session and add to poller.
    AddToPoller(CreateSession(newSock, 0, session));
}

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_WIN32

#include "llbc/common/AfterIncl.h"
