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


#include "core/utils/TestCase_Core_Utils_MD5.h"

TestCase_Core_Utils_MD5::TestCase_Core_Utils_MD5()
{
}

TestCase_Core_Utils_MD5::~TestCase_Core_Utils_MD5()
{
}

int TestCase_Core_Utils_MD5::Run(int argc, char *argv[])
{
    LLBC_PrintLn("core/utils/md5 test:");

    LLBC_String str1;
    LLBC_String str2 = "Hello World";
    LLBC_String str3 = "Hello WorldHello WorldHello WorldHello WorldHello WorldHello WorldHello World";

    // d41d8cd98f00b204e9800998ecf8427e
    LLBC_PrintLn("Content: %s", str1.c_str());
    LLBC_PrintLn("MD5: %s",  LLBC_MD5::MD5_String(str1.c_str()).c_str());

    // b10a8db164e0754105b7a99be72e3fe5
    LLBC_PrintLn("Content: %s", str2.c_str());
    LLBC_PrintLn("MD5: %s", LLBC_MD5::MD5_String(str2.c_str()).c_str());

    // f90ca33af8bc681c9affec37711e7593
    LLBC_PrintLn("Content: %s", str3.c_str());
    LLBC_PrintLn("Content: %s", LLBC_MD5::MD5_String(str3.c_str()).c_str());

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}
