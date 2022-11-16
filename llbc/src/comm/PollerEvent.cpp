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
#include "llbc/comm/PollerEvent.h"

namespace
{
    typedef LLBC_NS LLBC_PollerEvent _Ev;
    typedef LLBC_NS LLBC_MessageBlock _Block;

    typedef LLBC_NS LLBC_PollerEvUtil This;
}

__LLBC_NS_BEGIN

LLBC_MessageBlock *LLBC_PollerEvUtil::BuildAddSockEv(LLBC_Socket *sock,
                                                     int sessionId,
                                                     const LLBC_SessionOpts &sessionOpts)
{
    _Block *block = new _Block(sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    ev.type = _Ev::AddSock;
    ev.un.socket = sock;
    ev.sessionId = sessionId;
    ev.sessionOpts = new LLBC_SessionOpts(sessionOpts);

    block->SetWritePos(sizeof(_Ev));
    return block;
}

LLBC_MessageBlock *LLBC_PollerEvUtil::BuildAsyncConnEv(int sessionId,
                                                       const LLBC_SessionOpts &sessionOpts,
                                                       const LLBC_SockAddr_IN &peerAddr)
{
    _Block *block = new _Block(sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    ev.type = _Ev::AsyncConn;
    ev.sessionId = sessionId;
    ev.peerAddr = peerAddr;
    ev.sessionOpts = new LLBC_SessionOpts(sessionOpts);

    block->SetWritePos(sizeof(_Ev));
    return block;
}

LLBC_MessageBlock *LLBC_PollerEvUtil::BuildSendEv(LLBC_Packet *packet)
{
    _Block *block = new _Block(sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    ev.type = _Ev::Send;
    ev.un.packet = packet;

    block->SetWritePos(sizeof(_Ev));
    return block;
}

LLBC_MessageBlock *LLBC_PollerEvUtil::BuildCloseEv(int sessionId, const char *reason)
{
    _Block *block = new _Block(sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    ev.type = _Ev::Close;
    ev.sessionId = sessionId;

    if (reason != nullptr)
    {
        const size_t reasonLen = strlen(reason);
        ev.un.closeReason = LLBC_Malloc(char, reasonLen + 1);
        memcpy(ev.un.closeReason, reason, reasonLen + 1);
    }
    else
    {
        ev.un.closeReason = nullptr;
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
    _Block *block = new _Block(sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    ev.type = _Ev::Monitor;
    ev.un.monitorEv = LLBC_Malloc(char, sizeof(int) + sizeof(LLBC_POverlapped) + sizeof(int) * 2);

    size_t off = 0;
    // Wait return value.
    memcpy(ev.un.monitorEv, &ret, sizeof(int)), off += sizeof(int);
    // Overlapped data.
    memcpy(ev.un.monitorEv + off, &ol, sizeof(LLBC_POverlapped)), off += sizeof(LLBC_POverlapped);
    // Error no.
    memcpy(ev.un.monitorEv + off, &errNo, sizeof(int)), off += sizeof(int);
    // Sub error no.
    memcpy(ev.un.monitorEv + off, &subErrNo, sizeof(int));

    block->SetWritePos(sizeof(_Ev));
    return block;
}
#endif // LLBC_TARGET_PLATFORM_WIN32

#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
LLBC_MessageBlock *LLBC_PollerEvUtil::BuildEpollMonitorEv(const LLBC_EpollEvent *evs, int count)
{
    _Block *block = new _Block(sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    ev.type = _Ev::Monitor;
    ev.un.monitorEv = LLBC_Malloc(char, sizeof(int) + sizeof(LLBC_EpollEvent) * count);

    // Write count.
    memcpy(ev.un.monitorEv, &count, sizeof(int));
    // Write events.
    memcpy(ev.un.monitorEv + sizeof(int), evs, sizeof(LLBC_EpollEvent) * count);

    block->SetWritePos(sizeof(_Ev));
    return block;
}
#endif // LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID

LLBC_MessageBlock *LLBC_PollerEvUtil::BuildTakeOverSessionEv(LLBC_Session *session)
{
    _Block *block = new _Block(sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    ev.type = _Ev::TakeOverSession;
    ev.un.session = session;

    block->SetWritePos(sizeof(_Ev));
    return block;
}

LLBC_MessageBlock * LLBC_PollerEvUtil::BuildCtrlProtocolStackEv(int sessionId,
                                                                int ctrlCmd,
                                                                const LLBC_Variant &ctrlData)
{
    _Block *block = new _Block(sizeof(_Ev));
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());

    LLBC_Stream ctrlDataStream;
    ctrlDataStream.Write(ctrlData);

    ev.type = _Ev::CtrlProtocolStack;
    ev.sessionId = sessionId;
    ev.un.protocolStackCtrlInfo.ctrlCmd = ctrlCmd;
    ev.un.protocolStackCtrlInfo.ctrlData = new LLBC_Variant(ctrlData);

    block->SetWritePos(sizeof(_Ev));

    return block;
}

void LLBC_PollerEvUtil::DestroyEv(LLBC_PollerEvent &ev)
{
    switch (ev.type)
    {
    case _Ev::AddSock:
        delete ev.un.socket;
        LLBC_XDelete(ev.sessionOpts);
        break;

    case _Ev::AsyncConn:
        LLBC_XDelete(ev.sessionOpts);
        break;

    case _Ev::Send:
        LLBC_Recycle(ev.un.packet);
        break;

    case _Ev::Close:
        LLBC_XFree(ev.un.closeReason);
        break;

    case _Ev::Monitor:
        LLBC_XFree(ev.un.monitorEv);
        break;

    case _Ev::TakeOverSession:
        delete ev.un.session;
        break;

    case _Ev::CtrlProtocolStack:
        delete ev.un.protocolStackCtrlInfo.ctrlData;
        break;

    default:
        break;
    }
}

void LLBC_PollerEvUtil::DestroyEv(LLBC_MessageBlock *block)
{
    _Ev &ev = *reinterpret_cast<_Ev *>(block->GetData());
    This::DestroyEv(ev);

    LLBC_Recycle(block);
}

__LLBC_NS_END
