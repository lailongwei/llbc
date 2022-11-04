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

csllbc_Coder::csllbc_Coder()
: _packetId(0)
, _encodeDeleg(nullptr)

, _decodeDelegs(nullptr)
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
            free(data);
        }

        return false;
    }

    // Copy managed encoded data bytes to packet.
    if (data)
    {
        packet.Write(data, encodedLen);
        free(data);
    }

    return true;
}

bool csllbc_Coder::Decode(LLBC_Packet &packet)
{
    int errMsgLen;
    _decodeDelegs = csllbc_Service::GetPacketDecodeDelegates(packet.GetSenderServiceId());
    void *decodeRet = (*_decodeDelegs->decodeDeleg)(packet.GetSessionId(),
                                                    packet.GetOpcode(),
                                                    packet.GetPayload(),
                                                    static_cast<int>(packet.GetPayloadLength()),
                                                    packet.GetStatus(),
                                                    &errMsgLen);
    if (decodeRet == nullptr)
        return true;

    packet.SetCodecError(LLBC_String(reinterpret_cast<char *>(decodeRet), errMsgLen));
    free(decodeRet);

    return false;
}

LLBC_Coder *csllbc_CoderFactory::Create() const
{
    return new csllbc_Coder;
}