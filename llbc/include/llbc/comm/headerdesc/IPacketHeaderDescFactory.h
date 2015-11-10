/**
 * @file    IPacketHeaderDescFactory.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_IPACKET_HEADER_DESC_FACTORY_H__
#define __LLBC_COMM_IPACKET_HEADER_DESC_FACTORY_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN

/**
 * Previous declare LLBC_PacketHeaderDesc class.
 */
class LLBC_PacketHeaderDesc;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The packet header describe factory interface class encapsulation.
 */
class LLBC_EXPORT LLBC_IPacketHeaderDescFactory
{
public:
    virtual ~LLBC_IPacketHeaderDescFactory() {  }

public:
    /**
     * Create the packet header describe.
     * @return LLBC_PacketHeaderDesc * - the packet header describe.
     */
    virtual LLBC_PacketHeaderDesc *Create() const = 0;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_IPACKET_HEADER_DESC_FACTORY_H__
