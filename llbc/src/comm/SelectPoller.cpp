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
#include "llbc/comm/SelectPoller.h"
#include "llbc/comm/Service.h"

namespace
{
    typedef LLBC_NS LLBC_BasePoller Base;
    typedef LLBC_NS LLBC_SocketHandle _Handle;
}

__LLBC_NS_BEGIN

LLBC_SelectPoller::LLBC_SelectPoller()
: _maxFd(0)
{
    LLBC_ZeroFdSet(&_reads);
    LLBC_ZeroFdSet(&_writes);
    LLBC_ZeroFdSet(&_excepts);
}

LLBC_SelectPoller::~LLBC_SelectPoller()
{
    Stop();
}

int LLBC_SelectPoller::Start()
{
    if (GetTaskState() != LLBC_TaskState::NotActivated)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    if (Activate() != LLBC_OK)
        return LLBC_FAILED;

    return LLBC_OK;
}

void LLBC_SelectPoller::Svc()
{
    static const int interval = 20;
    LLBC_FdSet reads, writes, excepts;

    while (!_stopping)
    {
        HandleQueuedEvents(0);
        if (_maxFd == 0)
        {
            LLBC_Sleep(interval);
            continue;
        }

        reads = _reads;
        writes = _writes;
        excepts = _excepts;
        const int evCount = LLBC_Select(
            static_cast<int>(_maxFd + 1),&reads, &writes, &excepts, interval);

        const sint64 begin = LLBC_GetMilliSeconds();
        const int processed = HandleConnecting(writes, excepts);

        if (processed < evCount)
        {
#if LLBC_TARGET_PLATFORM_WIN32
            // In WIN32 platform, the fd_set use {int fd_array[]; int count} way to implement.
            for (uint32 i = 0; i < excepts.fd_count; ++i)
            {
                LLBC_Session *session = 
                    _sockets.find(excepts.fd_array[i])->second;
                LLBC_Socket *sock = session->GetSocket();

                int sockErr;
                LLBC_SessionCloseInfo *closeInfo;
                if (sock->GetPendingError(sockErr) != LLBC_OK)
                    closeInfo = new LLBC_SessionCloseInfo;
                else
                    closeInfo = new LLBC_SessionCloseInfo(LLBC_ERROR_NETAPI, sockErr);

                session->OnClose(nullptr, closeInfo);
            }

            for (uint32 i = 0; i < reads.fd_count; ++i)
            {
                LLBC_Session *session = 
                    _sockets.find(reads.fd_array[i])->second;
                if (session->GetSocket()->IsListen())
                    Accept(session);
                else
                    session->OnRecv();
            }

            for (uint32 i = 0; i < writes.fd_count; ++i)
            {
                _Sockets::iterator it = _sockets.find(writes.fd_array[i]);
                if (it != _sockets.end())
                    it->second->OnSend();
            }
#else // Non-WIN32
            // In Non-WIN32 platform, the fd_set use bit-set way to implement.
            // So, we must use the looks like stupid method to lookup.
            _Sockets sockets = _sockets;
            for (_Sockets::iterator it = sockets.begin();
                    it != sockets.end();
                    ++it)
            {
                LLBC_Session *session = it->second;
                const LLBC_SocketHandle handle = it->first;
                if (LLBC_FdIsSet(handle, &excepts))
                {
                    LLBC_Socket *sock = session->GetSocket();

                    int sockErr;
                    LLBC_SessionCloseInfo *closeInfo;
                    if (UNLIKELY(sock->GetPendingError(sockErr) != LLBC_OK))
                        closeInfo = new LLBC_SessionCloseInfo;
                    else
                        closeInfo = new LLBC_SessionCloseInfo(LLBC_ERROR_CLIB, sockErr);

                    session->OnClose(closeInfo);
                }
                else if (LLBC_FdIsSet(handle, &reads))
                {
                    if (session->GetSocket()->IsListen())
                        Accept(session);
                    else
                        session->OnRecv();
                }
                else if (LLBC_FdIsSet(handle, &writes))
                {
                    session->OnSend();
                }
            }
#endif // LLBC_TARGET_PLATFORM_WIN32
        }

        const sint64 elapsed = LLBC_GetMilliSeconds() - begin;
        if (UNLIKELY(elapsed < 0))
            continue;
        else if (elapsed < interval)
            LLBC_Sleep(static_cast<int>(interval - elapsed));
    }
}

