/**
 * @file    RawProtocol.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/12
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/PacketHeaderDescAccessor.h"
#include "llbc/comm/headerdesc/PacketHeaderDesc.h"

#include "llbc/comm/protocol/ProtocolLayer.h"
#include "llbc/comm/protocol/IProtocol.h"

__LLBC_NS_BEGIN

LLBC_RawProtocol::LLBC_RawProtocol()
: _headerLen(0)
, _lenPartId(0)

, _lenOffset(0)
, _lenSize(0)
{
    typedef LLBC_PacketHeaderDescAccessor _HDAccessor;

    const LLBC_PacketHeaderDesc *headerDesc = _HDAccessor::GetHeaderDesc();

    _headerLen = headerDesc->GetHeaderLen();
    _lenPartId = headerDesc->GetLenPart()->GetSerialNo();

    _lenOffset = headerDesc->GetLenPartOffset();
    _lenSize = headerDesc->GetLenPartLen();
}

LLBC_RawProtocol::~LLBC_RawProtocol()
{
}

int LLBC_RawProtocol::GetLayer() const
{
    return LLBC_ProtocolLayer::PackLayer;
}

int LLBC_RawProtocol::Connect(LLBC_SockAddr_IN &local, LLBC_SockAddr_IN &peer)
{
    return LLBC_OK;
}

int LLBC_RawProtocol::Send(void *in, void *&out, bool &removeSession)
{
    LLBC_Packet *packet = reinterpret_cast<LLBC_Packet *>(in);
    LLBC_MessageBlock *block = packet->GiveUp();

    block->SetReadPos(_headerLen);
    out = block;

    LLBC_Delete(packet);

    return LLBC_OK;
}

int LLBC_RawProtocol::Recv(void *in, void *&out, bool &removeSession)
{
    LLBC_MessageBlock *block = reinterpret_cast<LLBC_MessageBlock *>(in);

    // Create packet and write data.
    LLBC_Packet *packet = LLBC_New(LLBC_Packet);
    const size_t readableSize = block->GetReadableSize();
    packet->Write(block->GetDataStartWithReadPos(), readableSize);

    // Write length part.
    packet->SetHeaderPartVal(_lenPartId, static_cast<sint32>(readableSize));

    // Delete this block.
    LLBC_Delete(block);

    // Create output.
    out = LLBC_New1(LLBC_MessageBlock, sizeof(LLBC_Packet *));
    (reinterpret_cast<LLBC_MessageBlock *>(out))->Write(&packet, sizeof(LLBC_Packet *));

    return LLBC_OK;
}

int LLBC_RawProtocol::AddCoder(int opcode, LLBC_ICoderFactory *coder)
{
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
