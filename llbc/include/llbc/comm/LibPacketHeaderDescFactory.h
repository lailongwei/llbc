/**
 * @file    LibPacketHeaderDescFactory.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_LIB_PACKET_HEADER_DESC_FACTORY_H__
#define __LLBC_COMM_LIB_PACKET_HEADER_DESC_FACTORY_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

#include "llbc/comm/headerdesc/PacketHeaderDesc.h"

__LLBC_NS_BEGIN

/**
 * The llbc library default packet header describe factory encapsulation.
 * Header Format.
 *   |    Type   | Offset |  Len |
 * --|-----------|--------|------|--
 *   |   Length  |    0   |   4  |
 *   |   Opcode  |    4   |   2  |
 *   |   Status  |    6   |   2  |
 *   | ServiceId |    8   |   4  |
 *   |   Flags   |   12   |   2  |
 *Header total length: 14 bytes.
 */
class LLBC_EXPORT LLBC_LibPacketHeaderDescFactory : public LLBC_IPacketHeaderDescFactory
{
public:
    virtual LLBC_PacketHeaderDesc *Create() const;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_LIB_PACKET_HEADER_DESC_FACTORY_H__
