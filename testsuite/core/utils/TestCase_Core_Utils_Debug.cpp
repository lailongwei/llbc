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


#include "core/utils/TestCase_Core_Utils_Debug.h"

int TestCase_Core_Utils_Debug::Run(int argc, char *argv[])
{
    LLBC_PrintLine("core/utils/Util_Debug test:");

    // Byte2Hex test.
    char str[256];
    for(int i = 0; i < 256; ++i)
    {
        str[i] = static_cast<char>(i);
    }

    LLBC_PrintLine("Byte2Hex test:\n%s", LLBC_Byte2Hex(str, 256, 32).c_str());
    LLBC_PrintLine("");

    // CPUTime test.
    LLBC_CPUTime begin = LLBC_CPUTime::Current();
    for(int i = 0; i < 10000000; ++ i)
    {
    }
    LLBC_CPUTime diff = LLBC_CPUTime::Current() - begin;
    LLBC_PrintLine("CPUTime, diff time: %s", diff.ToString().c_str());
    LLBC_PrintLine("");

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
