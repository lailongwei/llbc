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
// @coverage-target: llbc/include/llbc/core/utils/Util_MathInl.h

// The math utility deliberately supplies generic integral handling and dedicated
// floating-point specializations. Verify sign, zero, and unsigned passthrough.
TEST(MathUtilTest, AbsoluteValueSupportsIntegralAndFloatingTypes)
{
    EXPECT_EQ(LLBC_Abs(-42), 42);
    EXPECT_EQ(LLBC_Abs(42), 42);
    EXPECT_EQ(LLBC_Abs(0), 0);
    EXPECT_EQ(LLBC_Abs<uint32>(42u), 42u);

    EXPECT_FLOAT_EQ(LLBC_Abs(-1.25f), 1.25f);
    EXPECT_FLOAT_EQ(LLBC_Abs(1.25f), 1.25f);
    EXPECT_DOUBLE_EQ(LLBC_Abs(-2.5), 2.5);
    EXPECT_DOUBLE_EQ(LLBC_Abs(2.5), 2.5);
    EXPECT_DOUBLE_EQ(static_cast<double>(LLBC_Abs(-3.75L)), 3.75);
}
