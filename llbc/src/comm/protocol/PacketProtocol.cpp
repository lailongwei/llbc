/**
 * @file    PacketProtocol.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/12
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/PacketHeaderDescAccessor.h"

#include "llbc/comm/protocol/ProtocolLayer.h"
#include "llbc/comm/protocol/ProtoReportLevel.h"
#include "llbc/comm/protocol/IProtocol.h"
#include "llbc/comm/protocol/ProtocolStack.h"

namespace
{
    typedef LLBC_NS LLBC_IProtocol Base;
    typedef LLBC_NS LLBC_ProtocolLayer _Layer;

    typedef LLBC_NS LLBC_PacketHeaderDescAccessor _HDAccessor;
}

__LLBC_INTERNAL_NS_BEGIN

void inline __DelBlock(void *data)
{
    LLBC_Delete(reinterpret_cast<LLBC_NS LLBC_MessageBlock *>(data));
}

void inline __DelPacketList(void *&data)
{
    if (!data)
        return;

    LLBC_NS LLBC_MessageBlock *block = 
        reinterpret_cast<LLBC_NS LLBC_MessageBlock *>(data);

    LLBC_NS LLBC_Packet *packet;
    while (block->Read(&packet, sizeof(LLBC_NS LLBC_Packet *)) == LLBC_RTN_OK)
        LLBC_Delete(packet);

    LLBC_Delete(block);

    data = NULL;
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_PacketProtocol::LLBC_PacketProtocol()
: _headerAssembler(_HDAccessor::GetHeaderDesc()->GetHeaderLen())

, _packet(NULL)
, _payloadNeedRecv(0)
, _payloadRecved(0)

, _headerIncludedLen(static_cast<int>(_HDAccessor::GetHeaderDesc()->GetLenPartIncludedLen()))
{
}

LLBC_PacketProtocol::~LLBC_PacketProtocol()
{
    LLBC_XDelete(_packet);
}

int LLBC_PacketProtocol::GetLayer() const
{
    return _Layer::PackLayer;
}

int LLBC_PacketProtocol::Connect(LLBC_SockAddr_IN &local, LLBC_SockAddr_IN &peer)
{
    return LLBC_RTN_OK;
}

int LLBC_PacketProtocol::Send(void *in, void *&out)
{
    LLBC_Packet *packet = reinterpret_cast<LLBC_Packet *>(in);

    out = packet->GiveUp();
    LLBC_Delete(packet);

    return LLBC_RTN_OK;
}

int LLBC_PacketProtocol::Recv(void *in, void *&out)
{
    out = NULL;
    LLBC_MessageBlock *block = reinterpret_cast<LLBC_MessageBlock *>(in);

    LLBC_InvokeGuard guard(&LLBC_INL_NS __DelBlock, block);
    
    size_t readableSize;
    while ((readableSize = block->GetReadableSize()) > 0)
    {
        const char *readableBuf = reinterpret_cast<const char *>(block->GetDataStartWithReadPos());
        
        // Construct packet header.
        if (!_packet)
        {
            size_t headerUsed;
            if (!_headerAssembler.Assemble(readableBuf, readableSize, headerUsed)) // If header recv not done, return.
                return LLBC_RTN_OK;

            // Create new packet.
            _packet = LLBC_New(LLBC_Packet);
            _packet->WriteHeader(_headerAssembler.GetHeader());
            _payloadNeedRecv = _packet->GetLength() - _headerIncludedLen;
            if (_payloadNeedRecv < 0)
            {
                _stack->Report(this,
                               LLBC_ProtoReportLevel::Error,
                               LLBC_String().format("invalid packet len: %d", _packet->GetLength()));

                _headerAssembler.Reset();

                LLBC_XDelete(_packet);
                _payloadNeedRecv = 0;

                LLBC_INL_NS __DelPacketList(out);

                return LLBC_RTN_FAILED;
            }

            // Reset the header assembler.
            _headerAssembler.Reset();
            if (headerUsed == readableSize) // If readable size equal headerUsed, just return.
                return LLBC_RTN_OK;

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

        // Content packet content.
        size_t contentNeedRecv = _payloadNeedRecv - _payloadRecved;
        if (readableSize < contentNeedRecv) // if the readable data size < content need receive size, copy the data and return.
        {
            _packet->Write(readableBuf, readableSize);
#if LLBC_TARGET_PLATFORM_WIN32 && defined(_WIN64)
            _payloadRecved += static_cast<int>(readableSize);
#else
            _payloadRecved += readableSize;
#endif // target platform is WIN32 and defined _WIN64 macro.

            return LLBC_RTN_OK;
        }

        // Readable data size >= content need receive size.
        _packet->Write(readableBuf, contentNeedRecv);
        if (!out)
            out = LLBC_New1(LLBC_MessageBlock, sizeof(LLBC_Packet *));
        (reinterpret_cast<LLBC_MessageBlock *>(out))->Write(&_packet, sizeof(LLBC_Packet *));

        // Reset packet about data members.
        _packet = NULL;
        _payloadRecved = 0;
        _payloadNeedRecv = 0;

        // Shift block read position.
#if LLBC_TARGET_PLATFORM_WIN32 && defined(_WIN64)
        block->ShiftReadPos(static_cast<long>(contentNeedRecv));
#else
        block->ShiftReadPos(contentNeedRecv);
#endif // target platform is WIN32 and defined _WIN64 macro.
    }

    return LLBC_RTN_OK;
}

int LLBC_PacketProtocol::AddCoder(int opcode, LLBC_ICoderFactory *coder)
{
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_RTN_FAILED;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
