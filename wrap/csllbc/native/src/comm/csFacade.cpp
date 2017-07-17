/**
 * @file    csFacade.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/21
 * @version 1.0
 *
 * @brief
 */

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

    (*_sessionCreateDeleg)(sessionInfo.GetSessionId(),
                           static_cast<int>(sessionInfo.GetSocket()),
                           sessionInfo.IsListenSession(),
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

    (*_sessionDestroyDeleg)(sessionInfo.GetSessionId(),
                            static_cast<int>(sessionInfo.GetSocket()),
                            sessionInfo.IsListenSession(),
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
                             packet.GetPayload(),
                             static_cast<int>(packet.GetPayloadLength()),
                             packet.GetStatus());
}