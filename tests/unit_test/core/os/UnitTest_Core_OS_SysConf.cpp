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
// @coverage-target: llbc/src/core/os/OS_SysConf.cpp

// Page size is initialized during LLBC startup on every supported platform. The
// explicit cleanup/init cycle verifies that non-Windows sysconf paths restore it.
TEST(SysConfTest, InitializesAndCleansUpPageSize)
{
    EXPECT_GT(LLBC_pageSize, 0);
    EXPECT_EQ(LLBC_pageSize % 512, 0);

    __LLBC_CleanUpSysConf();
    EXPECT_EQ(LLBC_pageSize, 0);

    EXPECT_EQ(__LLBC_InitSysConf(), LLBC_OK);
    EXPECT_GT(LLBC_pageSize, 0);
    EXPECT_EQ(LLBC_pageSize % 512, 0);
}
