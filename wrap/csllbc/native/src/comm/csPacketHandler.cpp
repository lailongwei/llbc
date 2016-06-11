/**
 * @file    csPacketHandler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/23
 * @version 1.0
 *
 * @brief
 */

#include "csllbc/common/Export.h"

#include "csllbc/comm/csCoder.h"
#include "csllbc/comm/csPacketHandler.h"

namespace
{
    typedef csllbc_Coder::PacketDecodeDelegates _DecodeDelegs;
}

csllbc_PacketHandler::csllbc_PacketHandler(_NotFoundDecoderReport notFoundDecoderReport)
: _notFoundDecoderReport(notFoundDecoderReport)
{
}

void csllbc_PacketHandler::Handle(LLBC_Packet &packet)
{
    csllbc_Coder *coder = (csllbc_Coder *)packet.GetDecoder();
    if (coder == NULL &&
            packet.GetPreHandleResult() == NULL)
        ReportNotFoundDecoder(Phase_PreHandle, packet);

    _DecodeDelegs *delegs = coder->GetDecodeDelegates();
    (*delegs->handlerDeleg)(packet.GetSessionId(),
                            packet.GetOpcode(),
                            packet.GetPayload());
}

void *csllbc_PacketHandler::PreHandle(LLBC_Packet &packet)
{
    csllbc_Coder *coder = (csllbc_Coder *)packet.GetDecoder();
    if (coder == NULL &&
            packet.GetPreHandleResult() == NULL)
        ReportNotFoundDecoder(Phase_PreHandle, packet);

    _DecodeDelegs *delegs = coder->GetDecodeDelegates();
    return (*delegs->preHandlerDeleg)(packet.GetSessionId(),
                                      packet.GetOpcode(),
                                      packet.GetPayload()) ? (void *)1 : NULL;
}

void *csllbc_PacketHandler::UnifyPreHandle(LLBC_Packet &packet)
{
    csllbc_Coder *coder = (csllbc_Coder *)packet.GetDecoder();
    if (coder == NULL &&
            packet.GetPreHandleResult() == NULL)
        ReportNotFoundDecoder(Phase_PreHandle, packet);

    _DecodeDelegs *delegs = coder->GetDecodeDelegates();
    return (*delegs->unifyPreHandlerDeleg)(packet.GetSessionId(),
                                           packet.GetOpcode(),
                                           packet.GetPayload()) ? (void *)1 : NULL;
}

void csllbc_PacketHandler::ReportNotFoundDecoder(int phase, LLBC_Packet &packet)
{
    (*_notFoundDecoderReport)(packet.GetSessionId(),
                              packet.GetOpcode(),
                              packet.GetPayload(),
                              static_cast<int>(packet.GetPayloadLength()),
                              packet.GetStatus());
    packet.SetPreHandleResult<csllbc_PacketHandler>((void *)1, NULL, NULL);
}
