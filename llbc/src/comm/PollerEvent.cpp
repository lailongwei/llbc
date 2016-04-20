/**
 * @file    PollerEvent.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/13
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/Packet.h"
#include "llbc/comm/Socket.h"
#include "llbc/comm/Session.h"
#include "llbc/comm/PollerEvent.h"

namespace
{
    typedef LLBC_NS LLBC_PollerEvent _Ev;
    typedef LLBC_NS LLBC_MessageBlock _Block;

    typedef LLBC_NS LLBC_PollerEvUtil This;
}

__LLBC_NS_BEGIN

LLBC_MessageBlock *LLBC_PollerEvUtil::BuildAddSockEv(int sessionId, LLBC_Socket *sock)
{
    _Block *block = LLBC_New1(_Block, sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    ev.type = _Ev::AddSock;
    ev.un.socket = sock;
    ev.sessionId = sessionId;

    block->SetWritePos(sizeof(_Ev));
    return block;
}

LLBC_MessageBlock *LLBC_PollerEvUtil::BuildAsyncConnEv(int sessionId, const LLBC_SockAddr_IN &peerAddr)
{
    _Block *block = LLBC_New1(_Block, sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    ev.type = _Ev::AsyncConn;
    ev.sessionId = sessionId;
    ev.peerAddr = peerAddr;

    block->SetWritePos(sizeof(_Ev));
    return block;
}

LLBC_MessageBlock *LLBC_PollerEvUtil::BuildSendEv(LLBC_Packet *packet)
{
    _Block *block = LLBC_New1(_Block, sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    ev.type = _Ev::Send;
    ev.un.packet = packet;

    block->SetWritePos(sizeof(_Ev));
    return block;
}

LLBC_MessageBlock *LLBC_PollerEvUtil::BuildCloseEv(int sessionId, const char *reason)
{
    _Block *block = LLBC_New1(_Block, sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    ev.type = _Ev::Close;
    ev.sessionId = sessionId;

    if (reason != NULL)
    {
        ev.un.closeReason = LLBC_Malloc(char, LLBC_StrLenA(reason) + 1);
        LLBC_StrCpyA(ev.un.closeReason, reason);
    }
    else
    {
        ev.un.closeReason = NULL;
    }

    block->SetWritePos(sizeof(_Ev));
    return block;
}

#if LLBC_TARGET_PLATFORM_WIN32
LLBC_MessageBlock *LLBC_PollerEvUtil::BuildIocpMonitorEv(int ret, 
                                                         LLBC_POverlapped ol, 
                                                         int errNo, 
                                                         int subErrNo)
{
    _Block *block = LLBC_New1(_Block, sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    ev.type = _Ev::Monitor;
    ev.un.monitorEv = LLBC_Malloc(char, sizeof(int) + sizeof(LLBC_POverlapped) + sizeof(int) * 2);

    size_t off = 0;
    // Wait return value.
    ::memcpy(ev.un.monitorEv, &ret, sizeof(int)), off += sizeof(int);
    // Overlapped data.
    ::memcpy(ev.un.monitorEv + off, &ol, sizeof(LLBC_POverlapped)), off += sizeof(LLBC_POverlapped);
    // Error no.
    ::memcpy(ev.un.monitorEv + off, &errNo, sizeof(int)), off += sizeof(int);
    // Sub error no.
    ::memcpy(ev.un.monitorEv + off, &subErrNo, sizeof(int));

    block->SetWritePos(sizeof(_Ev));
    return block;
}
#endif // LLBC_TARGET_PLATFORM_WIN32

#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
LLBC_MessageBlock *LLBC_PollerEvUtil::BuildEpollMonitorEv(const LLBC_EpollEvent *evs, int count)
{
    _Block *block = LLBC_New1(_Block, sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    ev.type = _Ev::Monitor;
    ev.un.monitorEv = LLBC_Malloc(char, sizeof(int) + sizeof(LLBC_EpollEvent) * count);

    // Write count.
    ::memcpy(ev.un.monitorEv, &count, sizeof(int));
    // Write events.
    ::memcpy(ev.un.monitorEv + sizeof(int), evs, sizeof(LLBC_EpollEvent) * count);

    block->SetWritePos(sizeof(_Ev));
    return block;
}
#endif // LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID

LLBC_MessageBlock *LLBC_PollerEvUtil::BuildTakeOverSessionEv(LLBC_Session *session)
{
    _Block *block = LLBC_New1(_Block, sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    ev.type = _Ev::TakeOverSession;
    ev.un.session = session;

    block->SetWritePos(sizeof(_Ev));
    return block;
}

void LLBC_PollerEvUtil::DestroyEv(LLBC_PollerEvent &ev)
{
    switch (ev.type)
    {
    case _Ev::AddSock:
        LLBC_Delete(ev.un.socket);
        break;

    case _Ev::Send:
        LLBC_Delete(ev.un.packet);
        break;

	case _Ev::Close:
		LLBC_XFree(ev.un.closeReason);
		break;

    case _Ev::Monitor:
        LLBC_XFree(ev.un.monitorEv);
        break;

    case _Ev::TakeOverSession:
        LLBC_Delete(ev.un.session);
        break;

    default:
        break;
    }
}

void LLBC_PollerEvUtil::DestroyEv(LLBC_MessageBlock *block)
{
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    This::DestroyEv(ev);

    LLBC_Delete(block);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
