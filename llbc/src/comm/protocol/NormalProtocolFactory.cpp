/**
 * @file    NormalProtocolFactory.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2018/02/07
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/protocol/ProtocolLayer.h"
#include "llbc/comm/protocol/PacketProtocol.h"
#include "llbc/comm/protocol/CompressProtocol.h"
#include "llbc/comm/protocol/CodecProtocol.h"
#include "llbc/comm/protocol/NormalProtocolFactory.h"

__LLBC_NS_BEGIN

LLBC_IProtocol *LLBC_NormalProtocolFactory::Create(int layer) const
{
    switch (layer)
    {
    case LLBC_ProtocolLayer::CodecLayer:
        return LLBC_New(LLBC_CodecProtocol);

    case LLBC_ProtocolLayer::CompressLayer:
        return LLBC_New(LLBC_CompressProtocol);

    case LLBC_ProtocolLayer::PackLayer:
        return LLBC_New(LLBC_PacketProtocol);

    default:
        return NULL;
    }
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
