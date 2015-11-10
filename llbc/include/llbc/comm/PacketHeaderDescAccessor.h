/**
 * @file    PacketHeaderDescAccessor.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_PACKET_HEADER_DESC_ACCESSOR_H__
#define __LLBC_COMM_PACKET_HEADER_DESC_ACCESSOR_H__

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
 * \brief The packet header describe cleaner encapsulation.
 */
class LLBC_HIDDEN LLBC_PacketHeaderDescAccessor
{
public:
    /**
     * Get the packet header describe.
     * @param[in] tryCreate - if not set any header describe, will auto create llbc library default header describe.
     * @return LLBC_PacketHeaderDesc * - the packet header describe.
     */
    static const LLBC_PacketHeaderDesc *GetHeaderDesc(bool tryCreate = true);

    /**
     * Set the packet header describe.
     * @param[in] headerDesc - the packet header describe.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int SetPacketDesc(LLBC_PacketHeaderDesc *headerDesc);

public:
    /**
     * Cleanup the packet header describe.
     */
    static void CleanupHeaderDesc();

private:
    static LLBC_PacketHeaderDesc *_headerDesc;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_PACKET_HEADER_DESC_ACCESSOR_H__
