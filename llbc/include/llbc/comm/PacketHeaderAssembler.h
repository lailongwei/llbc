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

#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

/**
 * Previous declare LLBC_PacketHeaderDesc class.
 */
class LLBC_Packet;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The packet header assembler encapsulation.
 */
class LLBC_EXPORT LLBC_PacketHeaderAssembler
{
public:
    /**
     * Constructor & Destructor.
     */
    explicit LLBC_PacketHeaderAssembler(size_t headerLen);
    virtual ~LLBC_PacketHeaderAssembler();

public:
    /**
     * Assemble packet header.
     * @param[in] data - the stream data.
     * @param[in] len  - the stream data length, in bytes.
     * @param[out] len - the used stream data, in bytes..
     * @return int - return 0 if success, otherwise return -1.
     */
    bool Assemble(const void *data, size_t len, size_t &used);

    /**
     * Reset the header assembler.
     */
    void Reset();

public:
    /**
     * Set header content to packet.
     * @param[in] packet - the packet.
     */
    void SetToPacket(LLBC_Packet &packet) const;

private:
    size_t _headerLen;

    char *_header;
    size_t _curRecved;
};

__LLBC_NS_END
