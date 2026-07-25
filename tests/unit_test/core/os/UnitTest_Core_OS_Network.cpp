// The MIT License (MIT)
//
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
// @coverage-target: llbc/src/core/os/OS_Network.cpp

// The OS network adapter is used by socket setup code to translate numeric
// endpoints without relying on DNS. Verify successful result ownership and the
// framework error/sub-error mapping for a rejected numeric host.
TEST(NetworkOsTest, ResolvesNumericEndpointsAndMapsGetAddrInfoFailures)
{
    addrinfo hints {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;

    addrinfo *result = nullptr;
    ASSERT_EQ(LLBC_GetAddrInfo("127.0.0.1", "80", &hints, &result), LLBC_OK);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->ai_family, AF_INET);
    EXPECT_EQ(result->ai_socktype, SOCK_STREAM);
    LLBC_FreeAddrInfo(result);

    result = nullptr;
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    LLBC_SetSubErrorNo(0);
    EXPECT_EQ(LLBC_GetAddrInfo("not-a-numeric-address", "80", &hints, &result), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_GAI);
    EXPECT_NE(LLBC_GetSubErrorNo(), 0);
    EXPECT_EQ(result, nullptr);
}
