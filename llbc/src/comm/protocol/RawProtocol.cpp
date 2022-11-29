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

#include "llbc/comm/protocol/ProtocolLayer.h"
#include "llbc/comm/protocol/RawProtocol.h"

#include "llbc/comm/Service.h"

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

int LLBC_RawProtocol::Send(void *in, void *&out, bool &removeSession)
{
    LLBC_Packet *packet = reinterpret_cast<LLBC_Packet *>(in);

    LLBC_MessageBlock *block = packet->GiveUpPayload();
    LLBC_Recycle(packet);

    if (!block)
        return LLBC_OK;

    out = block;
    return LLBC_OK;
}

int LLBC_RawProtocol::Recv(void *in, void *&out, bool &removeSession)
{
    LLBC_MessageBlock *block = reinterpret_cast<LLBC_MessageBlock *>(in);

    // Create packet and write data.
    LLBC_Packet *packet = _pktPoolInst->GetObject();
    const size_t readableSize = block->GetReadableSize();

    packet->SetLength(readableSize);
    packet->SetSessionId(_sessionId);
    packet->SetPayload(block);

    // Create output.
    out = new LLBC_MessageBlock(sizeof(LLBC_Packet *));
    (reinterpret_cast<LLBC_MessageBlock *>(out))->Write(&packet, sizeof(LLBC_Packet *));

    return LLBC_OK;
}

int LLBC_RawProtocol::AddCoder(int opcode, LLBC_CoderFactory *coder)
{
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
}

__LLBC_NS_END
