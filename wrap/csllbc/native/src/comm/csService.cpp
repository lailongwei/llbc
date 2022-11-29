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

#include "csllbc/comm/csCoder.h"
#include "csllbc/comm/csComponent.h"
#include "csllbc/comm/csService.h"
#include "csllbc/comm/csPacketHandler.h"

LLBC_SpinLock csllbc_Service::_packetDelegatesLock;
csllbc_Service::_PacketDecodeDelegs csllbc_Service::_packetDecodeDelegs;

csllbc_Service::csllbc_Service(const LLBC_String &name,
                               bool useNormalProtocolFactory,
                               bool fullStack,
                               _D::Deleg_Service_EncodePacket encodeDeleg,
                               _D::Deleg_Service_DecodePacket decodeDeleg, 
                               _D::Deleg_Service_PacketHandler handlerDeleg,
                               _D::Deleg_Service_PacketPreHandler preHandlerDeleg,
                               _D::Deleg_Service_PacketUnifyPreHandler unifyPreHandlerDeleg,
                               _D::Deleg_Service_NativeCouldNotFoundDecoderReport notFoundDecoderDeleg)
{
    // Create llbc service.
    LLBC_IProtocolFactory *protocolFactory;
    if (useNormalProtocolFactory)
        protocolFactory = new LLBC_NormalProtocolFactory;
    else
        protocolFactory = new LLBC_RawProtocolFactory;

    _llbcSvc = LLBC_Service::Create(name,
                                     protocolFactory,
                                     fullStack);

    // Set packet encode delegate.
    _packetEncodeDeleg = encodeDeleg;
    // Set all packet decode/handle about csharp delegates.
    PacketDecodeDelegates *delegs = new PacketDecodeDelegates;
    delegs->decodeDeleg = decodeDeleg;
    delegs->handlerDeleg = handlerDeleg;
    delegs->preHandlerDeleg = preHandlerDeleg;
    delegs->unifyPreHandlerDeleg = unifyPreHandlerDeleg;
    AddPacketDecodeDelegates(_llbcSvc->GetId(), delegs);

    // Create packet handler object.
    _packetHandler = new csllbc_PacketHandler(notFoundDecoderDeleg);
}

csllbc_Service::~csllbc_Service()
{
    Stop();

    LLBC_Recycle(_packetHandler);
    RemovePacketDecodeDelegates(_llbcSvc->GetId());

    delete _llbcSvc;
}

int csllbc_Service::Start(int pollerCount)
{
    if (IsStarted())
    {
        LLBC_SetLastError(LLBC_ERROR_INITED);
        return LLBC_FAILED;
    }

    return _llbcSvc->Start(pollerCount);
}

void csllbc_Service::Stop()
{
    if (!IsStarted())
        return;

    _llbcSvc->Stop();
}

bool csllbc_Service::IsStarted() const
{
    return _llbcSvc->IsStarted();
}

int csllbc_Service::GetId() const
{
    return _llbcSvc->GetId();
}

bool csllbc_Service::IsFullStack() const
{
    return _llbcSvc->IsFullStack();
}

int csllbc_Service::GetFPS() const
{
    return _llbcSvc->GetFPS();
}

int csllbc_Service::SetFPS(int fps)
{
    return _llbcSvc->SetFPS(fps);
}

int csllbc_Service::GetFrameInterval() const
{
    return _llbcSvc->GetFrameInterval();
}

csllbc_Service::DriveMode csllbc_Service::GetDriveMode() const
{
    return _llbcSvc->GetDriveMode();
}

int csllbc_Service::SetDriveMode(DriveMode mode)
{
    return _llbcSvc->SetDriveMode(mode);
}

csllbc_Delegates::Deleg_Service_EncodePacket csllbc_Service::GetEncodePacketDeleg() const
{
    return _packetEncodeDeleg;
}

int csllbc_Service::Listen(const char *ip, uint16 port)
{
    return _llbcSvc->Listen(ip, port);
}

int csllbc_Service::Connect(const char *ip, uint16 port)
{
    return _llbcSvc->Connect(ip, port);
}

int csllbc_Service::AsyncConn(const char *ip, uint16 port)
{
    return _llbcSvc->AsyncConn(ip, port);
}

bool csllbc_Service::IsSessionValidate(int sessionId)
{
    return _llbcSvc->IsSessionValidate(sessionId);
}

int csllbc_Service::RemoveSession(int sessionId, const char *reason)
{
    return _llbcSvc->RemoveSession(sessionId, reason);
}

int csllbc_Service::Send(int sessionId, int opcode, csllbc_Coder *coder, int status)
{
    return _llbcSvc->Send(sessionId, opcode, coder, status);
}

int csllbc_Service::Send(int sessionId, int opcode, const void *bytes, size_t len, int status)
{
    return _llbcSvc->Send(sessionId, opcode, bytes, len, status);
}

int csllbc_Service::Multicast(const LLBC_SessionIdList &sessionIds, int opcode, const void *bytes, size_t len, int status)
{
    return _llbcSvc->Multicast(sessionIds, opcode, bytes, len,status);
}

int csllbc_Service::Broadcast(int opcode, const void *bytes, size_t len, int status)
{
    return _llbcSvc->Broadcast(opcode, bytes, len, status);
}

int csllbc_Service::AddComponent(csllbc_Component *comp)
{
    return _llbcSvc->AddComponent(comp);
}

int csllbc_Service::AddCoder(int opcode)
{
    csllbc_CoderFactory *coderFactory = new csllbc_CoderFactory;
    if (_llbcSvc->AddCoderFactory(opcode, coderFactory) != LLBC_OK)
    {
        delete coderFactory;
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int csllbc_Service::Subscribe(int opcode)
{
    return _llbcSvc->Subscribe(opcode, _packetHandler, &csllbc_PacketHandler::Handle);
}

int csllbc_Service::PreSubscribe(int opcode)
{
    return _llbcSvc->PreSubscribe(opcode, _packetHandler, &csllbc_PacketHandler::PreHandle);
}

int csllbc_Service::UnifyPreSubscribe()
{
#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
    return _llbcSvc->UnifyPreSubscribe(_packetHandler, &csllbc_PacketHandler::UnifyPreHandle);
#else
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#endif
}

void csllbc_Service::OnSvc(bool fullFrame)
{
    _llbcSvc->OnSvc(fullFrame);
}

void csllbc_Service::AddPacketDecodeDelegates(int svcId, PacketDecodeDelegates *delegates)
{
    _packetDelegatesLock.Lock();
    _packetDecodeDelegs.insert(std::make_pair(svcId, delegates));
    _packetDelegatesLock.Unlock();
}

csllbc_Service::PacketDecodeDelegates *csllbc_Service::GetPacketDecodeDelegates(int svcId)
{
    _packetDelegatesLock.Lock();
    _PacketDecodeDelegs::iterator it = _packetDecodeDelegs.find(svcId);
    PacketDecodeDelegates *delegs = (it != _packetDecodeDelegs.end() ? it->second : nullptr);

    _packetDelegatesLock.Unlock();

    return delegs;
}

void csllbc_Service::RemovePacketDecodeDelegates(int svcId)
{
    _packetDelegatesLock.Lock();
    _PacketDecodeDelegs::iterator it = _packetDecodeDelegs.find(svcId);
    if (it != _packetDecodeDelegs.end())
    {
        delete it->second;
        _packetDecodeDelegs.erase(it);
    }

    _packetDelegatesLock.Unlock();
}
