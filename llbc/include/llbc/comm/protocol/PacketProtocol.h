/**
 * @file    PacketProtocol.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/12
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_PACKET_PROTOCOL_H__
#define __LLBC_COMM_PACKET_PROTOCOL_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

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
 *Header total length: 20 bytes.
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
     * When one connection established, will call this method.
     * @param[in] local - the local address.
     * @param[in] peer  - the peer address.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Connect(LLBC_SockAddr_IN &local, LLBC_SockAddr_IN &peer);

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
     *                             in this protocol, out data type: LLBC_MessageBlock *, NULL if not packet constructed.
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

#endif // !__LLBC_COMM_PACKET_PROTOCOL_H__
