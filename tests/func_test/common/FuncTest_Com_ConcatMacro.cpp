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

#include "common/TestCase_Com_ConcatMacro.h"

#define __ClassNamePart1 F
#define __ClassNamePart2 oo
#define __MethodNamePart1 Say
#define __MethodNamePart2 Hello

class LLBC_Concat(__ClassNamePart1, __ClassNamePart2)
{
public:
    void LLBC_Concat(__MethodNamePart1, __MethodNamePart2)()
    {
        LLBC_PrintLn("%s called, __FUNCTION__:%s", LLBC_ConcatStr(__MethodNamePart1, __MethodNamePart2), __FUNCTION__);
    }
};

int TestCase_Com_ConcatMacro::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Concat macro test:");

    // Create class object.
    LLBC_Concat(__ClassNamePart1, __ClassNamePart2) obj;
    LLBC_PrintLn("Class name(rtti):%s", LLBC_GetTypeName(obj));
    LLBC_PrintLn("Call concat name method(inner use LLBC_ConcatStr to format string):");
    obj.LLBC_Concat(__MethodNamePart1, __MethodNamePart2)();

    LLBC_PrintLn("Press any key to continue...");
    getchar();

    return 0;
}