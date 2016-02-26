/**
 * @file    ProtocolStack.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/13
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/Packet.h"
#include "llbc/comm/protocol/IProtocol.h"
#include "llbc/comm/protocol/ProtocolStack.h"
#include "llbc/comm/Session.h"
#include "llbc/comm/IService.h"
#include "llbc/comm/ServiceEvent.h"

namespace
{
    typedef LLBC_NS LLBC_ProtocolStack This;
    typedef LLBC_NS LLBC_ProtocolLayer _Layer;

}

__LLBC_INTERNAL_NS_BEGIN

void __DeletePacketsBlock(void *data)
{
    typedef LLBC_NS LLBC_Packet _Packet;
    typedef LLBC_NS LLBC_MessageBlock _Block;

    _Block *block = reinterpret_cast<_Block *>(data);

    _Packet *packet;
    while (block->Read(&packet, sizeof(_Packet *)) == LLBC_OK)
        LLBC_Delete(packet);

    LLBC_Delete(block);
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_ProtocolStack::LLBC_ProtocolStack(This::StackType type)
: _type(type)
, _builded(false)

, _svc(NULL)
, _session(NULL)
{
    ::memset(_protos, 0, sizeof(_protos));
}

LLBC_ProtocolStack::~LLBC_ProtocolStack()
{
    for (int i = _Layer::Begin; i < _Layer::End; i++)
        LLBC_XDelete(_protos[i]);
}

void LLBC_ProtocolStack::SetService(LLBC_IService *svc)
{
    _svc = svc;
}

void LLBC_ProtocolStack::SetSession(LLBC_Session *session)
{
    _session = session;
}

int LLBC_ProtocolStack::AddProtocol(LLBC_IProtocol *proto)
{
    const int layer = proto->GetLayer();
    if (_type == This::RawStack &&
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

int LLBC_ProtocolStack::AddCoder(int opcode, LLBC_ICoderFactory *coder)
{
    if (!_protos[_Layer::CodecLayer])
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    return _protos[_Layer::CodecLayer]->AddCoder(opcode, coder);
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

int LLBC_ProtocolStack::Connect(LLBC_SockAddr_IN &local, LLBC_SockAddr_IN &peer)
{
    for (int i = _Layer::Begin; i < _Layer::End; i++)
        if (_protos[i])
            if (_protos[i]->Connect(local, peer) != LLBC_OK)
                return LLBC_FAILED;

    return LLBC_OK;
}

int LLBC_ProtocolStack::SendCodec(LLBC_Packet *willEncode, LLBC_Packet *&encoded)
{
    void *in, *out = willEncode;
    for (int i = _Layer::End - 1; i >= _Layer::CodecLayer; i--)
    {
        if (!_protos[i])
            continue;

        in = out, out = NULL;
        if (_protos[i]->Send(in, out) != LLBC_OK)
            return LLBC_FAILED;
    }

    encoded = reinterpret_cast<LLBC_Packet *>(out);
    return LLBC_OK;
}

int LLBC_ProtocolStack::SendRaw(LLBC_Packet *packet, LLBC_MessageBlock *&block)
{
    void *in, *out = packet;
    for (int i = _Layer::CompressLayer; i >= _Layer::Begin; i--)
    {
        if (!_protos[i])
            continue;

        in = out, out = NULL;
        if (_protos[i]->Send(in, out) != LLBC_OK)
            return LLBC_FAILED;
    }

    block = reinterpret_cast<LLBC_MessageBlock *>(out);
    return LLBC_OK;
}

int LLBC_ProtocolStack::Send(LLBC_Packet *packet, LLBC_MessageBlock *&block)
{
    if (SendCodec(packet, packet) != LLBC_OK)
        return LLBC_FAILED;
    return SendRaw(packet, block);
}

int LLBC_ProtocolStack::RecvRaw(LLBC_MessageBlock *block, std::vector<LLBC_Packet *> &packets)
{
    void *in, *out = NULL;
    if (UNLIKELY(_protos[_Layer::PackLayer]->Recv(block, out) != LLBC_OK))
        return LLBC_FAILED;
    else if (!out)
        return LLBC_OK;

    LLBC_MessageBlock *packetsBlock = reinterpret_cast<LLBC_MessageBlock *>(out);
    LLBC_InvokeGuard guard(&LLBC_INL_NS __DeletePacketsBlock, packetsBlock);

    LLBC_Packet *packet;
    while (packetsBlock->Read(&packet, sizeof(LLBC_Packet *)) == LLBC_OK)
    {
        out = packet;
        for (int layer = _Layer::PackLayer + 1; layer <= _Layer::CompressLayer; layer++)
        {
            if (!_protos[layer])
                continue;

            in = out, out = NULL;
            if (_protos[layer]->Recv(in, out) != LLBC_OK)
            {
                //! Current in-data already deleted in specific protocol, we don't need care it.
                //  Just need delete decoded packets, and non-decode packets.
             
                // Delete all decoded packets.
                LLBC_STLHelper::DeleteContainer(packets);
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

int LLBC_ProtocolStack::RecvCodec(LLBC_Packet *willDecode, LLBC_Packet *&decoded)
{
    void *in, *out = willDecode;
    for (int i = _Layer::CodecLayer; i < _Layer::End; i++)
    {
        if (!_protos[i])
            continue;

        in = out, out = NULL;
        if (_protos[i]->Recv(in, out) != LLBC_OK)
            return LLBC_FAILED;
    }

    decoded = reinterpret_cast<LLBC_Packet *>(out);
    return LLBC_OK;
}

int LLBC_ProtocolStack::Recv(LLBC_MessageBlock *block, std::vector<LLBC_Packet *> &packets)
{
    std::vector<LLBC_Packet *> rawPackets;
    if (RecvRaw(block, rawPackets) != LLBC_OK)
        return LLBC_FAILED;

    for (size_t i = 0;  i < rawPackets.size(); i++)
    {
        LLBC_Packet *packet;
        if (RecvCodec(rawPackets[i], packet) != LLBC_OK)
        {
            LLBC_STLHelper::DeleteContainer(packets);
            for (++i; i < rawPackets.size(); i++)
                LLBC_Delete(rawPackets[i]);

            return LLBC_FAILED;
        }

        packets.push_back(packet);
    }

    return LLBC_OK;
}

void LLBC_ProtocolStack::Report(LLBC_IProtocol *proto, int level, const LLBC_String &err)
{
    Report(_session->GetId(), proto, level, err);
}

void LLBC_ProtocolStack::Report(int sessionId, LLBC_IProtocol *proto, int level, const LLBC_String &err)
{
    if (sessionId == 0)
        sessionId = _session->GetId();

    _svc->Push(LLBC_SvcEvUtil::BuildProtoReportEv(
            sessionId, proto->GetLayer(), level, err));
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
