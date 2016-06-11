/**
 * @file    csCoder.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/23
 * @version 1.0
 *
 * @brief
 */

#include "csllbc/common/Export.h"

#include "csllbc/comm/csCoder.h"

csllbc_Coder::csllbc_Coder()
: _packetId(0)
, _encodeDeleg(NULL)

, _decodeDelegs(NULL)
{
}

csllbc_Coder::~csllbc_Coder()
{
}

void csllbc_Coder::SetEncodeInfo(sint64 packetId, _D::Deleg_Service_EncodePacket encodeDeleg)
{
    _packetId = packetId;
    _encodeDeleg = encodeDeleg;
}

csllbc_Coder::PacketDecodeDelegates *csllbc_Coder::GetDecodeDelegates()
{
    return _decodeDelegs;
}

bool csllbc_Coder::Encode(LLBC_Packet &packet)
{
    // Call csharp managed encode delegate to encode data.
    int errMsgLen, encodedLen;
    int encodedSucceed = 0;
    void *data = (*_encodeDeleg)(packet.GetSessionId(),
                                 _packetId,
                                 &encodedSucceed,
                                 &encodedLen,
                                 &errMsgLen);
    if (encodedSucceed == 0) // Encode failed
    {
        if (data)
        {
            packet.SetCodecError(LLBC_String(reinterpret_cast<char *>(data), errMsgLen));
            LLBC_Free(data);
        }

        return false;
    }

    // Copy managed encoded data bytes to packet.
    if (data)
    {
        packet.Write(data, encodedLen);
        LLBC_Free(data);
    }

    return true;
}

bool csllbc_Coder::Decode(LLBC_Packet &packet)
{
    int errMsgLen;
    _decodeDelegs = csllbc_Service::GetPacketDecodeDelegates(packet.GetServiceId());
    void *decodeRet = (*_decodeDelegs->decodeDeleg)(packet.GetSessionId(),
                                                    packet.GetOpcode(),
                                                    packet.GetPayload(),
                                                    static_cast<int>(packet.GetPayloadLength()),
                                                    packet.GetStatus(),
                                                    &errMsgLen);
    if (decodeRet == NULL)
        return true;

    packet.SetCodecError(LLBC_String(reinterpret_cast<char *>(decodeRet), errMsgLen));
    LLBC_Free(decodeRet);

    return false;
}

LLBC_ICoder *csllbc_CoderFactory::Create() const
{
    return LLBC_New(csllbc_Coder);
}