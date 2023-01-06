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

#include "llbc/comm/Coder.h"
#include "llbc/comm/Packet.h"

#include "llbc/comm/protocol/ProtocolLayer.h"
#include "llbc/comm/protocol/ProtoReportLevel.h"
#include "llbc/comm/protocol/CodecProtocol.h"
#include "llbc/comm/protocol/ProtocolStack.h"

__LLBC_NS_BEGIN

LLBC_CodecProtocol::LLBC_CodecProtocol()
{
}

LLBC_CodecProtocol::~LLBC_CodecProtocol()
{
}

int LLBC_CodecProtocol::GetLayer() const
{
    return LLBC_ProtocolLayer::CodecLayer;
}

int LLBC_CodecProtocol::Send(void *in, void *&out, bool &removeSession)
{
    // Set packet payload(from object-pool).
    LLBC_Packet *packet = reinterpret_cast<LLBC_Packet *>(in);
    if (packet->GetEncoder())
    {
        // Encode coder to packet.
        if (UNLIKELY(!packet->Encode()))
        {
            LLBC_String reportMsg = LLBC_String().format(
                "Encode packet failed, opcode: %d, payloadLen: %ld", packet->GetOpcode(), packet->GetPayloadLength());

            const LLBC_String &codecErr = packet->GetCodecError();
            if (!codecErr.empty())
                reportMsg.append_format("\ndetail error:%s", codecErr.c_str());

            _stack->Report(packet->GetSessionId(),
                           packet->GetOpcode(),
                           this,
                           LLBC_ProtoReportLevel::Warn,
                           reportMsg);

            removeSession = false;

            LLBC_Recycle(packet);
            LLBC_SetLastError(LLBC_ERROR_ENCODE);

            return LLBC_FAILED;
        }
    }

    out = packet;
    return LLBC_OK;
}

int LLBC_CodecProtocol::Recv(void *in, void *&out, bool &removeSession)
{
    LLBC_Packet *packet = reinterpret_cast<LLBC_Packet *>(in);
    Coders::const_iterator it = _coders->find(packet->GetOpcode());
    if (it != _coders->end())
    {
        LLBC_Coder *coder = it->second->Create();
        if (UNLIKELY(!coder->Decode(*packet)))
        {
            LLBC_String reportMsg = LLBC_String().format(
                "Decode packet failed, opcode: %d, payloadLen: %ld",
                packet->GetOpcode(),
                packet->GetPayloadLength());

            const LLBC_String &codecErr = packet->GetCodecError();
            if (!codecErr.empty())
                reportMsg.append_format("\ndetail error:%s", codecErr.c_str());

            _stack->Report(packet->GetSessionId(),
                           packet->GetOpcode(),
                           this,
                           LLBC_ProtoReportLevel::Error,
                           reportMsg);

            removeSession = true;

            LLBC_Recycle(coder);
            LLBC_Recycle(packet);
            LLBC_SetLastError(LLBC_ERROR_DECODE);

            return LLBC_FAILED;
        }

        packet->SetDecoder(coder);
    }
    else if (!_stack->GetIsSuppressedCoderNotFoundWarning())
    {
        const LLBC_String reportMsg = LLBC_String().format(
            "Coder not found, decode packet failed, opcode: %d, payloadLen: %ld",
            packet->GetOpcode(),
            packet->GetPayloadLength());
        _stack->Report(packet->GetSessionId(),
                       packet->GetOpcode(),
                       this,
                       LLBC_ProtoReportLevel::Warn,
                       reportMsg);

        removeSession = false;

        LLBC_Recycle(packet);
        LLBC_SetLastError(LLBC_ERROR_ENCODE);

        return LLBC_FAILED;
    }

    out = packet;
    return LLBC_OK;
}

__LLBC_NS_END