void LLBC_SelectPoller::Cleanup()
{
    _maxFd = 0;

    LLBC_ZeroFdSet(&_reads);
    LLBC_ZeroFdSet(&_writes);
    LLBC_ZeroFdSet(&_excepts);

    Base::Cleanup();
}

void LLBC_SelectPoller::HandleEv_AddSock(LLBC_PollerEvent &ev)
{
    Base::HandleEv_AddSock(ev);
}

void LLBC_SelectPoller::HandleEv_AsyncConn(LLBC_PollerEvent &ev)
{
    LLBC_Socket *socket = new LLBC_Socket;
    socket->SetNonBlocking();
    socket->SetPollerType(LLBC_PollerType::SelectPoller);

    const LLBC_SocketHandle handle = socket->Handle();
    if (socket->Connect(ev.peerAddr) == LLBC_OK)
    {
        _svc->Push(LLBC_SvcEvUtil::
                BuildAsyncConnResultEv(ev.sessionId, true, "Success", ev.peerAddr));

        SetConnectedSocketOpts(socket, *ev.sessionOpts);
        AddSession(CreateSession(socket, ev.sessionId, *ev.sessionOpts, nullptr));

        LLBC_XDelete(ev.sessionOpts);
    }
    else if (LLBC_GetLastError() == LLBC_ERROR_WBLOCK)
    {
        LLBC_AsyncConnInfo conn;
        conn.socket = socket;
        conn.peerAddr = ev.peerAddr;
        conn.sessionId = ev.sessionId;
        conn.sessionOpts = *ev.sessionOpts;

        _connecting.insert(std::make_pair(handle, conn));

        LLBC_SetFd(handle, &_writes);
        LLBC_SetFd(handle, &_excepts);
        UpdateMaxFd();

        LLBC_XDelete(ev.sessionOpts);
    }
    else
    {
        delete socket;
        LLBC_XDelete(ev.sessionOpts);

        _svc->Push(LLBC_SvcEvUtil::
                BuildAsyncConnResultEv(ev.sessionId, false, LLBC_FormatLastError(), ev.peerAddr));
    }
}

void LLBC_SelectPoller::HandleEv_Send(LLBC_PollerEvent &ev)
{
    Base::HandleEv_Send(ev);
}

void LLBC_SelectPoller::HandleEv_Close(LLBC_PollerEvent &ev)
{
    Base::HandleEv_Close(ev);
}

void LLBC_SelectPoller::HandleEv_Monitor(LLBC_PollerEvent &ev)
{
    ASSERT(false && "Select Poller could not process Monitor Event");
}

void LLBC_SelectPoller::HandleEv_TakeOverSession(LLBC_PollerEvent &ev)
{
    Base::HandleEv_TakeOverSession(ev);
}

#if LLBC_TARGET_PLATFORM_NON_WIN32
void LLBC_SelectPoller::AddSession(LLBC_Session *session)
#else
void LLBC_SelectPoller::AddSession(LLBC_Session *session, bool needAddToIocp)
#endif
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    Base::AddSession(session);
#else
    Base::AddSession(session, needAddToIocp);
#endif

    const _Handle handle = session->GetSocketHandle();
    LLBC_SetFd(handle, &_reads);
    LLBC_SetFd(handle, &_writes);
    LLBC_SetFd(handle, &_excepts);

    UpdateMaxFd();
}

