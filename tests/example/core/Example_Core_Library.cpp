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

#include "core/Example_Core_Library.h"

int Example_Core_Library::Run(int argc, char *argv[])
{
    // LLBC_Library wraps cross-platform dynamic library (.so/.dylib/.dll) loading:
    //   Open(fileName)      - load the dynamic library, returns LLBC_OK on success
    //   GetProcAddress(sym) - get a symbol address (function pointer), returns nullptr on failure
    //   Close()             - unload the dynamic library
    LLBC_PrintLn("LLBC_Library API demo:");
    LLBC_PrintLn("\n");

    // ========================= Open the current process (self) =========================
    // Open(nullptr) means loading the current process itself; this is the most stable, environment-independent usage
    LLBC_PrintLn("Open self(current process):");
    LLBC_Library selfLib;
    if (selfLib.Open(nullptr) == LLBC_OK)
    {
        LLBC_PrintLn("- Open self success.");
        selfLib.Close();
    }
    else
    {
        // A very few platforms don't support this; print it but don't treat it as fatal
        LLBC_PrintLn("- Open self failed: %s (skipped)", LLBC_FormatLastError());
    }
    LLBC_PrintLn("\n");

    // ========================= Open a system library and get a symbol (guarded) =========================
    // Which library to load / which symbol to get is environment-specific, so this is guarded:
    // even if the library doesn't exist, it only prints a note and the sample still returns LLBC_OK normally.
#if LLBC_TARGET_PLATFORM_WIN32
    const char *libName = "Kernel32.dll";
    const char *procName = "lstrlenA";
#elif LLBC_TARGET_PLATFORM_MAC || LLBC_TARGET_PLATFORM_IPHONE
    const char *libName = "libm.dylib";
    const char *procName = "cos";
#else
    const char *libName = "libm.so.6";
    const char *procName = "cos";
#endif

    LLBC_PrintLn("Open system library [%s]:", libName);
    LLBC_Library lib;
    if (lib.Open(libName) != LLBC_OK)
    {
        LLBC_PrintLn("- Open failed: %s (environment-specific, skipped)", LLBC_FormatLastError());
        return LLBC_OK;
    }
    LLBC_PrintLn("- Open success.");

    // GetProcAddress: returns a generic function pointer LLBC_LibraryFun; cast it to the real signature yourself
    LLBC_LibraryFun fun = lib.GetProcAddress(procName);
    if (fun == nullptr)
    {
        LLBC_PrintLn("- GetProcAddress [%s] failed: %s (skipped)", procName, LLBC_FormatLastError());
        lib.Close();
        return LLBC_OK;
    }
    LLBC_PrintLn("- GetProcAddress [%s] success.", procName);

#if LLBC_TARGET_PLATFORM_WIN32
    typedef int (WINAPI *Fn_lstrlenA)(const char *);
    const int ret = reinterpret_cast<Fn_lstrlenA>(fun)("Hello");
    LLBC_PrintLn("- Call lstrlenA(\"Hello\") = %d", ret);
#else
    typedef double (*Fn_cos)(double);
    const double ret = reinterpret_cast<Fn_cos>(fun)(0.0);
    LLBC_PrintLn("- Call cos(0.0) = %f", ret);
#endif

    // Unload the dynamic library
    lib.Close();
    LLBC_PrintLn("- Closed library.");

    return LLBC_OK;
}
