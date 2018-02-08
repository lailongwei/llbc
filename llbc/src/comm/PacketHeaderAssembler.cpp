/**
 * @file    PacketHeaderAssembler.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/30
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/Packet.h"
#include "llbc/comm/PacketHeaderAssembler.h"

__LLBC_NS_BEGIN

LLBC_PacketHeaderAssembler::LLBC_PacketHeaderAssembler(size_t headerLen)
: _headerLen(headerLen)
, _header(LLBC_Malloc(char, headerLen))

, _curRecved(0)
{
}

LLBC_PacketHeaderAssembler::~LLBC_PacketHeaderAssembler()
{
    LLBC_Free(_header);
}

bool LLBC_PacketHeaderAssembler::Assemble(const void *data, size_t len ,size_t &used)
{
    used = 0;
    if (_curRecved == _headerLen)
        return true;

    size_t needSize = _headerLen - _curRecved;
    if (len >= needSize)
    {
        ::memcpy(_header + _curRecved, data, needSize);
        _curRecved = _headerLen;

        used = needSize;

        return true;
    }
    else
    {
        ::memcpy(_header + _curRecved, data, len);
        _curRecved += len;

        used = len;

        return false;
    }
}

void LLBC_PacketHeaderAssembler::Reset()
{
    _curRecved = 0;
}

void LLBC_PacketHeaderAssembler::SetToPacket(LLBC_Packet &packet) const
{
    uint32 len = *reinterpret_cast<uint32 *>(_header);
    sint32 opcode = *reinterpret_cast<sint32 *>(_header + 4);
    uint16 status = *reinterpret_cast<uint16 *>(_header + 8);
    int senderServiceId = *reinterpret_cast<int *>(_header + 10);
    int recverServiceId = *reinterpret_cast<int *>(_header + 14);
    uint16 flags = *reinterpret_cast<uint16 *>(_header + 18);

#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
    LLBC_Net2Host(len);
    LLBC_Net2Host(opcode);
    LLBC_Net2Host(status);
    LLBC_Net2Host(senderServiceId);
    LLBC_Net2Host(recverServiceId);
    LLBC_Net2Host(flags);
#endif

    packet.SetLength(len);
    packet.SetOpcode(opcode);
    packet.SetStatus(status);
    packet.SetSenderServiceId(senderServiceId);
    packet.SetRecverServiceId(recverServiceId);
    packet.SetFlags(flags);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
