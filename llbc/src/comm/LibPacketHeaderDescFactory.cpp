/**
 * @file    LibPacketHeaderDescFactory.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/30
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/LibPacketHeaderDescFactory.h"

namespace
{
    static const int LengthPartNo = 0, LengthPartLen = 4;
    static const int OpcodePartNo = 1, OpcodePartLen = 2;
    static const int StatusPartNo = 2, StatusPartLen = 2;
    static const int ServiceIdPartNo = 3, ServiceIdPartLen = 4;
    static const int FlagsPartNo = 4, FlagsPartLen = 2;
}

__LLBC_NS_BEGIN

LLBC_PacketHeaderDesc *LLBC_LibPacketHeaderDescFactory::Create() const
{
    LLBC_PacketHeaderDesc *desc = LLBC_New(LLBC_PacketHeaderDesc);
    // Length part.
    desc->AddPartDesc().SetSerialNo(LengthPartNo)
        .SetPartLen(LengthPartLen)
        .SetIsLenPart(true)
        .Done();

    // Opcode part.
    desc->AddPartDesc().SetSerialNo(OpcodePartNo)
        .SetPartLen(OpcodePartLen)
        .SetIsOpcodePart(true)
        .Done();

    // Status part.
    desc->AddPartDesc().SetSerialNo(StatusPartNo)
        .SetPartLen(StatusPartLen)
        .SetIsStatusPart(true)
        .Done();

    // ServiceId part.
    desc->AddPartDesc().SetSerialNo(ServiceIdPartNo)
        .SetPartLen(ServiceIdPartLen)
        .SetIsServiceIdPart(true)
        .Done();

    // Flags part.
    desc->AddPartDesc().SetSerialNo(FlagsPartNo)
        .SetPartLen((FlagsPartLen))
        .SetIsFlagsPart(true)
        .Done();

    return desc;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
