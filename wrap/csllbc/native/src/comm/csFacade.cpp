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

#include "csllbc/common/Export.h"

#include "csllbc/comm/csFacade.h"

namespace
{
    typedef csllbc_Delegates _D;
}

csllbc_Facade::csllbc_Facade(_D::Deleg_Facade_OnInit initDeleg,
                             _D::Deleg_Facade_OnDestroy destroyDeleg,
                             _D::Deleg_Facade_OnStart startDeleg,
                             _D::Deleg_Facade_OnStop stopDeleg,
                             _D::Deleg_Facade_OnUpdate updateDeleg,
                             _D::Deleg_Facade_OnIdle idleDeleg,
                             _D::Deleg_Facade_OnSessionCreate sessionCreateDeleg,
                             _D::Deleg_Facade_OnSessionDestroy sessionDestroyDeleg,
                             _D::Deleg_Facade_OnAsyncConnResult asyncConnResultDeleg,
                             _D::Deleg_Facade_OnProtoReport protoReportDeleg,
                             _D::Deleg_Facade_OnUnHandledPacket unHandledPacketDeleg)
{
    _initDeleg = initDeleg;
    _destroyDeleg = destroyDeleg;
    _startDeleg = startDeleg;
    _stopDeleg = stopDeleg;

    _updateDeleg = updateDeleg;
    _idleDeleg = idleDeleg;

    _sessionCreateDeleg = sessionCreateDeleg;
    _sessionDestroyDeleg = sessionDestroyDeleg;
    _asyncConnResultDeleg = asyncConnResultDeleg;

    _protoReportDeleg = protoReportDeleg;
    _unHandledPacketDeleg = unHandledPacketDeleg;
}

void csllbc_Facade::OnInitialize()
{
    (*_initDeleg)();
}

void csllbc_Facade::OnDestroy()
{
    (*_destroyDeleg)();
}

void csllbc_Facade::OnStart()
{
    (*_startDeleg)();
}

void csllbc_Facade::OnStop()
{
    (*_stopDeleg)();
}

void csllbc_Facade::OnUpdate()
{
    (*_updateDeleg)();
}

void csllbc_Facade::OnIdle(int idleTime)
{
    (*_idleDeleg)(idleTime);
}

void csllbc_Facade::OnSessionCreate(const LLBC_SessionInfo &sessionInfo)
{
    const LLBC_SockAddr_IN &localAddr = sessionInfo.GetLocalAddr();
    const LLBC_String localHost = localAddr.GetIpAsString();

    const LLBC_SockAddr_IN &remoteAddr = sessionInfo.GetPeerAddr();
    const LLBC_String remoteHost = remoteAddr.GetIpAsString();

    (*_sessionCreateDeleg)(sessionInfo.IsListenSession(),
                           sessionInfo.GetSessionId(),
                           sessionInfo.GetAcceptSessionId(),
                           static_cast<int>(sessionInfo.GetSocket()),
                           localHost.c_str(),
                           static_cast<int>(localHost.length()),
                           localAddr.GetPort(),
                           remoteHost.c_str(),
                           static_cast<int>(remoteHost.length()),
                           remoteAddr.GetPort());
}

void csllbc_Facade::OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo)
{
    const LLBC_SessionInfo &sessionInfo = destroyInfo.GetSessionInfo();
    const LLBC_SockAddr_IN &localAddr = sessionInfo.GetLocalAddr();
    const LLBC_String localHost = localAddr.GetIpAsString();

    const LLBC_SockAddr_IN &remoteAddr = sessionInfo.GetPeerAddr();
    const LLBC_String remoteHost = remoteAddr.GetIpAsString();

    (*_sessionDestroyDeleg)(sessionInfo.IsListenSession(),
                            sessionInfo.GetSessionId(),
                            sessionInfo.GetAcceptSessionId(),
                            static_cast<int>(sessionInfo.GetSocket()),
                            localHost.c_str(),
                            static_cast<int>(localHost.length()),
                            localAddr.GetPort(),
                            remoteHost.c_str(),
                            static_cast<int>(remoteHost.length()),
                            remoteAddr.GetPort(),
                            destroyInfo.IsDestroyedFromService(),
                            destroyInfo.GetReason().c_str(),
                            static_cast<int>(destroyInfo.GetReason().length()),
                            destroyInfo.GetErrno(),
                            destroyInfo.GetSubErrno());
}

void csllbc_Facade::OnAsyncConnResult(const LLBC_AsyncConnResult &result)
{
    const LLBC_SockAddr_IN &remoteAddr = result.GetPeerAddr();
    const LLBC_String remoteHost = remoteAddr.GetIpAsString();

    (*_asyncConnResultDeleg)(result.IsConnected(),
                             result.GetReason().c_str(),
                             static_cast<int>(result.GetReason().length()),
                             remoteHost.c_str(),
                             static_cast<int>(remoteHost.length()),
                             remoteAddr.GetPort());
}

void csllbc_Facade::OnProtoReport(const LLBC_ProtoReport &report)
{
    (*_protoReportDeleg)(report.GetSessionId(),
                         report.GetLayer(),
                         report.GetLevel(),
                         report.GetReport().c_str(),
                         static_cast<int>(report.GetReport().length()));
}

void csllbc_Facade::OnUnHandledPacket(const LLBC_Packet &packet)
{
    (*_unHandledPacketDeleg)(packet.GetSessionId(),
                             packet.GetOpcode(),
                             const_cast<void *>(packet.GetPayload()),
                             static_cast<int>(packet.GetPayloadLength()),
                             packet.GetStatus());
}