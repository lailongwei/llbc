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
#include "llbc/comm/Session.h"
#include "llbc/comm/ServiceEvent.h"
#include "llbc/comm/PollerType.h"
#include "llbc/comm/BasePoller.h"
#include "llbc/comm/SelectPoller.h"
#if LLBC_TARGET_PLATFORM_WIN32
 #include "llbc/comm/IocpPoller.h"
#endif // Win32
#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
 #include "llbc/comm/EpollPoller.h"
#endif // Linux or Android
#include "llbc/comm/PollerMgr.h"
#include "llbc/comm/Service.h"

namespace
{
    typedef LLBC_NS LLBC_BasePoller This;
}

__LLBC_NS_BEGIN

This::_Handler This::_handlers[LLBC_PollerEvent::End] =
{
    &This::HandleEv_AddSock,
    &This::HandleEv_AsyncConn,
    &This::HandleEv_Send,
    &This::HandleEv_Close,
    &This::HandleEv_Monitor,
    &This::HandleEv_TakeOverSession,
    &This::HandleEv_CtrlProtocolStack
};

LLBC_BasePoller::LLBC_BasePoller()
: _stopping(false)

, _id(-1)
, _brotherCount(0)
, _svc(nullptr)
, _pollerMgr(nullptr)

, _sockets()
, _sessions()

, _connecting()
{
}

LLBC_BasePoller::~LLBC_BasePoller()
{
}

This *LLBC_BasePoller::Create(int type)
{
    This *poller = nullptr;
    switch (type)
    {
    case LLBC_PollerType::SelectPoller:
        poller = new LLBC_SelectPoller;
        break;

#if LLBC_TARGET_PLATFORM_WIN32
    case LLBC_PollerType::IocpPoller:
        poller = new LLBC_IocpPoller;
        break;
#endif

#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
    case LLBC_PollerType::EpollPoller:
        poller = new LLBC_EpollPoller;
        break;
#endif

    default:
        break;
    }

    if (!poller)
        LLBC_SetLastError(LLBC_ERROR_INVALID);

    return poller;
}

void LLBC_BasePoller::SetPollerId(int id)
{
    _id = id;
}

void LLBC_BasePoller::SetBrothersCount(int count)
{
    _brotherCount = count;
}

void LLBC_BasePoller::SetService(LLBC_Service *svc)
{
    _svc = svc;
}

void LLBC_BasePoller::SetPollerMgr(LLBC_PollerMgr *mgr)
{
    _pollerMgr = mgr;
}

int LLBC_BasePoller::Start()
{
    ASSERT(false && "Please implement LLBC_BasePoller::Start() method!");
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
}

void LLBC_BasePoller::Stop()
{
    if (!IsActivated() || _stopping)
        return;

    _stopping = true;
    Wait();
}

void LLBC_BasePoller::Cleanup()
{
    // Notify poller manager I'm stop.
    _pollerMgr->OnPollerStop(_id);

    // Cleanup all queued events.
    LLBC_PollerEvent ev;
    LLBC_MessageBlock *block;
    while (TryPop(block) == LLBC_OK)
    {
        block->Read(&ev, sizeof(LLBC_PollerEvent));
        LLBC_PollerEvUtil::DestroyEv(ev);

        delete block;
    }

    // Delete all sessions.
#if LLBC_TARGET_PLATFORM_WIN32
    for (_Sessions::iterator it = _sessions.begin();
         it != _sessions.end();
         ++it)
        it->second->GetSocket()->DeleteAllOverlappeds();
#endif // LLBC_TARGET_PLATFORM_WIN32
    LLBC_STLHelper::DeleteContainer(_sessions);
    _sockets.clear();

    // Delete all connecting sockets.
    for (_Connecting::iterator it = _connecting.begin();
         it != _connecting.end();
         ++it)
        delete it->second.socket;
    _connecting.clear();

    // Reset stopping flag.
    _stopping = false;
}

void LLBC_BasePoller::HandleQueuedEvents(int waitTime)
{
    LLBC_MessageBlock *block;
    while (TimedPop(block, waitTime) == LLBC_OK)
    {
        LLBC_PollerEvent &ev = 
            *reinterpret_cast< LLBC_PollerEvent *>(block->GetData());

        (this->*_handlers[ev.type])(ev);

        delete block;
    }
}

void LLBC_BasePoller::HandleEv_AddSock(LLBC_PollerEvent &ev)
{
    AddSession(CreateSession(ev.un.socket,
                             ev.sessionId,
                             *ev.sessionOpts,
                             nullptr));

    LLBC_XDelete(ev.sessionOpts);
}

void LLBC_BasePoller::HandleEv_AsyncConn(LLBC_PollerEvent &ev)
{
    ASSERT(false && "Please implement LLBC_Base_Poller::HandleEv_AsyncConn() method!");
}

void LLBC_BasePoller::HandleEv_Send(LLBC_PollerEvent &ev)
{
    _Sessions::iterator it = _sessions.find(ev.un.packet->GetSessionId());
    if (it == _sessions.end())
    {
        LLBC_Recycle(ev.un.packet);
        return;
    }

    LLBC_Session *session = it->second;
    if (UNLIKELY(session->IsListen()))
        LLBC_Recycle(ev.un.packet);
    else if (UNLIKELY(session->Send(ev.un.packet) != LLBC_OK))
        session->OnClose();
}

