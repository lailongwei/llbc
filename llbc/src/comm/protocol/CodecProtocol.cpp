/**
 * @file    CodecProtocol.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/12
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/ICoder.h"
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

int LLBC_CodecProtocol::Connect(LLBC_SockAddr_IN &local, LLBC_SockAddr_IN &peer)
{
    return LLBC_OK;
}

int LLBC_CodecProtocol::Send(void *in, void *&out, bool &removeSession)
{
    LLBC_Packet *packet = reinterpret_cast<LLBC_Packet *>(in);
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

        LLBC_Delete(packet);
        LLBC_SetLastError(LLBC_ERROR_ENCODE);

        return LLBC_FAILED;
    }

    out = in;
    return LLBC_OK;
}

int LLBC_CodecProtocol::Recv(void *in, void *&out, bool &removeSession)
{
    LLBC_Packet *packet = reinterpret_cast<LLBC_Packet *>(in);
    Coders::const_iterator it = _coders->find(packet->GetOpcode());
    if (it != _coders->end())
    {
        LLBC_ICoder *coder = it->second->Create();
        if (UNLIKELY(!coder->Decode(*packet)))
        {
            LLBC_String reportMsg = LLBC_String().format(
                    "Decode packet failed, opcode: %d, payloadLen: %ld", packet->GetOpcode(), packet->GetPayloadLength());

            const LLBC_String &codecErr = packet->GetCodecError();
            if (!codecErr.empty())
                reportMsg.append_format("\ndetail error:%s", codecErr.c_str());

            _stack->Report(packet->GetSessionId(),
                           packet->GetOpcode(),
                           this,
                           LLBC_ProtoReportLevel::Error,
                           reportMsg);

            removeSession = true;

            LLBC_Delete(coder);
            LLBC_Delete(packet);
            LLBC_SetLastError(LLBC_ERROR_DECODE);

            return LLBC_FAILED;
        }

        packet->SetDecoder(coder);
    }
    else if (!_stack->_suppressCoderNotFoundError)
    {
        const LLBC_String reportMsg = LLBC_String().format(
            "Coder not found, decode packet failed, opcode: %d, payloadLen: %ld", packet->GetOpcode(), packet->GetPayloadLength());
        _stack->Report(packet->GetSessionId(),
                       packet->GetOpcode(),
                       this,
                       LLBC_ProtoReportLevel::Warn,
                       reportMsg);

        removeSession = false;

        LLBC_Delete(packet);
        LLBC_SetLastError(LLBC_ERROR_ENCODE);

        return LLBC_FAILED;
    }

    out = in;
    return LLBC_OK;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
