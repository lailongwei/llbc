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
// @coverage-target: llbc/src/common/Version.cpp

// Version reporting is consumed by diagnostics and deployment tooling. The
// concise form identifies the build while the verbose form exposes configured
// core and communication capabilities.
TEST(VersionTest, ReportsConciseAndVerboseBuildInformation)
{
    EXPECT_GT(LLBC_majorVersion, 0);
    EXPECT_GE(LLBC_minorVersion, 0);
    EXPECT_GE(LLBC_updateNo, 0);

    const auto concise = LLBC_GetVersionInfo();
    const LLBC_String expectedPrefix =
        LLBC_String().format("%d.%d.%d_",
                             LLBC_majorVersion,
                             LLBC_minorVersion,
                             LLBC_updateNo);
    EXPECT_EQ(concise.find(expectedPrefix), 0lu);
    EXPECT_NE(concise.find(LLBC_isDebugVer ? "debug" : "release"), LLBC_String::npos);
    EXPECT_NE(concise.find("arch:"), LLBC_String::npos);
    EXPECT_NE(concise.find("compiled with:"), LLBC_String::npos);

    const auto verbose = LLBC_GetVersionInfo(true);
    EXPECT_TRUE(verbose.find(concise) == 0);
    EXPECT_NE(verbose.find("core info:"), LLBC_String::npos);
    EXPECT_NE(verbose.find("thread info:"), LLBC_String::npos);
    EXPECT_NE(verbose.find("logger info:"), LLBC_String::npos);
    EXPECT_NE(verbose.find("communication info:"), LLBC_String::npos);
}
