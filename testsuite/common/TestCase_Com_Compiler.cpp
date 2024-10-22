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


#include "common/TestCase_Com_Compiler.h"

TestCase_Com_Compiler::TestCase_Com_Compiler()
{
}

TestCase_Com_Compiler::~TestCase_Com_Compiler()
{
}

int TestCase_Com_Compiler::Run(int argc, char *argv[])
{
    LLBC_PrintLn("common/compiler test:");

    LLBC_String compInfo;
    compInfo.append_format("  Compiler Type: %d\n", LLBC_CUR_COMP)
        .append_format("  Compiler Version: %d\n", LLBC_COMP_VER)
        .append_format("  Major Version: %d\n", LLBC_COMP_MAJOR_VER)
        .append_format("  Minor Version: %d\n", LLBC_COMP_MINOR_VER)
        .append_format("  Patch Level: %d\n", LLBC_COMP_PATCH_LEVEL);

    LLBC_PrintLn("%s", compInfo.c_str());

    LLBC_PrintLn("Press any key to continue...");
    getchar();

    return LLBC_OK;
}

