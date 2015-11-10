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
     * @param[in] in   - the in data.
     *                  in this protocol, in data type: LLBC_Packet *.
     * @param[out] out - the out data.
     *                  in this protocol, out data type: LLBC_MessageBlock *.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Send(void *in, void *&out);

    /**
     * When data received, will call this method.
     * @param[in]  in  - the in data.
     *                  in this protocol, in data type: LLBC_MessageBlock.
     * @param[out] out - the out data.
     *                  in this protocol, out data type: LLBC_MessageBlock *, NULL if not packet constructed.
     *                  in LLBC_MessageBlock, store the LLBC_Packet * list.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Recv(void *in, void *&out);

    /**
     * Add coder factory to protocol, only available in Codec-Layer.
     * @param[in] opcode - the opcode.
     * @param[in] coder  - the coder factory
     *  Note: When protocol deleted, it will not delete coder pointer,
     *        It means that you must self manage your coder memory.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int AddCoder(int opcode, LLBC_ICoderFactory *coder);

private:
    LLBC_PacketHeaderAssembler _headerAssembler;

    LLBC_Packet *_packet;
    int _payloadNeedRecv;
    int _payloadRecved;

    const int _headerIncludedLen;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_PACKET_PROTOCOL_H__
