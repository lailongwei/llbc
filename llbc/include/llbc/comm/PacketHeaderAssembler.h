/**
 * @file    PacketHeaderAssembler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/29
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_PACKET_HEADER_ASSEMBLER_H__
#define __LLBC_COMM_PACKET_HEADER_ASSEMBLER_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

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
    LLBC_PacketHeaderAssembler(size_t headerLen);
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

#endif // !__LLBC_COMM_PACKET_HEADER_ASSEMBLER_H__
