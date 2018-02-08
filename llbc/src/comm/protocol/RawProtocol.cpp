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

#include "llbc/comm/Packet.h"

#include "llbc/comm/protocol/ProtocolLayer.h"
#include "llbc/comm/protocol/RawProtocol.h"

#include "llbc/comm/IService.h"

__LLBC_NS_BEGIN

LLBC_RawProtocol::LLBC_RawProtocol()
{
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
    LLBC_Delete(packet);

    if (!block)
        return LLBC_OK;

    out = block;
    return LLBC_OK;
}

int LLBC_RawProtocol::Recv(void *in, void *&out, bool &removeSession)
{
    LLBC_MessageBlock *block = reinterpret_cast<LLBC_MessageBlock *>(in);

    // Create packet and write data.
    LLBC_Packet *packet = LLBC_New(LLBC_Packet);
    const size_t readableSize = block->GetReadableSize();

    packet->SetLength(readableSize);
    packet->SetSessionId(_sessionId);
    packet->Write(block->GetDataStartWithReadPos(), readableSize);

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
