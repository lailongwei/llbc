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
// @coverage-target: llbc/src/core/utils/Util_Network.cpp

// This predicate is used to decide whether a socket endpoint is a literal IPv4
// address or must be resolved as a host name. It therefore validates both shape
// and each octet's legal 0..255 range.
TEST(NetworkUtilTest, RecognizesValidIPv4Literals)
{
    EXPECT_TRUE(LLBC_IsIPv4Addr("127.0.0.1"));
    EXPECT_TRUE(LLBC_IsIPv4Addr("0.0.0.0"));
    EXPECT_TRUE(LLBC_IsIPv4Addr("255.255.255.255"));
    EXPECT_TRUE(LLBC_IsIPv4Addr("001.002.003.004"));
}

TEST(NetworkUtilTest, RejectsInvalidIPv4ShapesAndOctets)
{
    EXPECT_FALSE(LLBC_IsIPv4Addr(""));
    EXPECT_FALSE(LLBC_IsIPv4Addr("127.0.0"));
    EXPECT_FALSE(LLBC_IsIPv4Addr("127.0.0.1.2"));
    EXPECT_FALSE(LLBC_IsIPv4Addr("127..0.1"));
    EXPECT_FALSE(LLBC_IsIPv4Addr(".127.0.1"));
    EXPECT_FALSE(LLBC_IsIPv4Addr("127.0.0."));
    EXPECT_FALSE(LLBC_IsIPv4Addr("127.a.0.1"));
    EXPECT_FALSE(LLBC_IsIPv4Addr("127.-1.0.1"));
    EXPECT_FALSE(LLBC_IsIPv4Addr("256.0.0.1"));
    EXPECT_FALSE(LLBC_IsIPv4Addr("999.1.1.1"));
    EXPECT_FALSE(LLBC_IsIPv4Addr("www.example.com"));
}
