/**
 * @file    CompressProtocol.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/12
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/protocol/ProtocolLayer.h"
#include "llbc/comm/protocol/IProtocol.h"

__LLBC_NS_BEGIN

LLBC_CompressProtocol::LLBC_CompressProtocol()
{
}

LLBC_CompressProtocol::~LLBC_CompressProtocol()
{
}

int LLBC_CompressProtocol::GetLayer() const
{
    return LLBC_ProtocolLayer::CompressLayer;
}

int LLBC_CompressProtocol::Connect(LLBC_SockAddr_IN &local, LLBC_SockAddr_IN &peer)
{
    return LLBC_OK;
}

int LLBC_CompressProtocol::Send(void *in, void *&out, bool &removeSession)
{
    out = in;
    return LLBC_OK;
}

int LLBC_CompressProtocol::Recv(void *in, void *&out, bool &removeSession)
{
    out = in;
    return LLBC_OK;
}

int LLBC_CompressProtocol::AddCoder(int opcode, LLBC_ICoderFactory *coder)
{
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
