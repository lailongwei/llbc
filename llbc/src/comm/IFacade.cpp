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
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/IFacade.h"
#include "llbc/comm/Session.h"
#include "llbc/comm/protocol/ProtocolLayer.h"
#include "llbc/comm/protocol/ProtoReportLevel.h"

__LLBC_NS_BEGIN

LLBC_SessionInfo::LLBC_SessionInfo()
: _isListen(false)
, _sessionId(0)
, _acceptSessionId(0)
, _localAddr()
, _peerAddr()
, _sockHandle(LLBC_INVALID_SOCKET_HANDLE)
{
}

LLBC_SessionInfo::~LLBC_SessionInfo()
{
}

bool LLBC_SessionInfo::IsListenSession() const
{
    return _isListen;
}

void LLBC_SessionInfo::SetIsListenSession(bool flag)
{
    _isListen = flag;
}

int LLBC_SessionInfo::GetSessionId() const
{
    return _sessionId;
}

void LLBC_SessionInfo::SetSessionId(int sessionId)
{
    _sessionId = sessionId;
}

int LLBC_SessionInfo::GetAcceptSessionId() const
{
    return _acceptSessionId;
}

void LLBC_SessionInfo::SetAcceptSessionId(int acceptSessionId)
{
    _acceptSessionId = acceptSessionId;
}

const LLBC_SockAddr_IN &LLBC_SessionInfo::GetLocalAddr() const
{
    return _localAddr;
}

void LLBC_SessionInfo::SetLocalAddr(const LLBC_SockAddr_IN &addr)
{
    _localAddr = addr;
}

const LLBC_SockAddr_IN &LLBC_SessionInfo::GetPeerAddr() const
{
    return _peerAddr;
}

void LLBC_SessionInfo::SetPeerAddr(const LLBC_SockAddr_IN &addr)
{
    _peerAddr = addr;
}

LLBC_SocketHandle LLBC_SessionInfo::GetSocket() const
{
    return _sockHandle;
}

void LLBC_SessionInfo::SetSocket(LLBC_SocketHandle handle)
{
    _sockHandle = handle;
}

LLBC_String LLBC_SessionInfo::ToString() const
{
    LLBC_String repr;
    repr.append_format("isListen:%s, ", _isListen?"true":"false")
        .append_format("sessionId:%d, ", _sessionId)
        .append_format("acceptSessionId:%d, ", _acceptSessionId)
        .append_format("local:{%s}, ", _localAddr.ToString().c_str())
        .append_format("peer:{%s}, ", _peerAddr.ToString().c_str())
        .append_format("handle:%d", _sockHandle);

    return repr;
}

LLBC_SessionDestroyInfo::LLBC_SessionDestroyInfo(LLBC_SessionInfo *sessionInfo,
                                                 LLBC_SessionCloseInfo *closeInfo)
: _sessionInfo(sessionInfo)
, _closeInfo(closeInfo)
{
}

LLBC_SessionDestroyInfo::~LLBC_SessionDestroyInfo()
{
    LLBC_XDelete(_sessionInfo);
    LLBC_XDelete(_closeInfo);
}

const LLBC_SessionInfo &LLBC_SessionDestroyInfo::GetSessionInfo() const
{
    return *_sessionInfo;
}

bool LLBC_SessionDestroyInfo::IsListenSession() const
{
    return _sessionInfo->IsListenSession();
}

int LLBC_SessionDestroyInfo::GetSessionId() const
{
    return _sessionInfo->GetSessionId();
}

int LLBC_SessionDestroyInfo::GetAcceptSessionId() const
{
    return _sessionInfo->GetAcceptSessionId();
}

LLBC_SocketHandle LLBC_SessionDestroyInfo::GetSocket() const
{
    return _sessionInfo->GetSocket();
}

const LLBC_SockAddr_IN &LLBC_SessionDestroyInfo::GetLocalAddr() const
{
    return _sessionInfo->GetLocalAddr();
}

const LLBC_SockAddr_IN &LLBC_SessionDestroyInfo::GetPeerAddr() const
{
    return _sessionInfo->GetPeerAddr();
}

bool LLBC_SessionDestroyInfo::IsDestroyedFromService() const
{
    return _closeInfo->IsFromService();
}

int LLBC_SessionDestroyInfo::GetErrno() const
{
    return _closeInfo->GetErrno();
}

int LLBC_SessionDestroyInfo::GetSubErrno() const
{
    return _closeInfo->GetSubErrno();
}

const LLBC_String &LLBC_SessionDestroyInfo::GetReason() const
{
    return _closeInfo->GetReason();
}

