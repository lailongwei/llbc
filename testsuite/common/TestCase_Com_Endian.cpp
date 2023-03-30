// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "common/TestCase_Com_Endian.h"

int TestCase_Com_Endian::Run(int argc, char *argv[])
{
    LLBC_PrintLn("common/endian test:");

    // Get machine endian type.
    LLBC_PrintLn("machine endian type: %s\n", 
        LLBC_Endian::Type2Str(LLBC_GetMachineEndianType()));

    // Endian type -> Endian describe test.
    LLBC_PrintLn("LLBC_Endian::BigEndian describe: %s", 
        LLBC_Endian::Type2Str(LLBC_Endian::BigEndian));
    LLBC_PrintLn("LLBC_Endian::LittleEndian describe: %s",
        LLBC_Endian::Type2Str(LLBC_Endian::LittleEndian));
    LLBC_PrintLn("LLBC_Endian::UnknownEndian describe: %s",
        LLBC_Endian::Type2Str(LLBC_Endian::UnknownEndian));
    LLBC_PrintLn("LLBC_Endian::UnknownEndian + 300 describe: %s",
        LLBC_Endian::Type2Str(LLBC_Endian::UnknownEndian + 300));
    LLBC_Print("\n");

    // Endian describe -> Endian type test.
    LLBC_PrintLn("'big endian' type: %d", LLBC_Endian::Str2Type("big endian"));
    LLBC_PrintLn("'little endian' type: %d", LLBC_Endian::Str2Type("little endian"));
    LLBC_PrintLn("'unknown endian' type: %d", LLBC_Endian::Str2Type("unknown type"));
    LLBC_PrintLn("'hello' type: %d", LLBC_Endian::Str2Type("hello"));
    LLBC_Print("\n");

    // Test host <-> net.
    int data = 0x01020304;
    LLBC_PrintLn("host to net test, host: 0x%08x, net: 0x%08x",
        data, LLBC_Host2Net2(data));
    LLBC_Host2Net(data);
    LLBC_PrintLn("net to host test, net: 0x%08x, host: 0x%08x",
        data, LLBC_Net2Host2(data));
    LLBC_Print("\n");
    sint64 longData = 0x0506070801020304L;
    LLBC_PrintLn("host to net test(long data), host: 0x%16llx, net: 0x%16llx", longData, LLBC_Host2Net2(longData));
    LLBC_Host2Net(longData);
    LLBC_PrintLn("net to host test, net: 0x%16llx, host: 0x%16llx", longData, LLBC_Net2Host2(longData));

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}