void LLBC_BasePoller::HandleEv_Close(LLBC_PollerEvent &ev)
{
    _Sessions::iterator it = _sessions.find(ev.sessionId);
    if (it == _sessions.end())
    {
        LLBC_XFree(ev.un.closeReason);
        return;
    }

    LLBC_SessionCloseInfo *closeInfo = 
        new LLBC_SessionCloseInfo(ev.un.closeReason);
    LLBC_XFree(ev.un.closeReason);

    LLBC_Session *session = it->second;
#if LLBC_TARGET_PLATFORM_NON_WIN32
    session->OnClose(closeInfo);
#else
    session->OnClose(nullptr, closeInfo);
#endif
}

void LLBC_BasePoller::HandleEv_Monitor(LLBC_PollerEvent &ev)
{
    ASSERT(false && "Please implement LLBC_BasePoller::HandleEv_Monitor() method!");
}

void LLBC_BasePoller::HandleEv_TakeOverSession(LLBC_PollerEvent &ev)
{
    AddSession(ev.un.session);
}

void LLBC_BasePoller::HandleEv_CtrlProtocolStack(LLBC_PollerEvent &ev)
{
    // Get session.
    _Sessions::iterator it = _sessions.find(ev.sessionId);
    if (it == _sessions.end())
    {
        delete ev.un.protocolStackCtrlInfo.ctrlData;
        ev.un.protocolStackCtrlInfo.ctrlData = nullptr;

        return;
    }

    // Do protocol stack control.
    bool removeSession = false;
    LLBC_Session *&session = it->second;
    session->CtrlProtocolStack(ev.un.protocolStackCtrlInfo.ctrlCmd,
                               *ev.un.protocolStackCtrlInfo.ctrlData,
                               removeSession);

    // Delete ctrl data.
    delete ev.un.protocolStackCtrlInfo.ctrlData;
    ev.un.protocolStackCtrlInfo.ctrlData = nullptr;

    // Remove session, if specified(Error number must be set when business logic determine remove this session).
    if (removeSession)
    {
        session->OnClose();
        return;
    }
}

LLBC_Session *LLBC_BasePoller::CreateSession(LLBC_Socket *socket,
                                             int sessionId,
                                             const LLBC_SessionOpts &sessionOpts,
                                             LLBC_Session *acceptSession)
{
    if (sessionId == 0)
        sessionId = _pollerMgr->AllocSessionId();

    LLBC_Session *session = new LLBC_Session(sessionOpts);
    session->SetId(sessionId);
    session->SetSocket(socket);
    socket->SetSession(session);
    if (acceptSession)
        session->SetAcceptId(acceptSession->GetId());

    session->SetService(_svc);

    return session;
}

void LLBC_BasePoller::AddToPoller(LLBC_Session *session)
{
    const int hash = session->GetId() % _brotherCount;

    if (hash == _id)
    {
        AddSession(session);
    }
    else
    {
        LLBC_MessageBlock *ev = 
            LLBC_PollerEvUtil::BuildTakeOverSessionEv(session);
        if (_pollerMgr->PushMsgToPoller(hash, ev) != LLBC_OK)
        {
            trace("LLBC_BasePoller::AddToPoller() could not found poller, hash val: %d\n", hash);
            LLBC_PollerEvUtil::DestroyEv(ev);
            return;
        }
    }
}

#if LLBC_TARGET_PLATFORM_NON_WIN32
void LLBC_BasePoller::AddSession(LLBC_Session *session)
#else
void LLBC_BasePoller::AddSession(LLBC_Session *session, bool needAddToIocp)
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
{
    // Insert to socket & session map.
    session->SetPoller(this);
    _sessions.insert(std::make_pair(session->GetId(), session));
    _sockets.insert(std::make_pair(session->GetSocketHandle(), session));

    // Pre-Add Service-Level session info to makesure protocol stack's 
    // Service::Send()/Multicast()/Broadcast() methods call successfully.
    LLBC_Socket *sock = session->GetSocket();
    _svc->AddReadySession(session->GetId(),
                          session->GetAcceptId(),
                          sock->IsListen(),
                          true);

    // Build session-create event and push to service.
    LLBC_MessageBlock *block = LLBC_SvcEvUtil::BuildSessionCreateEv(sock->GetLocalAddress(),
                                                                    sock->GetPeerAddress(),
                                                                    sock->IsListen(),
                                                                    session->GetId(),
                                                                    session->GetAcceptId(),
                                                                    sock->Handle());

    _svc->Push(block);
}

void LLBC_BasePoller::RemoveSession(LLBC_Session *session)
{
    _sessions.erase(session->GetId());
    _sockets.erase(session->GetSocketHandle());
    delete session;
}

void LLBC_BasePoller::SetConnectedSocketOpts(LLBC_Socket *sock, const LLBC_SessionOpts &sessionOpts)
{
    sock->UpdateLocalAddress();
    sock->UpdatePeerAddress();
    sock->SetMaxPacketSize(sessionOpts.GetMaxPacketSize());

    if (sessionOpts.GetSockSendBufSize() != 0)
        sock->SetSendBufSize(sessionOpts.GetSockSendBufSize());
    if (sessionOpts.GetSockRecvBufSize() != 0)
        sock->SetRecvBufSize(sessionOpts.GetSockRecvBufSize());

    if (!sock->IsListen())
        sock->SetNoDelay(sessionOpts.IsNoDelay());
}

__LLBC_NS_END
