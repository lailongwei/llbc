/**
 * @file    CodecProtocol.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/12
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/ICoder.h"
#include "llbc/comm/protocol/ProtocolLayer.h"
#include "llbc/comm/protocol/IProtocol.h"

__LLBC_NS_BEGIN

LLBC_CodecProtocol::LLBC_CodecProtocol()
{
}

LLBC_CodecProtocol::~LLBC_CodecProtocol()
{
}

int LLBC_CodecProtocol::GetLayer() const
{
    return LLBC_ProtocolLayer::CodecLayer;
}

int LLBC_CodecProtocol::Connect(LLBC_SockAddr_IN &local, LLBC_SockAddr_IN &peer)
{
    return LLBC_RTN_OK;
}

int LLBC_CodecProtocol::Send(void *in, void *&out)
{
    out = in;
    reinterpret_cast<LLBC_Packet *>(in)->Encode();

    return LLBC_RTN_OK;
}

int LLBC_CodecProtocol::Recv(void *in, void *&out)
{
    out = in;
    LLBC_Packet *packet = reinterpret_cast<LLBC_Packet *>(in);
    _Coders::iterator it = _coders.find(packet->GetOpcode());
    if (it != _coders.end())
    {
        LLBC_ICoder *coder = it->second->Create();
        coder->Decode(*packet);

        packet->SetDecoder(coder);
    }

    return LLBC_RTN_OK;
}

int LLBC_CodecProtocol::AddCoder(int opcode, LLBC_ICoderFactory *coder)
{
    if (!_coders.insert(std::make_pair(opcode, coder)).second)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
