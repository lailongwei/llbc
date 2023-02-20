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

#include "csllbc/common/Common.h"
#include "csllbc/core/Core.h"

#include "csllbc/comm/csService.h"

/**
 * \brief The csharp coder class encapsulation.
 */
class CSLLBC_HIDDEN csllbc_Coder : public LLBC_Coder
{
    typedef csllbc_Delegates _D;

public:
    typedef csllbc_Service::PacketDecodeDelegates PacketDecodeDelegates;

public:
    /**
     *Constructor.
     */
    csllbc_Coder();

    /**
     * Destructor.
     */
    virtual ~csllbc_Coder();
    
public:
    /**
     * Set csharp layer encode info.
     */
    void SetEncodeInfo(sint64 packetId, _D::Deleg_Service_EncodePacket encodeDeleg);

    /**
     * Get packet decode delegates.
     * @return PacketDecodeDelegates * - the packet decode delegates.
     */
    PacketDecodeDelegates *GetDecodeDelegates();

public:
    /**
     * Packet encode/decode methods.
     */
    virtual bool Encode(LLBC_Packet &packet);
    virtual bool Decode(LLBC_Packet &packet);

private:
    sint64 _packetId;
    _D::Deleg_Service_EncodePacket _encodeDeleg;

    PacketDecodeDelegates *_decodeDelegs;
};

/**
 * \brief The csharp codr factory class encapsulation.
 */
class CSLLBC_HIDDEN csllbc_CoderFactory : public LLBC_CoderFactory
{
public:
    /**
     * Create csharp layer coder.
     */
    virtual LLBC_Coder *Create() const;

private:
    sint64 _packetId;
};


