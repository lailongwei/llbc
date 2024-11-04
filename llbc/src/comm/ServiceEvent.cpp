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
#include "llbc/comm/ServiceEvent.h"

#include "llbc/app/App.h"

namespace
{
    template <typename Ev>
    static inline LLBC_NS LLBC_MessageBlock *__CreateEvBlock(Ev *&ev)
    {
        ev = new Ev;
        auto block = new LLBC_NS LLBC_MessageBlock(ev, sizeof(Ev));

        return block;
    }
}

__LLBC_NS_BEGIN

LLBC_MessageBlock *LLBC_SvcEvUtil::BuildSessionCreateEv(const LLBC_SockAddr_IN &local,
                                                        const LLBC_SockAddr_IN &peer,
                                                        bool isListen,
                                                        int sessionId,
                                                        int acceptSessionId,
                                                        LLBC_SocketHandle handle)
{
    LLBC_SvcEv_SessionCreate *ev;
    auto evBlock = __CreateEvBlock(ev);
    ev->isListen = isListen;
    ev->sessionId = sessionId;
    ev->acceptSessionId = acceptSessionId;
    ev->local = local;
    ev->peer = peer;
    ev->handle = handle;

    return evBlock;
}

LLBC_MessageBlock *LLBC_SvcEvUtil::BuildSessionDestroyEv(const LLBC_SockAddr_IN &local,
                                                         const LLBC_SockAddr_IN &peer,
                                                         bool isListen,
                                                         int sessionId,
                                                         int acceptSessionId,
                                                         LLBC_SocketHandle handle,
                                                         LLBC_SessionCloseInfo *closeInfo)
{
    LLBC_SvcEv_SessionDestroy *ev;
    auto evBlock = __CreateEvBlock(ev);
    ev->local = local;
    ev->peer = peer;
    ev->sessionId = sessionId;
    ev->acceptSessionId = acceptSessionId;
    ev->isListen = isListen;
    ev->handle = handle;

    ev->closeInfo = closeInfo;

    return evBlock;
}

LLBC_MessageBlock *LLBC_SvcEvUtil::BuildAsyncConnResultEv(int sessionId,
                                                          bool connected,
                                                          const LLBC_String &reason,
                                                          const LLBC_SockAddr_IN &peer)
{
    LLBC_SvcEv_AsyncConn *ev;
    auto evBlock = __CreateEvBlock(ev);
    ev->sessionId = sessionId;
    ev->connected = connected;
    ev->reason.append(reason);
    ev->peer = peer;

    return evBlock;
}

LLBC_MessageBlock *LLBC_SvcEvUtil::BuildDataArrivalEv(LLBC_Packet *packet)
{
    LLBC_SvcEv_DataArrival *ev;
    auto evBlock = __CreateEvBlock(ev);
    ev->packet = packet;

    return evBlock;
}

LLBC_MessageBlock *LLBC_SvcEvUtil::BuildProtoReportEv(int sessionId,
                                                      int opcode,
                                                      int layer,
                                                      int level,
                                                      const LLBC_String &report)
{
    LLBC_SvcEv_ProtoReport *ev;
    auto evBlock = __CreateEvBlock(ev);
    ev->sessionId = sessionId;
    ev->opcode = opcode;
    ev->layer = layer;
    ev->level = level;
    ev->report.append(report);

    return evBlock;
}

LLBC_MessageBlock *LLBC_SvcEvUtil::BuildSubscribeEventEv(int id,
                                                         const LLBC_ListenerStub &stub,
                                                         const LLBC_Delegate<void(LLBC_Event &)> &deleg,
                                                         LLBC_EventListener *listener)
{
    LLBC_SvcEv_SubscribeEv *ev;
    auto evBlock = __CreateEvBlock(ev);
    ev->id = id;
    ev->stub = stub;
    if (deleg)
        ev->deleg = deleg;
    if (listener)
        ev->listener = listener;

    return evBlock;
}

LLBC_MessageBlock *LLBC_SvcEvUtil::BuildUnsubscribeEventEv(int id, const LLBC_ListenerStub &stub)
{
    LLBC_SvcEv_UnsubscribeEv *ev;
    auto evBlock = __CreateEvBlock(ev);
    ev->id = id;
    ev->stub = stub;

    return evBlock;
}

LLBC_MessageBlock *LLBC_SvcEvUtil::BuildFireEventEv(LLBC_Event *ev,
                                                    const LLBC_Delegate<void(LLBC_Event *)> &dequeueHandler)
{
    LLBC_SvcEv_FireEv *wrapEv;
    auto evBlock = __CreateEvBlock(wrapEv);
    wrapEv->ev = ev;
    if (dequeueHandler)
        wrapEv->dequeueHandler = dequeueHandler;

    return evBlock;
}

LLBC_MessageBlock *LLBC_SvcEvUtil::BuildAppPhaseEv(bool willStart,
                                                   bool startFailed,
                                                   bool startFinished,
                                                   bool willStop,
                                                   int cfgType,
                                                   const LLBC_Variant &cfg)
{
    LLBC_SvcEv_AppPhaseEv *wrapEv;
    auto evBlock = __CreateEvBlock(wrapEv);
    wrapEv->willStart = willStart;
    wrapEv->startFailed = startFailed;
    wrapEv->startFinished = startFinished;
    wrapEv->willStop = willStop;

    wrapEv->cfgType = cfgType;
    wrapEv->cfg = cfg;

    return evBlock;
}

LLBC_MessageBlock *LLBC_SvcEvUtil::BuildAppReloadedEv(int cfgType,
                                                      const LLBC_Variant &cfg)
{
    LLBC_SvcEv_AppReloadedEv *wrapEv;
    auto evBlock = __CreateEvBlock(wrapEv);
    wrapEv->cfgType = cfgType;
    wrapEv->cfg = cfg;

    return evBlock;
}

LLBC_MessageBlock *LLBC_SvcEvUtil::BuildComponentEventEv(int eventType,
                                                         const LLBC_Variant &eventParams)
{
    LLBC_SvcEv_ComponentEventEv *wrapEv;
    auto evBlock = __CreateEvBlock(wrapEv);
    wrapEv->eventType = eventType;
    wrapEv->eventParams = eventParams;

    return evBlock;
}

void LLBC_SvcEvUtil::DestroyEvBlock(LLBC_MessageBlock *block)
{
    delete reinterpret_cast<LLBC_ServiceEvent *>(block->GetData());
    delete block;
}

__LLBC_NS_END
