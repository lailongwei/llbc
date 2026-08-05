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

#include <cstdint>
#include <limits>
#include <string>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/utils/Util_Text.cpp
// @coverage-target: llbc/include/llbc/core/utils/Util_TextInl.h

namespace
{

enum class TextTestEnum : sint64
{
    Value = 42,
};

enum class UnsignedTextTestEnum : uint64
{
    Max = std::numeric_limits<uint64>::max(),
};

} // namespace

// Numeric formatting is a low-allocation TLS utility used throughout logging and
// diagnostics. Exercise integral, enum, pointer, hexadecimal, and float paths.
TEST(TextUtilTest, NumberToStringFormatsSupportedValues)
{
    size_t length = 0;
    EXPECT_STREQ(LLBC_Num2Str2(true, &length), "1");
    EXPECT_EQ(length, 1lu);
    EXPECT_STREQ(LLBC_Num2Str2(false, &length), "0");
    EXPECT_EQ(length, 1lu);

    EXPECT_EQ(LLBC_Num2Str<sint32>(-123456), "-123456");
    EXPECT_EQ(LLBC_Num2Str<uint64>(9876543210ull), "9876543210");
    EXPECT_EQ(LLBC_Num2Str<sint64>(std::numeric_limits<sint64>::min()),
              "-9223372036854775808");
    EXPECT_EQ(LLBC_Num2Str(TextTestEnum::Value), "42");
    EXPECT_EQ(LLBC_Num2Str(UnsignedTextTestEnum::Max), "18446744073709551615");
    EXPECT_EQ((LLBC_Num2Str<uint32, true>(0xdeadbeefu)), "0xdeadbeef");

    void *ptr = reinterpret_cast<void *>(static_cast<uintptr_t>(0x1234));
    EXPECT_EQ(LLBC_Num2Str<void *>(ptr), "4660");
    EXPECT_EQ((LLBC_Num2Str<void *, true>(ptr)), "0x1234");
    EXPECT_EQ(LLBC_Num2Str<float>(1.25f), "1.250000");
    EXPECT_EQ(LLBC_Num2Str<double>(-2.5), "-2.500000");
    EXPECT_EQ(LLBC_Num2Str<ldouble>(3.0L), "3.000000");
}

// Loose booleans are designed for configuration values: accept trimmed numeric,
// hexadecimal, decimal, and optional yes/true spellings without raising errors.
TEST(TextUtilTest, LooseBooleanRecognizesConfigurationValues)
{
    EXPECT_FALSE(LLBC_Str2LooseBool(nullptr));
    EXPECT_FALSE(LLBC_Str2LooseBool(""));
    EXPECT_FALSE(LLBC_Str2LooseBool(" \t "));

    EXPECT_TRUE(LLBC_Str2LooseBool("YES"));
    EXPECT_TRUE(LLBC_Str2LooseBool("  true  "));
    EXPECT_FALSE(LLBC_Str2LooseBool("true", 10, false));
    EXPECT_FALSE(LLBC_Str2LooseBool("no"));

    EXPECT_TRUE(LLBC_Str2LooseBool("17"));
    EXPECT_FALSE(LLBC_Str2LooseBool("0"));
    EXPECT_TRUE(LLBC_Str2LooseBool("-0.25"));
    EXPECT_FALSE(LLBC_Str2LooseBool("0.000000"));
    EXPECT_TRUE(LLBC_Str2LooseBool("0x10"));
    EXPECT_TRUE(LLBC_Str2LooseBool("0X10"));
    EXPECT_FALSE(LLBC_Str2LooseBool("-0x0"));
    EXPECT_FALSE(LLBC_Str2LooseBool("foo"));

    // Oversized input is rejected before copying into the TLS conversion buffer.
    EXPECT_FALSE(LLBC_Str2LooseBool(std::string(512, '1').c_str()));
}

// Integer conversion must distinguish a valid zero from malformed, partial, and
// range-error input through the framework's last-error contract.
TEST(TextUtilTest, IntegerConversionReportsErrorsAndRangeChecks)
{
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(LLBC_Str2Num<long>("0"), 0l);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);

    EXPECT_EQ(LLBC_Str2Num<long>("-12345"), -12345l);
    EXPECT_EQ(LLBC_Str2Num<ulong>("ff", 16), 255ul);
    EXPECT_EQ(LLBC_Str2Num<sint64>("-922337203685477580"), -922337203685477580ll);
    EXPECT_EQ(LLBC_Str2Num<uint64>("18446744073709551615"), std::numeric_limits<uint64>::max());
    EXPECT_EQ(LLBC_Str2Num<TextTestEnum>("42"), TextTestEnum::Value);
    EXPECT_EQ(LLBC_Str2Num<sint8>("127"), 127);
    EXPECT_EQ(LLBC_Str2Num<uint8>("255"), 255u);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(LLBC_Str2Num<long>("12tail"), 0l);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_PARTIAL_PARSED);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(LLBC_Str2Num<long>("not-a-number"), 0l);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(LLBC_Str2Num<long>(nullptr), 0l);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(LLBC_Str2Num<sint8>("128"), 0);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(LLBC_Str2Num<uint8>("-1"), 0u);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(LLBC_Str2Num<uint64>("18446744073709551616"), 0ull);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
}

// Pointer and floating-point overloads use specialized parsing rules. Pointer
// parsing auto-detects 0x prefixes, while all floating types share the same
// malformed/partial-result error semantics.
TEST(TextUtilTest, PointerFloatAndBoolConversionPaths)
{
    const auto ptr = LLBC_Str2Num<void *>("0x1234", 10);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr), 0x1234u);
    EXPECT_EQ(LLBC_Str2Num<void *>(nullptr), nullptr);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_FLOAT_EQ(LLBC_Str2Num<float>("0.0"), 0.0f);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(LLBC_Str2Num<double>("1.25"), 1.25);
    EXPECT_DOUBLE_EQ(static_cast<double>(LLBC_Str2Num<ldouble>("-2.5")), -2.5);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(LLBC_Str2Num<double>("3.5tail"), 0.0);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_PARTIAL_PARSED);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(LLBC_Str2Num<double>("not-a-number"), 0.0);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);

    EXPECT_TRUE(LLBC_Str2Num<bool>("1"));
    EXPECT_FALSE(LLBC_Str2Num<bool>("0"));
    EXPECT_FALSE(LLBC_Str2Num<bool>("true"));
}
