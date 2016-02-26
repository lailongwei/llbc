/**
 * @file    TestCase_Comm_HeaderDesc.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/08/31
 * @version 1.0
 *
 * @brief
 */

#include "comm/TestCase_Comm_HeaderDesc.h"

TestCase_Comm_HeaderDesc::TestCase_Comm_HeaderDesc()
{
}

TestCase_Comm_HeaderDesc::~TestCase_Comm_HeaderDesc()
{
}

int TestCase_Comm_HeaderDesc::Run(int argc, char *argv[])
{
    LLBC_PrintLine("comm/PacketHeaderDesc test:");
    HeaderPartDescTest();
    
    LLBC_PrintLine("=========================================================================");
    HeaderDescTest();

    LLBC_PrintLine("Press any key to exit...");
    getchar();

    return LLBC_OK;
}

void TestCase_Comm_HeaderDesc::HeaderPartDescTest()
{
    LLBC_PrintLine("Packet header part describe test:");

    LLBC_PacketHeaderPartDesc partDesc;
    partDesc.SetSerialNo(10)
        .SetIsLenPart(true)
        .SetPartLen(2);

    LLBC_PrintLine("After set, part desc:\n%s", partDesc.ToString().c_str());

    partDesc.SetPartLen(4);
    LLBC_PrintLine("After change part length to 4:\n%s", partDesc.ToString().c_str());

    partDesc.SetIsLenIncludedSelf(false);
    LLBC_PrintLine("After change lenIncludedSelf to false:\n%s", partDesc.ToString().c_str());

    partDesc.SetIsLenPart(false);
    partDesc.SetIsOpcodePart(true);
    LLBC_PrintLine("After set opcode to true:\n%s", partDesc.ToString().c_str());

    partDesc.SetIsOpcodePart(false);
    partDesc.SetIsStatusPart(true);
    LLBC_PrintLine("After set statusPart to true:\n%s", partDesc.ToString().c_str());

    partDesc.SetIsStatusPart(false);
    partDesc.SetIsServiceIdPart(true);
    LLBC_PrintLine("After set serviceIdPart to true:\n%s", partDesc.ToString().c_str());

    partDesc.SetIsServiceIdPart(false);
    partDesc.SetIsFlagsPart(true);
    LLBC_PrintLine("After set flags to true:\n%s", partDesc.ToString().c_str());
}

void TestCase_Comm_HeaderDesc::HeaderDescTest()
{
    LLBC_PrintLine("Packet describe test:");

    LLBC_PacketHeaderDesc desc;
    desc.AddPartDesc().SetSerialNo(0)
        .SetIsLenPart(true)
        .SetPartLen(4)
        .Done();

    desc.AddPartDesc().SetSerialNo(1)
        .SetIsOpcodePart(true)
        .SetPartLen(2)
        .Done();
    desc.RemovePartDesc(1);
    desc.AddPartDesc().SetSerialNo(1)
        .SetIsOpcodePart(true)
        .SetPartLen(2)
        .Done();

    desc.AddPartDesc().SetSerialNo(2)
        .SetIsStatusPart(true)
        .SetPartLen(2)
        .Done();

    desc.AddPartDesc().SetSerialNo(3)
        .SetIsServiceIdPart(true)
        .SetPartLen(4)
        .Done();
    
    desc.AddPartDesc().SetSerialNo(4)
        .SetIsFlagsPart(true)
        .SetPartLen(2)
        .SetIsLenIncludedSelf(false)
        .Done();

    LLBC_PrintLine("After set header describe:\n%s", desc.ToString().c_str());

    LLBC_PrintLine("desc header size: %lu", desc.GetHeaderLen());
    LLBC_PrintLine("desc int included header len: %lu", desc.GetLenPartIncludedLen());

    LLBC_PrintLine("Length part:%s, part offset: %lu",
        desc.GetLenPart()->ToString().c_str(), desc.GetLenPartOffset());
    LLBC_PrintLine("Opcode part:%s, part offset: %lu",
        desc.GetPart(1)->ToString().c_str(), desc.GetPartOffset(1));
    LLBC_PrintLine("Status part:%s, part offset: %lu",
        desc.GetPart(2)->ToString().c_str(), desc.GetPartOffset(2));
    LLBC_PrintLine("ServiceId part:%s, part offset: %lu",
        desc.GetPart(3)->ToString().c_str(), desc.GetPartOffset(3));
    LLBC_PrintLine("Flags part:%s, part offset: %lu",
        desc.GetPart(4)->ToString().c_str(), desc.GetPartOffset(4));
}
