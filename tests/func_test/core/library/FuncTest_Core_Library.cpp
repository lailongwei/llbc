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


#include "core/library/TestCase_Core_Library.h"

TestCase_Core_Library::TestCase_Core_Library()
{
}

TestCase_Core_Library::~TestCase_Core_Library()
{
}

int TestCase_Core_Library::Run(int argc, char *argv[])
{
    LLBC_PrintLn("llbc library core/library module test case routine:");

#if LLBC_TARGET_PLATFORM_WIN32
    const char *libName = "Kernel32.dll";
    const char *procName = "lstrlenA";
#else
    const char *libName = "libm.so";
    const char *procName = "cos";
#endif

    // Open specific library.
    LLBC_Library lib;
    if(lib.Open(libName) != LLBC_OK)
    {
        LLBC_PrintLn("Open library %s failed, reason: %s", libName, LLBC_FormatLastError());
        return -1;
    }
    LLBC_PrintLn("Open library %s successed.", libName);

    LLBC_LibraryFun fun = lib.GetProcAddress(procName);
    if(fun == nullptr)
    {
        LLBC_PrintLn("Retrieve library symbol [%s] failed, reason: %s", procName, LLBC_FormatLastError());
        return -1;
    }
    LLBC_PrintLn("Retrieve library symbol [%s] success, call it", procName);
#if LLBC_TARGET_PLATFORM_WIN32
    typedef int (WINAPI *LIB_lstrlen)(LPCSTR);
    int libFunRet = ((LIB_lstrlen)(fun))("Hello");
    LLBC_PrintLn("Lib function call successed, param: Hello, ret: %d", libFunRet);
#else
    typedef double (*LIB_cos)(double);
    double libFunRet = ((LIB_cos)(fun))(2.0);
    LLBC_PrintLn("Lib function call successed, param: 2.0, ret: %f", libFunRet);
#endif

    // Open self library.
    LLBC_Library selfLib;
	if(selfLib.Open(nullptr) != LLBC_OK)
    {
        LLBC_PrintLn("Open self library failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }
    LLBC_PrintLn("Open self library successed!");

    LLBC_PrintLn("press any key to continue ... ...");
    getchar();

    return 0;
}
