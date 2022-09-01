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

#ifdef __LLBC_COMM_SERVICE_EVENT_H__

#include "llbc/comm/Session.h"
#include "llbc/comm/protocol/ProtocolLayer.h"
#include "llbc/comm/protocol/ProtoReportLevel.h"

__LLBC_NS_BEGIN

inline LLBC_ServiceEvent::LLBC_ServiceEvent(int type)
: type(type)
{
}

inline LLBC_SvcEv_SessionCreate::LLBC_SvcEv_SessionCreate()
: LLBC_ServiceEvent(LLBC_ServiceEventType::SessionCreate)
, isListen(false)
, sessionId(0)
, acceptSessionId(0)
{
}

inline LLBC_SvcEv_SessionDestroy::LLBC_SvcEv_SessionDestroy()
: LLBC_ServiceEvent(LLBC_ServiceEventType::SessionDestroy)
, isListen(false)
, sessionId(0)
, acceptSessionId(0)
, handle(LLBC_INVALID_SOCKET_HANDLE)

, closeInfo(nullptr)
{
}

inline LLBC_SvcEv_SessionDestroy::~LLBC_SvcEv_SessionDestroy()
{
    if (closeInfo)
        LLBC_Delete(closeInfo);
}

inline LLBC_SvcEv_AsyncConn::LLBC_SvcEv_AsyncConn()
: LLBC_ServiceEvent(LLBC_ServiceEventType::AsyncConnResult)
, sessionId(0)
, connected(false)
{
}

inline LLBC_SvcEv_DataArrival::LLBC_SvcEv_DataArrival()
: LLBC_ServiceEvent(LLBC_ServiceEventType::DataArrival)
, packet(nullptr)
{
}

inline LLBC_SvcEv_DataArrival::~LLBC_SvcEv_DataArrival()
{
    if (packet)
        LLBC_Recycle(packet);
}

inline LLBC_SvcEv_ProtoReport::LLBC_SvcEv_ProtoReport()
: LLBC_ServiceEvent(LLBC_ServiceEventType::ProtoReport)
, sessionId(0)
, opcode(0)

, layer(LLBC_ProtocolLayer::End)
, level(LLBC_ProtoReportLevel::Error)
{
}

inline LLBC_SvcEv_SubscribeEv::LLBC_SvcEv_SubscribeEv()
: LLBC_ServiceEvent(LLBC_ServiceEventType::SubscribeEv)
, id(0)
, stub(0)
, deleg(nullptr)
, listener(nullptr)
{
}

inline LLBC_SvcEv_SubscribeEv::~LLBC_SvcEv_SubscribeEv()
{
    if (listener)
        LLBC_Delete(listener);
}

inline LLBC_SvcEv_UnsubscribeEv::LLBC_SvcEv_UnsubscribeEv()
: LLBC_ServiceEvent(LLBC_ServiceEventType::UnsubscribeEv)
, id(0)
, stub(0)
{
}

inline LLBC_SvcEv_AppCfgReloadedEv::LLBC_SvcEv_AppCfgReloadedEv()
: LLBC_ServiceEvent(LLBC_ServiceEventType::AppCfgReloaded)
{
}

inline LLBC_SvcEv_FireEv::LLBC_SvcEv_FireEv()
: LLBC_ServiceEvent(LLBC_ServiceEventType::FireEv)
, ev(nullptr)
{
}

inline LLBC_SvcEv_FireEv::~LLBC_SvcEv_FireEv()
{
    if (ev)
    {
        if (!ev->IsDontDelAfterFire())
            LLBC_Delete(ev);
        else if (dequeueHandler)
            dequeueHandler(ev);
    }
}

__LLBC_NS_END

#endif // __LLBC_COMM_SERVICE_EVENT_H__

