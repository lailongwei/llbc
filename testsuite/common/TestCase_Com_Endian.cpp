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
    sint16 i16 = 0x0102;
    LLBC_PrintLn("sint16 host<->net byte order convert test:");
    LLBC_PrintLn("- val:0x%04x", i16);
    LLBC_PrintLn("- val -> net order:0x%04x", LLBC_Host2Net2(i16));
    LLBC_PrintLn("- val -> net order -> host order:0x%04x\n", LLBC_Net2Host2(LLBC_Host2Net2(i16)));

    sint32 i32 = 0x01020304;
    LLBC_PrintLn("sint32 host<->net byte order convert test:");
    LLBC_PrintLn("- val:0x%08x", i32);
    LLBC_PrintLn("- val -> net order:0x%08x", LLBC_Host2Net2(i32));
    LLBC_PrintLn("- val -> net order -> host order:0x%08x\n", LLBC_Net2Host2(LLBC_Host2Net2(i32)));

    sint64 i64 = 0x0506070801020304L;
    LLBC_PrintLn("sint64 host<->net byte order convert test:");
    LLBC_PrintLn("- val:0x%016llx", i64);
    LLBC_PrintLn("- val -> net order:0x%016llx", LLBC_Host2Net2(i64));
    LLBC_PrintLn("- val -> net order -> host order:0x%016llx\n", LLBC_Net2Host2(LLBC_Host2Net2(i64)));

    double dbl = 3.1415926;
    LLBC_PrintLn("double host<->net byte order convert test:");
    LLBC_PrintLn("- val:%f", dbl);
    LLBC_PrintLn("- val -> net order:%f", LLBC_Host2Net2(dbl));
    LLBC_PrintLn("- val -> net order -> host order:%f\n", LLBC_Net2Host2(LLBC_Host2Net2(dbl)));

    long double ldbl = 9.87654321;
    LLBC_PrintLn("long double host<->net byte order convert test:");
    LLBC_PrintLn("- val:%Lf", ldbl);
    LLBC_PrintLn("- val -> net order:%Lf", LLBC_Host2Net2(ldbl));
    LLBC_PrintLn("- val -> net order -> host order:%Lf\n", LLBC_Net2Host2(LLBC_Host2Net2(ldbl)));

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}
