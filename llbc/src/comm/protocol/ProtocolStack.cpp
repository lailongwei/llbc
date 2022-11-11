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
#include "llbc/comm/protocol/IProtocol.h"
#include "llbc/comm/protocol/ProtocolStack.h"
#include "llbc/comm/Session.h"
#include "llbc/comm/Service.h"
#include "llbc/comm/ServiceEvent.h"

namespace
{
    typedef LLBC_NS LLBC_ProtocolStack This;
    typedef LLBC_NS LLBC_ProtocolLayer _Layer;

}

__LLBC_INTERNAL_NS_BEGIN

void __DeletePacketsBlock(LLBC_NS LLBC_MessageBlock *block)
{
    typedef LLBC_NS LLBC_Packet _Packet;

    _Packet *packet;
    while (block->Read(&packet, sizeof(_Packet *)) == LLBC_OK)
        LLBC_Recycle(packet);

    LLBC_Recycle(block);
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_ProtocolStack::LLBC_ProtocolStack(This::StackType type)
: _type(type)

, _svc(nullptr)
, _session(nullptr)
, _suppressCoderNotFoundError(false)
{
    memset(_protos, 0, sizeof(_protos));
}

LLBC_ProtocolStack::~LLBC_ProtocolStack()
{
    for (int i = _Layer::Begin; i < _Layer::End; ++i)
        LLBC_XDelete(_protos[i]);
}

LLBC_Service *LLBC_ProtocolStack::GetService()
{
    return _svc;
}

void LLBC_ProtocolStack::SetService(LLBC_Service *svc)
{
    _svc = svc;
}

LLBC_Session *LLBC_ProtocolStack::GetSession()
{
    return _session;
}

void LLBC_ProtocolStack::SetSession(LLBC_Session *session)
{
    _session = session;
    for (int i = _Layer::Begin; i < _Layer::End; ++i)
    {
        LLBC_IProtocol *proto = _protos[i];
        if (!proto)
            continue;

        proto->SetSession(session);
    }
}

bool LLBC_ProtocolStack::GetIsSuppressedCoderNotFoundWarning() const
{
    return _suppressCoderNotFoundError;
}

void LLBC_ProtocolStack::SetIsSuppressedCoderNotFoundWarning(bool suppressed)
{
    _suppressCoderNotFoundError = suppressed;
}

int LLBC_ProtocolStack::AddProtocol(LLBC_IProtocol *proto)
{
    const int layer = proto->GetLayer();
    if (_type == This::PackStack &&
            layer == _Layer::CodecLayer)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (_type == This::CodecStack &&
            (layer == _Layer::PackLayer ||
             layer == _Layer::CompressLayer))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_XDelete(_protos[layer]);

    proto->SetStack(this);
    _protos[layer] = proto;

    return LLBC_OK;
}

int LLBC_ProtocolStack::SetCoders(const LLBC_IProtocol::Coders *coders)
{
    if (!_protos[_Layer::CodecLayer])
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    return _protos[_Layer::CodecLayer]->SetCoders(coders);
}

const LLBC_IProtocol *LLBC_ProtocolStack::GetCoderProtocol() const
{
    return _protos[_Layer::CodecLayer];
}

int LLBC_ProtocolStack::SetFilter(LLBC_IProtocolFilter *filter, int toProto)
{
    if (UNLIKELY(!filter || !LLBC_ProtocolLayer::IsValid(toProto)))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_IProtocol *proto = _protos[toProto];
    if (!proto)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    proto->SetFilter(filter);

    return LLBC_OK;
}

int LLBC_ProtocolStack::SendCodec(LLBC_Packet *willEncode, LLBC_Packet *&encoded, bool &removeSession)
{
    void *in, *out = willEncode;
    for (int i = _Layer::End - 1; i >= _Layer::CodecLayer; --i)
    {
        if (!_protos[i])
            continue;

        in = out, out = nullptr;
        if (_protos[i]->Send(in, out, removeSession) != LLBC_OK)
            return LLBC_FAILED;
    }

    encoded = reinterpret_cast<LLBC_Packet *>(out);
    return LLBC_OK;
}

int LLBC_ProtocolStack::SendRaw(LLBC_Packet *packet, LLBC_MessageBlock *&block, bool &removeSession)
{
    void *in, *out = packet;
    for (int layer = _Layer::CompressLayer; layer >= _Layer::Begin; --layer)
    {
        if (!_protos[layer])
            continue;

        in = out, out = nullptr;
        if (_protos[layer]->Send(in, out, removeSession) != LLBC_OK)
            return LLBC_FAILED;
    }

    block = reinterpret_cast<LLBC_MessageBlock *>(out);
    return LLBC_OK;
}

int LLBC_ProtocolStack::Send(LLBC_Packet *packet, LLBC_MessageBlock *&block, bool &removeSession)
{
    if (SendCodec(packet, packet, removeSession) != LLBC_OK)
        return LLBC_FAILED;

    return SendRaw(packet, block, removeSession);
}

int LLBC_ProtocolStack::RecvRaw(LLBC_MessageBlock *block, std::vector<LLBC_Packet *> &packets, bool &removeSession)
{
    void *in, *out = nullptr;
    if (UNLIKELY(_protos[_Layer::PackLayer]->Recv(block, out, removeSession) != LLBC_OK))
        return LLBC_FAILED;
    else if (!out)
        return LLBC_OK;

    LLBC_MessageBlock *packetsBlock = reinterpret_cast<LLBC_MessageBlock *>(out);
    LLBC_Defer(LLBC_INL_NS __DeletePacketsBlock(packetsBlock));

    LLBC_Packet *packet;
    while (packetsBlock->Read(&packet, sizeof(LLBC_Packet *)) == LLBC_OK)
    {
        out = packet;
        for (int layer = _Layer::PackLayer + 1; layer <= _Layer::CompressLayer; ++layer)
        {
            if (!_protos[layer])
                continue;

            in = out, out = nullptr;
            if (_protos[layer]->Recv(in, out, removeSession) != LLBC_OK)
            {
                //! Current in-data already deleted in specific protocol, we don't need care it.
                //  Just need delete decoded packets, and non-decode packets.
             
                // Delete all decoded packets.
                LLBC_STLHelper::RecycleContainer(packets);
                // Delete non-decode packets and the message-block.
                // Yeah, this operation will done by LLBC_InvokeGuard, we don't need care it too.
                return LLBC_FAILED;
            }
        }

        // Done, push it to decoded list.
        packets.push_back(reinterpret_cast<LLBC_Packet *>(out));
    }

    return LLBC_OK;
}

int LLBC_ProtocolStack::RecvCodec(LLBC_Packet *willDecode, LLBC_Packet *&decoded, bool &removeSession)
{
    void *in, *out = willDecode;
    for (int layer = _Layer::CodecLayer; layer != _Layer::End; ++layer)
    {
        if (!_protos[layer])
            continue;

        in = out, out = nullptr;
        if (_protos[layer]->Recv(in, out, removeSession) != LLBC_OK)
            return LLBC_FAILED;
    }

    decoded = reinterpret_cast<LLBC_Packet *>(out);
    return LLBC_OK;
}

int LLBC_ProtocolStack::Recv(LLBC_MessageBlock *block, std::vector<LLBC_Packet *> &packets, bool &removeSession)
{
    _rawPackets.clear();
    if (RecvRaw(block, _rawPackets, removeSession) != LLBC_OK)
        return LLBC_FAILED;
        
    for (size_t i = 0;  i != _rawPackets.size(); ++i)
    {
        LLBC_Packet *packet;
        if (RecvCodec(_rawPackets[i], packet, removeSession) != LLBC_OK)
        {
            LLBC_STLHelper::RecycleContainer(packets);
            for (++i; i < _rawPackets.size(); ++i)
                LLBC_Recycle(_rawPackets[i]);

            return LLBC_FAILED;
        }

        packets.push_back(packet);
    }

    return LLBC_OK;
}

void LLBC_ProtocolStack::Report(LLBC_IProtocol *proto, int level, const LLBC_String &msg)
{
    Report(_session->GetId(), proto, level, msg);
}

void LLBC_ProtocolStack::Report(int sessionId, LLBC_IProtocol *proto, int level, const LLBC_String &msg)
{
    Report(sessionId, 0, proto, level, msg);
}

void LLBC_ProtocolStack::Report(int sessionId, int opcode, LLBC_IProtocol *proto, int level, const LLBC_String &msg)
{
    if (sessionId == 0)
        sessionId = _session->GetId();

    _svc->Push(LLBC_SvcEvUtil::BuildProtoReportEv(sessionId,
                                                  opcode,
                                                  proto->GetLayer(),
                                                  level,
                                                  msg));
}

void LLBC_ProtocolStack::CtrlStack(int cmd, const LLBC_Variant &ctrlData, bool &removeSession)
{
    if (!CtrlStackCodec(cmd, ctrlData, removeSession))
        return;

    CtrlStackRaw(cmd, ctrlData, removeSession);
}

bool LLBC_ProtocolStack::CtrlStackRaw(int cmd, const LLBC_Variant &ctrlData, bool &removeSession)
{
    for (int layer = _Layer::PackLayer; layer <= _Layer::CompressLayer; ++layer)
    {
        removeSession = false;
        if (!_protos[layer]->Ctrl(cmd, ctrlData, removeSession))
            return false;
    }

    return true;
}

bool LLBC_ProtocolStack::CtrlStackCodec(int cmd, const LLBC_Variant &ctrlData, bool &removeSession)
{
    for (int layer = _Layer::CodecLayer; layer != _Layer::End; ++layer)
    {
        removeSession = false;
        if (!_protos[layer]->Ctrl(cmd, ctrlData, removeSession))
            return false;
    }

    return true;
}

__LLBC_NS_END
