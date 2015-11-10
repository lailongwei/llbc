/**
 * @file    TestCase_Com_Endian.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/04
 * @version 1.0
 *
 * @brief
 */

#include "common/TestCase_Com_Endian.h"

int TestCase_Com_Endian::Run(int argc, char *argv[])
{
    LLBC_PrintLine("common/endian test:");

    // Get machine endian type.
    LLBC_PrintLine("machine endian type: %s\n", 
        LLBC_Endian::Type2Str( LLBC_GetMachineEndianType() ) );

    // Endian type -> Endian describe test.
    LLBC_PrintLine("LLBC_Endian::BigEndian describe: %s", 
        LLBC_Endian::Type2Str(LLBC_Endian::BigEndian) );
    LLBC_PrintLine("LLBC_Endian::LittleEndian describe: %s",
        LLBC_Endian::Type2Str(LLBC_Endian::LittleEndian) );
    LLBC_PrintLine("LLBC_Endian::UnknownEndian describe: %s",
        LLBC_Endian::Type2Str(LLBC_Endian::UnknownEndian) );
    LLBC_PrintLine("LLBC_Endian::UnknownEndian + 300 describe: %s",
        LLBC_Endian::Type2Str(LLBC_Endian::UnknownEndian + 300) );
    LLBC_PrintLine("");

    // Endian describe -> Endian type test.
    LLBC_PrintLine("'big endian' type: %d", LLBC_Endian::Str2Type("big endian") );
    LLBC_PrintLine("'little endian' type: %d", LLBC_Endian::Str2Type("little endian") );
    LLBC_PrintLine("'unknown endian' type: %d", LLBC_Endian::Str2Type("unknown type") );
    LLBC_PrintLine("'hello' type: %d", LLBC_Endian::Str2Type("hello") );
    LLBC_PrintLine("");

    // Test host <-> net.
    int data = 0x01020304;
    LLBC_PrintLine("host to net test, host: 0x%08x, net: 0x%08x",
        data, LLBC_Host2Net2(data) );
    LLBC_Host2Net(data);
    LLBC_PrintLine("net to host test, net: 0x%08x, host: 0x%08x",
        data, LLBC_Net2Host2(data) );
    LLBC_PrintLine("");

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
