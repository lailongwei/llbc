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
    free(_header);
}

bool LLBC_PacketHeaderAssembler::Assemble(const void *data, size_t len ,size_t &used)
{
    used = 0;
    if (_curRecved == _headerLen)
        return true;

    size_t needSize = _headerLen - _curRecved;
    if (len >= needSize)
    {
        memcpy(_header + _curRecved, data, needSize);
        _curRecved = _headerLen;

        used = needSize;

        return true;
    }
    else
    {
        memcpy(_header + _curRecved, data, len);
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
    sint64 extData1 = *reinterpret_cast<sint64 *>(_header + 20);

#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
    LLBC_Net2Host(len);
    LLBC_Net2Host(opcode);
    LLBC_Net2Host(status);
    LLBC_Net2Host(senderServiceId);
    LLBC_Net2Host(recverServiceId);
    LLBC_Net2Host(flags);
    LLBC_Net2Host(extData1);
#endif

    packet.SetLength(len);
    packet.SetOpcode(opcode);
    packet.SetStatus(status);
    packet.SetSenderServiceId(senderServiceId);
    packet.SetRecverServiceId(recverServiceId);
    packet.SetFlags(flags);
    packet.SetExtData1(extData1);
}

__LLBC_NS_END
