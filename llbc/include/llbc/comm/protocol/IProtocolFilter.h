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
 * \brief The protocol filter class encapsulation.
 */
class LLBC_EXPORT LLBC_IProtocolFilter
{
public:
    virtual ~LLBC_IProtocolFilter() { }

public:
    /**
     * When packet send, will call this filter method to filter packet.
     * @param[in] packet - packet.
     * @return int - return 0 tell service send this packet, if return -1, this packet will discard.
     */
    virtual int FilterSend(const LLBC_Packet &packet) = 0;

    /**
     * When packet received, will call this filter method to filter packet.
     * @param[in] packet - packet.
     * @return int - return 0 if want to receive this packet, if return -1, this packet will discard.
     */
    virtual int FilterRecv(const LLBC_Packet &packet) = 0;

    /**
     * When one connection established, will call this filter method to filter packet.
     * @param[in] local - the local socket address.
     * @param[in] peer  - the peer socket address.
     * @return int - return 0 if want to accept this connect, if return -1, will close this connection.
     */
    virtual int FilterConnect(const LLBC_SockAddr_IN &local, const LLBC_SockAddr_IN &peer) = 0;
};

__LLBC_NS_END
