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


#include "common/TestCase_Com_Version.h"

TestCase_Com_Version::TestCase_Com_Version()
{
}

TestCase_Com_Version::~TestCase_Com_Version()
{
}

int TestCase_Com_Version::Run(int argc, char *argv[])
{
    LLBC_PrintLine("llbc library version test: ");

    LLBC_PrintLine("Major version: %d", LLBC_majorVersion);
    LLBC_PrintLine("Minor version: %d", LLBC_minorVersion);
    LLBC_PrintLine("Update No: %d", LLBC_updateNo);
    LLBC_PrintLine("Is debug version: %s", LLBC_isDebugVer ? "true" : "false");

    LLBC_PrintLine("Version describe(verbose): \n%s", LLBC_GetVersionInfo(true).c_str());

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
