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

#include "llbc/comm/Session.h"
#include "llbc/comm/Socket.h"

#include "llbc/comm/protocol/ProtocolLayer.h"
#include "llbc/comm/protocol/ProtoReportLevel.h"
#include "llbc/comm/protocol/PacketProtocol.h"
#include "llbc/comm/protocol/ProtocolStack.h"

#include "llbc/comm/Service.h"

namespace
{
    typedef LLBC_NS LLBC_ProtocolLayer _Layer;
}

__LLBC_INTERNAL_NS_BEGIN

static constexpr size_t __llbc_headerLen = 20;

void inline __DelPacketList(void *&data)
{
    if (!data)
        return;

    LLBC_NS LLBC_MessageBlock *block = 
        reinterpret_cast<LLBC_NS LLBC_MessageBlock *>(data);

    LLBC_NS LLBC_Packet *packet;
    while (block->Read(&packet, sizeof(LLBC_NS LLBC_Packet *)) == LLBC_OK)
        LLBC_Recycle(packet);

    delete block;

    data = nullptr;
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_PacketProtocol::LLBC_PacketProtocol()
: _headerAssembler(LLBC_INL_NS __llbc_headerLen)

, _packet(nullptr)
, _payloadNeedRecv(0)
, _payloadRecved(0)
{
}

LLBC_PacketProtocol::~LLBC_PacketProtocol()
{
    LLBC_XRecycle(_packet);
}

int LLBC_PacketProtocol::GetLayer() const
{
    return _Layer::PackLayer;
}

int LLBC_PacketProtocol::Send(void *in, void *&out, bool &removeSession)
{
    LLBC_Packet *packet = reinterpret_cast<LLBC_Packet *>(in);

    uint32 length = static_cast<uint32>(
        LLBC_INL_NS __llbc_headerLen + packet->GetPayloadLength());

    // Create block and write header in.
    LLBC_MessageBlock *block = new LLBC_MessageBlock(length);

    sint32 opcode = packet->GetOpcode();
    sint16 status = static_cast<sint16>(packet->GetStatus());
    uint16 flags = static_cast<uint16>(packet->GetFlags());
    sint64 extData1 = packet->GetExtData1();

#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
    length = LLBC_Host2Net(length);
    opcode = LLBC_Host2Net(opcode);
    status = LLBC_Host2Net(status);
    flags = LLBC_Host2Net(flags);
    extData1 = LLBC_Host2Net(extData1);
#endif // Net order.

    block->Write(&length, sizeof(length));
    block->Write(&opcode, sizeof(opcode));
    block->Write(&status, sizeof(status));
    block->Write(&flags, sizeof(flags));
    block->Write(&extData1, sizeof(extData1));

    // Write packet data and delete packet.
    block->Write(packet->GetPayload(), packet->GetPayloadLength());
    LLBC_Recycle(packet);

    out = block;

    return LLBC_OK;
}

int LLBC_PacketProtocol::Recv(void *in, void *&out, bool &removeSession)
{
    out = nullptr;
    LLBC_MessageBlock *block = reinterpret_cast<LLBC_MessageBlock *>(in);
    const size_t maxPacketLen = _session->GetSocket()->GetMaxPacketSize();

    LLBC_Defer(LLBC_Recycle(block));

    size_t readableSize;
    while ((readableSize = block->GetReadableSize()) > 0)
    {
        const char *readableBuf = reinterpret_cast<const char *>(block->GetDataStartWithReadPos());
        
        // Construct packet header.
        if (!_packet)
        {
            // If header recv not done, return.
            size_t headerUsed;
            if (!_headerAssembler.Assemble(readableBuf, readableSize, headerUsed))
                return LLBC_OK;

            // Create new packet.
            _packet = _pktPoolInst->GetObject();
            _headerAssembler.SetToPacket(*_packet);
            _packet->SetSessionId(_sessionId);
            _packet->SetAcceptSessionId(_acceptSessionId);

            // Reset header assembler.
            _headerAssembler.Reset();

            // Check length.
            const size_t packetLen = _packet->GetLength();
            if (packetLen < LLBC_INL_NS __llbc_headerLen || packetLen > maxPacketLen)
            {
                _stack->Report(this,
                               LLBC_ProtoReportLevel::Error,
                               LLBC_String().format("invalid packet len: %lu", _packet->GetLength()));

                LLBC_XRecycle(_packet);
                _payloadNeedRecv = 0;

                LLBC_INL_NS __DelPacketList(out);

                removeSession = true;
                LLBC_SetLastError(LLBC_ERROR_PACK);
                return LLBC_FAILED;
            }

            // Calculate payload need receive bytes.
            _payloadNeedRecv = packetLen - LLBC_INL_NS __llbc_headerLen;

            // Offset the readable buffer pointer and modify readable size value.
            readableBuf += headerUsed;
            readableSize -= headerUsed;

            // Shift block read pos.
#if LLBC_TARGET_PLATFORM_WIN32 && defined(_WIN64)
            block->ShiftReadPos(static_cast<long>(headerUsed));
#else
            block->ShiftReadPos(headerUsed);
#endif // target platform is WIN32 and in x64 module.
        }

        // Read packet content.
        size_t contentNeedRecv = _payloadNeedRecv - _payloadRecved;
        if (readableSize < contentNeedRecv) // if the readable data size < content need receive size, copy the data and return.
        {
            _packet->Write(readableBuf, readableSize);
            _payloadRecved += readableSize;
            return LLBC_OK;
        }

        // Readable data size >= content need receive size.
        _packet->Write(readableBuf, contentNeedRecv);
        if (!out)
            out = new LLBC_MessageBlock(sizeof(LLBC_Packet *));
        (reinterpret_cast<LLBC_MessageBlock *>(out))->Write(&_packet, sizeof(LLBC_Packet *));

        // Reset packet about data members.
        _packet = nullptr;
        _payloadRecved = 0;
        _payloadNeedRecv = 0;

        // Shift block read position.
#if LLBC_TARGET_PLATFORM_WIN32 && defined(_WIN64)
        block->ShiftReadPos(static_cast<long>(contentNeedRecv));
#else
        block->ShiftReadPos(contentNeedRecv);
#endif // target platform is WIN32 and defined _WIN64 macro.
    }

    return LLBC_OK;
}

__LLBC_NS_END
