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


#include "core/utils/TestCase_Core_Utils_Algorithm.h"

int TestCase_Core_Utils_Algorithm::Run(int argc, char *argv[])
{
    LLBC_PrintLine("llbc library core/utils/Util_Algorithm test");

    LLBC_PrintLine("flow check test:");
    LLBC_PrintLine("3 + 4: %s", 
        LLBC_FlowType::Type2Str( LLBC_CheckFlowUseAdd(3, 4)));
    LLBC_PrintLine("(sint16)32767 + (sint16)2: %s",
        LLBC_FlowType::Type2Str( LLBC_CheckFlowUseAdd( (sint16)32767, (sint16)2)));
    LLBC_PrintLine("(sint16)-32768 + (sint16)-2: %s",
        LLBC_FlowType::Type2Str( LLBC_CheckFlowUseAdd( (sint16)-32768, (sint16)-2)));

    LLBC_PrintLine("2 - 1: %s", 
        LLBC_FlowType::Type2Str( LLBC_CheckFlowUseSub(2, 1)));
    LLBC_PrintLine("(sint16)-32768 - (sing16)1: %s",
        LLBC_FlowType::Type2Str( LLBC_CheckFlowUseSub( (sint16)-32768, (sint16)1)));
    LLBC_PrintLine("(sint16)32767 - (sint16)-2: %s",
        LLBC_FlowType::Type2Str( LLBC_CheckFlowUseSub( (sint16)32767, (sint16)-2)));
    LLBC_PrintLine("");

    LLBC_PrintLine("NOFLOW string: %s", LLBC_FlowType::Type2Str(LLBC_FlowType::NoFlow));
    LLBC_PrintLine("UNDERFLOW string: %s", LLBC_FlowType::Type2Str(LLBC_FlowType::UnderFlow));
    LLBC_PrintLine("OVERFLOW string: %s", LLBC_FlowType::Type2Str(LLBC_FlowType::OverFlow));
    LLBC_PrintLine("UNKNOWN string: %s", LLBC_FlowType::Type2Str(LLBC_FlowType::Unknown));
    LLBC_PrintLine("0xff3d string: %s", LLBC_FlowType::Type2Str(0xff3d));

    LLBC_PrintLine("test completed, press any key to exit");

    getchar();

    return 0;
}

