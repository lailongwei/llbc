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
// @coverage-target: llbc/src/core/variant/Variant.cpp
// @coverage-target: llbc/include/llbc/core/variant/VariantInl.h

namespace
{

// Traditional enumeration type encapsulation for test.
enum TestTraditionalEnum
{
    TestTraditionalEnum1 = 1,
    TestTraditionalEnum2 = 2
};

// New style enumeration type encapsulation for test.
enum class TestNewStyleEnum
{
    TestNewStyleEnum1 = 1,
    TestNewStyleEnum2 = 2
};

}

// Variant test fixture.
class VariantTest : public testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

// Variant type test:
TEST_F(VariantTest, VariantType)
{
    /// XXX_TYPES array:
    // - RAW_TYPES:
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES.size(), 14lu);
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES[0], LLBC_VariantType::RAW_BOOL);
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES[1], LLBC_VariantType::RAW_SINT8);
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES[2], LLBC_VariantType::RAW_UINT8);
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES[3], LLBC_VariantType::RAW_SINT16);
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES[4], LLBC_VariantType::RAW_UINT16);
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES[5], LLBC_VariantType::RAW_SINT32);
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES[6], LLBC_VariantType::RAW_UINT32);
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES[7], LLBC_VariantType::RAW_LONG);
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES[8], LLBC_VariantType::RAW_ULONG);
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES[9], LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES[10], LLBC_VariantType::RAW_SINT64);
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES[11], LLBC_VariantType::RAW_UINT64);
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES[12], LLBC_VariantType::RAW_FLOAT);
    EXPECT_EQ(LLBC_VariantType::RAW_TYPES[13], LLBC_VariantType::RAW_DOUBLE);
    // - STR_TYPES:
    EXPECT_EQ(LLBC_VariantType::STR_TYPES.size(), 1lu);
    EXPECT_EQ(LLBC_VariantType::STR_TYPES[0], LLBC_VariantType::STR_DFT);
    // - SEQ_TYPES:
    EXPECT_EQ(LLBC_VariantType::SEQ_TYPES.size(), 1lu);
    EXPECT_EQ(LLBC_VariantType::SEQ_TYPES[0], LLBC_VariantType::SEQ_DFT);
    // - DICT_TYPES:
    EXPECT_EQ(LLBC_VariantType::DICT_TYPES.size(), 1lu);
    EXPECT_EQ(LLBC_VariantType::DICT_TYPES[0], LLBC_VariantType::DICT_DFT);
    // - ALL_TYPES:
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES.size(), 18lu);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[0], LLBC_VariantType::NIL);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[1], LLBC_VariantType::RAW_BOOL);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[2], LLBC_VariantType::RAW_SINT8);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[3], LLBC_VariantType::RAW_UINT8);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[4], LLBC_VariantType::RAW_SINT16);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[5], LLBC_VariantType::RAW_UINT16);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[6], LLBC_VariantType::RAW_SINT32);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[7], LLBC_VariantType::RAW_UINT32);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[8], LLBC_VariantType::RAW_LONG);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[9], LLBC_VariantType::RAW_ULONG);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[10], LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[11], LLBC_VariantType::RAW_SINT64);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[12], LLBC_VariantType::RAW_UINT64);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[13], LLBC_VariantType::RAW_FLOAT);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[14], LLBC_VariantType::RAW_DOUBLE);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[15], LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[16], LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ(LLBC_VariantType::ALL_TYPES[17], LLBC_VariantType::DICT_DFT);

    // NIL
    EXPECT_EQ(LLBC_VariantType::DeduceType<void>(), LLBC_VariantType::NIL);
    EXPECT_EQ(LLBC_VariantType::DeduceType<LLBC_Variant>(), LLBC_VariantType::NIL);
    EXPECT_EQ(LLBC_VariantType::DeduceType<LLBC_Packet>(), LLBC_VariantType::NIL);

    // RAW_BOOL
    EXPECT_EQ(LLBC_VariantType::DeduceType<bool>(), LLBC_VariantType::RAW_BOOL);
    EXPECT_EQ(LLBC_VariantType::DeduceType<bool &>(), LLBC_VariantType::RAW_BOOL);
    EXPECT_EQ(LLBC_VariantType::DeduceType<bool &&>(), LLBC_VariantType::RAW_BOOL);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const bool>(), LLBC_VariantType::RAW_BOOL);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const bool &&>(), LLBC_VariantType::RAW_BOOL);

    // Unsupported - signed char:
    EXPECT_EQ(LLBC_VariantType::DeduceType<signed char>(), LLBC_VariantType::NIL);
    // SINT8
    EXPECT_EQ(LLBC_VariantType::DeduceType<char>(), LLBC_VariantType::RAW_SINT8);
    EXPECT_EQ(LLBC_VariantType::DeduceType<char &>(), LLBC_VariantType::RAW_SINT8);
    EXPECT_EQ(LLBC_VariantType::DeduceType<char &&>(), LLBC_VariantType::RAW_SINT8);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const char>(), LLBC_VariantType::RAW_SINT8);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const char &>(), LLBC_VariantType::RAW_SINT8);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const char &&>(), LLBC_VariantType::RAW_SINT8);
    // UINT8 
    EXPECT_EQ(LLBC_VariantType::DeduceType<uint8>(), LLBC_VariantType::RAW_UINT8);
    EXPECT_EQ(LLBC_VariantType::DeduceType<uint8 &>(), LLBC_VariantType::RAW_UINT8);
    EXPECT_EQ(LLBC_VariantType::DeduceType<uint8 &&>(), LLBC_VariantType::RAW_UINT8);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const uint8>(), LLBC_VariantType::RAW_UINT8);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const uint8 &>(), LLBC_VariantType::RAW_UINT8);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const uint8 &&>(), LLBC_VariantType::RAW_UINT8);

    // Unsupported - wchar_t:
    EXPECT_EQ(LLBC_VariantType::DeduceType<wchar_t>(), LLBC_VariantType::NIL);
    // SINT16
    EXPECT_EQ(LLBC_VariantType::DeduceType<sint16>(), LLBC_VariantType::RAW_SINT16);
    EXPECT_EQ(LLBC_VariantType::DeduceType<sint16 &&>(), LLBC_VariantType::RAW_SINT16);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const sint16>(), LLBC_VariantType::RAW_SINT16);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const sint16 &>(), LLBC_VariantType::RAW_SINT16);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const sint16 &&>(), LLBC_VariantType::RAW_SINT16);
    // UINT16
    EXPECT_EQ(LLBC_VariantType::DeduceType<uint16>(), LLBC_VariantType::RAW_UINT16);
    EXPECT_EQ(LLBC_VariantType::DeduceType<uint16 &>(), LLBC_VariantType::RAW_UINT16);
    EXPECT_EQ(LLBC_VariantType::DeduceType<uint16 &&>(), LLBC_VariantType::RAW_UINT16);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const uint16>(), LLBC_VariantType::RAW_UINT16);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const uint16 &>(), LLBC_VariantType::RAW_UINT16);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const uint16 &&>(), LLBC_VariantType::RAW_UINT16);

    // SINT32
    EXPECT_EQ(LLBC_VariantType::DeduceType<sint32>(), LLBC_VariantType::RAW_SINT32);
    EXPECT_EQ(LLBC_VariantType::DeduceType<sint32 &>(), LLBC_VariantType::RAW_SINT32);
    EXPECT_EQ(LLBC_VariantType::DeduceType<sint32 &&>(), LLBC_VariantType::RAW_SINT32);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const sint32>(), LLBC_VariantType::RAW_SINT32);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const sint32 &>(), LLBC_VariantType::RAW_SINT32);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const sint32 &&>(), LLBC_VariantType::RAW_SINT32);
    // UINT32
    EXPECT_EQ(LLBC_VariantType::DeduceType<uint32>(), LLBC_VariantType::RAW_UINT32);
    EXPECT_EQ(LLBC_VariantType::DeduceType<uint32 &>(), LLBC_VariantType::RAW_UINT32);
    EXPECT_EQ(LLBC_VariantType::DeduceType<uint32 &&>(), LLBC_VariantType::RAW_UINT32);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const uint32>(), LLBC_VariantType::RAW_UINT32);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const uint32 &>(), LLBC_VariantType::RAW_UINT32);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const uint32 &&>(), LLBC_VariantType::RAW_UINT32);

    // LONG
    EXPECT_EQ(LLBC_VariantType::DeduceType<long>(), LLBC_VariantType::RAW_LONG);
    EXPECT_EQ(LLBC_VariantType::DeduceType<long &>(), LLBC_VariantType::RAW_LONG);
    EXPECT_EQ(LLBC_VariantType::DeduceType<long &&>(), LLBC_VariantType::RAW_LONG);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const long>(), LLBC_VariantType::RAW_LONG);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const long &>(), LLBC_VariantType::RAW_LONG);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const long &&>(), LLBC_VariantType::RAW_LONG);
    // ULONG
    EXPECT_EQ(LLBC_VariantType::DeduceType<ulong>(), LLBC_VariantType::RAW_ULONG);
    EXPECT_EQ(LLBC_VariantType::DeduceType<ulong &>(), LLBC_VariantType::RAW_ULONG);
    EXPECT_EQ(LLBC_VariantType::DeduceType<ulong &&>(), LLBC_VariantType::RAW_ULONG);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const ulong>(), LLBC_VariantType::RAW_ULONG);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const ulong &>(), LLBC_VariantType::RAW_ULONG);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const ulong &&>(), LLBC_VariantType::RAW_ULONG);

    // PTR
    EXPECT_EQ(LLBC_VariantType::DeduceType<std::nullptr_t>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<void *>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<void * const>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const void *>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const void * const>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<void *&>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<void *&&>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const void *&>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const void *&&>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<void * const &>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<void * const &&>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const void * const &>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const void * const &&>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<LLBC_Packet *>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<LLBC_Packet *&>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const LLBC_Packet *>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const LLBC_Packet *&>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<LLBC_Packet * const>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<LLBC_Packet * const &>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const LLBC_Packet * const>(), LLBC_VariantType::RAW_PTR);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const LLBC_Packet * const &>(), LLBC_VariantType::RAW_PTR);

    // SINT64
    EXPECT_EQ(LLBC_VariantType::DeduceType<sint64>(), LLBC_VariantType::RAW_SINT64);
    EXPECT_EQ(LLBC_VariantType::DeduceType<sint64 &>(), LLBC_VariantType::RAW_SINT64);
    EXPECT_EQ(LLBC_VariantType::DeduceType<sint64 &&>(), LLBC_VariantType::RAW_SINT64);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const sint64>(), LLBC_VariantType::RAW_SINT64);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const sint64 &>(), LLBC_VariantType::RAW_SINT64);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const sint64 &&>(), LLBC_VariantType::RAW_SINT64);
    // UINT64
    EXPECT_EQ(LLBC_VariantType::DeduceType<uint64>(), LLBC_VariantType::RAW_UINT64);
    EXPECT_EQ(LLBC_VariantType::DeduceType<uint64 &>(), LLBC_VariantType::RAW_UINT64);
    EXPECT_EQ(LLBC_VariantType::DeduceType<uint64 &&>(), LLBC_VariantType::RAW_UINT64);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const uint64>(), LLBC_VariantType::RAW_UINT64);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const uint64 &>(), LLBC_VariantType::RAW_UINT64);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const uint64 &&>(), LLBC_VariantType::RAW_UINT64);

    // ENUM
    #if LLBC_CUR_COMP == LLBC_COMP_MSVC
    EXPECT_EQ(LLBC_VariantType::DeduceType<TestTraditionalEnum>(), LLBC_VariantType::RAW_SINT32);
    #else // Non-MSVC compiler.
    EXPECT_EQ(LLBC_VariantType::DeduceType<TestTraditionalEnum>(), LLBC_VariantType::RAW_UINT32);
    #endif // MSVC compiler.
    EXPECT_EQ(LLBC_VariantType::DeduceType<TestNewStyleEnum>(), LLBC_VariantType::RAW_SINT32);

    // FLOAT
    EXPECT_EQ(LLBC_VariantType::DeduceType<float>(), LLBC_VariantType::RAW_FLOAT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<float &>(), LLBC_VariantType::RAW_FLOAT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<float &&>(), LLBC_VariantType::RAW_FLOAT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const float>(), LLBC_VariantType::RAW_FLOAT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const float &>(), LLBC_VariantType::RAW_FLOAT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const float &&>(), LLBC_VariantType::RAW_FLOAT);
    // DOUBLE
    EXPECT_EQ(LLBC_VariantType::DeduceType<double>(), LLBC_VariantType::RAW_DOUBLE);
    EXPECT_EQ(LLBC_VariantType::DeduceType<double &>(), LLBC_VariantType::RAW_DOUBLE);
    EXPECT_EQ(LLBC_VariantType::DeduceType<double &&>(), LLBC_VariantType::RAW_DOUBLE);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const double>(), LLBC_VariantType::RAW_DOUBLE);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const double &>(), LLBC_VariantType::RAW_DOUBLE);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const double &&>(), LLBC_VariantType::RAW_DOUBLE);
    // Unsupported - ldouble:
    EXPECT_EQ(LLBC_VariantType::DeduceType<ldouble>(), LLBC_VariantType::NIL);

    // Str - std::string:
    EXPECT_EQ(LLBC_VariantType::DeduceType<std::string>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<std::string &>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<std::string &&>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const std::string>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const std::string &>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const std::string &&>(), LLBC_VariantType::STR_DFT);
    // Str - LLBC_String:
    EXPECT_EQ(LLBC_VariantType::DeduceType<LLBC_String>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<LLBC_String &>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<LLBC_String &&>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const LLBC_String>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const LLBC_String &>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const LLBC_String &&>(), LLBC_VariantType::STR_DFT);
    // Str - std::string_view:
    EXPECT_EQ(LLBC_VariantType::DeduceType<std::string_view>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<std::string_view &>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const std::string_view>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const std::string_view &>(), LLBC_VariantType::STR_DFT);
    // Str - LLBC_CString:
    EXPECT_EQ(LLBC_VariantType::DeduceType<LLBC_CString>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<LLBC_CString &>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<LLBC_CString &&>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const LLBC_CString>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const LLBC_CString &>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const LLBC_CString &&>(), LLBC_VariantType::STR_DFT);
    // Str - string pointer:
    EXPECT_EQ(LLBC_VariantType::DeduceType<char *>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<char *&>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<char *&&>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const char *>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const char * &>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const char * &&>(), LLBC_VariantType::STR_DFT);
    // Str - char array:
    EXPECT_EQ(LLBC_VariantType::DeduceType<char[3]>(), LLBC_VariantType::STR_DFT);
    EXPECT_EQ(LLBC_VariantType::DeduceType<const char[3]>(), LLBC_VariantType::STR_DFT);

    // Seq:
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::pair<int, int>>()), LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::pair<LLBC_Variant, LLBC_Variant>>()), LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::pair<LLBC_Packet, int>>()), LLBC_VariantType::NIL);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::vector<int>>()), LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::vector<LLBC_Variant>>()), LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::vector<LLBC_Packet>>()), LLBC_VariantType::NIL);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::list<int>>()), LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::list<LLBC_Variant>>()), LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::list<LLBC_Packet>>()), LLBC_VariantType::NIL);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::queue<int>>()), LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::queue<LLBC_Variant>>()), LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::queue<LLBC_Packet>>()), LLBC_VariantType::NIL);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::deque<int>>()), LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::deque<LLBC_Variant>>()), LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::deque<LLBC_Packet>>()), LLBC_VariantType::NIL);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::set<int>>()), LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::set<LLBC_Variant>>()), LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::set<LLBC_Packet>>()), LLBC_VariantType::NIL);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_set<int>>()), LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_set<LLBC_Variant>>()), LLBC_VariantType::SEQ_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_set<LLBC_Packet>>()), LLBC_VariantType::NIL);
    EXPECT_EQ((LLBC_VariantType::DeduceType<
        std::pair<
            std::vector<
                std::list<
                    std::queue<
                        std::deque<
                            std::unordered_set<int>>>>>,
            std::vector<
                std::list<
                    std::queue<
                        std::deque<
                            std::set<
                                int>>>>>>>()), LLBC_VariantType::SEQ_DFT);

    // Dict:
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<int, float>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<int, LLBC_Packet>>()), LLBC_VariantType::NIL);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<int, LLBC_Packet *>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_map<int, float>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_map<int, LLBC_Packet>>()), LLBC_VariantType::NIL);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_map<int, LLBC_Packet *>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<bool, bool>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<sint8, uint8>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<sint16, uint16>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<sint32, uint32>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<long, ulong>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<sint64, uint64>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<float, double>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<std::string, int>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<LLBC_String, float>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<const char *, double>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<int, std::vector<int>>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<std::string, std::list<float>>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::map<int, std::map<int, std::string>>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_map<bool, bool>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_map<sint8, uint8>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_map<sint16, uint16>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_map<sint32, uint32>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_map<long, ulong>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_map<sint64, uint64>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_map<float, double>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_map<double, ldouble>>()), LLBC_VariantType::NIL);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_map<std::string, int>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_map<LLBC_String, float>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<std::unordered_map<const char *, double>>()), LLBC_VariantType::DICT_DFT);
    EXPECT_EQ((LLBC_VariantType::DeduceType<LLBC_Variant::Dict>()), LLBC_VariantType::DICT_DFT);
    
    // GetFirstType():
    EXPECT_EQ(LLBC_VariantType::GetFirstType(LLBC_VariantType::NIL), LLBC_VariantType::NIL);
    for (auto &rawType : LLBC_VariantType::RAW_TYPES)
        EXPECT_EQ(LLBC_VariantType::GetFirstType(rawType), LLBC_VariantType::RAW);
    for (auto &strType : LLBC_VariantType::STR_TYPES)
        EXPECT_EQ(LLBC_VariantType::GetFirstType(strType), LLBC_VariantType::STR);
    for (auto &seqType : LLBC_VariantType::SEQ_TYPES)
        EXPECT_EQ(LLBC_VariantType::GetFirstType(seqType), LLBC_VariantType::SEQ);
    for (auto &dictType : LLBC_VariantType::DICT_TYPES)
        EXPECT_EQ(LLBC_VariantType::GetFirstType(dictType), LLBC_VariantType::DICT);

    // IsConvertable():
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<void>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<bool>());
    EXPECT_FALSE(LLBC_VariantType::IsConvertable<signed char>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<sint8>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<uint8>());
    EXPECT_FALSE(LLBC_VariantType::IsConvertable<wchar_t>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<sint16>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<uint16>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<sint32>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<uint32>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<long>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<ulong>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<sint64>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<uint64>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<float>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<double>());
    EXPECT_FALSE(LLBC_VariantType::IsConvertable<ldouble>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<void *>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<LLBC_Packet *>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<LLBC_Packet * const>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<const LLBC_Packet *>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<const LLBC_Packet * const>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<TestTraditionalEnum>());
    EXPECT_TRUE(LLBC_VariantType::IsConvertable<TestNewStyleEnum>());
    EXPECT_FALSE(LLBC_VariantType::IsConvertable<LLBC_Packet>());

    EXPECT_TRUE((LLBC_VariantType::IsConvertable<typename std::pair<int, void *>>()));
    EXPECT_TRUE((LLBC_VariantType::IsConvertable<
        typename std::pair<int, typename std::pair<void *, float>>>()));

    EXPECT_TRUE((LLBC_VariantType::IsConvertable<typename std::vector<int>>()));
    EXPECT_TRUE((LLBC_VariantType::IsConvertable<typename std::list<int>>()));
    EXPECT_TRUE((LLBC_VariantType::IsConvertable<typename std::queue<int>>()));
    EXPECT_TRUE((LLBC_VariantType::IsConvertable<typename std::deque<int>>()));
    EXPECT_TRUE((LLBC_VariantType::IsConvertable<typename std::set<int>>()));
    EXPECT_TRUE((LLBC_VariantType::IsConvertable<typename std::unordered_set<int>>()));
    EXPECT_TRUE((LLBC_VariantType::IsConvertable<typename std::map<int, float>>()));
    EXPECT_TRUE((LLBC_VariantType::IsConvertable<typename std::unordered_map<int, float>>()));
    EXPECT_TRUE((LLBC_VariantType::IsConvertable<
        typename std::vector<std::list<std::queue<std::deque<std::set<std::unordered_set<int>>>>>>>()));

    EXPECT_FALSE((LLBC_VariantType::IsConvertable<typename std::vector<LLBC_Packet>>()));
    EXPECT_FALSE((LLBC_VariantType::IsConvertable<typename std::list<LLBC_Packet>>()));
    EXPECT_FALSE((LLBC_VariantType::IsConvertable<typename std::queue<LLBC_Packet>>()));
    EXPECT_FALSE((LLBC_VariantType::IsConvertable<typename std::deque<LLBC_Packet>>()));
    EXPECT_FALSE((LLBC_VariantType::IsConvertable<typename std::set<LLBC_Packet>>()));
    EXPECT_FALSE((LLBC_VariantType::IsConvertable<typename std::unordered_set<LLBC_Packet>>()));
    EXPECT_FALSE((LLBC_VariantType::IsConvertable<typename std::map<int, LLBC_Packet>>()));
    EXPECT_FALSE((LLBC_VariantType::IsConvertable<typename std::unordered_map<int, LLBC_Packet>>()));
    EXPECT_FALSE((LLBC_VariantType::IsConvertable<
        typename std::vector<std::list<std::queue<std::deque<LLBC_Packet>>>>>()));
    EXPECT_FALSE((LLBC_VariantType::IsConvertable<
        typename std::map<int, std::unordered_map<int, LLBC_Packet>>>()));

    EXPECT_TRUE(LLBC_VariantType::IsConvertable<LLBC_Variant>());

    //DeduceClass::Get<>:
    // - Nil:
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::NIL>::Class, void>));
    // - Raw:
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_BOOL>::Class, bool>));
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_SINT8>::Class, sint8>));
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_UINT8>::Class, uint8>));
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_SINT16>::Class, sint16>));
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_UINT16>::Class, uint16>));
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_SINT32>::Class, sint32>));
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_UINT32>::Class, uint32>));
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_LONG>::Class, long>));
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_ULONG>::Class, ulong>));
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_SINT64>::Class, sint64>));
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_UINT64>::Class, uint64>));
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_FLOAT>::Class, float>));
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::RAW_DOUBLE>::Class, double>));
    // - Str:
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::STR_DFT>::Class, LLBC_Variant::Str>));
    // - Seq:
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::SEQ_DFT>::Class, LLBC_Variant::Seq>));
    // - Dict:
    EXPECT_TRUE((std::is_same_v<LLBC_VariantType::DeduceClass::Get<LLBC_VariantType::DICT_DFT>::Class, LLBC_Variant::Dict>));

    // IsNil():
    EXPECT_TRUE(LLBC_VariantType::IsNil<void>());
    EXPECT_TRUE(LLBC_VariantType::IsNil<LLBC_Variant>());
    EXPECT_TRUE(LLBC_VariantType::IsNil<LLBC_Packet>());
    
    // IsValid():
    for (auto &varType : LLBC_VariantType::ALL_TYPES)
        EXPECT_TRUE(LLBC_VariantType::IsValid(varType));

    EXPECT_TRUE(!LLBC_VariantType::IsValid(0x9527));

    for (auto varType : LLBC_VariantType::ALL_TYPES)
        EXPECT_TRUE(LLBC_VariantType::IsValid(varType)) << "ALL_TYPES - " << LLBC_VariantType::Type2Str(varType);

    // IsRaw()/IsStr()/IsSeq()/IsDict():
    std::set<LLBC_VariantType::ENUM> rawTypeSet;
    for (auto &rawType : LLBC_VariantType::RAW_TYPES)
    {
        EXPECT_TRUE(!LLBC_VariantType::IsNil(rawType)) << "rawType:" << LLBC_VariantType::Type2Str(rawType);
        EXPECT_TRUE(LLBC_VariantType::IsRaw(rawType)) << "rawType:" << LLBC_VariantType::Type2Str(rawType);
        EXPECT_TRUE(!LLBC_VariantType::IsStr(rawType)) << "rawType:" << LLBC_VariantType::Type2Str(rawType);
        EXPECT_TRUE(!LLBC_VariantType::IsSeq(rawType)) << "rawType:" << LLBC_VariantType::Type2Str(rawType);
        EXPECT_TRUE(!LLBC_VariantType::IsDict(rawType)) << "rawType:" << LLBC_VariantType::Type2Str(rawType);

        EXPECT_TRUE(rawTypeSet.count(rawType) == 0) << "rawType:" << LLBC_VariantType::Type2Str(rawType);
        rawTypeSet.emplace(rawType);
    }

    std::set<LLBC_VariantType::ENUM> strTypeSet;
    for (auto &strType : LLBC_VariantType::STR_TYPES)
    {
        EXPECT_TRUE(!LLBC_VariantType::IsNil(strType)) << "Type:" << LLBC_VariantType::Type2Str(strType).c_str();
        EXPECT_TRUE(!LLBC_VariantType::IsRaw(strType)) << "strType:" << LLBC_VariantType::Type2Str(strType).c_str();
        EXPECT_TRUE(LLBC_VariantType::IsStr(strType)) << "strType:" << LLBC_VariantType::Type2Str(strType).c_str();
        EXPECT_TRUE(!LLBC_VariantType::IsSeq(strType)) << "strType:" << LLBC_VariantType::Type2Str(strType).c_str();
        EXPECT_TRUE(!LLBC_VariantType::IsDict(strType)) << "strType:" << LLBC_VariantType::Type2Str(strType).c_str();

        EXPECT_TRUE(strTypeSet.count(strType) == 0) << "strType:" << LLBC_VariantType::Type2Str(strType).c_str();
        strTypeSet.emplace(strType);
    }

    std::set<LLBC_VariantType::ENUM> seqTypeSet;
    for (auto &seqType : LLBC_VariantType::SEQ_TYPES)
    {
        EXPECT_TRUE(!LLBC_VariantType::IsNil(seqType)) << "Type:" << LLBC_VariantType::Type2Str(seqType);
        EXPECT_TRUE(!LLBC_VariantType::IsRaw(seqType)) << "seqType:" << LLBC_VariantType::Type2Str(seqType);
        EXPECT_TRUE(!LLBC_VariantType::IsStr(seqType)) << "seqType:" << LLBC_VariantType::Type2Str(seqType);
        EXPECT_TRUE(LLBC_VariantType::IsSeq(seqType)) << "seqType:" << LLBC_VariantType::Type2Str(seqType);
        EXPECT_TRUE(!LLBC_VariantType::IsDict(seqType)) << "seqType:" << LLBC_VariantType::Type2Str(seqType);

        EXPECT_TRUE(seqTypeSet.count(seqType) == 0) << "seqType:" << LLBC_VariantType::Type2Str(seqType);
        seqTypeSet.emplace(seqType);
    }

    std::set<LLBC_VariantType::ENUM> dictTypeSet;
    for (auto &dictType : LLBC_VariantType::DICT_TYPES)
    {
        EXPECT_TRUE(!LLBC_VariantType::IsNil(dictType)) << "Type:" << LLBC_VariantType::Type2Str(dictType);
        EXPECT_TRUE(!LLBC_VariantType::IsRaw(dictType)) << "dictType:" << LLBC_VariantType::Type2Str(dictType);
        EXPECT_TRUE(!LLBC_VariantType::IsStr(dictType)) << "dictType:" << LLBC_VariantType::Type2Str(dictType);
        EXPECT_TRUE(!LLBC_VariantType::IsSeq(dictType)) << "dictType:" << LLBC_VariantType::Type2Str(dictType);
        EXPECT_TRUE(LLBC_VariantType::IsDict(dictType)) << "dictType:" << LLBC_VariantType::Type2Str(dictType);

        EXPECT_TRUE(dictTypeSet.count(dictType) == 0) << "dictType:" << LLBC_VariantType::Type2Str(dictType);
        dictTypeSet.emplace(dictType);
    }

    // Type2Str():
    EXPECT_EQ(LLBC_VariantType::Type2Str<void>(), "Nil");
    EXPECT_EQ(LLBC_VariantType::Type2Str<bool>(), "bool");
    EXPECT_EQ(LLBC_VariantType::Type2Str<sint8>(), "int8");
    EXPECT_EQ(LLBC_VariantType::Type2Str<uint8>(), "uint8");
    EXPECT_EQ(LLBC_VariantType::Type2Str<sint16>(), "int16");
    EXPECT_EQ(LLBC_VariantType::Type2Str<uint16>(), "uint16");
    EXPECT_EQ(LLBC_VariantType::Type2Str<sint32>(), "int32");
    EXPECT_EQ(LLBC_VariantType::Type2Str<uint32>(), "uint32");
    EXPECT_EQ(LLBC_VariantType::Type2Str<long>(), "long");
    EXPECT_EQ(LLBC_VariantType::Type2Str<ulong>(), "ulong");
    EXPECT_EQ(LLBC_VariantType::Type2Str<sint64>(), "int64");
    EXPECT_EQ(LLBC_VariantType::Type2Str<uint64>(), "uint64");
    EXPECT_EQ(LLBC_VariantType::Type2Str<float>(), "float");
    EXPECT_EQ(LLBC_VariantType::Type2Str<double>(), "double");
    EXPECT_EQ(LLBC_VariantType::Type2Str<std::string>(), "string");
    EXPECT_EQ(LLBC_VariantType::Type2Str<LLBC_Variant::Seq>(), "sequence");
    EXPECT_EQ(LLBC_VariantType::Type2Str<LLBC_Variant::Dict>(), "dictionary");

    // NIL/RAW_TYPES/STR_TYPES/SEQ_TYPES/DICT_TYPES about test.
    EXPECT_EQ((1/* NIL */ +
                LLBC_VariantType::RAW_TYPES.size() +
                LLBC_VariantType::STR_TYPES.size() +
                LLBC_VariantType::SEQ_TYPES.size() +
                LLBC_VariantType::DICT_TYPES.size()),
              LLBC_VariantType::ALL_TYPES.size());
}