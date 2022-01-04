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
    if (coder == nullptr &&
            packet.GetPreHandleResult() == nullptr)
        ReportNotFoundDecoder(Phase_PreHandle, packet);

    _DecodeDelegs *delegs = coder->GetDecodeDelegates();
    (*delegs->handlerDeleg)(packet.GetSessionId(),
                            packet.GetOpcode(),
                            packet.GetPayload());
}

bool csllbc_PacketHandler::PreHandle(LLBC_Packet &packet)
{
    csllbc_Coder *coder = (csllbc_Coder *)packet.GetDecoder();
    if (coder == nullptr &&
            packet.GetPreHandleResult() == nullptr)
        ReportNotFoundDecoder(Phase_PreHandle, packet);

    _DecodeDelegs *delegs = coder->GetDecodeDelegates();
    return (*delegs->preHandlerDeleg)(packet.GetSessionId(),
                                      packet.GetOpcode(),
                                      packet.GetPayload()) != 0;
}

bool csllbc_PacketHandler::UnifyPreHandle(LLBC_Packet &packet)
{
    csllbc_Coder *coder = (csllbc_Coder *)packet.GetDecoder();
    if (coder == nullptr &&
            packet.GetPreHandleResult() == nullptr)
        ReportNotFoundDecoder(Phase_PreHandle, packet);

    _DecodeDelegs *delegs = coder->GetDecodeDelegates();
    return (*delegs->unifyPreHandlerDeleg)(packet.GetSessionId(),
                                           packet.GetOpcode(),
                                           packet.GetPayload()) != 0;
}

void csllbc_PacketHandler::ReportNotFoundDecoder(int phase, LLBC_Packet &packet)
{
    (*_notFoundDecoderReport)(packet.GetSessionId(),
                              packet.GetOpcode(),
                              packet.GetPayload(),
                              static_cast<int>(packet.GetPayloadLength()),
                              packet.GetStatus());
    packet.SetPreHandleResult<csllbc_PacketHandler>((void *)1, nullptr, nullptr);
}
