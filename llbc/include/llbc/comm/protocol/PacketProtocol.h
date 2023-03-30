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

#pragma once

#include "llbc/comm/Packet.h"
#include "llbc/comm/protocol/IProtocol.h"
#include "llbc/comm/PacketHeaderAssembler.h"

__LLBC_NS_BEGIN

/**
 * \brief The Pack-Layer protocol implement.
 *        It will construct/destruct LLBC_Packet to/from byte stream.
 * 
 * Library default header format.
 *   |       Type       | Offset |  Len |
 * --|------------------|--------|------|--
 *   |      Length      |    0   |   4  |
 *   |      Opcode      |    4   |   4  |
 *   |      Status      |    8   |   2  |
 *   | Sender ServiceId |   10   |   4  |
 *   | Recver ServiceId |   14   |   4  |
 *   |      Flags       |   18   |   2  |
 *   |     ExtData1     |   20   |   8  |
 *Header total length: 28 bytes.
 */
class LLBC_EXPORT LLBC_PacketProtocol : public LLBC_IProtocol
{
public:
    /**
     * Constructor & Destructor.
     */
    LLBC_PacketProtocol();
    virtual ~LLBC_PacketProtocol();

public:
    /**
     * Get the protocol layer.
     * @return int - the protocol layer.
     */
    virtual int GetLayer() const;

public:
    /**
     * When data send, will call this method.
     * @param[in] in             - the in data.
     *                             in this protocol, in data type: LLBC_Packet *.
     * @param[out] out           - the out data.
     *                             in this protocol, out data type: LLBC_MessageBlock *.
     * @param[out] removeSession - when error occurred, this out param determine remove session or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Send(void *in, void *&out, bool &removeSession);

    /**
     * When data received, will call this method.
     * @param[in]  in            - the in data.
     *                             in this protocol, in data type: LLBC_MessageBlock.
     * @param[out] out           - the out data.
     *                             in this protocol, out data type: LLBC_MessageBlock *, nullptr if not packet constructed.
     *                             in LLBC_MessageBlock, store the LLBC_Packet * list.
     * @param[out] removeSession - when error occurred, this out param determine remove session or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Recv(void *in, void *&out, bool &removeSession);

private:
    LLBC_PacketHeaderAssembler _headerAssembler;

    LLBC_Packet *_packet;
    size_t _payloadNeedRecv;
    size_t _payloadRecved;
};

__LLBC_NS_END
