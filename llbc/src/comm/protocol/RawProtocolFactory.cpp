/**
 * @file    RawProtocolFactory.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2018/02/07
 * @version 1.0
 * 
 * @brief   
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/protocol/ProtocolLayer.h"
#include "llbc/comm/protocol/RawProtocol.h"
#include "llbc/comm/protocol/RawProtocolFactory.h"

__LLBC_NS_BEGIN

LLBC_IProtocol *LLBC_RawProtocolFactory::Create(int layer) const
{
    switch (layer)
    {
    case LLBC_ProtocolLayer::PackLayer:
        return LLBC_New(LLBC_RawProtocol);

    default:
        return NULL;
    }
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
