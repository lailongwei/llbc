/**
 * @file    CompressProtocol.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/12
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_COMPRESS_PROTOCOL_H__
#define __LLBC_COMM_COMPRESS_PROTOCOL_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

#include "llbc/comm/protocol/IProtocol.h"

__LLBC_NS_BEGIN

/**
 * \brief The Compress-Layer protocol implement.
 */
class LLBC_EXPORT LLBC_CompressProtocol : public LLBC_IProtocol
{
public:
    LLBC_CompressProtocol();
    virtual ~LLBC_CompressProtocol();

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
     * @param[out] out - the out data.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Send(void *in, void *&out);

    /**
     * When data received, will call this method.
     * @param[in] in   - the in data.
     * @param[out] out - the out data.
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
};

__LLBC_NS_END

#endif // !__LLBC_COMM_COMPRESS_PROTOCOL_H__
