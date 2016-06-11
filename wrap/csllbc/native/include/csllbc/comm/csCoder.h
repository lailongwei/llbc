/**
 * @file    csCoder.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/23
 * @version 1.0
 *
 * @brief
 */
#ifndef __CSLLBC_COMM_CSCODER_H__
#define __CSLLBC_COMM_CSCODER_H__

#include "csllbc/common/Common.h"
#include "csllbc/core/Core.h"

#include "csllbc/comm/csService.h"

/**
 * \brief The csharp coder class encapsulation.
 */
class CSLLBC_HIDDEN csllbc_Coder : public LLBC_ICoder
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
class CSLLBC_HIDDEN csllbc_CoderFactory : public LLBC_ICoderFactory
{
public:
    /**
     * Create csharp layer coder.
     */
    virtual LLBC_ICoder *Create() const;

private:
    sint64 _packetId;
};

#endif // !__CSLLBC_COMM_CSCODER_H__
