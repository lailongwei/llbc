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

#include <llbc.h>
using namespace llbc;

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/library/Library.cpp
// @coverage-target: llbc/src/core/os/OS_Library.cpp

namespace
{

#if LLBC_TARGET_PLATFORM_WIN32
constexpr const char *TestLibraryPath = "kernel32.dll";
constexpr const char *TestSymbolName = "GetCurrentProcessId";
#elif LLBC_TARGET_PLATFORM_MAC
// libSystem is already fundamental to the process; closing a test dlopen()
// reference to it can destabilize later allocations. Use an independent,
// closeable system extension instead.
constexpr const char *TestLibraryPath = "/System/Library/Tcl/sqlite3/libtclsqlite3.dylib";
constexpr const char *TestSymbolName = "Sqlite3_Init";
#else
constexpr const char *TestLibraryPath = "libc.so.6";
constexpr const char *TestSymbolName = "printf";
#endif

} // namespace

// Dynamic-library wrappers are used by plug-ins and optional integrations.
// Verify open, lookup, re-entry protection, close, and error paths against a
// platform-provided library rather than a repository-generated artifact.
TEST(LibraryTest, OpensLooksUpAndClosesSystemLibrary)
{
    LLBC_Library library;
    EXPECT_EQ(library.Close(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);

    EXPECT_EQ(library.Open("/definitely/not/a/real/llbc-library"), LLBC_FAILED);
    EXPECT_NE(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);

    ASSERT_EQ(library.Open(TestLibraryPath), LLBC_OK);
    EXPECT_EQ(library.Open(TestLibraryPath), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_REENTRY);

    EXPECT_NE(library.GetProcAddress(TestSymbolName), nullptr);
    EXPECT_EQ(library.GetProcAddress("llbc_this_symbol_does_not_exist"), nullptr);
    EXPECT_NE(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);

    EXPECT_EQ(library.Close(), LLBC_OK);
    EXPECT_EQ(library.Close(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
}

// The low-level OS wrappers must provide the same valid-symbol behavior and
// surface an error for a missing export before the library handle is closed.
TEST(LibraryTest, LowLevelLibraryHelpersLoadAndResolveSymbols)
{
    const auto handle = LLBC_LoadLibrary(TestLibraryPath);
    ASSERT_NE(handle, LLBC_INVALID_LIBRARY_HANDLE);

    EXPECT_NE(LLBC_GetProcAddress(handle, TestSymbolName), nullptr);
    EXPECT_EQ(LLBC_GetProcAddress(handle, "llbc_this_symbol_does_not_exist"), nullptr);
    EXPECT_NE(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);
    EXPECT_EQ(LLBC_CloseLibrary(handle), LLBC_OK);
}