LLBC_String LLBC_SessionDestroyInfo::ToString() const
{
    LLBC_String repr;
    repr.append_format("sessionInfo:{%s}, ", _sessionInfo->ToString().c_str())
        .append_format("fromService:%s, ", _closeInfo->IsFromService()?"true":"false")
        .append_format("reason:%s", _closeInfo->GetReason().c_str());

    return repr;
}

LLBC_AsyncConnResult::LLBC_AsyncConnResult()
: _sessionId(0)
, _connected(false)
, _reason()
, _peerAddr()
{
}

LLBC_AsyncConnResult::~LLBC_AsyncConnResult()
{
}

bool LLBC_AsyncConnResult::IsConnected() const
{
    return _connected;
}

void LLBC_AsyncConnResult::SetIsConnected(bool connected)
{
    _connected = connected;
}

const LLBC_String &LLBC_AsyncConnResult::GetReason() const
{
    return _reason;
}

void LLBC_AsyncConnResult::SetReason(const LLBC_String &reason)
{
    _reason.clear();
    _reason.append(reason);
}

const LLBC_SockAddr_IN &LLBC_AsyncConnResult::GetPeerAddr() const
{
    return _peerAddr;
}

void LLBC_AsyncConnResult::SetPeerAddr(const LLBC_SockAddr_IN &addr)
{
    _peerAddr = addr;
}

LLBC_String LLBC_AsyncConnResult::ToString() const
{
    LLBC_String repr;
    repr.append_format("connected:%s, ", _connected?"true":"false")
        .append_format("reason:%s, ", _reason.c_str())
        .append_format("peer_addr:{%s}", _peerAddr.ToString().c_str());

    return repr;
}

LLBC_ProtoReport::LLBC_ProtoReport()
: _sessionId(0)
, _opcode(0)

, _layer(0)
, _level(0)
, _report()
{
}

LLBC_ProtoReport::~LLBC_ProtoReport()
{
}

int LLBC_ProtoReport::GetSessionId() const
{
    return _sessionId;
}

void LLBC_ProtoReport::SetSessionId(int sessionId)
{
    _sessionId = sessionId;
}

int LLBC_ProtoReport::GetOpcode() const
{
    return _opcode;
}

void LLBC_ProtoReport::SetOpcode(int opcode)
{
    _opcode = opcode;
}

int LLBC_ProtoReport::GetLayer() const
{
    return _layer;
}

void LLBC_ProtoReport::SetLayer(int layer)
{
    _layer = layer;
}

int LLBC_ProtoReport::GetLevel() const
{
    return _level;
}

void LLBC_ProtoReport::SetLevel(int level)
{
    _level = level;
}

const LLBC_String &LLBC_ProtoReport::GetReport() const
{
    return _report;
}

void LLBC_ProtoReport::SetReport(const LLBC_String &report)
{
    _report.clear();
    _report.append(report);
}

LLBC_String LLBC_ProtoReport::ToString() const
{
    LLBC_String repr;
    repr.append_format("sessionId:%d, ", _sessionId)
        .append_format("opcode:%d, ", _opcode)
        .append_format("layer:%s, ", LLBC_ProtocolLayer::Layer2Str(_layer).c_str())
        .append_format("level:%s, ", LLBC_ProtoReportLevel::Level2Str(_level).c_str())
        .append_format("report:%s", _report.c_str());

    return repr;
}

LLBC_IFacade::LLBC_IFacade()
: _inited(false)
, _svc(NULL)
{
}

LLBC_IFacade::~LLBC_IFacade()
{
}

LLBC_IService *LLBC_IFacade::GetService() const
{
    return _svc;
}

void LLBC_IFacade::OnInitialize()
{
}

void LLBC_IFacade::OnDestroy()
{
}

void LLBC_IFacade::OnStart()
{
}

void LLBC_IFacade::OnStop()
{
}

void LLBC_IFacade::OnUpdate()
{
}

void LLBC_IFacade::OnIdle(int idleTime)
{
}

void LLBC_IFacade::OnSessionCreate(const LLBC_SessionInfo &sessionInfo)
{
}

void LLBC_IFacade::OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo)
{
}

void LLBC_IFacade::OnAsyncConnResult(const LLBC_AsyncConnResult &result)
{
}

void LLBC_IFacade::OnProtoReport(const LLBC_ProtoReport &report)
{
}

void LLBC_IFacade::OnUnHandledPacket(const LLBC_Packet &packet)
{
}

void LLBC_IFacade::SetService(LLBC_IService *svc)
{
    _svc = svc;
}

__LLBC_NS_END

std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_SessionInfo &si)
{
    return o <<si.ToString();
}

std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_SessionDestroyInfo &destroy)
{
    return o <<destroy.ToString();
}

std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_AsyncConnResult &result)
{
    return o <<result.ToString();
}

std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_ProtoReport &report)
{
    return o <<report.ToString();
}

#include "llbc/common/AfterIncl.h"