void LLBC_SelectPoller::RemoveSession(LLBC_Session *session)
{
    const _Handle handle = session->GetSocketHandle();
    LLBC_ClrFd(handle, &_reads);
    LLBC_ClrFd(handle, &_writes);
    LLBC_ClrFd(handle, &_excepts);

    Base::RemoveSession(session);
    
    UpdateMaxFd();
}

void LLBC_SelectPoller::UpdateMaxFd()
{
    _maxFd = 0;
    if (!_sockets.empty())
        _maxFd = _sockets.rbegin()->first;
    if (!_connecting.empty())
        _maxFd = MAX(_maxFd, _connecting.rbegin()->first);
}

void LLBC_SelectPoller::Accept(LLBC_Session *session)
{
    LLBC_Socket *newSocket = session->GetSocket()->Accept();
    if (LIKELY(newSocket))
    {
        newSocket->SetNonBlocking();

        SetConnectedSocketOpts(newSocket, session->GetSessionOpts());
        AddToPoller(CreateSession(newSocket, 0, session->GetSessionOpts(), session));
    }
}

int LLBC_SelectPoller::HandleConnecting(LLBC_FdSet &writes, LLBC_FdSet &excepts)
{
    int processed = 0;
    for (_Connecting::iterator it = _connecting.begin();
         it != _connecting.end();
         )
    {
        // Check event triggered in writeable-set or excepts-set
        const LLBC_SocketHandle handle = it->first;
        bool inExceptSet = false;
        const bool inWriteSet = !!LLBC_FdIsSet(handle, &writes);
        if (!inWriteSet)
            inExceptSet = !!LLBC_FdIsSet(handle, &excepts);

        // Not triggered, continue.
        if (!inWriteSet && !inExceptSet)
        {
            ++it;
            continue;
        }

        // Clear _writes & _excepts data members.
        LLBC_ClrFd(handle, &_writes);
        LLBC_ClrFd(handle, &_excepts);
        // Clear local sets.
        if (inWriteSet)
            LLBC_ClrFd(handle, &writes);
        else
            LLBC_ClrFd(handle, &excepts);

        // Process event result.
        LLBC_AsyncConnInfo &asyncInfo = it->second;
        LLBC_Socket *socket = asyncInfo.socket;

        processed += 1;

        int sockErr;
        const char *reason = nullptr;
        bool connected = inWriteSet;
        if (UNLIKELY(socket->GetPendingError(sockErr) != LLBC_OK))
        {
            connected = false;
            reason = LLBC_FormatLastError();
        }
        else
        {
#if LLBC_DEBUG
            if (inExceptSet)
            {
                ASSERT(sockErr != 0 && "llbc library internal error, in SelectPoller::HandleConnecting()!");
            }
#endif // LLBC_DEBUG

            if (sockErr != 0)
            {
                connected = false;
#if LLBC_TARGET_PLATFORM_NON_WIN32
                reason = LLBC_StrErrorEx(LLBC_ERROR_CLIB, sockErr);
#else // Win32
                reason = LLBC_StrErrorEx(LLBC_ERROR_NETAPI, sockErr);
#endif
            }
            else
            {
                reason = LLBC_StrError(LLBC_ERROR_SUCCESS);
            }
        }

        // Build async connect event and push it to service.
        _svc->Push(LLBC_SvcEvUtil::BuildAsyncConnResultEv(asyncInfo.sessionId, connected, reason, asyncInfo.peerAddr));

        if (connected)
        {
            SetConnectedSocketOpts(socket, asyncInfo.sessionOpts);
            AddSession(CreateSession(socket, asyncInfo.sessionId, asyncInfo.sessionOpts, nullptr));
        }
        else
        {
            delete socket;
        }

        // Erase socket from connecting map.
        _connecting.erase(it++);
        // Update max Fd.
        if (!connected)
            UpdateMaxFd();
    }

    return processed;
}

__LLBC_NS_END
