/**
 * @file    TestCase_Core_Library.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/10/01
 * @version 1.0
 *
 * @brief
 */

#include "core/library/TestCase_Core_Library.h"

TestCase_Core_Library::TestCase_Core_Library()
{
}

TestCase_Core_Library::~TestCase_Core_Library()
{
}

int TestCase_Core_Library::Run(int argc, char *argv[])
{
    LLBC_PrintLine("llbc library core/library module test case routine:");

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
        LLBC_PrintLine("Open library %s failed, reason: %s", libName, LLBC_FormatLastError());
        return -1;
    }
    LLBC_PrintLine("Open library %s successed.", libName);

    LLBC_LibraryFun fun = lib.GetProcAddress(procName);
    if(fun == NULL)
    {
        LLBC_PrintLine("Retrieve library symbol [%s] failed, reason: %s", procName, LLBC_FormatLastError());
        return -1;
    }
    LLBC_PrintLine("Retrieve library symbol [%s] success, call it", procName);
#if LLBC_TARGET_PLATFORM_WIN32
    typedef int (WINAPI *LIB_lstrlen)(LPCSTR);
    int libFunRet = ((LIB_lstrlen)(fun))("Hello");
    LLBC_PrintLine("Lib function call successed, param: Hello, ret: %d", libFunRet);
#else
    typedef double (*LIB_cos)(double);
    double libFunRet = ((LIB_cos)(fun))(2.0);
    LLBC_PrintLine("Lib function call successed, param: 2.0, ret: %f", libFunRet);
#endif

    // Open self library.
    LLBC_Library selfLib;
	if(selfLib.Open(NULL) != LLBC_OK)
    {
        LLBC_PrintLine("Open self library failed, reason: %s", LLBC_FormatLastError());
        return -1;
    }
    LLBC_PrintLine("Open self library successed!");

    LLBC_PrintLine("press any key to continue ... ...");
    getchar();

    return 0;
}
