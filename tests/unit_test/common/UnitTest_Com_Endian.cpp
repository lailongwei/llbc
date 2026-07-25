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

#include <cstring>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/common/Endian.cpp
// @coverage-target: llbc/include/llbc/common/EndianInl.h

namespace
{

enum class EndianTestEnum : uint32
{
    Value = 0x01020304u,
};

} // namespace

// Endian names are public configuration values. Matching must be case
// insensitive but exact: prefixes such as "big" are not valid names.
TEST(EndianTest, ConvertsAndValidatesEndianNames)
{
    EXPECT_TRUE(LLBC_Endian::IsValid(LLBC_Endian::BigEndian));
    EXPECT_TRUE(LLBC_Endian::IsValid(LLBC_Endian::LittleEndian));
    EXPECT_FALSE(LLBC_Endian::IsValid(LLBC_Endian::UnknownEndian));
    EXPECT_FALSE(LLBC_Endian::IsValid(-1));

    EXPECT_STREQ(LLBC_Endian::Type2Str(LLBC_Endian::BigEndian), "big endian");
    EXPECT_STREQ(LLBC_Endian::Type2Str(LLBC_Endian::LittleEndian), "little endian");
    EXPECT_STREQ(LLBC_Endian::Type2Str(-1), "unknown endian");
    EXPECT_STREQ(LLBC_Endian::Type2Str(999), "unknown endian");

    EXPECT_EQ(LLBC_Endian::Str2Type("BIG ENDIAN"), LLBC_Endian::BigEndian);
    EXPECT_EQ(LLBC_Endian::Str2Type("little endian"), LLBC_Endian::LittleEndian);
    EXPECT_EQ(LLBC_Endian::Str2Type("big"), LLBC_Endian::UnknownEndian);
    EXPECT_EQ(LLBC_Endian::Str2Type("little"), LLBC_Endian::UnknownEndian);
    EXPECT_EQ(LLBC_Endian::Str2Type(nullptr), LLBC_Endian::UnknownEndian);
}

// Byte reversal covers scalar widths and enum values. Applying it twice must
// restore the original bit representation.
TEST(EndianTest, ReversesByteOrderForSupportedScalarWidths)
{
    EXPECT_EQ(LLBC_ReverseBytes<uint8>(0x5au), 0x5au);
    EXPECT_EQ(LLBC_ReverseBytes<uint16>(0x1234u), 0x3412u);
    EXPECT_EQ(LLBC_ReverseBytes<uint32>(0x01020304u), 0x04030201u);
    EXPECT_EQ(LLBC_ReverseBytes<uint64>(0x0102030405060708ULL), 0x0807060504030201ULL);
    EXPECT_EQ(LLBC_ReverseBytes(EndianTestEnum::Value),
              static_cast<EndianTestEnum>(0x04030201u));

    const long nativeSigned = -123456789L;
    const ulong nativeUnsigned = 0x12345678UL;
    EXPECT_EQ(LLBC_ReverseBytes(LLBC_ReverseBytes(nativeSigned)), nativeSigned);
    EXPECT_EQ(LLBC_ReverseBytes(LLBC_ReverseBytes(nativeUnsigned)), nativeUnsigned);

    const double value = 123.25;
    const double reversedTwice = LLBC_ReverseBytes(LLBC_ReverseBytes(value));
    EXPECT_EQ(::memcmp(&value, &reversedTwice, sizeof(value)), 0);

    // Do not use long double here: x87 ABIs store it with padding, and the
    // byte-reversed intermediate can be an unstable floating representation.
}

// Host/network conversion is an involution regardless of the local machine
// endian. The exported machine-endian constant must match the actual layout.
TEST(EndianTest, ConvertsBetweenHostAndNetworkOrder)
{
    const uint32 judge = 1;
    const int actualEndian =
        (*reinterpret_cast<const uint8 *>(&judge) == 1) ?
            LLBC_Endian::LittleEndian : LLBC_Endian::BigEndian;
    EXPECT_EQ(LLBC_MachineEndian, actualEndian);
    EXPECT_TRUE(LLBC_Endian::IsValid(LLBC_DefaultEndian));

    const uint16 value16 = 0x1234u;
    const uint32 value32 = 0x01020304u;
    const uint64 value64 = 0x0102030405060708ULL;
    EXPECT_EQ(LLBC_Net2Host(LLBC_Host2Net(value16)), value16);
    EXPECT_EQ(LLBC_Net2Host(LLBC_Host2Net(value32)), value32);
    EXPECT_EQ(LLBC_Net2Host(LLBC_Host2Net(value64)), value64);
}
