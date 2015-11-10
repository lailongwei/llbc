/**
 * @file    IProtocolFilter.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/01/10
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_IPROTOCOL_FILTER_H__
#define __LLBC_COMM_IPROTOCOL_FILTER_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

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

#endif // !__LLBC_COMM_IPROTOCOL_FILTER_H__
