/**
 * @file    ProtocolLayer.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/12
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/protocol/ProtocolLayer.h"

namespace
{
    typedef LLBC_NS LLBC_ProtocolLayer This;
}

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_String __g_descs[] =
{
    "PackLayer",
    "CompressLayer",
    "CodecLayer",

    "Invalid"
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

bool LLBC_ProtocolLayer::IsValid(int layer)
{
    return (This::Begin <= layer && layer < This::End);
}

const LLBC_String &LLBC_ProtocolLayer::Layer2Str(int layer)
{
    return LLBC_INL_NS __g_descs[This::IsValid(layer) ? layer : This::End];
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
