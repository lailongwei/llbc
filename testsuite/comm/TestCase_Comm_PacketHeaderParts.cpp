/**
 * @file    TestCase_Comm_PacketHeaderParts.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/10/01
 * @version 1.0
 *
 * @brief
 */

#include "comm/TestCase_Comm_PacketHeaderParts.h"

TestCase_Comm_PacketHeaderParts::TestCase_Comm_PacketHeaderParts()
{
}

TestCase_Comm_PacketHeaderParts::~TestCase_Comm_PacketHeaderParts()
{
}

int TestCase_Comm_PacketHeaderParts::Run(int argc, char *argv[])
{
    LLBC_PrintLine("comm/PacketeHeaderParts test:");

    int serialNo = 0;
    TestOne<sint8>(serialNo++, 'a', "sint8");
    TestOne<uint8>(serialNo++, 8, "uint8");
    TestOne<sint16>(serialNo++, -16, "sint16");
    TestOne<uint16>(serialNo++, 16, "uint16");
    TestOne<sint32>(serialNo++, -32, "sint32");
    TestOne<uint32>(serialNo++, 32, "uint32");
    TestOne<sint64>(serialNo++, -64, "sint64");
    TestOne<uint64>(serialNo++, 64, "uint64");
    TestOne<float>(serialNo++, 1.618f, "float");
    TestOne<double>(serialNo++, 8.161, "double");
    TestOne<const char *>(serialNo++, "Hello World!", "char *");
    TestOne<std::string>(serialNo++, "Hello std::string!", "std::string");
    TestOne<LLBC_String>(serialNo++, "Hello LLBC_String!", "LLBC_String");

    TestSetToPacket();

    LLBC_PrintLine("Press any key to continue...");
    getchar();

    return 0;
}

template <typename T>
void TestCase_Comm_PacketHeaderParts::TestOne(int serialNo, const T &val, const char *typeDesc)
{
    LLBC_PrintLine("Set %s value, serial no: %d", typeDesc, serialNo);
    _parts.SetPart(serialNo, val);
    LLBC_PrintLine("  After set, has value? %s", _parts.IsHasPart(serialNo) ? "true" : "false");
}

void TestCase_Comm_PacketHeaderParts::TestSetToPacket()
{
    LLBC_PrintLine("Test SetToPacket() method:");
    //! First, We must design packet header format.
    LLBC_PrintLine("  Fristly, we design packet header format.");
    LLBC_PacketHeaderDesc *headerDesc = LLBC_New(LLBC_PacketHeaderDesc);

    int serialNo = 0;
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++).SetPartLen(sizeof(sint8)).Done();
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++).SetPartLen(sizeof(uint8)).Done();
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++).SetPartLen(sizeof(sint16)).Done();
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++).SetPartLen(sizeof(uint16)).Done();
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++).SetPartLen(sizeof(sint32))
        .SetIsLenPart(true).Done();
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++).SetPartLen(sizeof(uint32))
        .SetIsOpcodePart(true).Done();
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++).SetPartLen(sizeof(sint64)).Done();
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++).SetPartLen(sizeof(uint64)).Done();
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++).SetPartLen(sizeof(float)).Done();
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++).SetPartLen(sizeof(double)).Done();
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++).SetPartLen(13).Done(); // "Hello World!"\0
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++).SetPartLen(19).Done(); // "Hello std::string!"\0
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++).SetPartLen(19).Done(); // "Hello LLBC_String!"\0

    LLBC_PrintLine("  Design packet header done, header length: %lu", headerDesc->GetHeaderLen());

    // Set header format describe to llbc framework.
    LLBC_Print("  Set to llbc framework...");
    LLBC_IService::SetPacketHeaderDesc(headerDesc);
    LLBC_PrintLine("Done!");

    // Set header value to packet.
    LLBC_Print("Set the header parts to packet...");
    LLBC_Packet packet;
    _parts.SetToPacket(packet);
    LLBC_PrintLine("Done!");

    // Fetch from packet.
    LLBC_PrintLine("Fetch values from packet.");
    serialNo = 0;
    LLBC_PrintLine("  serial %d: %c", serialNo, packet.GetHeaderPartAsSInt8(serialNo)), serialNo++;
    LLBC_PrintLine("  serial %d: %d", serialNo, packet.GetHeaderPartAsUInt8(serialNo)), serialNo++;
    LLBC_PrintLine("  serial %d: %d", serialNo, packet.GetHeaderPartAsSInt16(serialNo)), serialNo++;
    LLBC_PrintLine("  serial %d: %u", serialNo, packet.GetHeaderPartAsUInt16(serialNo)), serialNo++;
    LLBC_PrintLine("  serial %d: %d", serialNo, packet.GetHeaderPartAsSInt32(serialNo)), serialNo++;
    LLBC_PrintLine("  serial %d: %u", serialNo, packet.GetHeaderPartAsUInt32(serialNo)), serialNo++;
    LLBC_PrintLine("  serial %d: %lld", serialNo, packet.GetHeaderPartAsSInt64(serialNo)), serialNo++;
    LLBC_PrintLine("  serial %d: %llu", serialNo, packet.GetHeaderPartAsUInt64(serialNo)), serialNo++;
    LLBC_PrintLine("  serial %d: %f", serialNo, packet.GetHeaderPartAsFloat(serialNo)), serialNo++;
    LLBC_PrintLine("  serial %d: %f", serialNo, packet.GetHeaderPartAsDouble(serialNo)), serialNo++;
    LLBC_PrintLine("  serial %d: %s", serialNo, packet.GetHeaderPartAsStr(serialNo).c_str()), serialNo++; // "Hello World!"
    LLBC_PrintLine("  serial %d: %s", serialNo, packet.GetHeaderPartAsStr(serialNo).c_str()), serialNo++; // "Hello std::string!"
    LLBC_PrintLine("  serial %d: %s", serialNo, packet.GetHeaderPartAsStr(serialNo).c_str()), serialNo++; // "Hello LLBC_String!"
    LLBC_PrintLine("  Fetch Done!");
}
