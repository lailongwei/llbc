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


#include "common/TestCase_Com_RTTI.h"

namespace
{
    struct UserDefineStruct {};

    class UserDefineClass {};
}

TestCase_Com_RTTI::TestCase_Com_RTTI()
{
}

TestCase_Com_RTTI::~TestCase_Com_RTTI()
{
}

int TestCase_Com_RTTI::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Common/RTTI test:");

    LLBC_PrintLn("Raw type name test:");
    LLBC_PrintLn("  LLBC_GetTypeName(bool): %s", LLBC_GetTypeName(bool));
    LLBC_PrintLn("  LLBC_GetTypeName(sint8): %s", LLBC_GetTypeName(sint8));
    LLBC_PrintLn("  LLBC_GetTypeName(uint8): %s", LLBC_GetTypeName(uint8));
    LLBC_PrintLn("  LLBC_GetTypeName(sint16): %s", LLBC_GetTypeName(sint16));
    LLBC_PrintLn("  LLBC_GetTypeName(uint16): %s", LLBC_GetTypeName(uint16));
    LLBC_PrintLn("  LLBC_GetTypeName(sint32): %s", LLBC_GetTypeName(sint32));
    LLBC_PrintLn("  LLBC_GetTypeName(uint32): %s", LLBC_GetTypeName(uint32));
    LLBC_PrintLn("  LLBC_GetTypeName(sint64): %s", LLBC_GetTypeName(sint64));
    LLBC_PrintLn("  LLBC_GetTypeName(uint64): %s", LLBC_GetTypeName(uint64));
    LLBC_PrintLn("  LLBC_GetTypeName(float): %s", LLBC_GetTypeName(float));
    LLBC_PrintLn("  LLBC_GetTypeName(double): %s", LLBC_GetTypeName(double));
    LLBC_PrintLn("  LLBC_GetTypeName(std::string): %s", LLBC_GetTypeName(std::string));
    LLBC_PrintLn("  LLBC_GetTypeName(LLBC_String): %s", LLBC_GetTypeName(LLBC_String));

    LLBC_PrintLn("\nPoint type test:");
    LLBC_PrintLn("  LLBC_GetTypeName((int *)): %s", LLBC_GetTypeName(int*));
    LLBC_PrintLn("  LLBC_GetTypeName((int *)nullptr): %s", LLBC_GetTypeName((int*)nullptr));
    LLBC_PrintLn("  LLBC_GetTypeName(int **): %s", LLBC_GetTypeName(int**));
    LLBC_PrintLn("  LLBC_GetTypeName((int **)nullptr): %s", LLBC_GetTypeName((int**)nullptr));
    LLBC_PrintLn("  LLBC_GetTypeName((int ***)nullptr): %s", LLBC_GetTypeName((int***)nullptr));

    LLBC_PrintLn("\nUser defined classes/structs test:");
    LLBC_PrintLn("  LLBC_GetTypeName(UserDefineStruct): %s", LLBC_GetTypeName(UserDefineStruct));
    LLBC_PrintLn("  LLBC_GetTypeName(UserDefineClass): %s", LLBC_GetTypeName(UserDefineClass));

    LLBC_PrintLn("Press any key to continue...");
    getchar();

    return LLBC_OK;
}


