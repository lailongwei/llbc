/**
 * @file    SelectPoller.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/13
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
#include "llbc/comm/SelectPoller.h"
#include "llbc/comm/IService.h"

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
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    if (Activate() != LLBC_OK)
        return LLBC_FAILED;

    _started = true;
    return LLBC_OK;
}

void LLBC_SelectPoller::Svc()
{
    while (!_started)
        LLBC_Sleep(20);

    static const int interval = 20;
    LLBC_FdSet reads, writes, excepts;

    while (!_stopping)
    {
        HandleQueuedEvents(0);
        if (_maxFd == 0)
        {
            LLBC_ThreadManager::Sleep(interval);
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
            for (uint32 i = 0; i < excepts.fd_count; i++)
                _sockets.find(excepts.fd_array[i])->second->OnClose();

            for (uint32 i = 0; i < reads.fd_count; i++)
            {
                LLBC_Session *session = 
                    _sockets.find(reads.fd_array[i])->second;
                if (session->GetSocket()->IsListen())
                    Accept(session);
                else
                    session->OnRecv();
            }

            for (uint32 i = 0; i < writes.fd_count; i++)
            {
                _Sockets::iterator it = _sockets.find(writes.fd_array[i]);
                if (it != _sockets.end())
                    it->second->OnSend();
            }
#else // Non-WIN32
            // In Non-WIN32 platform, the fd_set use bit-set way to implement.
            // So, we must use the looks like stupid method to lookup.
            _Sockets sockets = _sockets;
            for (_Sockets::iterator it = _sockets.begin();
                    it != _sockets.end();
                    it++)
            {
                const LLBC_SocketHandle handle = it->first;
                if (LLBC_FdIsSet(handle, &excepts))
                {
                    _sockets.find(handle)->second->OnClose();
                }
                else if (LLBC_FdIsSet(handle, &reads))
                {
                    LLBC_Session *session = _sockets.find(handle)->second;
                    if (session->GetSocket()->IsListen())
                        Accept(session);
                    else
                        session->OnRecv();
                }
                else if (LLBC_FdIsSet(handle, &writes))
                {
                    _Sessions::iterator sit = _sockets.find(handle);
                    if (it != _sockets.end())
                        it->second->OnSend();
                }
            }
#endif // LLBC_TARGET_PLATFORM_WIN32
        }

        const sint64 elapsed = LLBC_GetMilliSeconds() - begin;
        if (UNLIKELY(elapsed < 0))
            continue;
        else if (elapsed < interval)
            LLBC_ThreadManager::Sleep(static_cast<int>(interval - elapsed));
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
    LLBC_Socket *socket = LLBC_New(LLBC_Socket);
    socket->SetNonBlocking();
    socket->SetPollerType(LLBC_PollerType::SelectPoller);

    const LLBC_SocketHandle handle = socket->Handle();
    if (socket->Connect(ev.peerAddr) == LLBC_OK)
    {
        _svc->Push(LLBC_SvcEvUtil::
                BuildAsyncConnResultEv(true, "Success", ev.peerAddr));
        AddSession(CreateSession(socket, ev.sessionId));
    }
    else if (LLBC_GetLastError() == LLBC_ERROR_WBLOCK)
    {
        LLBC_AsyncConnInfo conn;
        conn.socket = socket;
        conn.peerAddr = ev.peerAddr;
        conn.sessionId = ev.sessionId;

        _connecting.insert(std::make_pair(handle, conn));

        LLBC_SetFd(handle, &_writes);
        LLBC_SetFd(handle, &_excepts);

        UpdateMaxFd();
    }
    else
    {
        LLBC_Delete(socket);
        _svc->Push(LLBC_SvcEvUtil::
                BuildAsyncConnResultEv(false, LLBC_FormatLastError(), ev.peerAddr));
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

        SetConnectedSocketDftOpts(newSocket);
        AddToPoller(CreateSession(newSocket));
    }
}

int LLBC_SelectPoller::HandleConnecting(LLBC_FdSet &writes, LLBC_FdSet &excepts)
{
    int processed = 0;
    for (_Connecting::iterator it = _connecting.begin();
         it != _connecting.end();
         )
    {
        const LLBC_SocketHandle handle = it->first;
        LLBC_AsyncConnInfo &asyncInfo = it->second;
        LLBC_Socket *socket = asyncInfo.socket;

        bool connected = true;
        if (LLBC_FdIsSet(handle, &writes))
        {
            processed += 1;
            LLBC_ClrFd(handle, &writes);
            LLBC_ClrFd(handle, &_writes);
            LLBC_ClrFd(handle, &_excepts);

            int optval = 0;
            LLBC_SocketLen optlen = sizeof(int);
            if (socket->GetOption(SOL_SOCKET,
                                  SO_ERROR,
                                  &optval,
                                  &optlen) != LLBC_OK || optval != 0)
                connected = false;
        }
        else if (LLBC_FdIsSet(it->first, &excepts))
        {
            processed += 1;
            LLBC_ClrFd(handle, &excepts);
            LLBC_ClrFd(handle, &_writes);
            LLBC_ClrFd(handle, &_excepts);

            connected = false;
        }
        else
        {
            it++;
            continue;
        }

        // Build async connect event and push it to service.
        _svc->Push(LLBC_SvcEvUtil::BuildAsyncConnResultEv(connected, 
                connected ? "Success" : LLBC_FormatLastError(), asyncInfo.peerAddr));

        if (connected)
        {
            SetConnectedSocketDftOpts(socket);
            AddSession(CreateSession(socket, asyncInfo.sessionId));
        }
        else
        {
            LLBC_Delete(socket);
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

#include "llbc/common/AfterIncl.h"
