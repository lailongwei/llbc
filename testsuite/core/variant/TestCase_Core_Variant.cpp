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


#include "TestCase_Core_Variant.h"

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

int TestCase_Core_Variant::Run(int argc, char *argv[])
{
    LLBC_Expect(VariantTypeTest() == LLBC_OK);
    LLBC_Expect(ConstructTest() == LLBC_OK);
    LLBC_Expect(AssignmentTest() == LLBC_OK);
    LLBC_Expect(GetTypeTest() == LLBC_OK);
    LLBC_Expect(IsXxxTest() == LLBC_OK);
    LLBC_Expect(BecomeTest() == LLBC_OK);
    LLBC_Expect(AsTest() == LLBC_OK);
    LLBC_Expect(AsLooseBoolTest() == LLBC_OK);
    LLBC_Expect(ImplicitValueFetchTest() == LLBC_OK);
    LLBC_Expect(ClearTest() == LLBC_OK);
    LLBC_Expect(IsEmptyTest() == LLBC_OK);
    LLBC_Expect(SizeAndCapacityTest() == LLBC_OK);
    LLBC_Expect(CountAndContainsTest() == LLBC_OK);
    LLBC_Expect(SeqSpecificTest() == LLBC_OK);
    LLBC_Expect(DictSpecificTest() == LLBC_OK);

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}

int TestCase_Core_Variant::VariantTypeTest()
{
    LLBC_PrintLn("Variant type test:");

    // NIL
    LLBC_Expect(LLBC_VariantType::DeduceType<void>() == LLBC_VariantType::NIL);
    LLBC_Expect(LLBC_VariantType::DeduceType<LLBC_Variant>() == LLBC_VariantType::NIL);
    LLBC_Expect(LLBC_VariantType::DeduceType<LLBC_Packet>() == LLBC_VariantType::NIL);

    // RAW_BOOL
    LLBC_Expect(LLBC_VariantType::DeduceType<bool>() == LLBC_VariantType::RAW_BOOL);
    LLBC_Expect(LLBC_VariantType::DeduceType<bool &>() == LLBC_VariantType::RAW_BOOL);
    LLBC_Expect(LLBC_VariantType::DeduceType<bool &&>() == LLBC_VariantType::RAW_BOOL);
    LLBC_Expect(LLBC_VariantType::DeduceType<const bool>() == LLBC_VariantType::RAW_BOOL);
    LLBC_Expect(LLBC_VariantType::DeduceType<const bool &&>() == LLBC_VariantType::RAW_BOOL);

    // SINT8
    LLBC_Expect(LLBC_VariantType::DeduceType<char>() == LLBC_VariantType::RAW_SINT8);
    LLBC_Expect(LLBC_VariantType::DeduceType<char &>() == LLBC_VariantType::RAW_SINT8);
    LLBC_Expect(LLBC_VariantType::DeduceType<char &&>() == LLBC_VariantType::RAW_SINT8);
    LLBC_Expect(LLBC_VariantType::DeduceType<const char>() == LLBC_VariantType::RAW_SINT8);
    LLBC_Expect(LLBC_VariantType::DeduceType<const char &>() == LLBC_VariantType::RAW_SINT8);
    LLBC_Expect(LLBC_VariantType::DeduceType<const char &&>() == LLBC_VariantType::RAW_SINT8);
    // UINT8 
    LLBC_Expect(LLBC_VariantType::DeduceType<uint8>() == LLBC_VariantType::RAW_UINT8);
    LLBC_Expect(LLBC_VariantType::DeduceType<uint8 &>() == LLBC_VariantType::RAW_UINT8);
    LLBC_Expect(LLBC_VariantType::DeduceType<uint8 &&>() == LLBC_VariantType::RAW_UINT8);
    LLBC_Expect(LLBC_VariantType::DeduceType<const uint8>() == LLBC_VariantType::RAW_UINT8);
    LLBC_Expect(LLBC_VariantType::DeduceType<const uint8 &>() == LLBC_VariantType::RAW_UINT8);
    LLBC_Expect(LLBC_VariantType::DeduceType<const uint8 &&>() == LLBC_VariantType::RAW_UINT8);

    // SINT16
    LLBC_Expect(LLBC_VariantType::DeduceType<sint16>() == LLBC_VariantType::RAW_SINT16);
    LLBC_Expect(LLBC_VariantType::DeduceType<sint16 &&>() == LLBC_VariantType::RAW_SINT16);
    LLBC_Expect(LLBC_VariantType::DeduceType<const sint16>() == LLBC_VariantType::RAW_SINT16);
    LLBC_Expect(LLBC_VariantType::DeduceType<const sint16 &>() == LLBC_VariantType::RAW_SINT16);
    LLBC_Expect(LLBC_VariantType::DeduceType<const sint16 &&>() == LLBC_VariantType::RAW_SINT16);
    // UINT16
    LLBC_Expect(LLBC_VariantType::DeduceType<uint16>() == LLBC_VariantType::RAW_UINT16);
    LLBC_Expect(LLBC_VariantType::DeduceType<uint16 &>() == LLBC_VariantType::RAW_UINT16);
    LLBC_Expect(LLBC_VariantType::DeduceType<uint16 &&>() == LLBC_VariantType::RAW_UINT16);
    LLBC_Expect(LLBC_VariantType::DeduceType<const uint16>() == LLBC_VariantType::RAW_UINT16);
    LLBC_Expect(LLBC_VariantType::DeduceType<const uint16 &>() == LLBC_VariantType::RAW_UINT16);
    LLBC_Expect(LLBC_VariantType::DeduceType<const uint16 &&>() == LLBC_VariantType::RAW_UINT16);

    // SINT32
    LLBC_Expect(LLBC_VariantType::DeduceType<sint32>() == LLBC_VariantType::RAW_SINT32);
    LLBC_Expect(LLBC_VariantType::DeduceType<sint32 &>() == LLBC_VariantType::RAW_SINT32);
    LLBC_Expect(LLBC_VariantType::DeduceType<sint32 &&>() == LLBC_VariantType::RAW_SINT32);
    LLBC_Expect(LLBC_VariantType::DeduceType<const sint32>() == LLBC_VariantType::RAW_SINT32);
    LLBC_Expect(LLBC_VariantType::DeduceType<const sint32 &>() == LLBC_VariantType::RAW_SINT32);
    LLBC_Expect(LLBC_VariantType::DeduceType<const sint32 &&>() == LLBC_VariantType::RAW_SINT32);
    // UINT32
    LLBC_Expect(LLBC_VariantType::DeduceType<uint32>() == LLBC_VariantType::RAW_UINT32);
    LLBC_Expect(LLBC_VariantType::DeduceType<uint32 &>() == LLBC_VariantType::RAW_UINT32);
    LLBC_Expect(LLBC_VariantType::DeduceType<uint32 &&>() == LLBC_VariantType::RAW_UINT32);
    LLBC_Expect(LLBC_VariantType::DeduceType<const uint32>() == LLBC_VariantType::RAW_UINT32);
    LLBC_Expect(LLBC_VariantType::DeduceType<const uint32 &>() == LLBC_VariantType::RAW_UINT32);
    LLBC_Expect(LLBC_VariantType::DeduceType<const uint32 &&>() == LLBC_VariantType::RAW_UINT32);

    // LONG
    LLBC_Expect(LLBC_VariantType::DeduceType<long>() == LLBC_VariantType::RAW_LONG);
    LLBC_Expect(LLBC_VariantType::DeduceType<long &>() == LLBC_VariantType::RAW_LONG);
    LLBC_Expect(LLBC_VariantType::DeduceType<long &&>() == LLBC_VariantType::RAW_LONG);
    LLBC_Expect(LLBC_VariantType::DeduceType<const long>() == LLBC_VariantType::RAW_LONG);
    LLBC_Expect(LLBC_VariantType::DeduceType<const long &>() == LLBC_VariantType::RAW_LONG);
    LLBC_Expect(LLBC_VariantType::DeduceType<const long &&>() == LLBC_VariantType::RAW_LONG);
    // ULONG
    LLBC_Expect(LLBC_VariantType::DeduceType<ulong>() == LLBC_VariantType::RAW_ULONG);
    LLBC_Expect(LLBC_VariantType::DeduceType<ulong &>() == LLBC_VariantType::RAW_ULONG);
    LLBC_Expect(LLBC_VariantType::DeduceType<ulong &&>() == LLBC_VariantType::RAW_ULONG);
    LLBC_Expect(LLBC_VariantType::DeduceType<const ulong>() == LLBC_VariantType::RAW_ULONG);
    LLBC_Expect(LLBC_VariantType::DeduceType<const ulong &>() == LLBC_VariantType::RAW_ULONG);
    LLBC_Expect(LLBC_VariantType::DeduceType<const ulong &&>() == LLBC_VariantType::RAW_ULONG);

    // PTR
    LLBC_Expect(LLBC_VariantType::DeduceType<std::nullptr_t>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<void *>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<void * const>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<const void *>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<const void * const>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<void *&>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<void *&&>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<const void *&>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<const void *&&>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<void * const &>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<void * const &&>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<const void * const &>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<const void * const &&>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<LLBC_Packet *>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<LLBC_Packet *&>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<const LLBC_Packet *>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<const LLBC_Packet *&>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<LLBC_Packet * const>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<LLBC_Packet * const &>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<const LLBC_Packet * const>() == LLBC_VariantType::RAW_PTR);
    LLBC_Expect(LLBC_VariantType::DeduceType<const LLBC_Packet * const &>() == LLBC_VariantType::RAW_PTR);

    // SINT64
    LLBC_Expect(LLBC_VariantType::DeduceType<sint64>() == LLBC_VariantType::RAW_SINT64);
    LLBC_Expect(LLBC_VariantType::DeduceType<sint64 &>() == LLBC_VariantType::RAW_SINT64);
    LLBC_Expect(LLBC_VariantType::DeduceType<sint64 &&>() == LLBC_VariantType::RAW_SINT64);
    LLBC_Expect(LLBC_VariantType::DeduceType<const sint64>() == LLBC_VariantType::RAW_SINT64);
    LLBC_Expect(LLBC_VariantType::DeduceType<const sint64 &>() == LLBC_VariantType::RAW_SINT64);
    LLBC_Expect(LLBC_VariantType::DeduceType<const sint64 &&>() == LLBC_VariantType::RAW_SINT64);
    // UINT64
    LLBC_Expect(LLBC_VariantType::DeduceType<uint64>() == LLBC_VariantType::RAW_UINT64);
    LLBC_Expect(LLBC_VariantType::DeduceType<uint64 &>() == LLBC_VariantType::RAW_UINT64);
    LLBC_Expect(LLBC_VariantType::DeduceType<uint64 &&>() == LLBC_VariantType::RAW_UINT64);
    LLBC_Expect(LLBC_VariantType::DeduceType<const uint64>() == LLBC_VariantType::RAW_UINT64);
    LLBC_Expect(LLBC_VariantType::DeduceType<const uint64 &>() == LLBC_VariantType::RAW_UINT64);
    LLBC_Expect(LLBC_VariantType::DeduceType<const uint64 &&>() == LLBC_VariantType::RAW_UINT64);

    // ENUM
    LLBC_Expect(LLBC_VariantType::DeduceType<TestTraditionalEnum>() == LLBC_VariantType::RAW_UINT32,
                "TestTraditonalEnum underlying type:%s",
                LLBC_GetTypeName(std::underlying_type_t<TestTraditionalEnum>));
    LLBC_Expect(LLBC_VariantType::DeduceType<TestNewStyleEnum>() == LLBC_VariantType::RAW_SINT32);

    // FLOAT
    LLBC_Expect(LLBC_VariantType::DeduceType<float>() == LLBC_VariantType::RAW_FLOAT);
    LLBC_Expect(LLBC_VariantType::DeduceType<float &>() == LLBC_VariantType::RAW_FLOAT);
    LLBC_Expect(LLBC_VariantType::DeduceType<float &&>() == LLBC_VariantType::RAW_FLOAT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const float>() == LLBC_VariantType::RAW_FLOAT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const float &>() == LLBC_VariantType::RAW_FLOAT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const float &&>() == LLBC_VariantType::RAW_FLOAT);
    // DOUBLE
    LLBC_Expect(LLBC_VariantType::DeduceType<double>() == LLBC_VariantType::RAW_DOUBLE);
    LLBC_Expect(LLBC_VariantType::DeduceType<double &>() == LLBC_VariantType::RAW_DOUBLE);
    LLBC_Expect(LLBC_VariantType::DeduceType<double &&>() == LLBC_VariantType::RAW_DOUBLE);
    LLBC_Expect(LLBC_VariantType::DeduceType<const double>() == LLBC_VariantType::RAW_DOUBLE);
    LLBC_Expect(LLBC_VariantType::DeduceType<const double &>() == LLBC_VariantType::RAW_DOUBLE);
    LLBC_Expect(LLBC_VariantType::DeduceType<const double &&>() == LLBC_VariantType::RAW_DOUBLE);

    // Str - std::string:
    LLBC_Expect(LLBC_VariantType::DeduceType<std::string>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::string &>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::string &&>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const std::string>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const std::string &>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const std::string &&>() == LLBC_VariantType::STR_DFT);
    // Str - LLBC_String:
    LLBC_Expect(LLBC_VariantType::DeduceType<LLBC_String>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<LLBC_String &>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<LLBC_String &&>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const LLBC_String>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const LLBC_String &>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const LLBC_String &&>() == LLBC_VariantType::STR_DFT);
    // Str - std::string_view:
    LLBC_Expect(LLBC_VariantType::DeduceType<std::string_view>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::string_view &>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const std::string_view>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const std::string_view &>() == LLBC_VariantType::STR_DFT);
    // Str - LLBC_CString:
    LLBC_Expect(LLBC_VariantType::DeduceType<LLBC_CString>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<LLBC_CString &>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<LLBC_CString &&>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const LLBC_CString>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const LLBC_CString &>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const LLBC_CString &&>() == LLBC_VariantType::STR_DFT);
    // Str - string pointer:
    LLBC_Expect(LLBC_VariantType::DeduceType<char *>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<char *&>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<char *&&>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const char *>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const char * &>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const char * &&>() == LLBC_VariantType::STR_DFT);
    // Str - char array:
    LLBC_Expect(LLBC_VariantType::DeduceType<char[3]>() == LLBC_VariantType::STR_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<const char[3]>() == LLBC_VariantType::STR_DFT);

    // Seq:
    LLBC_Expect((LLBC_VariantType::DeduceType<std::pair<int, int>>() == LLBC_VariantType::SEQ_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::pair<LLBC_Variant, LLBC_Variant>>() == LLBC_VariantType::SEQ_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::pair<LLBC_Packet, int>>() == LLBC_VariantType::NIL));
    LLBC_Expect(LLBC_VariantType::DeduceType<std::vector<int>>() == LLBC_VariantType::SEQ_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::vector<LLBC_Variant>>() == LLBC_VariantType::SEQ_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::vector<LLBC_Packet>>() == LLBC_VariantType::NIL);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::list<int>>() == LLBC_VariantType::SEQ_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::list<LLBC_Variant>>() == LLBC_VariantType::SEQ_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::list<LLBC_Packet>>() == LLBC_VariantType::NIL);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::queue<int>>() == LLBC_VariantType::SEQ_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::queue<LLBC_Variant>>() == LLBC_VariantType::SEQ_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::queue<LLBC_Packet>>() == LLBC_VariantType::NIL);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::deque<int>>() == LLBC_VariantType::SEQ_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::deque<LLBC_Variant>>() == LLBC_VariantType::SEQ_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::deque<LLBC_Packet>>() == LLBC_VariantType::NIL);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::set<int>>() == LLBC_VariantType::SEQ_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::set<LLBC_Variant>>() == LLBC_VariantType::SEQ_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::set<LLBC_Packet>>() == LLBC_VariantType::NIL);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::unordered_set<int>>() == LLBC_VariantType::SEQ_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::unordered_set<LLBC_Variant>>() == LLBC_VariantType::SEQ_DFT);
    LLBC_Expect(LLBC_VariantType::DeduceType<std::unordered_set<LLBC_Packet>>() == LLBC_VariantType::NIL);
    LLBC_Expect((LLBC_VariantType::DeduceType<
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
                                int>>>>>>>() == LLBC_VariantType::SEQ_DFT));

    // Dict:
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<int, float>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<int, LLBC_Packet>>() == LLBC_VariantType::NIL));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<int, LLBC_Packet *>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::unordered_map<int, float>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::unordered_map<int, LLBC_Packet>>() == LLBC_VariantType::NIL));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::unordered_map<int, LLBC_Packet *>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<bool, bool>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<sint8, uint8>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<sint16, uint16>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<sint32, uint32>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<long, ulong>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<sint64, uint64>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<float, double>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<std::string, int>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<LLBC_String, float>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<const char *, double>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<int, std::vector<int>>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<std::string, std::list<float>>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::map<int, std::map<int, std::string>>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::unordered_map<bool, bool>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::unordered_map<sint8, uint8>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::unordered_map<sint16, uint16>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::unordered_map<sint32, uint32>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::unordered_map<long, ulong>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::unordered_map<sint64, uint64>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::unordered_map<float, double>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::unordered_map<std::string, int>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::unordered_map<LLBC_String, float>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<std::unordered_map<const char *, double>>() == LLBC_VariantType::DICT_DFT));
    LLBC_Expect((LLBC_VariantType::DeduceType<LLBC_Variant::Dict>() == LLBC_VariantType::DICT_DFT));
    
    // GetFirstType():
    LLBC_Expect(LLBC_VariantType::GetFirstType(LLBC_VariantType::NIL) == LLBC_VariantType::NIL);
    for (auto &rawType : LLBC_VariantType::RAW_TYPES)
        LLBC_Expect(LLBC_VariantType::GetFirstType(rawType) == LLBC_VariantType::RAW,
                    "rawType:%s", LLBC_VariantType::Type2Str(rawType).c_str());
    for (auto &strType : LLBC_VariantType::STR_TYPES)
        LLBC_Expect(LLBC_VariantType::GetFirstType(strType) == LLBC_VariantType::STR,
                    "strType:%s", LLBC_VariantType::Type2Str(strType).c_str());
    for (auto &seqType : LLBC_VariantType::SEQ_TYPES)
        LLBC_Expect(LLBC_VariantType::GetFirstType(seqType) == LLBC_VariantType::SEQ,
                    "seqType:%s", LLBC_VariantType::Type2Str(seqType).c_str());
    for (auto &dictType : LLBC_VariantType::DICT_TYPES)
        LLBC_Expect(LLBC_VariantType::GetFirstType(dictType) == LLBC_VariantType::DICT,
                    "dictType:%s", LLBC_VariantType::Type2Str(dictType).c_str());

    // IsConvertable():
    LLBC_Expect(LLBC_VariantType::IsConvertable<void>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<bool>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<sint8>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<uint8>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<sint16>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<uint16>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<sint32>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<uint32>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<long>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<ulong>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<sint64>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<uint64>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<float>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<double>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<void *>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<LLBC_Packet *>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<LLBC_Packet *>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<LLBC_Packet *>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<TestTraditionalEnum>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<TestNewStyleEnum>() == true);
    LLBC_Expect(LLBC_VariantType::IsConvertable<LLBC_Packet>() == false);

    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::pair<int, void *>>()) == true);
    LLBC_Expect((LLBC_VariantType::IsConvertable<
        typename std::pair<int, typename std::pair<void *, float>>>()) == true);

    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::vector<int>>()) == true);
    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::list<int>>()) == true);
    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::queue<int>>()) == true);
    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::deque<int>>()) == true);
    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::set<int>>()) == true);
    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::unordered_set<int>>()) == true);
    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::map<int, float>>()) == true);
    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::unordered_map<int, float>>()) == true);
    LLBC_Expect((LLBC_VariantType::IsConvertable<
        typename std::vector<std::list<std::queue<std::deque<std::set<std::unordered_set<int>>>>>>>()) == true);

    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::vector<LLBC_Packet>>()) == false);
    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::list<LLBC_Packet>>()) == false);
    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::queue<LLBC_Packet>>()) == false);
    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::deque<LLBC_Packet>>()) == false);
    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::set<LLBC_Packet>>()) == false);
    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::unordered_set<LLBC_Packet>>()) == false);
    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::map<int, LLBC_Packet>>()) == false);
    LLBC_Expect((LLBC_VariantType::IsConvertable<typename std::unordered_map<int, LLBC_Packet>>()) == false);
    LLBC_Expect((LLBC_VariantType::IsConvertable<
        typename std::vector<std::list<std::queue<std::deque<LLBC_Packet>>>>>()) == false);
    LLBC_Expect((LLBC_VariantType::IsConvertable<
        typename std::map<int, std::unordered_map<int, LLBC_Packet>>>()) == false);

    LLBC_Expect((LLBC_VariantType::IsConvertable<LLBC_Variant>()) == true);

    // IsNil():
    LLBC_Expect(LLBC_VariantType::IsNil<void>());
    LLBC_Expect(LLBC_VariantType::IsNil<LLBC_Variant>());
    LLBC_Expect(LLBC_VariantType::IsNil<LLBC_Packet>());
    
    // IsValid():
    for (auto &varType : LLBC_VariantType::ALL_TYPES)
    {
        LLBC_Expect(LLBC_VariantType::IsValid(varType),
                    "varType:%s", LLBC_VariantType::Type2Str(varType).c_str());
    }
    LLBC_Expect(!LLBC_VariantType::IsValid(0x9527));

    for (auto varType : LLBC_VariantType::ALL_TYPES)
        LLBC_Expect(LLBC_VariantType::IsValid(varType), "ALL_TYPES - %s", LLBC_VariantType::Type2Str(varType).c_str());

    // IsRaw()/IsStr()/IsSeq()/IsDict():
    std::set<LLBC_VariantType::ENUM> rawTypeSet;
    for (auto &rawType : LLBC_VariantType::RAW_TYPES)
    {
        LLBC_Expect(!LLBC_VariantType::IsNil(rawType),
                    "rawType:%s", LLBC_VariantType::Type2Str(rawType).c_str());
        LLBC_Expect(LLBC_VariantType::IsRaw(rawType),
                    "rawType:%s", LLBC_VariantType::Type2Str(rawType).c_str());
        LLBC_Expect(!LLBC_VariantType::IsStr(rawType),
                    "rawType:%s", LLBC_VariantType::Type2Str(rawType).c_str());
        LLBC_Expect(!LLBC_VariantType::IsSeq(rawType),
                    "rawType:%s", LLBC_VariantType::Type2Str(rawType).c_str());
        LLBC_Expect(!LLBC_VariantType::IsDict(rawType),
                    "rawType:%s", LLBC_VariantType::Type2Str(rawType).c_str());

        LLBC_Expect(rawTypeSet.count(rawType) == 0);
        rawTypeSet.emplace(rawType);
    }

    std::set<LLBC_VariantType::ENUM> strTypeSet;
    for (auto &strType : LLBC_VariantType::STR_TYPES)
    {
        LLBC_Expect(!LLBC_VariantType::IsNil(strType),
                    "Type:%s", LLBC_VariantType::Type2Str(strType).c_str());
        LLBC_Expect(!LLBC_VariantType::IsRaw(strType),
                    "strType:%s", LLBC_VariantType::Type2Str(strType).c_str());
        LLBC_Expect(LLBC_VariantType::IsStr(strType),
                    "strType:%s", LLBC_VariantType::Type2Str(strType).c_str());
        LLBC_Expect(!LLBC_VariantType::IsSeq(strType),
                    "strType:%s", LLBC_VariantType::Type2Str(strType).c_str());
        LLBC_Expect(!LLBC_VariantType::IsDict(strType),
                    "strType:%s", LLBC_VariantType::Type2Str(strType).c_str());

        LLBC_Expect(strTypeSet.count(strType) == 0);
        strTypeSet.emplace(strType);
    }

    std::set<LLBC_VariantType::ENUM> seqTypeSet;
    for (auto &seqType : LLBC_VariantType::SEQ_TYPES)
    {
        LLBC_Expect(!LLBC_VariantType::IsNil(seqType),
                    "Type:%s", LLBC_VariantType::Type2Str(seqType).c_str());
        LLBC_Expect(!LLBC_VariantType::IsRaw(seqType),
                    "seqType:%s", LLBC_VariantType::Type2Str(seqType).c_str());
        LLBC_Expect(!LLBC_VariantType::IsStr(seqType),
                    "seqType:%s", LLBC_VariantType::Type2Str(seqType).c_str());
        LLBC_Expect(LLBC_VariantType::IsSeq(seqType),
                    "seqType:%s", LLBC_VariantType::Type2Str(seqType).c_str());
        LLBC_Expect(!LLBC_VariantType::IsDict(seqType),
                    "seqType:%s", LLBC_VariantType::Type2Str(seqType).c_str());

        LLBC_Expect(seqTypeSet.count(seqType) == 0);
        seqTypeSet.emplace(seqType);
    }

    std::set<LLBC_VariantType::ENUM> dictTypeSet;
    for (auto &dictType : LLBC_VariantType::DICT_TYPES)
    {
        LLBC_Expect(!LLBC_VariantType::IsNil(dictType),
                    "Type:%s", LLBC_VariantType::Type2Str(dictType).c_str());
        LLBC_Expect(!LLBC_VariantType::IsRaw(dictType),
                    "dictType:%s", LLBC_VariantType::Type2Str(dictType).c_str());
        LLBC_Expect(!LLBC_VariantType::IsStr(dictType),
                    "dictType:%s", LLBC_VariantType::Type2Str(dictType).c_str());
        LLBC_Expect(!LLBC_VariantType::IsSeq(dictType),
                    "dictType:%s", LLBC_VariantType::Type2Str(dictType).c_str());
        LLBC_Expect(LLBC_VariantType::IsDict(dictType),
                    "dictType:%s", LLBC_VariantType::Type2Str(dictType).c_str());

        LLBC_Expect(dictTypeSet.count(dictType) == 0);
        dictTypeSet.emplace(dictType);
    }

    // Type2Str():
    LLBC_Expect(LLBC_VariantType::Type2Str<void>() == "Nil");
    LLBC_Expect(LLBC_VariantType::Type2Str<bool>() == "bool");
    LLBC_Expect(LLBC_VariantType::Type2Str<sint8>() == "int8");
    LLBC_Expect(LLBC_VariantType::Type2Str<uint8>() == "uint8");
    LLBC_Expect(LLBC_VariantType::Type2Str<sint16>() == "int16");
    LLBC_Expect(LLBC_VariantType::Type2Str<uint16>() == "uint16");
    LLBC_Expect(LLBC_VariantType::Type2Str<sint32>() == "int32");
    LLBC_Expect(LLBC_VariantType::Type2Str<uint32>() == "uint32");
    LLBC_Expect(LLBC_VariantType::Type2Str<long>() == "long");
    LLBC_Expect(LLBC_VariantType::Type2Str<ulong>() == "ulong");
    LLBC_Expect(LLBC_VariantType::Type2Str<sint64>() == "int64");
    LLBC_Expect(LLBC_VariantType::Type2Str<uint64>() == "uint64");
    LLBC_Expect(LLBC_VariantType::Type2Str<float>() == "float");
    LLBC_Expect(LLBC_VariantType::Type2Str<double>() == "double");
    LLBC_Expect(LLBC_VariantType::Type2Str<std::string>() == "string");
    LLBC_Expect(LLBC_VariantType::Type2Str<LLBC_Variant::Seq>() == "sequence");
    LLBC_Expect(LLBC_VariantType::Type2Str<LLBC_Variant::Dict>() == "dictionary");

    // NIL/RAW_TYPES/STR_TYPES/SEQ_TYPES/DICT_TYPES about test.
    LLBC_Expect((1/* NIL */ +
                 LLBC_VariantType::RAW_TYPES.size() +
                 LLBC_VariantType::STR_TYPES.size() +
                 LLBC_VariantType::SEQ_TYPES.size() +
                 LLBC_VariantType::DICT_TYPES.size()) == LLBC_VariantType::ALL_TYPES.size());

    return LLBC_OK;
}

int TestCase_Core_Variant::ConstructTest()
{
    LLBC_Expect(ConstructTest_DftConstruct() == LLBC_OK);
    LLBC_Expect(ConstructTest_RawType() == LLBC_OK);
    LLBC_Expect(ConstructTest_StrType() == LLBC_OK);
    LLBC_Expect(ConstructTest_SeqType() == LLBC_OK);
    LLBC_Expect(ConstructTest_DictType() == LLBC_OK);
    LLBC_Expect(ConstructTest_VariantType() == LLBC_OK);

    return LLBC_OK;
}

int TestCase_Core_Variant::ConstructTest_DftConstruct()
{
    LLBC_PrintLn("Construct test(Default Construct):");

    // Default construct.
    LLBC_Variant dftVar;
    LLBC_Expect(dftVar.Is<void>() &&
                !dftVar.IsRaw() &&
                !dftVar.IsSigned() &&
                !dftVar.IsUnsigned() &&
                !dftVar.Is<LLBC_Variant::Str>() &&
                !dftVar.Is<LLBC_Variant::Seq>() &&
                !dftVar.Is<LLBC_Variant::Dict>() &&
                dftVar.GetData().ui64() == 0);

    return LLBC_OK;
}

int TestCase_Core_Variant::ConstructTest_RawType()
{
    LLBC_PrintLn("Construct test(Raw Type):");

    // Construct from bool:
    LLBC_Variant trueVar(true);
    LLBC_Variant falseVar(false);
    LLBC_Expect((trueVar.Is<bool>() &&
                 !trueVar.Is<void>() &&
                 trueVar.IsRaw() &&
                 !trueVar.IsSigned() &&
                 trueVar.IsUnsigned() &&
                 !trueVar.Is<LLBC_Variant::Str>() &&
                 !trueVar.Is<LLBC_Variant::Seq>() &&
                 !trueVar.Is<LLBC_Variant::Dict>() &&
                 trueVar.GetType() == LLBC_VariantType::RAW_BOOL &&
                 trueVar.GetData().ui64() == 1) &&
                (falseVar.Is<bool>() &&
                 !falseVar.Is<void>() &&
                 falseVar.IsRaw() &&
                 !falseVar.IsSigned() &&
                 falseVar.IsUnsigned() &&
                 !falseVar.Is<LLBC_Variant::Str>() &&
                 !falseVar.Is<LLBC_Variant::Seq>() &&
                 !falseVar.Is<LLBC_Variant::Dict>() &&
                 falseVar.GetType() == LLBC_VariantType::RAW_BOOL &&
                 falseVar.GetData().ui64() == 0),
                "Construct from bool(true/false)");

    // Consrtuct from sint8/uint8:
    LLBC_Variant sint8Var(sint8(-8));
    LLBC_Variant uint8Var(uint8(8));
    LLBC_Expect((sint8Var.Is<sint8>() &&
                 !sint8Var.Is<void>() &&
                 sint8Var.IsRaw() &&
                 sint8Var.IsSigned() &&
                 !sint8Var.IsUnsigned() &&
                 !sint8Var.Is<LLBC_Variant::Str>() &&
                 !sint8Var.Is<LLBC_Variant::Seq>() &&
                 !sint8Var.Is<LLBC_Variant::Dict>() &&
                 sint8Var.GetType() == LLBC_VariantType::RAW_SINT8 &&
                 sint8Var.GetData().i64() == -8) &&
                (uint8Var.Is<uint8>() &&
                 !uint8Var.Is<void>() &&
                 uint8Var.IsRaw() &&
                 !uint8Var.IsSigned() &&
                 uint8Var.IsUnsigned() &&
                 !uint8Var.Is<LLBC_Variant::Str>() &&
                 !uint8Var.Is<LLBC_Variant::Seq>() &&
                 !uint8Var.Is<LLBC_Variant::Dict>() &&
                 uint8Var.GetType() == LLBC_VariantType::RAW_UINT8 &&
                 uint8Var.GetData().ui64() == 8),
                "Construct from sint8/uint8");

    // Consrtuct from sint16/uint16:
    LLBC_Variant sint16Var(sint16(-16));
    LLBC_Variant uint16Var(uint16(16));
    LLBC_Expect((sint16Var.Is<sint16>() &&
                 !sint16Var.Is<void>() &&
                 sint16Var.IsRaw() &&
                 sint16Var.IsSigned() &&
                 !sint16Var.IsUnsigned() &&
                 !sint16Var.Is<LLBC_Variant::Str>() &&
                 !sint16Var.Is<LLBC_Variant::Seq>() &&
                 !sint16Var.Is<LLBC_Variant::Dict>() &&
                 sint16Var.GetType() == LLBC_VariantType::RAW_SINT16 &&
                 sint16Var.GetData().i64() == -16) &&
                (uint16Var.Is<uint16>() &&
                 !uint16Var.Is<void>() &&
                 uint16Var.IsRaw() &&
                 !uint16Var.IsSigned() &&
                 uint16Var.IsUnsigned() &&
                 !uint16Var.Is<LLBC_Variant::Str>() &&
                 !uint16Var.Is<LLBC_Variant::Seq>() &&
                 !uint16Var.Is<LLBC_Variant::Dict>() &&
                 uint16Var.GetType() == LLBC_VariantType::RAW_UINT16 &&
                 uint16Var.GetData().ui64() == 16),
                "Construct from sint16/uint16");

    // Consrtuct from sint32/uint32:
    LLBC_Variant sint32Var(sint32(-32));
    LLBC_Variant uint32Var(uint32(32));
    LLBC_Expect((sint32Var.Is<sint32>() &&
                 !sint32Var.Is<void>() &&
                 sint32Var.IsRaw() &&
                 sint32Var.IsSigned() &&
                 !sint32Var.IsUnsigned() &&
                 !sint32Var.Is<LLBC_Variant::Str>() &&
                 !sint32Var.Is<LLBC_Variant::Seq>() &&
                 !sint32Var.Is<LLBC_Variant::Dict>() &&
                 sint32Var.GetType() == LLBC_VariantType::RAW_SINT32 &&
                 sint32Var.GetData().i64() == -32) &&
                (uint32Var.Is<uint32>() &&
                 !uint32Var.Is<void>() &&
                 uint32Var.IsRaw() &&
                 !uint32Var.IsSigned() &&
                 uint32Var.IsUnsigned() &&
                 !uint32Var.Is<LLBC_Variant::Str>() &&
                 !uint32Var.Is<LLBC_Variant::Seq>() &&
                 !uint32Var.Is<LLBC_Variant::Dict>() &&
                 uint32Var.GetType() == LLBC_VariantType::RAW_UINT32 &&
                 uint32Var.GetData().ui64() == 32),
                "Construct from sint32/uint32");

    // Consrtuct from long/ulong:
    LLBC_Variant longVar(long(-48));
    LLBC_Variant ulongVar(ulong(48));
    LLBC_Expect((longVar.Is<long>() &&
                 !longVar.Is<void>() &&
                 longVar.IsRaw() &&
                 longVar.IsSigned() &&
                 !longVar.IsUnsigned() &&
                 !longVar.Is<LLBC_Variant::Str>() &&
                 !longVar.Is<LLBC_Variant::Seq>() &&
                 !longVar.Is<LLBC_Variant::Dict>() &&
                 longVar.GetType() == LLBC_VariantType::RAW_LONG &&
                 longVar.GetData().i64() == -48) &&
                (ulongVar.Is<ulong>() &&
                 !ulongVar.Is<void>() &&
                 ulongVar.IsRaw() &&
                 !ulongVar.IsSigned() &&
                 ulongVar.IsUnsigned() &&
                 !ulongVar.Is<LLBC_Variant::Str>() &&
                 !ulongVar.Is<LLBC_Variant::Seq>() &&
                 !ulongVar.Is<LLBC_Variant::Dict>() &&
                 ulongVar.GetType() == LLBC_VariantType::RAW_ULONG &&
                 ulongVar.GetData().ui64() == 48),
                "Construct from long/ulong");

    // Consrtuct from sint64/uint64:
    LLBC_Variant sint64Var(sint64(-64));
    LLBC_Variant uint64Var(uint64(64));
    LLBC_Expect((sint64Var.Is<sint64>() &&
                 !sint64Var.Is<void>() &&
                 sint64Var.IsRaw() &&
                 sint64Var.IsSigned() &&
                 !sint64Var.IsUnsigned() &&
                 !sint64Var.Is<LLBC_Variant::Str>() &&
                 !sint64Var.Is<LLBC_Variant::Seq>() &&
                 !sint64Var.Is<LLBC_Variant::Dict>() &&
                 sint64Var.GetType() == LLBC_VariantType::RAW_SINT64 &&
                 sint64Var.GetData().i64() == -64) &&
                (uint64Var.Is<uint64>() &&
                 !uint64Var.Is<void>() &&
                 uint64Var.IsRaw() &&
                 !uint64Var.IsSigned() &&
                 uint64Var.IsUnsigned() &&
                 !uint64Var.Is<LLBC_Variant::Str>() &&
                 !uint64Var.Is<LLBC_Variant::Seq>() &&
                 !uint64Var.Is<LLBC_Variant::Dict>() &&
                 uint64Var.GetType() == LLBC_VariantType::RAW_UINT64 &&
                 uint64Var.GetData().ui64() == 64),
                "Construct from sint64/uint64");

    // Consrtuct from float/double:
    LLBC_Variant floatVar(.5f);
    LLBC_Variant doubleVar(.8);
    LLBC_Expect((floatVar.Is<float>() &&
                 !floatVar.Is<void>() &&
                 floatVar.IsRaw() &&
                 floatVar.IsSigned() &&
                 !floatVar.IsUnsigned() &&
                 !floatVar.Is<LLBC_Variant::Str>() &&
                 !floatVar.Is<LLBC_Variant::Seq>() &&
                 !floatVar.Is<LLBC_Variant::Dict>() &&
                 floatVar.GetType() == LLBC_VariantType::RAW_FLOAT &&
                 floatVar.GetData().dbl() == .5) &&
                (doubleVar.Is<double>() &&
                 !doubleVar.Is<void>() &&
                 doubleVar.IsRaw() &&
                 doubleVar.IsSigned() &&
                 !doubleVar.IsUnsigned() &&
                 !doubleVar.Is<LLBC_Variant::Str>() &&
                 !doubleVar.Is<LLBC_Variant::Seq>() &&
                 !doubleVar.Is<LLBC_Variant::Dict>() &&
                 doubleVar.GetType() == LLBC_VariantType::RAW_DOUBLE &&
                 doubleVar.GetData().dbl() == .8),
                "Construct from float/double");

    // Construct from pointer.
    LLBC_Packet packet;
    LLBC_Variant ptrVar(&packet);
    LLBC_Expect(ptrVar.Is<void *>() &&
                !ptrVar.Is<void>() &&
                ptrVar.IsRaw() &&
                !ptrVar.IsSigned() &&
                ptrVar.IsUnsigned() &&
                !ptrVar.Is<LLBC_Variant::Str>() &&
                !ptrVar.Is<LLBC_Variant::Seq>() &&
                !ptrVar.Is<LLBC_Variant::Dict>() &&
                ptrVar.GetType() == LLBC_VariantType::RAW_PTR &&
                ptrVar.As<LLBC_Packet *>() == &packet &&
                ptrVar.As<const LLBC_Packet *>() == &packet &&
                ptrVar == &packet &&
                ptrVar.GetData().ui64() == reinterpret_cast<uint64>(&packet),
                "Construct from pointer(LLBC_Packet *)");
    const LLBC_Packet constPacket;
    LLBC_Variant constPtrVar(&constPacket);
    LLBC_Expect(constPtrVar.Is<const void *>() &&
                !constPtrVar.Is<void>() &&
                constPtrVar.IsRaw() &&
                !constPtrVar.IsSigned() &&
                constPtrVar.IsUnsigned() &&
                !constPtrVar.Is<LLBC_Variant::Str>() &&
                !constPtrVar.Is<LLBC_Variant::Seq>() &&
                !constPtrVar.Is<LLBC_Variant::Dict>() &&
                constPtrVar.GetType() == LLBC_VariantType::RAW_PTR &&
                constPtrVar.As<LLBC_Packet *>() == &constPacket &&
                constPtrVar.As<const LLBC_Packet *>() == &constPacket &&
                constPtrVar.As<LLBC_Packet * const>() == &constPacket &&
                constPtrVar.As<const LLBC_Packet * const>() == &constPacket &&
                constPtrVar == &constPacket &&
                constPtrVar.GetData().ui64() == reinterpret_cast<uint64>(&constPacket),
                "Construct from pointer(const LLBC_Packet *)");

    // Construct from nullptr.
    LLBC_Variant nullPtrVar(nullptr);
    LLBC_Expect(nullPtrVar.Is<void *>() &&
                !nullPtrVar.Is<void>() &&
                nullPtrVar.IsRaw() &&
                !nullPtrVar.IsSigned() &&
                nullPtrVar.IsUnsigned() &&
                !nullPtrVar.Is<LLBC_Variant::Str>() &&
                !nullPtrVar.Is<LLBC_Variant::Seq>() &&
                !nullPtrVar.Is<LLBC_Variant::Dict>() &&
                nullPtrVar.GetType() == LLBC_VariantType::RAW_PTR &&
                nullPtrVar.As<void *>() == nullptr &&
                nullPtrVar.As<const void *>() == nullptr &&
                nullPtrVar.As<void * const>() == nullptr &&
                nullPtrVar.As<const void * const>() == nullptr &&
                nullPtrVar == nullptr &&
                nullPtrVar.GetData().ui64() == 0,
                "Construct from nullptr");

    // Construct from enum.
    LLBC_Variant traditionalEnumVar(TestTraditionalEnum::TestTraditionalEnum1);
    LLBC_Expect(traditionalEnumVar.Is<uint32>() &&
                !traditionalEnumVar.Is<void>() &&
                traditionalEnumVar.IsRaw() &&
                !traditionalEnumVar.IsSigned() &&
                traditionalEnumVar.IsUnsigned() &&
                !traditionalEnumVar.Is<LLBC_Variant::Str>() &&
                !traditionalEnumVar.Is<LLBC_Variant::Seq>() &&
                !traditionalEnumVar.Is<LLBC_Variant::Dict>() &&
                traditionalEnumVar.GetType() == LLBC_VariantType::RAW_UINT32 &&
                traditionalEnumVar == TestTraditionalEnum::TestTraditionalEnum1 &&
                traditionalEnumVar.As<TestTraditionalEnum>() == TestTraditionalEnum::TestTraditionalEnum1,
                "Construct from traditional enum");

    enum class TestNewStyleEnum
    {
        TestNewStyleEnum1 = 1,
        TestNewStyleEnum2 = 2
    };
    LLBC_Variant newStyleEnumVar(TestNewStyleEnum::TestNewStyleEnum2);
    LLBC_Expect(newStyleEnumVar.Is<sint32>() &&
                !newStyleEnumVar.Is<void>() &&
                newStyleEnumVar.IsRaw() &&
                newStyleEnumVar.IsSigned() &&
                !newStyleEnumVar.IsUnsigned() &&
                !newStyleEnumVar.Is<LLBC_Variant::Str>() &&
                !newStyleEnumVar.Is<LLBC_Variant::Seq>() &&
                !newStyleEnumVar.Is<LLBC_Variant::Dict>() &&
                newStyleEnumVar.GetType() == LLBC_VariantType::RAW_SINT32 &&
                newStyleEnumVar == TestNewStyleEnum::TestNewStyleEnum2 &&
                newStyleEnumVar.As<TestNewStyleEnum>() == TestNewStyleEnum::TestNewStyleEnum2,
                "Construct from new style enum");

    return LLBC_OK;
}

int TestCase_Core_Variant::ConstructTest_StrType()
{
    LLBC_PrintLn("Construct test(Str type):");

    // Construct from char array.
    char charArr[] = "hello";
    LLBC_Variant charArrVar(charArr);
    LLBC_Expect(charArrVar.Is<LLBC_String>() &&
                charArrVar.Is<LLBC_CString>() &&
                charArrVar.Is<std::string>() &&
                charArrVar.Is<std::string_view>() &&
                charArrVar.Is<char *>() &&
                charArrVar.Is<const char *>() &&
                charArrVar.Is<char * const>() &&
                charArrVar.Is<const char * const>() &&
                !charArrVar.Is<void>() &&
                charArrVar.Is<LLBC_Variant::Str>() &&
                !charArrVar.Is<LLBC_Variant::Seq>() &&
                !charArrVar.Is<LLBC_Variant::Dict>() &&
                charArrVar.GetType() == LLBC_VariantType::STR_DFT &&
                charArrVar == charArr &&
                charArrVar.As<LLBC_String>() == charArr,
                "Construct from char array");

    // Construct from null char pointer.
    char *nullCharPtr = nullptr;
    LLBC_Variant nullCharPtrVar(nullCharPtr);
    LLBC_Expect(nullCharPtrVar.Is<LLBC_String>() &&
                nullCharPtrVar.Is<LLBC_CString>() &&
                nullCharPtrVar.Is<std::string>() &&
                nullCharPtrVar.Is<std::string_view>() &&
                nullCharPtrVar.Is<char *>() &&
                nullCharPtrVar.Is<const char *>() &&
                nullCharPtrVar.Is<char * const>() &&
                nullCharPtrVar.Is<const char * const>() &&
                !nullCharPtrVar.Is<void>() &&
                nullCharPtrVar.Is<LLBC_Variant::Str>() &&
                !nullCharPtrVar.Is<LLBC_Variant::Seq>() &&
                !nullCharPtrVar.Is<LLBC_Variant::Dict>() &&
                nullCharPtrVar.GetType() == LLBC_VariantType::STR_DFT,
                "Construct from null char pointer");

    // Consrtuct from char pointer(non-const char, non-const pointer).
    char *strPtr = LLBC_Malloc(char, 12);
    memcpy(strPtr, "hello world", 12);
    LLBC_Defer(free(strPtr));
    LLBC_Variant strPtrVar(strPtr);
    LLBC_Expect(strPtrVar.Is<LLBC_String>() &&
                strPtrVar.Is<LLBC_CString>() &&
                strPtrVar.Is<std::string>() &&
                strPtrVar.Is<std::string_view>() &&
                strPtrVar.Is<char *>() &&
                strPtrVar.Is<const char *>() &&
                strPtrVar.Is<char * const>() &&
                strPtrVar.Is<const char * const>() &&
                !strPtrVar.Is<void>() &&
                strPtrVar.Is<LLBC_Variant::Str>() &&
                !strPtrVar.Is<LLBC_Variant::Seq>() &&
                !strPtrVar.Is<LLBC_Variant::Dict>() &&
                strPtrVar.GetType() == LLBC_VariantType::STR_DFT &&
                strPtrVar == strPtr &&
                strPtrVar.As<LLBC_String>() == strPtr,
                "Construct from char pointer(non-const char, non-const pointer)");
    // Consrtuct from char pointer(non-const char, const pointer).
    char * const strPtr2 = LLBC_Malloc(char, 6);
    memcpy(strPtr2, "hello", 6);
    LLBC_Defer(free(strPtr2));
    LLBC_Variant strPtrVar2(strPtr2);
    LLBC_Expect(strPtrVar2.Is<char * const>() && strPtrVar2 == strPtr2,
                "Construct from char pointer(non-const char, const pointer)");
    // Consrtuct from char pointer(const char, non-const pointer).
    const char *cstrPtr = "hello world(const)";
    LLBC_Variant cstrPtrVar(cstrPtr);
    LLBC_Expect(cstrPtrVar.Is<const char *>() && cstrPtrVar == cstrPtr,
                "Construct from char pointer(const char, non-const pointer)");
    // Consrtuct from char pointer(const char, const pointer).
    const char * const cstrPtr2 = "hello world(const char && const pointer)";
    LLBC_Variant cstrPtrVar2(cstrPtr2);
    LLBC_Expect(cstrPtrVar2.Is<const char * const>() && cstrPtrVar2 == cstrPtr2,
                "Construct from char pointer(const char, const pointer)");

    // Construct from std::string/const string.
    std::string stlStr("hello");
    LLBC_Variant stlStrVar(stlStr);
    LLBC_Expect(stlStrVar.Is<std::string>() && stlStrVar == stlStr,
                "Construct from std::string");
    const std::string cstlStr("hello world");
    LLBC_Variant cstlStrVar(cstlStr);
    LLBC_Expect(cstlStrVar.Is<const std::string>() && cstlStrVar == cstlStr,
                "Construct from const std::string");

    // Move Construct from std::string.
    std::string stlStrForMove("hello world(for move)");
    LLBC_Variant stlStrVarFromMove(std::move(stlStrForMove));
    LLBC_Expect(stlStrVarFromMove.Is<std::string>() &&
                    stlStrVarFromMove.GetData().str() == "hello world(for move)" &&
                    stlStrForMove.empty(),
                "Move Construct from std::string");

    // Construct from std::string_view/const std::string_view.
    std::string_view stlStrView("hello world");
    LLBC_Variant stlStrViewVar(stlStrView);
    LLBC_Expect(stlStrViewVar.Is<std::string_view>() && stlStrViewVar == stlStrView,
                "Construct from std::string_view");
    const std::string_view cstlStrView("hello world");
    LLBC_Variant cstlStrViewVar(cstlStrView);
    LLBC_Expect(cstlStrViewVar.Is<const std::string_view>() && cstlStrViewVar == cstlStrView,
                "Construct from const std::string_view");

    // Construct from LLBC_String/const LLBC_String.
    LLBC_String llbcStr("hello");
    LLBC_Variant llbcStrVar(llbcStr);
    LLBC_Expect(llbcStrVar.Is<LLBC_String>() && llbcStrVar == llbcStr,
                "Construct from LLBC_String");
    LLBC_String cllbcStr("hello");
    LLBC_Variant cllbcStrVar(cllbcStr);
    LLBC_Expect(cllbcStrVar.Is<const LLBC_String>() && cllbcStrVar == cllbcStr,
                "Construct from const LLBC_String");

    // Move construct from LLBC_String.
    LLBC_String llbcStrForMove("hello world(for move)");
    LLBC_Variant llbcStrVarFromMove(std::move(llbcStrForMove));
    LLBC_Expect(llbcStrVarFromMove.Is<LLBC_String>() &&
                    llbcStrVarFromMove.GetData().str() == "hello world(for move)" &&
                    llbcStrForMove.empty(),
                "Move Construct from LLBC_String");

    // Construct from LLBC_CString/const LLBC_CString.
    LLBC_CString llbcCStr("hello world");
    LLBC_Variant llbcCStrVar(llbcCStr);
    LLBC_Expect(llbcCStrVar.Is<LLBC_CString>() && llbcCStrVar == llbcCStr,
                "Construct from LLBC_CString");
    LLBC_CString cllbcCStr("hello world");
    LLBC_Variant cllbcCStrVar(cllbcCStr);
    LLBC_Expect(cllbcCStrVar.Is<const LLBC_CString>() && cllbcCStrVar == cllbcCStr,
                "Construct from const LLBC_CString");

    return LLBC_OK;
}

int TestCase_Core_Variant::ConstructTest_SeqType()
{
    LLBC_PrintLn("Construct test(All Seq type):");

    // std::pair<LLBC_String, LLBC_String>:
    std::pair<LLBC_String, LLBC_String> pa { "hello world(first)", "hello world(second)" };
    LLBC_Expect(ConstructTest_OneSeqType(pa) == LLBC_OK);
    LLBC_Expect(ConstructTest_OneSeqType(static_cast<const std::pair<LLBC_String, LLBC_String> &>(pa)) == LLBC_OK);
    LLBC_Expect(ConstructTest_OneSeqType(std::move(pa)) == LLBC_OK);

    // std::vector<int>:
    std::vector<std::vector<int>> intVecs{{}, {1, 3, 5, 7}};
    for (auto &intVec : intVecs)
    {
        LLBC_Expect(ConstructTest_OneSeqType(intVec) == LLBC_OK);
        LLBC_Expect(ConstructTest_OneSeqType(static_cast<const std::vector<int> &>(intVec)) == LLBC_OK);
        LLBC_Expect(ConstructTest_OneSeqType(std::move(intVec)) == LLBC_OK);
    }

    // std::list<int>:
    std::vector<std::list<int>> intLists{{}, {2, 4, 6, 8}};
    for (auto &intList : intLists)
    {
        LLBC_Expect(ConstructTest_OneSeqType(intList) == LLBC_OK);
        LLBC_Expect(ConstructTest_OneSeqType(static_cast<const std::list<int> &>(intList)) == LLBC_OK);
        LLBC_Expect(ConstructTest_OneSeqType(std::move(intList)) == LLBC_OK);
    }

    // std::queue<int>:
    std::vector<std::queue<int>> intQues;
    intQues.resize(2);
    for (int i = 0; i < 10; ++i)
        intQues[0].push(i);
    for (auto &que : intQues)
    {
        LLBC_Expect(ConstructTest_OneSeqType(que) == LLBC_OK);
        LLBC_Expect(ConstructTest_OneSeqType(static_cast<const std::queue<int> &>(que)) == LLBC_OK);
        LLBC_Expect(ConstructTest_OneSeqType(std::move(que)) == LLBC_OK);
    }

    // std::deque<LLBC_String>:
    std::vector<std::deque<LLBC_String>> strDeqs;
    strDeqs.resize(2);
    for (int i = 0; i < 10; ++i)
        strDeqs[1].emplace_back(LLBC_String().format("deque elem:%d", i));
    for (auto &deq : strDeqs)
    {
        LLBC_Expect(ConstructTest_OneSeqType(deq) == LLBC_OK);
        LLBC_Expect(ConstructTest_OneSeqType(static_cast<const std::deque<LLBC_String> &>(deq)) == LLBC_OK);
        LLBC_Expect(ConstructTest_OneSeqType(std::move(deq)) == LLBC_OK);
    }

    // std::set<sint64>:
    std::vector<std::set<sint64>> int64Sets;
    int64Sets.resize(2);
    for (int i = 0; i < 50; ++i)
        int64Sets[1].emplace(i);
    for (auto &int64Set : int64Sets)
    {
        LLBC_Expect(ConstructTest_OneSeqType(int64Set) == LLBC_OK);
        LLBC_Expect(ConstructTest_OneSeqType(static_cast<const std::set<sint64> &>(int64Set)) == LLBC_OK);
        LLBC_Expect(ConstructTest_OneSeqType(std::move(int64Set)) == LLBC_OK);
    }

    // std::unordered_set<uint64>:
    std::vector<std::unordered_set<uint64>> uint64UnorderedSets;
    uint64UnorderedSets.resize(2);
    for (int i = 0; i < 20; ++i)
        uint64UnorderedSets[1].emplace(i);
    for (auto &uint64UnorderedSet : uint64UnorderedSets)
    {
        LLBC_Expect(ConstructTest_OneSeqType(uint64UnorderedSet) == LLBC_OK);
        LLBC_Expect(ConstructTest_OneSeqType(static_cast<const std::unordered_set<uint64> &>(uint64UnorderedSet)) == LLBC_OK);
        LLBC_Expect(ConstructTest_OneSeqType(std::move(uint64UnorderedSet)) == LLBC_OK);
    }

    // LLBC_Variant::Seq:
    LLBC_Variant::Seq seq;
    for (int i = 0; i < 10; ++i)
        seq.emplace_back(i);
    LLBC_Expect(ConstructTest_OneSeqType(seq) == LLBC_OK);
    LLBC_Expect(ConstructTest_OneSeqType(static_cast<const LLBC_Variant::Seq &>(seq)) == LLBC_OK);
    LLBC_Expect(ConstructTest_OneSeqType(std::move(seq)) == LLBC_OK);

    return LLBC_OK;
}

template <typename _SeqTy>
int TestCase_Core_Variant::ConstructTest_OneSeqType(_SeqTy &&seq)
{
    typedef std::remove_cv_t<std::remove_reference_t<_SeqTy>> _PureSeqTy;

    LLBC_PrintLn("Seq construct test(Seq type: %s, rvalue_ref?:%s):",
                 LLBC_GetTypeName(seq), std::is_rvalue_reference_v<_SeqTy &&> ? "true" : "false");

    if constexpr (LLBC_IsTemplSpec<_PureSeqTy, std::pair>::value)
    {
        if constexpr (std::is_rvalue_reference_v<_SeqTy &&>)
        {
            const _PureSeqTy copySeq(seq);
            LLBC_Variant seqVar(std::forward<_SeqTy>(seq));
            LLBC_Expect(seqVar.Is<LLBC_Variant::Seq>() && seqVar == copySeq &&
                            seq.first == typename _PureSeqTy::first_type() &&
                            seq.second == typename _PureSeqTy::second_type(),
                        "Move Construct from pair, ty:%s, seqVar:%s",
                        LLBC_GetTypeName(seq), seqVar.ToString().c_str());
        }
        else
        {
            LLBC_Variant seqVar(std::forward<_SeqTy>(seq));
            LLBC_Expect(seqVar.Is<LLBC_Variant::Seq>() && seqVar == seq,
                        "Construct from pair, ty:%s: seqVar:%s",
                        LLBC_GetTypeName(seq), seqVar.ToString().c_str());
        }
    }
    else
    {
        if constexpr (std::is_rvalue_reference_v<_SeqTy &&>)
        {
            const _PureSeqTy copySeq(seq);
            LLBC_Variant seqVar(std::forward<_SeqTy>(seq));
            LLBC_Expect(seqVar.Is<LLBC_Variant::Seq>() &&
                        seqVar.Size() == copySeq.size() &&
                        seqVar == copySeq &&
                        seq.empty(),
                        "Move Construct from %s, seq.size():%lu, copySeq.size():%lu, "
                            "seqVar.Size():%lu, seqVar.ToString():%s",
                        LLBC_GetTypeName(seq), seq.size(), copySeq.size(), seqVar.Size(), seqVar.ToString().c_str());
        }
        else
        {
            LLBC_Variant seqVar(std::forward<_SeqTy>(seq));
            LLBC_Expect(seqVar.Is<LLBC_Variant::Seq>() &&
                        seqVar.Size() == seq.size() &&
                        seqVar == seq,
                        "Construct from %s: %s",
                        LLBC_GetTypeName(seq), seqVar.ToString().c_str());
        }
    }

    return LLBC_OK;
}

int TestCase_Core_Variant::ConstructTest_DictType()
{
    LLBC_PrintLn("Construct test(Dict type):");

    // std::map<LLBC_String, LLBC_String>:
    std::vector<std::map<LLBC_String, LLBC_String>> maps;
    maps.resize(2);
    for (int i = 0; i < 10; ++i)
        maps[1].emplace(LLBC_String().format("Key:%d", i), LLBC_String().format("Value:%d", i * 10 + 1));
    for (auto &m : maps)
    {
        // non-const ref:
        LLBC_Expect(ConstructTest_OneDictType(m) == LLBC_OK);
        // const ref:
        LLBC_Expect(ConstructTest_OneDictType(static_cast<const std::map<LLBC_String, LLBC_String> &>(m)) == LLBC_OK);
        // rvalue ref:
        LLBC_Expect(ConstructTest_OneDictType(std::move(m)) == LLBC_OK);
    }

    // std::unordered_map<int, LLBC_String>:
    std::vector<std::unordered_map<int, LLBC_String>> unorderedMaps;
    unorderedMaps.resize(2);
    for (int i = 0; i < 10; ++i)
        unorderedMaps[1].emplace(i, LLBC_String().format("Value:%d", i + 1));
    for (auto &unorderedMap : unorderedMaps)
    {
        // non-const ref:
        LLBC_Expect(ConstructTest_OneDictType(unorderedMap) == LLBC_OK);
        // const ref:
        LLBC_Expect(ConstructTest_OneDictType(static_cast<const std::unordered_map<int, LLBC_String> &>(unorderedMap)) == LLBC_OK);
        // rvalue ref:
        LLBC_Expect(ConstructTest_OneDictType(std::move(unorderedMap)) == LLBC_OK);
    }

    // LLBC_Variant::Dict:
    LLBC_Variant::Dict dict;
    for (int i = 0; i < 10; ++i)
        dict.emplace(i, i + 10);
    LLBC_Expect(ConstructTest_OneDictType(dict) == LLBC_OK);
    LLBC_Expect(ConstructTest_OneDictType(static_cast<const LLBC_Variant::Dict &>(dict)) == LLBC_OK);
    LLBC_Expect(ConstructTest_OneDictType(std::move(dict)) == LLBC_OK);

    return LLBC_OK;
}

template <typename _DictTy>
int TestCase_Core_Variant::ConstructTest_OneDictType(_DictTy &&dict)
{
    LLBC_PrintLn("Dict construct test(dict type:%s, rvalue_ref:%s)",
                 LLBC_GetTypeName(dict), std::is_rvalue_reference_v<_DictTy &&> ? "true" : "false");

    typedef std::remove_cv_t<std::remove_reference_t<_DictTy>> _PureDictTy;

    if constexpr (std::is_rvalue_reference_v<_DictTy &&>)
    {
        const _PureDictTy copyDict(dict);
        LLBC_Variant dictVar(std::forward<_DictTy>(dict));
        LLBC_Expect(dictVar.Is<LLBC_Variant::Dict>() && dictVar == copyDict && dict.empty(),
                    "Move construct from %s, dict.size():%lu, copyDict.size():%lu, "
                    "dictVar.size():%lu, dictVar.ToString():%s",
                    LLBC_GetTypeName(dict), dict.size(), copyDict.size(), dictVar.Size(), dictVar.ToString().c_str());
    }
    else
    {
        LLBC_Variant dictVar(std::forward<_DictTy>(dict));
        LLBC_Expect(dictVar.Is<LLBC_Variant::Dict>() && dictVar == dict,
                    "Construct from %s, dictVar:%s",
                    LLBC_GetTypeName(dict), dictVar.ToString().c_str());
    }

    return LLBC_OK;
}

int TestCase_Core_Variant::ConstructTest_VariantType()
{
    LLBC_PrintLn("Construct test(variant type):");

    // Construct from nil variant.
    LLBC_Variant nilVar;
    LLBC_Variant constructFromNilVar(nilVar);
    LLBC_Expect(constructFromNilVar == nilVar &&
                constructFromNilVar.Is<void>(),
                "Construct from nil variant");

    // Construct from bool nil variant.
    LLBC_Variant falseVar(false);
    LLBC_Variant constructFromFalseVar(falseVar);
    LLBC_Expect(constructFromFalseVar == falseVar &&
                constructFromFalseVar.Is<bool>() &&
                constructFromFalseVar.As<bool>() == false,
                "Construct from bool nil variant");

    // Construct from sint8 variant.
    LLBC_Variant sint8Var(static_cast<sint8>(-8));
    LLBC_Variant ctFFromSint8Var(sint8Var);
    LLBC_Expect(ctFFromSint8Var == sint8Var &&
                ctFFromSint8Var.Is<sint8>() &&
                ctFFromSint8Var.As<sint8>() == -8,
                "Construct from sint8 variant");
    // Construct from uint8 variant.
    LLBC_Variant uint8Var(static_cast<uint8>(8));
    LLBC_Variant ctFromUint8Var(uint8Var);
    LLBC_Expect(ctFromUint8Var == uint8Var &&
                ctFromUint8Var.Is<uint8>() &&
                ctFromUint8Var.As<uint8>() == 8,
                "Construct from uint8 variant");

    // Construct from sint16 variant.
    LLBC_Variant sint16Var(static_cast<sint16>(-16));
    LLBC_Variant ctFromSint16Var(sint16Var);
    LLBC_Expect(ctFromSint16Var == sint16Var &&
                ctFromSint16Var.Is<sint16>() &&
                ctFromSint16Var.As<sint16>() == -16,
                "Construct from sint16 variant");
    // Construct from uint16 variant.
    LLBC_Variant uint16Var(static_cast<uint16>(16));
    LLBC_Variant ctFromUint16Var(uint16Var);
    LLBC_Expect(ctFromUint16Var == uint16Var &&
                ctFromUint16Var.Is<uint16>() &&
                ctFromUint16Var.As<uint16>() == 16,
                "Construct from uint16 variant");

    // Construct from sint32 variant.
    LLBC_Variant sint32Var(static_cast<sint32>(-32));
    LLBC_Variant ctFromSint32Var(sint32Var);
    LLBC_Expect(ctFromSint32Var == sint32Var &&
                ctFromSint32Var.Is<sint32>() &&
                ctFromSint32Var.As<sint32>() == -32,
                "Construct from sint32 variant");
    // Construct from uint32 variant.
    LLBC_Variant uint32Var(static_cast<uint32>(32));
    LLBC_Variant ctFromUint32Var(uint32Var);
    LLBC_Expect(ctFromUint32Var == uint32Var &&
                ctFromUint32Var.Is<uint32>() &&
                ctFromUint32Var.As<uint32>() == 32,
                "Construct from uint32 variant");

    // Construct from long variant.
    LLBC_Variant longVar(static_cast<long>(-48));
    LLBC_Variant ctFromLongVar(longVar);
    LLBC_Expect(ctFromLongVar == longVar &&
                ctFromLongVar.Is<long>() &&
                ctFromLongVar.As<long>() == -48,
                "Construct from long variant");
    // Construct from ulong variant.
    LLBC_Variant ulongVar(static_cast<ulong>(48));
    LLBC_Variant ctFromUlongVar(ulongVar);
    LLBC_Expect(ctFromUlongVar == ulongVar &&
                ctFromUlongVar.Is<ulong>() &&
                ctFromUlongVar.As<ulong>() == 48,
                "Construct from ulong variant");
    
    // Construct from sint64 variant.
    LLBC_Variant sint64Var(static_cast<sint64>(-64));
    LLBC_Variant ctFromSint64Var(sint64Var);
    LLBC_Expect(ctFromSint64Var == sint64Var &&
                ctFromSint64Var.Is<sint64>() &&
                ctFromSint64Var.As<sint64>() == -64,
                "Construct from sint64 variant");
    // Construct from uint64 variant.
    LLBC_Variant uint64Var(static_cast<uint64>(64));
    LLBC_Variant ctFromUint64Var(uint64Var);
    LLBC_Expect(ctFromUint64Var == uint64Var &&
                ctFromUint64Var.Is<uint64>() &&
                ctFromUint64Var.As<uint64>() == 64,
                "Construct from uint64 variant");

    // Construct from float variant.
    LLBC_Variant floatVar(static_cast<float>(-3.14f));
    LLBC_Variant ctFromFloatVar(floatVar);
    LLBC_Expect(ctFromFloatVar == floatVar &&
                ctFromFloatVar.Is<float>() &&
                LLBC_IsFloatAlmostEqual(ctFromFloatVar.As<float>(), -3.14f),
                "Construct from float variant");
    // Construct from double variant.
    LLBC_Variant doubleVar(static_cast<double>(3.14));
    LLBC_Variant ctFromDoubleVar(doubleVar);
    LLBC_Expect(ctFromDoubleVar == doubleVar &&
                ctFromDoubleVar.Is<double>() &&
                LLBC_IsFloatAlmostEqual(ctFromDoubleVar.As<double>(), 3.14),
                "Construct from double variant");

    // Construct from string variant.
    LLBC_Variant strVar("hello world");
    LLBC_Variant ctFromStrVar(strVar);
    LLBC_Expect(ctFromStrVar == strVar &&
                ctFromStrVar.Is<LLBC_String>() &&
                ctFromStrVar.As<LLBC_String>() == "hello world",
                "Construct from string variant");

    // Construct from Seq variant.
    std::vector<int> seq{1, 2, 3, 4, 5};
    LLBC_Variant seqVar(seq);
    LLBC_Variant ctFromSeqVar(seqVar);
    LLBC_Expect(ctFromSeqVar == seqVar &&
                ctFromSeqVar.Is<LLBC_Variant::Seq>(),
                "Construct from Seq variant");

    // Construct from Dict variant.
    std::map<int, int> m{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}};
    LLBC_Variant dictVar(m);
    LLBC_Variant ctFromDictVar(dictVar);
    LLBC_Expect(ctFromDictVar == dictVar &&
                ctFromDictVar.Is<LLBC_Variant::Dict>(),
                "Construct from Dict variant");

    return LLBC_OK;
}

int TestCase_Core_Variant::AssignmentTest()
{
    LLBC_PrintLn("Assignment test:");

    LLBC_Expect(AssignmentTest_RawType() == LLBC_OK);
    LLBC_Expect(AssignmentTest_StrType() == LLBC_OK);
    LLBC_Expect(AssignmentTest_SeqType() == LLBC_OK);
    LLBC_Expect(AssignmentTest_DictType() == LLBC_OK);
    LLBC_Expect(AssignmentTest_VariantType() == LLBC_OK);

    return LLBC_OK;
}

int TestCase_Core_Variant::AssignmentTest_RawType()
{
    LLBC_PrintLn("Assignment test(Raw type):");

    // Assignment from nil.
    LLBC_Variant nilVar;
    nilVar = LLBC_Variant::nil;
    LLBC_Expect(nilVar.Is<void>() &&
                nilVar == LLBC_Variant::nil,
                "Assignment from nil");

    // Assignment from bool.
    LLBC_Variant falseVar;
    falseVar = false;
    LLBC_Variant trueVar;
    trueVar = true;
    LLBC_Expect(falseVar.Is<bool>() &&
                falseVar == false,
                "Assignment from bool false variant");
    LLBC_Expect(trueVar.Is<bool>() &&
                trueVar == true,
                "Assignment from bool true variant");

    // Assignment from sint8.
    LLBC_Variant sint8Var;
    sint8Var = static_cast<sint8>(-8);
    LLBC_Expect(sint8Var.Is<sint8>() &&
                sint8Var == static_cast<sint8>(-8),
                "Assignment from sint8 variant");
    // Assignment from uint8.
    LLBC_Variant uint8Var;
    uint8Var = static_cast<uint8>(8);
    LLBC_Expect(uint8Var.Is<uint8>() &&
                uint8Var == static_cast<uint8>(8),
                "Assignment from uint8 variant");

    // Assignment from sint16.
    LLBC_Variant sint16Var;
    sint16Var = static_cast<sint16>(-16);
    LLBC_Expect(sint16Var.Is<sint16>() &&
                sint16Var == static_cast<sint16>(-16),
                "Assignment from sint16 variant");
    // Assignment from uint16.
    LLBC_Variant uint16Var;
    uint16Var = static_cast<uint16>(16);
    LLBC_Expect(uint16Var.Is<uint16>() &&
                uint16Var == static_cast<uint16>(16),
                "Assignment from uint16 variant");

    // Assignment from sint32.
    LLBC_Variant sint32Var;
    sint32Var = -32;
    LLBC_Expect(sint32Var.Is<sint32>() &&
                sint32Var == -32,
                "Assignment from sint32 variant");
    // Assignment from uint32.
    LLBC_Variant uint32Var;
    uint32Var = 32u;
    LLBC_Expect(uint32Var.Is<uint32>() &&
                uint32Var == 32u,
                "Assignment from uint32 variant");

    // Assignment from long.
    LLBC_Variant longVar;
    longVar = -48l;
    LLBC_Expect(longVar.Is<long>() &&
                longVar == -48l,
                "Assignment from long variant");
    // Assignment from ulong.
    LLBC_Variant ulongVar;
    ulongVar = 48ul;
    LLBC_Expect(ulongVar.Is<ulong>() &&
                ulongVar == 48ul,
                "Assignment from ulong variant");

    // Assignment from sint64.
    LLBC_Variant sint64Var;
    sint64Var = -64ll;
    LLBC_Expect(sint64Var.Is<sint64>() &&
                sint64Var == -64ll,
                "Assignment from sint64 variant");
    // Assignment from uint64.
    LLBC_Variant uint64Var;
    uint64Var = 64ull;
    LLBC_Expect(uint64Var.Is<uint64>() &&
                uint64Var == 64ull,
                "Assignment from uint64 variant");

    // Assignment from float.
    LLBC_Variant floatVar;
    floatVar = -3.14f;
    LLBC_Expect(floatVar.Is<float>() &&
                LLBC_IsFloatAlmostEqual(floatVar.As<float>(), -3.14f),
                "Assignment from float variant");
    // Assignment from double.
    LLBC_Variant doubleVar;
    doubleVar = 3.14;
    LLBC_Expect(doubleVar.Is<double>() &&
                LLBC_IsFloatAlmostEqual(doubleVar.As<double>(), 3.14),
                "Assignment from double variant");

    return LLBC_OK;
}

int TestCase_Core_Variant::AssignmentTest_StrType()
{
    LLBC_PrintLn("Assignment test(Str type):");

    // Assignment from char array.
    LLBC_Variant charArrVar;
    char charArr[] = "hello world";
    charArrVar = charArr;
    LLBC_Expect(charArrVar.Is<LLBC_String>() &&
                charArrVar == charArr,
                "Assignment from char array");

    // Assignment from char *.
    char *charPtr = charArr;
    LLBC_Variant charPtrVar;
    charPtrVar = charPtr;
    LLBC_Expect(charPtrVar.Is<LLBC_String>() &&
                charPtrVar == charPtr,
                "Assignment from char *");
    // Assignment from char * const.
    char *const charPtrConst = charArr;
    LLBC_Variant charPtrConstVar;
    charPtrConstVar = charPtrConst;
    LLBC_Expect(charPtrConstVar.Is<LLBC_String>() &&
                charPtrConstVar == charPtrConst,
                "Assignment from char * const");
    // Assignment from const char *.
    const char *constCharPtr = charArr;
    LLBC_Variant constCharPtrVar;
    constCharPtrVar = constCharPtr;
    LLBC_Expect(constCharPtrVar.Is<LLBC_String>() &&
                constCharPtrVar == constCharPtr,
                "Assignment from const char *");
    // Asignment from const char * const.
    const char *const constCharPtrConst = charArr;
    LLBC_Variant constCharPtrConstVar;
    constCharPtrConstVar = constCharPtrConst;
    LLBC_Expect(constCharPtrConstVar.Is<LLBC_String>() &&
                constCharPtrConstVar == constCharPtrConst,
                "Assignment from const char * const");

    // Assignment from LLBC_String.
    LLBC_String llbcStr("hello world");
    LLBC_Variant llbcStrVar;
    llbcStrVar = LLBC_String(llbcStr);
    LLBC_Expect(llbcStrVar.Is<LLBC_String>() &&
                llbcStrVar == llbcStr,
                "Assignment from LLBC_String");
    // Assignment from const LLBC_String.
    LLBC_String constLLBCString("hello world");
    LLBC_Variant constLLBCStrVar;
    constLLBCStrVar = constLLBCString;
    LLBC_Expect(constLLBCStrVar.Is<LLBC_String>() &&
                constLLBCStrVar == constLLBCString,
                "Assignment from const LLBC_String");

    // Assignment from std::string.
    std::string stlStr("hello world");
    LLBC_Variant stlStrVar;
    stlStrVar = stlStr;
    LLBC_Expect(stlStrVar.Is<LLBC_String>() &&
                stlStrVar == stlStr,
                "Assignment from std::string");
    // Assignment from const std::string.
    std::string constSTLString("hello world");
    LLBC_Variant constSTLStrVar;
    constSTLStrVar = constSTLString;
    LLBC_Expect(constSTLStrVar.Is<LLBC_String>() &&
                constSTLStrVar == constSTLString,
                "Assignment from const std::string");

    // Assignment from LLBC_CString.
    LLBC_CString llbcCStr("hello world");
    LLBC_Variant llbcCStrVar;
    llbcCStrVar = llbcCStr;
    LLBC_Expect(llbcCStrVar.Is<LLBC_String>() &&
                llbcCStrVar == llbcCStr,
                "Assignment from LLBC_CString");
    // Assignment from const LLBC_CString.
    LLBC_CString constLLBCCString("hello world");
    LLBC_Variant constLLBCCStrVar;
    constLLBCCStrVar = constLLBCCString;
    LLBC_Expect(constLLBCCStrVar.Is<LLBC_String>() &&
                constLLBCCStrVar == constLLBCCString,
                "Assignment from const LLBC_CString");

    // Assignment from std::string_view.
    std::string_view stlStrView("hello world");
    LLBC_Variant stlStrViewVar;
    stlStrViewVar = stlStrView;
    LLBC_Expect(stlStrViewVar.Is<LLBC_String>() &&
                stlStrViewVar == stlStrView,
                "Assignment from std::string_view");
    // Assignment from const std::string_view.
    std::string_view constSTLStringView("hello world");
    LLBC_Variant constSTLStrViewVar;
    constSTLStrViewVar = constSTLStringView;
    LLBC_Expect(constSTLStrViewVar.Is<LLBC_String>() &&
                constSTLStrViewVar == constSTLStringView,
                "Assignment from const std::string_view");

    return LLBC_OK;
}

int TestCase_Core_Variant::AssignmentTest_SeqType()
{
    LLBC_PrintLn("Assignment test(All Seq type):");

    // std::pair<LLBC_String, LLBC_String>:
    std::pair<LLBC_String, LLBC_String> pa{"hello", "world"};
    LLBC_Expect(AssignmentTest_OneSeqType(pa) == LLBC_OK);
    LLBC_Expect(AssignmentTest_OneSeqType(static_cast<const std::pair<LLBC_String, LLBC_String> &>(pa)) == LLBC_OK);
    LLBC_Expect(AssignmentTest_OneSeqType(std::move(pa)) == LLBC_OK);

    // std::vector<int>:
    std::vector<std::vector<int>> intVecs{{}, {2, 4, 6, 8}};
    for (auto &intVec : intVecs)
    {
        LLBC_Expect(AssignmentTest_OneSeqType(intVec) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneSeqType(static_cast<const std::vector<int> &>(intVec)) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneSeqType(std::move(intVec)) == LLBC_OK);
    }

    // std::list<LLBC_String>:
    std::vector<std::list<LLBC_String>> strLists{{}, {"hello", "world"}};
    for (auto &strList : strLists)
    {
        LLBC_Expect(AssignmentTest_OneSeqType(strList) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneSeqType(static_cast<const std::list<LLBC_String> &>(strList)) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneSeqType(std::move(strList)) == LLBC_OK);
    }

    // std::queue<int>:
    std::vector<std::queue<int>> intQueues;
    intQueues.resize(2);
    for (int i = 0; i < 10; ++i)
        intQueues[1].push(i);
    for (auto &intQueue : intQueues)
    {
        LLBC_Expect(AssignmentTest_OneSeqType(intQueue) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneSeqType(static_cast<const std::queue<int> &>(intQueue)) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneSeqType(std::move(intQueue)) == LLBC_OK);
    }

    // std::deque<LLBC_String>:
    std::vector<std::deque<LLBC_String>> strDeques;
    strDeques.resize(2);
    for (int i = 0; i < 10; ++i)
        strDeques[1].push_back(LLBC_String().format("str_%d", i));
    for (auto &strDeque: strDeques)
    {
        LLBC_Expect(AssignmentTest_OneSeqType(strDeque) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneSeqType(static_cast<const std::deque<LLBC_String> &>(strDeque)) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneSeqType(std::move(strDeque)) == LLBC_OK);
    }

    // std::set<sint64>:
    std::vector<std::set<sint64>> int64Sets;
    int64Sets.resize(2);
    for (int i = 0; i < 50; ++i)
        int64Sets[1].emplace(i);
    for (auto &int64Set : int64Sets)
    {
        LLBC_Expect(AssignmentTest_OneSeqType(int64Set) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneSeqType(static_cast<const std::set<sint64> &>(int64Set)) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneSeqType(std::move(int64Set)) == LLBC_OK);
    }

    // std::unordered_set<uint64>:
    std::vector<std::unordered_set<uint64>> uint64UnorderedSets;
    uint64UnorderedSets.resize(2);
    for (int i = 0; i < 50; ++i)
        uint64UnorderedSets[1].emplace(i);
    for (auto &uint64UnorderedSet : uint64UnorderedSets)
    {
        LLBC_Expect(AssignmentTest_OneSeqType(uint64UnorderedSet) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneSeqType(static_cast<const std::unordered_set<uint64> &>(uint64UnorderedSet)) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneSeqType(std::move(uint64UnorderedSet)) == LLBC_OK);
    }

    // LLBC_Variant::Seq:
    LLBC_Variant::Seq seq;
    for (int i = 0; i < 10; ++i)
        seq.emplace_back(i);
    LLBC_Expect(AssignmentTest_OneSeqType(seq) == LLBC_OK);
    LLBC_Expect(AssignmentTest_OneSeqType(static_cast<const LLBC_Variant::Seq &>(seq)) == LLBC_OK);
    LLBC_Expect(AssignmentTest_OneSeqType(std::move(seq)) == LLBC_OK);

    return LLBC_OK;
}

template <typename _SeqTy>
int TestCase_Core_Variant::AssignmentTest_OneSeqType(_SeqTy &&seq)
{
    typedef std::remove_cv_t<std::remove_reference_t<_SeqTy>> _PureSeqTy;

    LLBC_PrintLn("Seq assignment test(seq type:%s, rvalue_ref:%s)",
                 LLBC_GetTypeName(seq), std::is_rvalue_reference_v<_SeqTy &&> ? "true" : "false");

    if constexpr (LLBC_IsTemplSpec<_PureSeqTy, std::pair>::value)
    {
        if constexpr (std::is_rvalue_reference_v<_SeqTy &&>)
        {
            const _PureSeqTy copySeq(seq);
            LLBC_Variant seqVar;
            seqVar = std::forward<_SeqTy>(seq);
            LLBC_Expect(seqVar.Is<LLBC_Variant::Seq>() &&
                        seqVar == copySeq &&
                        seq.first == typename _PureSeqTy::first_type() &&
                        seq.second == typename _PureSeqTy::second_type(),
                        "Move Assignment from pair, ty:%s, seqVar:%s",
                        LLBC_GetTypeName(seq), seqVar.ToString().c_str());
        }
        else
        {
            LLBC_Variant seqVar;
            seqVar = std::forward<_SeqTy>(seq);
            LLBC_Expect(seqVar.Is<LLBC_Variant::Seq>() &&
                        seqVar == seq,
                        "Copy Assignment from pair, ty:%s, seqVar:%s",
                        LLBC_GetTypeName(seq), seqVar.ToString().c_str());
        }
    }
    else
    {
        if constexpr (std::is_rvalue_reference_v<_SeqTy &&>)
        {
            const _PureSeqTy copySeq(seq);
            LLBC_Variant seqVar;
            seqVar = std::forward<_SeqTy>(seq);
            LLBC_Expect(seqVar.Is<LLBC_Variant::Seq>() &&
                        seqVar.Size() == copySeq.size() &&
                        seqVar == copySeq &&
                        seq.empty(),
                        "Move Assignment from %s, seqVar:%s",
                        LLBC_GetTypeName(seq), seqVar.ToString().c_str());
        }
        else
        {
            LLBC_Variant seqVar;
            seqVar = std::forward<_SeqTy>(seq);
            LLBC_Expect(seqVar.Is<LLBC_Variant::Seq>() &&
                        seqVar.Size() == seq.size() &&
                        seqVar == seq,
                        "Copy Assignment from %s, seqVar:%s",
                        LLBC_GetTypeName(seq), seqVar.ToString().c_str());
        }
    }

    return LLBC_OK;
}

int TestCase_Core_Variant::AssignmentTest_DictType()
{
    LLBC_PrintLn("Assignment test(Dict type):");

    // std::map<LLBC_String, LLBC_String>:
    std::vector<std::map<LLBC_String, LLBC_String>> maps;
    maps.resize(2);
    for (int i = 0; i < 10; ++i)
        maps[1].emplace(LLBC_String().format("key:%d", i), LLBC_String().format("value:%d", i));

    for (auto &m : maps)
    {
        LLBC_Expect(AssignmentTest_OneDictType(m) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneDictType(static_cast<const std::map<LLBC_String, LLBC_String> &>(m)) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneDictType(std::move(m)) == LLBC_OK);
    }

    // std::unordered_map<int, LLBC_String>:
    std::vector<std::unordered_map<int, LLBC_String>> unorderedMaps;
    unorderedMaps.resize(2);
    for (int i = 0; i < 10; ++i)
        unorderedMaps[1].emplace(i, LLBC_String().format("Value:%d", i));
    for (auto &unorderedMap : unorderedMaps)
    {
        LLBC_Expect(AssignmentTest_OneDictType(unorderedMap) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneDictType(static_cast<const std::unordered_map<int, LLBC_String> &>(unorderedMap)) == LLBC_OK);
        LLBC_Expect(AssignmentTest_OneDictType(std::move(unorderedMap)) == LLBC_OK);
    }

    // LLBC_Variant::Dict:
    LLBC_Variant::Dict dict;
    for (int i = 0; i < 10; ++i)
        dict.emplace(i, i + 10);
    LLBC_Expect(AssignmentTest_OneDictType(dict) == LLBC_OK);
    LLBC_Expect(AssignmentTest_OneDictType(static_cast<const LLBC_Variant::Dict &>(dict)) == LLBC_OK);
    LLBC_Expect(AssignmentTest_OneDictType(std::move(dict)) == LLBC_OK);

    return LLBC_OK;
}

template <typename _DictTy>
int TestCase_Core_Variant::AssignmentTest_OneDictType(_DictTy &&dict)
{
    LLBC_PrintLn("Dict assignment test(dict type:%s, rvalue_ref:%s)",
                 LLBC_GetTypeName(dict), std::is_rvalue_reference_v<_DictTy &&> ? "true" : "false");

    typedef std::remove_cv_t<std::remove_reference_t<_DictTy>> _PureDictTy;
    if constexpr (std::is_rvalue_reference_v<_DictTy &&>)
    {
        const _PureDictTy copyDict(dict);
        LLBC_Variant dictVar;
        dictVar = std::forward<_DictTy>(dict);
        LLBC_Expect(dictVar.Is<LLBC_Variant::Dict>() &&
                    dictVar.Size() == copyDict.size() &&
                    dictVar == copyDict &&
                    dict.empty(),
                    "Move assignment from %s, dict.size():%lu, copyDict.size():%lu, "
                    "dictVar.size():%lu, dictVar.ToString():%s",
                    LLBC_GetTypeName(dict), dict.size(), copyDict.size(), dictVar.Size(), dictVar.ToString().c_str());
    }
    else
    {
        LLBC_Variant dictVar;
        dictVar = std::forward<_DictTy>(dict);
        LLBC_Expect(dictVar.Is<LLBC_Variant::Dict>() && dictVar == dict,
                   "Copy assignment from %s, dictVar:%s",
                   LLBC_GetTypeName(dict), dictVar.ToString().c_str());
    }

    return LLBC_OK;
}

int TestCase_Core_Variant::AssignmentTest_VariantType()
{
    LLBC_PrintLn("Assignment test(Variant type):");

    // Nil:
    LLBC_Variant nilVar;
    LLBC_Expect(AssignmentTest_OneVariantType<void>(nilVar) == LLBC_OK);

    // Raw:
    // - Raw-bool:
    for (auto boolVal : {true, false})
    {
        LLBC_Variant boolVar(boolVal);
        LLBC_Expect(AssignmentTest_OneVariantType<bool>(boolVar) == LLBC_OK);
    }
    // Raw-nullptr:
    LLBC_Variant nullPtrVar(nullptr);
    LLBC_Expect(AssignmentTest_OneVariantType<void *>(nullPtrVar) == LLBC_OK);
    // Raw-void *:
    void *voidPtr;
    if constexpr (LLBC_64BIT_PROCESSOR)
        voidPtr = reinterpret_cast<void *>(0x1234567890ABCDEF);
    else
        voidPtr = reinterpret_cast<void *>(0x12345678);
    LLBC_Variant voidPtrVar(voidPtr);
    LLBC_Expect(AssignmentTest_OneVariantType<void *>(voidPtrVar) == LLBC_OK);
    // ... ...
    // - Raw: sint8/uint8/sint16/uint16/sint32/uint32/long/ulong/sint64/uint64:
    LLBC_Expect((AssignmentTest_RawVariantTypes<
        sint8, uint8, sint16, uint16, sint32, uint32, long, ulong, sint64, uint64>()) == LLBC_OK);
    // - Raw: float/double:
    LLBC_Expect((AssignmentTest_RawVariantTypes<float, double>()) == LLBC_OK);
    // - Raw: enum:
    for (auto traditionalEnumVal :
            {TestTraditionalEnum::TestTraditionalEnum1, TestTraditionalEnum::TestTraditionalEnum2})
    {
        LLBC_Variant traditionalEnumVar(traditionalEnumVal);
        LLBC_Expect(AssignmentTest_OneVariantType<uint32>(traditionalEnumVar) == LLBC_OK);
    }
    for (auto newStyleEnumVal :
            {TestNewStyleEnum::TestNewStyleEnum1, TestNewStyleEnum::TestNewStyleEnum2})
    {
        LLBC_Variant newStyleEnumVar(newStyleEnumVal);
        LLBC_Expect(AssignmentTest_OneVariantType<sint32>(newStyleEnumVar) == LLBC_OK);
    }

    // Str: char array:
    auto charArr = "hello world";
    LLBC_Variant charArrVar(charArr);
    LLBC_Expect(AssignmentTest_OneVariantType<decltype(charArr)>(charArrVar) == LLBC_OK);
    // Str: const char */LLBC_String/std::string/std::string_view/LLBC_CString:
    LLBC_Expect((AssignmentTest_StrVariantTypes<
        const char *, LLBC_String, std::string, std::string_view, LLBC_CString>()) == LLBC_OK);

    // Seq: std::pair/std::vector/std::list/std::queue/std::deque/std::set/std::unordered_set/LLBC_Variant::Seq:
    LLBC_Expect((AssignmentTest_SeqVariantTypes<
        std::pair<LLBC_String, LLBC_String>,
        std::pair<int, LLBC_String>,
        std::pair<LLBC_String, int>,
        std::vector<sint32>,
        std::vector<LLBC_String>,
        std::list<uint32>,
        std::list<LLBC_String>,
        std::queue<long>,
        std::queue<LLBC_String>,
        std::deque<ulong>,
        std::deque<LLBC_String>,
        std::set<sint64>,
        std::set<LLBC_String>,
        std::unordered_set<uint64>,
        std::unordered_set<LLBC_String>>()) == LLBC_OK);

    // Dict: std::map/std::unordered_map/LLBC_Variant::Dict:
    LLBC_Expect((AssignmentTest_DictVariantTypes<
        std::map<int, int>,
        std::map<LLBC_String, int>,
        std::map<int, LLBC_String>,
        std::map<LLBC_String, LLBC_String>,
        std::unordered_map<int, int>,
        std::unordered_map<LLBC_String, int>,
        std::unordered_map<int, LLBC_String>,
        std::unordered_map<LLBC_String, LLBC_String>>()) == LLBC_OK);

    return LLBC_OK;
}

template <typename... _RawTys>
int TestCase_Core_Variant::AssignmentTest_RawVariantTypes()
{
    const bool allExpect = (... && (AssignmentTest_OneRawVariantType<_RawTys>() == LLBC_OK));
    LLBC_Expect(allExpect, "RawTys Assignment test(Variant type)");

    return LLBC_OK;
}

template <typename _RawTy>
int TestCase_Core_Variant::AssignmentTest_OneRawVariantType()
{
    LLBC_Variant rawVar1(static_cast<_RawTy>(std::numeric_limits<_RawTy>::min() + 1));
    LLBC_Expect(AssignmentTest_OneVariantType<_RawTy>(rawVar1) == LLBC_OK);
    
    LLBC_Variant rawVar2(static_cast<_RawTy>(std::numeric_limits<_RawTy>::max() - 1));
    LLBC_Expect(AssignmentTest_OneVariantType<_RawTy>(rawVar2) == LLBC_OK);

    return LLBC_OK;
}

template <typename... _StrTys>
int TestCase_Core_Variant::AssignmentTest_StrVariantTypes()
{
    const bool allExpect = (... && (AssignmentTest_OneStrVariantType<_StrTys>() == LLBC_OK));
    LLBC_Expect(allExpect, "All StrTys Assignment test(Variant type)");

    return LLBC_OK;
}

template <typename _StrTy>
int TestCase_Core_Variant::AssignmentTest_OneStrVariantType()
{
    _StrTy strVal("Hello world");
    LLBC_Variant strVar(strVal);
    LLBC_Expect(AssignmentTest_OneVariantType<_StrTy>(strVar) == LLBC_OK);

    return LLBC_OK;
}

template <typename... _SeqTys>
int TestCase_Core_Variant::AssignmentTest_SeqVariantTypes()
{
    const bool allExpect = (... && (AssignmentTest_OneSeqVariantType<_SeqTys>() == LLBC_OK));
    LLBC_Expect(allExpect, "All SeqTys Assignment test(Variant type)");

    return LLBC_OK;
}

template <typename _SeqTy>
int TestCase_Core_Variant::AssignmentTest_OneSeqVariantType()
{
    _SeqTy seqVal;
    if constexpr (LLBC_IsTemplSpec<_SeqTy, std::pair>::value)
    {
        if constexpr (std::is_same_v<typename _SeqTy::first_type, LLBC_String>)
            seqVal.first = "Hello world(first)";
        else
            seqVal.first = std::numeric_limits<typename _SeqTy::first_type>::min() + 1;

        if constexpr (std::is_same_v<typename _SeqTy::second_type, LLBC_String>)
            seqVal.second = "Hello world(second)";
        else
            seqVal.second = std::numeric_limits<typename _SeqTy::second_type>::max() - 1;
    }
    else if constexpr (LLBC_IsTemplSpec<_SeqTy, std::vector>::value ||
                       LLBC_IsTemplSpec<_SeqTy, std::list>::value ||
                       LLBC_IsTemplSpec<_SeqTy, std::deque>::value)
    {
        for (int i = 0; i < 10; ++i)
        {
            if constexpr (std::is_same_v<typename _SeqTy::value_type, LLBC_String>)
                seqVal.emplace_back(LLBC_String().format("Hello world(%d)", i));
            else
                seqVal.emplace_back(std::numeric_limits<typename _SeqTy::value_type>::min() + i);
        }
    }
    else if constexpr (LLBC_IsTemplSpec<_SeqTy, std::queue>::value)
    {
        for (int i = 0; i < 10; ++i)
        {
            if constexpr (std::is_same_v<typename _SeqTy::value_type, LLBC_String>)
                seqVal.push(LLBC_String().format("Hello world(%d)", i));
            else
                seqVal.push(std::numeric_limits<typename _SeqTy::value_type>::min() + i);
        }
    }
    else if constexpr (LLBC_IsTemplSpec<_SeqTy, std::set>::value ||
                       LLBC_IsTemplSpec<_SeqTy, std::unordered_set>::value)
    {
        for (int i = 0; i < 10; ++i)
        {
            if constexpr (std::is_same_v<typename _SeqTy::value_type, LLBC_String>)
                seqVal.emplace(LLBC_String().format("Hello world(%d)", i));
            else
                seqVal.emplace(std::numeric_limits<typename _SeqTy::value_type>::min() + i);
        }
    }
    else
    {
        llbc_assert(false && "Variant test case error, unknown seq type");
    }

    LLBC_Variant seqVar(seqVal);
    LLBC_Expect(AssignmentTest_OneVariantType<_SeqTy>(seqVar) == LLBC_OK);

    return LLBC_OK;
}

template <typename... _DictTys>
int TestCase_Core_Variant::AssignmentTest_DictVariantTypes()
{
    const bool allExpect = (... && (AssignmentTest_OneDictVariantType<_DictTys>() == LLBC_OK));
    LLBC_Expect(allExpect, "All DictTys Assignment test(Variant type)");

    return LLBC_OK;
}

template <typename _DictTy>
int TestCase_Core_Variant::AssignmentTest_OneDictVariantType()
{
    _DictTy dictVal;
    if constexpr (LLBC_IsTemplSpec<_DictTy, std::map>::value ||
                  LLBC_IsTemplSpec<_DictTy, std::unordered_map>::value)
    {
        for (int i = 0; i < 10; ++i)
        {
            typename _DictTy::key_type key;
            if constexpr (std::is_same_v<typename _DictTy::key_type, LLBC_String>)
                key = LLBC_String().format("Key(%d)", i);
            else
                key = std::numeric_limits<typename _DictTy::key_type>::min() + i;

            typename _DictTy::mapped_type mapped;
            if constexpr (std::is_same_v<typename _DictTy::mapped_type, LLBC_String>)
                mapped = LLBC_String().format("Value(%d)", i);
            else
                mapped = std::numeric_limits<typename _DictTy::mapped_type>::max() - i;

            dictVal.emplace(key, mapped);
        }
    }
    else
    {
        llbc_assert(false && "Variant test case error, unknown dict type");
    }

    LLBC_Variant dictVar(dictVal);
    LLBC_Expect(AssignmentTest_OneVariantType<_DictTy>(dictVar) == LLBC_OK);

    return LLBC_OK;
}

template <typename _RealTy>
int TestCase_Core_Variant::AssignmentTest_OneVariantType(LLBC_Variant &var)
{
    LLBC_Expect(AssignmentTest_OneVariantTypeOneRefType<_RealTy>(var) == LLBC_OK);
    LLBC_Expect(AssignmentTest_OneVariantTypeOneRefType<_RealTy>(static_cast<const LLBC_Variant &>(var)) == LLBC_OK);
    LLBC_Expect(AssignmentTest_OneVariantTypeOneRefType<_RealTy>(std::move(var)) == LLBC_OK);

    return LLBC_OK;
}

template <typename _RealTy, typename _VarTy>
int TestCase_Core_Variant::AssignmentTest_OneVariantTypeOneRefType(_VarTy &&var)
{
    // Print assignment test info.
    constexpr bool isRValueRef = std::is_rvalue_reference_v<_VarTy &&>;
    const LLBC_String realTyStr = LLBC_GetTypeName(_RealTy);
    LLBC_PrintLn("%s-Assignment from variant type, realTy:%s, var:%s",
                 isRValueRef ? "Move" : "Copy",
                 realTyStr.c_str(),
                 var.ToString().c_str());

    if constexpr (isRValueRef)
    {
        const LLBC_Variant copyVar(var);
        LLBC_Variant moveAssignmentFromVar = std::forward<_VarTy>(var);
        LLBC_Expect(moveAssignmentFromVar == copyVar &&
                    moveAssignmentFromVar.Is<_RealTy>() &&
                    var.template Is<_RealTy>() &&
                    ((var.template Is<LLBC_Variant::Str, LLBC_Variant::Seq, LLBC_Variant::Dict>() && var.IsEmpty()) || true),
                    "Move-Assignment from variant type:%s",
                    var.TypeToString().c_str());
    }
    else
    {
        const bool varIsEmptyBeforeCopy =
            ((var.template Is<LLBC_Variant::Str, LLBC_Variant::Seq, LLBC_Variant::Dict>() && var.IsEmpty()) || true);

        LLBC_Variant copyAssignmentFromVar;
        copyAssignmentFromVar = std::forward<_VarTy>(var);

        const bool varIsEmptyAfterCopy =
            ((var.template Is<LLBC_Variant::Str, LLBC_Variant::Seq, LLBC_Variant::Dict>() && var.IsEmpty()) || true);

        const bool copyVarIsEmpty = 
            ((copyAssignmentFromVar.Is<LLBC_Variant::Str, LLBC_Variant::Seq, LLBC_Variant::Dict>() &&
              copyAssignmentFromVar.IsEmpty()) ||
             true);

        LLBC_Expect(copyAssignmentFromVar.Is<_RealTy>() &&
                    var.template Is<_RealTy>() &&
                    copyAssignmentFromVar == var &&
                    varIsEmptyBeforeCopy == varIsEmptyAfterCopy &&
                    copyVarIsEmpty == varIsEmptyBeforeCopy,
                    "Copy-Assignment from variant type:%s, _RealTy:%s, "
                    "copyAssignmentFromVar.Is<_RealTy>():%d, var.Is<_RealTy>():%d, "
                    "copyAssignmentFromVar == var:%d"
                    "varIsEmptyBeforeCopy:%d, varIsEmptyAfterCopy:%d, copyVarIsEmpty:%d, ",
                    var.TypeToString().c_str(),
                    LLBC_GetTypeName(_RealTy),
                    copyAssignmentFromVar.Is<_RealTy>(),
                    var.template Is<_RealTy>(),
                    copyAssignmentFromVar == var,
                    varIsEmptyBeforeCopy, varIsEmptyAfterCopy, copyVarIsEmpty);
    }

    return LLBC_OK;
}

int TestCase_Core_Variant::GetTypeTest()
{
    LLBC_Print("GetType/GetFirstType test:");

    // Nil.
    LLBC_Variant nilVar;
    LLBC_Expect(nilVar.GetType() == LLBC_VariantType::NIL &&
                nilVar.GetType() == LLBC_VariantType::NIL);

    // bool.
    for (auto &boolVal : {true, false})
    {
        LLBC_Variant boolVar(boolVal);
        LLBC_Expect(boolVar.GetType() == LLBC_VariantType::RAW_BOOL &&
                    boolVar.GetFirstType() == LLBC_VariantType::RAW);
    }

    // nullptr/void *.
    LLBC_Expect((GetTypeTest_RawTypes<std::nullptr_t, void *>()) == LLBC_OK);
    // Enum(Traditional/NewStyle).
    LLBC_Expect((GetTypeTest_RawTypes<TestTraditionalEnum, TestNewStyleEnum>()) == LLBC_OK);
    // sint8/uint8/.../sint64/uint64.
    LLBC_Expect((GetTypeTest_RawTypes<
        sint8,
        uint8,
        sint16,
        uint16,
        sint32,
        uint32,
        long,
        ulong,
        sint64,
        uint64>()) == LLBC_OK);
    // float/double.
    LLBC_Expect((GetTypeTest_RawTypes<float, double>()) == LLBC_OK);

    // char */const char *.
    LLBC_Expect((GetTypeTest_StrTypes<char *, const char *>()) == LLBC_OK);
    // LLBC_String/std::string/LLBC_CString/std::string_view.
    LLBC_Expect((GetTypeTest_StrTypes<
        LLBC_String,
        std::string,
        LLBC_CString,
        std::string_view>()) == LLBC_OK);

    // std::pair/std::vector/std::list/std::queue/std::deque/std::set/std::unordered_set.
    LLBC_Expect((GetTypeTest_SeqTypes<
        std::pair<int, int>,
        std::pair<LLBC_String, int>,
        std::pair<int, LLBC_String>,
        std::pair<LLBC_String, LLBC_String>,
        std::vector<int>,
        std::vector<LLBC_String>,
        std::list<int>,
        std::list<LLBC_String>,
        std::queue<int>,
        std::queue<LLBC_String>,
        std::deque<int>,
        std::deque<LLBC_String>,
        std::set<int>,
        std::set<LLBC_String>,
        std::unordered_set<int>,
        std::unordered_set<LLBC_String>>()) == LLBC_OK);

    // std::map/std::unordered_map.
    LLBC_Expect((GetTypeTest_DictTypes<
        std::map<int, int>,
        std::map<int, LLBC_String>,
        std::map<LLBC_String, int>,
        std::map<LLBC_String, LLBC_String>,
        std::unordered_map<int, int>,
        std::unordered_map<int, LLBC_String>,
        std::unordered_map<LLBC_String, int>,
        std::unordered_map<LLBC_String, LLBC_String>>()) == LLBC_OK);

    return LLBC_OK;
}

template <typename... _RawTys>
int TestCase_Core_Variant::GetTypeTest_RawTypes()
{
    LLBC_Expect(... && (GetTypeTest_OneRawType<_RawTys>() == LLBC_OK));

    return LLBC_OK;
}

template <typename _RawTy>
int TestCase_Core_Variant::GetTypeTest_OneRawType()
{
    LLBC_Variant rawVar(_RawTy{});
    LLBC_Expect((rawVar.GetType() == LLBC_VariantType::DeduceType<_RawTy>()) &&
                rawVar.GetFirstType() == LLBC_VariantType::RAW);

    return LLBC_OK;
}

template <typename... _StrTys>
int TestCase_Core_Variant::GetTypeTest_StrTypes()
{
    LLBC_Expect(... && (GetTypeTest_OneStrType<_StrTys>() == LLBC_OK));

    return LLBC_OK;
}

template <typename _StrTy>
int TestCase_Core_Variant::GetTypeTest_OneStrType()
{
    LLBC_Variant strVar(_StrTy{});
    LLBC_Expect((strVar.GetType() == LLBC_VariantType::DeduceType<_StrTy>()) &&
                strVar.GetFirstType() == LLBC_VariantType::STR);

    return LLBC_OK;
}

template <typename... _SeqTys>
int TestCase_Core_Variant::GetTypeTest_SeqTypes()
{
    LLBC_Expect(... && (GetTypeTest_OneSeqType<_SeqTys>() == LLBC_OK));
    return LLBC_OK;
}

template <typename _SeqTy>
int TestCase_Core_Variant::GetTypeTest_OneSeqType()
{
    LLBC_Variant seqVar(_SeqTy{});
    LLBC_Expect((seqVar.GetType() == LLBC_VariantType::DeduceType<_SeqTy>()) &&
                seqVar.GetFirstType() == LLBC_VariantType::SEQ);

    return LLBC_OK;
}

template <typename... _DictTys>
int TestCase_Core_Variant::GetTypeTest_DictTypes()
{
    LLBC_Expect(... &&(GetTypeTest_OneDictType<_DictTys>() == LLBC_OK));

    return LLBC_OK;
}

template <typename _DictTy>
int TestCase_Core_Variant::GetTypeTest_OneDictType()
{
    LLBC_Variant dictVar(_DictTy{});
    LLBC_Expect((dictVar.GetType() == LLBC_VariantType::DeduceType<_DictTy>()) &&
                dictVar.GetFirstType() == LLBC_VariantType::DICT);

    return LLBC_OK;
}

int TestCase_Core_Variant::IsXxxTest()
{
    LLBC_PrintLn("IsXxx() test:");

    // Nil.
    LLBC_Variant nilVar;
    LLBC_Expect(nilVar.Is<void>());

    // Raw.
    LLBC_Expect((IsXxxTest_RawTypes<
        bool,
        sint8,
        uint8,
        sint16,
        uint16,
        sint32,
        uint32,
        long,
        ulong,
        sint64,
        uint64,
        float,
        double,
        TestTraditionalEnum,
        TestNewStyleEnum,
        std::nullptr_t,
        void *>()) == LLBC_OK);

    // Str.
    LLBC_Expect((IsXxxTest_StrTypes<
        char *,
        const char *,
        LLBC_String,
        std::string,
        LLBC_CString,
        std::string_view>()) == LLBC_OK);

    // Seq.
    LLBC_Expect((IsXxxTest_SeqTypes<
        std::pair<int, int>,
        std::pair<LLBC_String, int>,
        std::pair<int, LLBC_String>,
        std::pair<LLBC_String, LLBC_String>,
        std::vector<int>,
        std::vector<LLBC_String>,
        std::list<int>,
        std::list<LLBC_String>,
        std::queue<int>,
        std::queue<LLBC_String>,
        std::deque<int>,
        std::deque<LLBC_String>,
        std::set<int>,
        std::unordered_set<LLBC_String>>()) == LLBC_OK);

    // Dict.
    LLBC_Expect((IsXxxTest_DictTypes<
        std::map<int, int>,
        std::map<LLBC_String, int>,
        std::map<int, LLBC_String>,
        std::map<LLBC_String, LLBC_String>,
        std::unordered_map<int, int>,
        std::unordered_map<LLBC_String, int>,
        std::unordered_map<int, LLBC_String>,
        std::unordered_map<LLBC_String, LLBC_String>>()) == LLBC_OK);

    return LLBC_OK;
}

template <typename... _RawTys>
int TestCase_Core_Variant::IsXxxTest_RawTypes()
{
    LLBC_Expect(... && (IsXxxTest_OneRawType<_RawTys>() == LLBC_OK));

    return LLBC_OK;
}

template <typename _RawTy>
int TestCase_Core_Variant::IsXxxTest_OneRawType()
{
    LLBC_Variant rawVar(_RawTy{});
    LLBC_Expect((rawVar.Is<_RawTy>() &&
                 rawVar.IsRaw() &&
                 rawVar.IsSigned() == LLBC_VariantType::IsSigned(rawVar.GetType()) &&
                 rawVar.IsUnsigned() == LLBC_VariantType::IsUnsigned(rawVar.GetType())));

    return LLBC_OK;
}

template <typename... _StrTys>
int TestCase_Core_Variant::IsXxxTest_StrTypes()
{
    LLBC_Expect(... && (IsXxxTest_OneStrType<_StrTys>() == LLBC_OK));

    return LLBC_OK;
}

template <typename _StrTy>
int TestCase_Core_Variant::IsXxxTest_OneStrType()
{
    LLBC_Variant strVar(_StrTy{});
    LLBC_Expect((strVar.Is<_StrTy>() && strVar.Is<LLBC_Variant::Str>()));

    return LLBC_OK;
}

template <typename... _SeqTys>
int TestCase_Core_Variant::IsXxxTest_SeqTypes()
{
    LLBC_Expect(... && (IsXxxTest_OneSeqType<_SeqTys>() == LLBC_OK));

    return LLBC_OK;
}

template <typename _SeqTy>
int TestCase_Core_Variant::IsXxxTest_OneSeqType()
{
    LLBC_Variant seqVar(_SeqTy{});
    LLBC_Expect(seqVar.Is<_SeqTy>() && seqVar.Is<LLBC_Variant::Seq>());

    return LLBC_OK;
}

template <typename... _DictTys>
int TestCase_Core_Variant::IsXxxTest_DictTypes()
{
    LLBC_Expect(... && (IsXxxTest_OneDictType<_DictTys>() == LLBC_OK));

    return LLBC_OK;
}

template <typename _DictTy>
int TestCase_Core_Variant::IsXxxTest_OneDictType()
{
    LLBC_Variant dictVar(_DictTy{});
    LLBC_Expect(dictVar.Is<_DictTy>() && dictVar.Is<LLBC_Variant::Dict>());

    return LLBC_OK;
}

int TestCase_Core_Variant::BecomeTest()
{
    LLBC_PrintLn("Become() test:");

    // Nil.
    LLBC_Variant nilVar;
    LLBC_Expect(nilVar.Become<void>().Is<void>() &&
                (LLBC_Variant(nilVar).Become<bool>().Is<bool>()) &&
                (LLBC_Variant(nilVar).Become<LLBC_Variant::Str>().Is<LLBC_Variant::Str>()) &&
                (LLBC_Variant(nilVar).Become<LLBC_Variant::Seq>().Is<LLBC_Variant::Seq>()) &&
                (LLBC_Variant(nilVar).Become<LLBC_Variant::Dict>().Is<LLBC_Variant::Dict>()));

    // Raw Types.
    LLBC_Expect((BecomeTest_RawTypes<
        std::nullptr_t,
        void *,
        bool,
        sint8,
        uint8,
        sint16,
        uint16,
        sint32,
        uint32,
        long,
        ulong,
        sint64,
        uint64,
        float,
        double,
        TestTraditionalEnum,
        TestNewStyleEnum>()) == LLBC_OK);

    // Str Types.
    LLBC_Expect((BecomeTest_StrTypes<
        std::string,
        LLBC_String,
        std::string_view,
        LLBC_CString,
        const char *>()) == LLBC_OK);

    //Seq Type.
    LLBC_Expect(BecomeTest_SeqType() == LLBC_OK);

    // Dict Type.
    LLBC_Expect(BecomeTest_DictType() == LLBC_OK);

    return LLBC_OK;
}

template <typename... _RawTys>
int TestCase_Core_Variant::BecomeTest_RawTypes()
{
    LLBC_Expect(... && (BecomeTest_OneRawType<_RawTys>() == LLBC_OK));
    return LLBC_OK;
}

template <typename _RawTy>
int TestCase_Core_Variant::BecomeTest_OneRawType()
{
    LLBC_Variant var;
    if constexpr (std::is_integral_v<_RawTy>)
        var = std::numeric_limits<_RawTy>::max();
    else if constexpr (std::is_floating_point<_RawTy>())
        var = _RawTy{3.14};
    else
        var = _RawTy{};

    LLBC_Variant copyVar = var;
    LLBC_Expect(var.Become<_RawTy>().template Is<_RawTy>() &&
                var == copyVar);

    LLBC_Expect(var.Become<void>().Is<void>());
    var = copyVar;
    LLBC_Expect(var.Become<LLBC_Variant::Str>().template Is<LLBC_Variant::Str>() && var.IsEmpty());
    var = copyVar;
    LLBC_Expect(var.Become<LLBC_Variant::Seq>().template Is<LLBC_Variant::Seq>() && var.IsEmpty());
    var = copyVar;
    LLBC_Expect(var.Become<LLBC_Variant::Dict>().template Is<LLBC_Variant::Dict>() && var.IsEmpty());

    return LLBC_OK;
}

template <typename... _StrTys>
int TestCase_Core_Variant::BecomeTest_StrTypes()
{
    LLBC_Expect(... && (BecomeTest_OneStrType<_StrTys>() == LLBC_OK));
    return LLBC_OK;
}

template <typename _StrTy>
int TestCase_Core_Variant::BecomeTest_OneStrType()
{
    // Str -> Str.
    _StrTy strVal("Hello world");
    LLBC_Variant strVar(strVal);
    LLBC_Variant copyStrVar(strVar);
    LLBC_Expect(strVar.Become<LLBC_Variant::Str>().Is<LLBC_Variant::Str>() &&
                strVar == copyStrVar);

    // Str -> Raw.
    LLBC_Expect(strVar.Become<sint32>().Is<sint32>() && strVar == 0);

    // Str -> Seq.
    strVar = copyStrVar;
    LLBC_Expect(strVar.Become<LLBC_Variant::Seq>().Is<LLBC_Variant::Seq>() && strVar.IsEmpty());

    // Str -> Dict.
    strVar = copyStrVar;
    LLBC_Expect(strVar.Become<LLBC_Variant::Dict>().Is<LLBC_Variant::Dict>() && strVar.IsEmpty());

    return LLBC_OK;
}

int TestCase_Core_Variant::BecomeTest_SeqType()
{
    LLBC_Variant seqVar;
    for (int i = 0; i < 10; ++i)
        seqVar.SeqPushBack(i);

    LLBC_Variant copySeqVar = seqVar;

    // Seq -> Seq.
    LLBC_Expect((seqVar.Become<LLBC_Variant::Seq>().Is<LLBC_Variant::Seq>() &&
                 seqVar == copySeqVar));

    // Seq -> Nil.
    LLBC_Expect((seqVar.Become<void>().Is<void>() &&
                 seqVar == LLBC_Variant::nil &&
                 seqVar != copySeqVar));

    // Seq -> Raw(sint32).
    LLBC_Expect((seqVar.Become<sint32>().Is<sint32>() &&
                 seqVar == 0 &&
                 seqVar != copySeqVar));

    // Seq -> Str.
    LLBC_Expect((seqVar.Become<LLBC_String>().Is<LLBC_String>() &&
                 seqVar == "" &&
                 seqVar.IsEmpty() &&
                 seqVar != copySeqVar));

    // Seq -> Dict.
    LLBC_Expect((seqVar.Become<std::map<int, int>>().Is<std::map<int, int>>() &&
                 seqVar == std::map<int, int>() &&
                 seqVar.IsEmpty() &&
                 seqVar != copySeqVar));

    return LLBC_OK;
}

int TestCase_Core_Variant::BecomeTest_DictType()
{
    LLBC_Variant dictVar;
    for (int i = 0; i < 10; ++i)
        dictVar.DictInsert(i, i * 100);

    LLBC_Variant copyDictVar = dictVar;

    // Dict -> Dict.
    LLBC_Expect(dictVar.Become<LLBC_Variant::Dict>().Is<LLBC_Variant::Dict>() &&
                dictVar == copyDictVar);

    // Dict -> Nil.
    LLBC_Expect((dictVar.Become<void>().Is<void>() &&
                 dictVar == LLBC_Variant::nil &&
                 dictVar != copyDictVar));

    // Dict -> Raw(sint32).
    LLBC_Expect((dictVar.Become<sint32>().Is<sint32>() &&
                 dictVar == 0 &&
                 dictVar != copyDictVar));

    // Dict -> Str.
    LLBC_Expect((dictVar.Become<LLBC_String>().Is<LLBC_String>() &&
                 dictVar == "" &&
                 dictVar.IsEmpty() &&
                 dictVar != copyDictVar));

    // Dict -> Seq.
    LLBC_Expect((dictVar.Become<std::vector<int>>().Is<std::vector<int>>() &&
                 dictVar == std::vector<int>() &&
                 dictVar.IsEmpty() &&
                 dictVar != copyDictVar));

    return LLBC_OK;
}

int TestCase_Core_Variant::AsTest()
{
    LLBC_PrintLn("As<Xxx>() test:");

    // Nil:
    LLBC_Variant nilVar;
    LLBC_Expect(
        nilVar.As<void>() == LLBC_Variant::nil &&
        nilVar.As<sint8>() == 0 &&
        nilVar.As<uint8>() == 0 &&
        nilVar.As<sint16>() == 0 &&
        nilVar.As<uint16>() == 0 &&
        nilVar.As<sint32>() == 0 &&
        nilVar.As<long>() == 0 &&
        nilVar.As<ulong>() == 0 &&
        nilVar.As<sint64>() == 0 &&
        nilVar.As<uint64>() == 0 &&
        nilVar.As<float>() == 0.0 &&
        nilVar.As<double>() == 0.0 &&
        nilVar.As<LLBC_String>() == "" &&
        nilVar.As<LLBC_Variant::Seq>().empty() &&
        nilVar.As<LLBC_Variant::Dict>().empty());

    // Raw - bool:
    for (auto &boolVal : {true, false})
    {
        LLBC_Variant boolVar(boolVal);
        LLBC_Expect(
            boolVar.As<void>() == LLBC_Variant::nil &&
            boolVar.As<bool>() == boolVal &&
            boolVar.As<sint8>() == static_cast<sint8>(boolVal) &&
            boolVar.As<uint8>() == static_cast<uint8>(boolVal) &&
            boolVar.As<sint16>() == static_cast<sint16>(boolVal) &&
            boolVar.As<uint16>() == static_cast<uint16>(boolVal) &&
            boolVar.As<sint32>() == static_cast<sint32>(boolVal) &&
            boolVar.As<uint32>() == static_cast<uint32>(boolVal) &&
            boolVar.As<long>() == static_cast<long>(boolVal) &&
            boolVar.As<ulong>() == static_cast<ulong>(boolVal) &&
            boolVar.As<sint64>() == static_cast<sint64>(boolVal) &&
            boolVar.As<uint64>() == static_cast<uint64>(boolVal) &&
            boolVar.As<float>() == static_cast<float>(boolVal) &&
            boolVar.As<double>() == static_cast<double>(boolVal) &&
            boolVar.As<LLBC_String>() == (boolVal ? "true" : "false") &&
            boolVar.As<LLBC_Variant::Seq>().empty() &&
            boolVar.As<LLBC_Variant::Dict>().empty());
    }

    // Raw - Traditional Enum:
    LLBC_Variant traditionalEnumVar(TestTraditionalEnum::TestTraditionalEnum2);
    LLBC_Expect(
        traditionalEnumVar.As<void>() == LLBC_Variant::nil &&
        traditionalEnumVar.As<sint8>() == TestTraditionalEnum::TestTraditionalEnum2 &&
        traditionalEnumVar.As<uint8>() == TestTraditionalEnum::TestTraditionalEnum2 &&
        traditionalEnumVar.As<sint16>() == TestTraditionalEnum::TestTraditionalEnum2 &&
        traditionalEnumVar.As<uint16>() == TestTraditionalEnum::TestTraditionalEnum2 &&
        traditionalEnumVar.As<sint32>() == TestTraditionalEnum::TestTraditionalEnum2 &&
        traditionalEnumVar.As<uint32>() == TestTraditionalEnum::TestTraditionalEnum2 &&
        traditionalEnumVar.As<long>() == TestTraditionalEnum::TestTraditionalEnum2 &&
        traditionalEnumVar.As<ulong>() == TestTraditionalEnum::TestTraditionalEnum2 &&
        traditionalEnumVar.As<sint64>() == TestTraditionalEnum::TestTraditionalEnum2 &&
        traditionalEnumVar.As<uint64>() == TestTraditionalEnum::TestTraditionalEnum2 &&
        traditionalEnumVar.As<float>() == TestTraditionalEnum::TestTraditionalEnum2 &&
        traditionalEnumVar.As<double>() == TestTraditionalEnum::TestTraditionalEnum2 &&
        traditionalEnumVar.As<LLBC_String>() ==
            LLBC_Num2Str(static_cast<int>(TestTraditionalEnum::TestTraditionalEnum2)) &&
        traditionalEnumVar.As<LLBC_Variant::Seq>().empty() &&
        traditionalEnumVar.As<LLBC_Variant::Dict>().empty());

    // Raw - NewStyle Enum:
    LLBC_Variant newStyleEnumVar(TestNewStyleEnum::TestNewStyleEnum2);
    LLBC_Expect(
        newStyleEnumVar.As<void>() == LLBC_Variant::nil &&
        newStyleEnumVar.As<sint8>() == static_cast<sint8>(TestNewStyleEnum::TestNewStyleEnum2) &&
        newStyleEnumVar.As<uint8>() == static_cast<uint8>(TestNewStyleEnum::TestNewStyleEnum2) &&
        newStyleEnumVar.As<sint16>() == static_cast<sint16>(TestNewStyleEnum::TestNewStyleEnum2) &&
        newStyleEnumVar.As<uint16>() == static_cast<uint16>(TestNewStyleEnum::TestNewStyleEnum2) &&
        newStyleEnumVar.As<sint32>() == static_cast<sint32>(TestNewStyleEnum::TestNewStyleEnum2) &&
        newStyleEnumVar.As<uint32>() == static_cast<uint32>(TestNewStyleEnum::TestNewStyleEnum2) &&
        newStyleEnumVar.As<long>() == static_cast<long>(TestNewStyleEnum::TestNewStyleEnum2) &&
        newStyleEnumVar.As<ulong>() == static_cast<ulong>(TestNewStyleEnum::TestNewStyleEnum2) &&
        newStyleEnumVar.As<sint64>() == static_cast<sint64>(TestNewStyleEnum::TestNewStyleEnum2) &&
        newStyleEnumVar.As<uint64>() == static_cast<uint64>(TestNewStyleEnum::TestNewStyleEnum2) &&
        newStyleEnumVar.As<float>() == static_cast<float>(TestNewStyleEnum::TestNewStyleEnum2) &&
        newStyleEnumVar.As<double>() == static_cast<double>(TestNewStyleEnum::TestNewStyleEnum2) &&
        newStyleEnumVar.As<LLBC_String>() ==
            LLBC_Num2Str(static_cast<int>(TestNewStyleEnum::TestNewStyleEnum2)) &&
        traditionalEnumVar.As<LLBC_Variant::Seq>().empty() &&
        traditionalEnumVar.As<LLBC_Variant::Dict>().empty());

    // Raw - Other types:
    LLBC_Expect((AsTest_RawTypes<
        sint8,
        uint8,
        sint16,
        uint16,
        sint32,
        uint32,
        long,
        ulong,
        sint64,
        uint64,
        float,
        double>()) == LLBC_OK);

    // Str Type:
    LLBC_Expect(AsTest_StrType() == LLBC_OK);

    // Seq Type:
    LLBC_Expect(AsTest_SeqType() == LLBC_OK);

    // Dict Type:
    LLBC_Expect(AsTest_DictType() == LLBC_OK);

    // Self(LLBC_Variant type):
    LLBC_Variant var;
    var[10086][LLBC_Variant::nil][10086] = "Hello World";
    LLBC_Expect(var.As<LLBC_Variant>() == var &&
                &var.As<LLBC_Variant>() == &var);

    return LLBC_OK;
}

template <typename... _RawTys>
int TestCase_Core_Variant::AsTest_RawTypes()
{
    LLBC_Expect(... && (AsTest_OneRawType<_RawTys>() == LLBC_OK));
    return LLBC_OK;
}

template <typename _RawTy>
int TestCase_Core_Variant::AsTest_OneRawType()
{
    _RawTy val;
    if constexpr (std::is_signed_v<_RawTy>)
        val = static_cast<_RawTy>(-8);
    else
        val = static_cast<_RawTy>(8);

    const LLBC_Variant var(val);
    LLBC_Expect(
        var.As<void>() == LLBC_Variant::nil &&
        var.As<sint8>() == static_cast<sint8>(val) &&
        var.As<uint8>() == static_cast<uint8>(val) &&
        var.As<sint16>() == static_cast<sint16>(val) &&
        var.As<uint16>() == static_cast<uint16>(val) &&
        var.As<sint32>() == static_cast<sint32>(val) &&
        var.As<uint32>() == static_cast<uint32>(val) &&
        var.As<long>() == static_cast<long>(val) &&
        var.As<ulong>() == static_cast<ulong>(val) &&
        var.As<sint64>() == static_cast<sint64>(val) &&
        var.As<uint64>() == static_cast<uint64>(val) &&
        var.As<float>() == static_cast<float>(val) &&
        var.As<double>() == static_cast<double>(val) &&
        var.As<LLBC_String>() == LLBC_Num2Str(val) &&
        var.As<LLBC_Variant::Seq>().empty() &&
        var.As<LLBC_Variant::Dict>().empty());

    return LLBC_OK;
}

int TestCase_Core_Variant::AsTest_StrType()
{
    LLBC_String strVal("8");
    const LLBC_Variant strVar(strVal);

    LLBC_Expect(strVar.As<void>() == LLBC_Variant::nil &&
                strVar.As<bool>() == true &&
                strVar.As<sint8>() == LLBC_Str2Num<sint8>(strVal.c_str()) &&
                strVar.As<uint8>() == LLBC_Str2Num<uint8>(strVal.c_str()) &&
                strVar.As<sint16>() == LLBC_Str2Num<sint16>(strVal.c_str()) &&
                strVar.As<uint16>() == LLBC_Str2Num<uint16>(strVal.c_str()) &&
                strVar.As<sint32>() == LLBC_Str2Num<sint32>(strVal.c_str()) &&
                strVar.As<uint32>() == LLBC_Str2Num<uint32>(strVal.c_str()) &&
                strVar.As<long>() == LLBC_Str2Num<long>(strVal.c_str()) &&
                strVar.As<ulong>() == LLBC_Str2Num<ulong>(strVal.c_str()) &&
                strVar.As<sint64>() == LLBC_Str2Num<sint64>(strVal.c_str()) &&
                strVar.As<uint64>() == LLBC_Str2Num<uint64>(strVal.c_str()) &&
                strVar.As<float>() == LLBC_Str2Num<float>(strVal.c_str()) &&
                strVar.As<double>() == LLBC_Str2Num<double>(strVal.c_str()) &&
                strVar.As<LLBC_Variant::Seq>().empty() &&
                strVar.As<LLBC_Variant::Dict>().empty());

    return LLBC_OK;
}

int TestCase_Core_Variant::AsTest_SeqType()
{
    // Emtpy Seq:
    LLBC_Variant emptySeqVar;
    LLBC_Expect(emptySeqVar.As<void>() == LLBC_Variant::nil &&
                emptySeqVar.IsEmpty() &&
                emptySeqVar.As<bool>() == false); // !!! seq is empty.

    // Non-Empty Seq:
    LLBC_Variant seqFirstElem(1);
    LLBC_Variant seqSecondElem("Hello World");
    LLBC_Variant::Seq rawSeq;
    rawSeq.push_back(seqFirstElem);
    rawSeq.push_back(seqSecondElem);

    LLBC_Variant seqVar;
    seqVar.SeqPushBack(1);
    seqVar.SeqPushBack("Hello World");
    LLBC_Expect(seqVar.Size() == 2 &&
                seqVar[0] == seqFirstElem &&
                seqVar[1] == seqSecondElem);

    LLBC_Expect(seqVar.As<void>() == LLBC_Variant::nil &&
                seqVar.As<bool>() == true && // !!! seq is not empty.
                seqVar.As<sint8>() == 0 &&
                seqVar.As<uint8>() == 0 &&
                seqVar.As<sint16>() == 0 &&
                seqVar.As<uint16>() == 0 &&
                seqVar.As<sint32>() == 0 &&
                seqVar.As<uint32>() == 0 &&
                seqVar.As<long>() == 0 &&
                seqVar.As<ulong>() == 0 &&
                seqVar.As<sint64>() == 0 &&
                seqVar.As<uint64>() == 0 &&
                seqVar.As<float>() == 0.0 &&
                seqVar.As<double>() == 0.0 &&
                seqVar.As<LLBC_String>() == seqVar.ValueToString() &&
                seqVar.As<LLBC_Variant::Seq>()[0] == seqFirstElem &&
                seqVar.As<LLBC_Variant::Seq>()[1] == seqSecondElem &&
                seqVar.As<std::vector<int>>()[0] == seqFirstElem.As<int>() &&
                seqVar.As<std::vector<LLBC_String>>()[1] == seqSecondElem.As<LLBC_String>() &&
                seqVar.As<LLBC_Variant::Seq>() == rawSeq &&
                seqVar.As<LLBC_Variant::Seq>().size() == 2 &&
                seqVar.As<LLBC_Variant::Dict>().empty());

    return LLBC_OK;
}

int TestCase_Core_Variant::AsTest_DictType()
{
    // Empty Dict:
    LLBC_Variant emptyDictVar;
    LLBC_Expect(emptyDictVar.As<void>() == LLBC_Variant::nil &&
                emptyDictVar.IsEmpty() &&
                emptyDictVar.As<bool>() == false); // !! seq is empty.

    // Non-Empty Dict:
    std::pair<LLBC_Variant, LLBC_Variant> dictFirstPair{1, 2.0};
    std::pair<LLBC_Variant, LLBC_Variant> dictSecondPair{"Hello World", 3};
    LLBC_Variant::Dict rawDict;
    rawDict.insert(dictFirstPair);
    rawDict.insert(dictSecondPair);

    LLBC_Variant dictVar;
    dictVar.DictInsert(dictFirstPair);
    dictVar.DictInsert(dictSecondPair);
    LLBC_Expect(dictVar.Size() == 2 &&
                (dictVar.DictFind(dictFirstPair.first) != dictVar.DictEnd() &&
                    dictVar.DictFind(dictFirstPair.first)->second == dictFirstPair.second) &&
                (dictVar.DictFind(dictSecondPair.first) != dictVar.DictEnd() &&
                    dictVar.DictFind(dictSecondPair.first)->second == dictSecondPair.second));

    const auto &asDictVal = dictVar.As<LLBC_Variant::Dict>();
    LLBC_Expect((dictVar.As<void>() == LLBC_Variant::nil &&
                 dictVar.As<bool>() == true && // !!! dict is not empty.
                 dictVar.As<sint8>() == 0 &&
                 dictVar.As<uint8>() == 0 &&
                 dictVar.As<sint16>() == 0 &&
                 dictVar.As<uint16>() == 0 &&
                 dictVar.As<sint32>() == 0 &&
                 dictVar.As<uint32>() == 0 &&
                 dictVar.As<long>() == 0 &&
                 dictVar.As<ulong>() == 0 &&
                 dictVar.As<sint64>() == 0 &&
                 dictVar.As<uint64>() == 0 &&
                 dictVar.As<float>() == 0 &&
                 dictVar.As<double>() == 0 &&
                 dictVar.As<LLBC_String>() == dictVar.ValueToString() &&
                 dictVar.As<LLBC_Variant::Seq>().empty() &&
                 (asDictVal.find(dictFirstPair.first) != asDictVal.end() &&
                    asDictVal.find(dictFirstPair.first)->second == dictFirstPair.second) &&
                 (asDictVal.find(dictSecondPair.first) != asDictVal.end() &&
                    asDictVal.find(dictSecondPair.first)->second == dictSecondPair.second) &&
                 dictVar.As<std::map<int, int>>().count(dictFirstPair.first) == 1 &&
                 dictVar.As<std::map<int, int>>().count(10086) == 0 &&
                 dictVar.As<std::map<LLBC_String, LLBC_String>>().count(
                     LLBC_Num2Str(dictFirstPair.first.As<sint32>())) == 1 &&
                 dictVar.As<std::map<LLBC_String, LLBC_String>>().count("Hello World") == 1 &&
                 dictVar.As<std::map<LLBC_String, LLBC_String>>().count("Not Exist Key") == 0));

    return LLBC_OK;
}

int TestCase_Core_Variant::AsLooseBoolTest()
{
    LLBC_PrintLn("AsLooseBool() test:");

    // Nil:
    LLBC_Variant nilVar;
    LLBC_Expect(nilVar.AsLooseBool() == false);

    // Raw-Bool:
    for (auto &boolVal : {true, false})
        LLBC_Expect(LLBC_Variant(boolVal).AsLooseBool() == boolVal);
    // Raw-Traditional enum:
    LLBC_Expect(LLBC_Variant(TestTraditionalEnum::TestTraditionalEnum2).AsLooseBool() == true);
    // Raw-NewStyle enum:
    LLBC_Expect(LLBC_Variant(TestNewStyleEnum::TestNewStyleEnum2).AsLooseBool() == true);
    // Raw-Other types:
    LLBC_Expect((AsLooseBoolTest_RawTypes<
        sint8,
        uint8,
        sint16,
        uint16,
        sint32,
        uint32,
        long,
        ulong,
        sint64,
        uint64,
        float,
        double>()) == LLBC_OK);

    // Str:
    LLBC_Expect(LLBC_Variant("") == false &&
                LLBC_Variant("XXFAEFDAS") == false);

    for (auto &trueStrVal : {"true", "True", "TRUE", " \tTrUe", "tRuE \t", "\t TrUe\t "})
        LLBC_Expect(LLBC_Variant(trueStrVal).AsLooseBool() == true, "trueStrVal:%s", trueStrVal);
    for (auto &yesStrVal : {"yes", "Yes", "YES", " \tYes", "yEs \t", "\t YeS\t "})
        LLBC_Expect(LLBC_Variant(yesStrVal).AsLooseBool() == true, "yesStrVal:%s", yesStrVal);

    for (auto &falseStrVal : {"false", "False", "FALSE", " \tFaLsE", "fAlSe \t", "\t FaLsE\t "})
        LLBC_Expect(LLBC_Variant(falseStrVal).AsLooseBool() == false, "falseStrVal:%s", falseStrVal);
    for (auto &noStrVal : {"no", "No", "NO", " \tNo", "nO \t", "\t NO\t "})
        LLBC_Expect(LLBC_Variant(noStrVal).AsLooseBool() == false, "noStrVal:%s", noStrVal);

    // Seq:
    LLBC_Variant seqVar;
    seqVar.Become<LLBC_Variant::Seq>();
    LLBC_Expect(seqVar.AsLooseBool() == false);
    seqVar.SeqPushBack(1);
    seqVar.SeqPushBack("Hello World");
    LLBC_Expect(seqVar.AsLooseBool() == true);

    // Dict:
    LLBC_Variant dictVar;
    dictVar.Become<LLBC_Variant::Dict>();
    LLBC_Expect(dictVar.AsLooseBool() == false);
    dictVar.DictInsert(1, 2.0);
    dictVar.DictInsert("Hello World", 3);
    LLBC_Expect(dictVar.AsLooseBool() == true);

    return LLBC_OK;
}

template <typename... _RawTys>
int TestCase_Core_Variant::AsLooseBoolTest_RawTypes()
{
    LLBC_Expect(... && (AsLooseBoolTest_OneRawType<_RawTys>() == LLBC_OK));
    return LLBC_OK;
}

template <typename _RawTy>
int TestCase_Core_Variant::AsLooseBoolTest_OneRawType()
{
    // Default Raw: false.
    LLBC_Expect(LLBC_Variant(_RawTy{}).AsLooseBool() == false);

    // Non-Default Raw: true.
    _RawTy val;
    if constexpr (std::is_signed_v<_RawTy>)
        val = static_cast<_RawTy>(-8);
    else
        val = static_cast<_RawTy>(8);

    const LLBC_Variant var(val);
    LLBC_Expect(var.AsLooseBool() == true);

    return LLBC_OK;
}

int TestCase_Core_Variant::ImplicitValueFetchTest()
{
    LLBC_PrintLn("Implicit value fetch test:");

    // Nil:
    LLBC_Variant nilVar;
    LLBC_Expect(nilVar == LLBC_Variant::nil &&
                &static_cast<const LLBC_Variant &>(nilVar) != &LLBC_Variant::nil &&
                &static_cast<const LLBC_Variant &>(nilVar) == &nilVar);

    // Self Var:
    LLBC_Variant selfVar(true);
    LLBC_Expect(selfVar == selfVar &&
                &static_cast<const LLBC_Variant &>(selfVar) == &selfVar);

    // Raw:
    LLBC_Variant rawVar(10086);
    LLBC_Expect(rawVar == 10086);

    // Str:
    LLBC_Variant strVar("Hello World");
    LLBC_Expect(strVar == LLBC_String("Hello World") &&
                strVar == std::string("Hello World") &&
                strVar == LLBC_CString("Hello World") &&
                strVar == std::string_view("Hello World") &&
                strVar == "Hello World");

    // Seq:
    LLBC_Variant seqVar;
    std::vector<int> rawVec;
    for (int i = 0; i < 10; ++i)
    {
        rawVec.push_back(i);
        seqVar.SeqPushBack(i);
    }
    LLBC_Expect(seqVar == rawVec &&
                seqVar == seqVar.GetData().seq());

    // Dict:
    LLBC_Variant dictVar;
    std::map<LLBC_String, LLBC_String> rawDict;
    for (int i = 0; i < 10; ++i)
    {
        rawDict.emplace(LLBC_String().format("Key:%d", i), LLBC_String().format("Value:%d", i));
        dictVar.DictInsert(LLBC_String().format("Key:%d", i), LLBC_String().format("Value:%d", i));
    }

    LLBC_Expect(dictVar == rawDict &&
                dictVar == dictVar.GetData().dict());

    return LLBC_OK;
}

int TestCase_Core_Variant::ClearTest()
{
    LLBC_PrintLn("Clear() test:");

    // Nil:
    LLBC_Variant nilVar;
    nilVar.Clear();
    LLBC_Expect(nilVar.Is<void>() &&
                nilVar.GetData().i64() == 0);

    // Raw:
    LLBC_Variant rawVar(32);
    rawVar.Clear();
    LLBC_Expect(rawVar.Is<sint32>() &&
                rawVar == 0);

    // Str:
    LLBC_Variant strVar("Hello World");
    strVar.Clear();
    LLBC_Expect(strVar.Is<LLBC_String>() &&
                strVar == "");

    // Seq:
    LLBC_Variant seqVar;
    seqVar.SeqPushBack(false);
    seqVar.SeqPushBack(LLBC_Variant::nil);
    seqVar.Clear();
    LLBC_Expect(seqVar.Is<LLBC_Variant::Seq>() &&
                seqVar.IsEmpty());

    // Dict:
    LLBC_Variant dictVar;
    dictVar.DictInsert(false, LLBC_Variant::nil);
    dictVar.DictInsert(3.14159, "Hello World");
    dictVar.Clear();
    LLBC_Expect(dictVar.Is<LLBC_Variant::Dict>() &&
                dictVar.IsEmpty());

    return LLBC_OK;
}

int TestCase_Core_Variant::IsEmptyTest()
{
    LLBC_PrintLn("IsEmpty() test:");

    // Nil:
    LLBC_Expect(LLBC_Variant::nil.IsEmpty());

    // Raw:
    LLBC_Expect(LLBC_Variant(32).IsEmpty());

    // Str:
    LLBC_Expect(LLBC_Variant("").IsEmpty() &&
                !LLBC_Variant("Hello World").IsEmpty());

    // Seq:
    LLBC_Variant emptySeqVar;
    emptySeqVar.Become<LLBC_Variant::Seq>();
    LLBC_Variant nonEmptySeqVar;
    nonEmptySeqVar.SeqPushBack("Hello World");
    LLBC_Expect(emptySeqVar.IsEmpty() &&
                !nonEmptySeqVar.IsEmpty());

    // Dict:
    LLBC_Variant emptyDictVar;
    emptyDictVar.Become<LLBC_Variant::Seq>();
    LLBC_Variant nonEmptyDictVar;
    nonEmptyDictVar.DictInsert("Hello World", 3.14159);
    LLBC_Expect(emptyDictVar.IsEmpty() &&
                !nonEmptyDictVar.IsEmpty());

    return LLBC_OK;
}

int TestCase_Core_Variant::SizeAndCapacityTest()
{
    LLBC_PrintLn("Size() && Capacity() test:");

    // Nil:
    LLBC_Expect(LLBC_Variant::nil.Size() == 0 &&
                LLBC_Variant::nil.Capacity() == 0);

    // Raw:
    LLBC_Expect(LLBC_Variant(-8).Size() == 0 &&
                LLBC_Variant(-8).Capacity() == 0);

    // Str:
    LLBC_Variant strVar("");
    LLBC_Expect(strVar.Size() == 0 &&
                strVar.Capacity() == 0);
    strVar = "Hello World";
    strVar.StrResize(20);
    strVar.StrReserve(80);
    LLBC_Expect(strVar.Size() == 20 &&
                strVar.Capacity() == 80 &&
                strVar.As<LLBC_Variant::Str>()[0] == 'H' &&
                strVar.As<LLBC_Variant::Str>()[19] == '\0',
                "strVar.Size():%lu, strVar.Capacity():%lu",
                strVar.Size(),
                strVar.Capacity());

    // Seq:
    LLBC_Variant seqVar;
    seqVar.Become<LLBC_Variant::Seq>();
    for (int i = 0; i < 8; ++i)
        seqVar.SeqPushBack(i);
    seqVar.SeqResize(20, 9999);
    seqVar.SeqReserve(30);
    LLBC_Expect(seqVar.Size() == 20 &&
                seqVar.Capacity() == 30 &&
                seqVar.As<LLBC_Variant::Seq>()[0] == 0 &&
                seqVar.As<LLBC_Variant::Seq>()[19] == 9999);

    // Dict:
    LLBC_Variant dictVar;
    dictVar.Become<LLBC_Variant::Dict>();
    dictVar.DictInsert("Hello", "World");
    dictVar.DictInsert(123, 456);
    LLBC_Expect(dictVar.Size() == 2 &&
                dictVar.Capacity() == dictVar.Size() &&
                dictVar.Count("Hello") == 1 &&
                dictVar.Count(123) == 1);

    return LLBC_OK;
}

int TestCase_Core_Variant::CountAndContainsTest()
{
    LLBC_PrintLn("Count() test:");

    // Nil:
    LLBC_Variant nilVar;
    LLBC_Expect(nilVar.Count(LLBC_Variant::nil) == 0 &&
                    !nilVar.Contains(LLBC_Variant::nil) &&
                nilVar.Count(LLBC_Variant(false)) == 0 &&
                    !nilVar.Contains(LLBC_Variant(false)) &&
                nilVar.Count(LLBC_Variant(static_cast<sint8>(-8))) == 0 &&
                    !nilVar.Contains(LLBC_Variant(static_cast<sint8>(-8))) &&
                nilVar.Count(LLBC_Variant(static_cast<uint8>(8))) == 0 &&
                    !nilVar.Contains(LLBC_Variant(static_cast<uint8>(8))) &&
                nilVar.Count(LLBC_Variant(static_cast<sint16>(-16))) == 0 &&
                    !nilVar.Contains(LLBC_Variant(static_cast<sint16>(-16))) &&
                nilVar.Count(LLBC_Variant(static_cast<uint16>(16))) == 0 &&
                    !nilVar.Contains(LLBC_Variant(static_cast<uint16>(16))) &&
                nilVar.Count(LLBC_Variant(static_cast<sint32>(-32))) == 0 &&
                    !nilVar.Contains(LLBC_Variant(static_cast<sint32>(-32))) &&
                nilVar.Count(LLBC_Variant(static_cast<uint32>(32))) == 0 &&
                    !nilVar.Contains(LLBC_Variant(static_cast<uint32>(32))) &&
                nilVar.Count(LLBC_Variant(static_cast<long>(-48))) == 0 &&
                    !nilVar.Contains(LLBC_Variant(static_cast<long>(-48))) &&
                nilVar.Count(LLBC_Variant(static_cast<long>(48))) == 0 &&
                    !nilVar.Contains(LLBC_Variant(static_cast<long>(48))) &&
                nilVar.Count(LLBC_Variant(static_cast<sint64>(-64))) == 0 &&
                    !nilVar.Contains(LLBC_Variant(static_cast<uint64>(64))) &&
                nilVar.Count(LLBC_Variant(3.14f)) == 0 &&
                    !nilVar.Contains(LLBC_Variant(3.14f)) &&
                nilVar.Count(LLBC_Variant(3.14)) == 0 &&
                    !nilVar.Contains(LLBC_Variant(3.14)) &&
                nilVar.Count(LLBC_Variant::Seq{}) == 0 &&
                    !nilVar.Contains(LLBC_Variant::Seq{}) &&
                nilVar.Count(LLBC_Variant::Dict{}) == 0 &&
                    !nilVar.Contains(LLBC_Variant::Dict{}));

    // Raw:
    LLBC_Variant rawVar(32);
    LLBC_Expect(rawVar.Count(LLBC_Variant::nil) == 0 &&
                    !rawVar.Contains(LLBC_Variant::nil) &&
                rawVar.Count(LLBC_Variant(false)) == 0 &&
                    !rawVar.Contains(LLBC_Variant(false)) &&
                rawVar.Count(LLBC_Variant(static_cast<sint8>(-8))) == 0 &&
                    !rawVar.Contains(LLBC_Variant(static_cast<sint8>(-8))) &&
                rawVar.Count(LLBC_Variant(static_cast<uint8>(8))) == 0 &&
                    !rawVar.Contains(LLBC_Variant(static_cast<uint8>(8))) &&
                rawVar.Count(LLBC_Variant(static_cast<sint16>(-16))) == 0 &&
                    !rawVar.Contains(LLBC_Variant(static_cast<sint16>(-16))) &&
                rawVar.Count(LLBC_Variant(static_cast<uint16>(16))) == 0 &&
                    !rawVar.Contains(LLBC_Variant(static_cast<uint16>(16))) &&
                rawVar.Count(LLBC_Variant(static_cast<sint32>(-32))) == 0 &&
                    !rawVar.Contains(LLBC_Variant(static_cast<sint32>(-32))) &&
                rawVar.Count(LLBC_Variant(static_cast<uint32>(32))) == 0 &&
                    !rawVar.Contains(LLBC_Variant(static_cast<uint32>(32))) &&
                rawVar.Count(LLBC_Variant(static_cast<long>(-48))) == 0 &&
                    !rawVar.Contains(LLBC_Variant(static_cast<long>(-48))) &&
                rawVar.Count(LLBC_Variant(static_cast<long>(48))) == 0 &&
                    !rawVar.Contains(LLBC_Variant(static_cast<long>(48))) &&
                rawVar.Count(LLBC_Variant(static_cast<sint64>(-64))) == 0 &&
                    !rawVar.Contains(LLBC_Variant(static_cast<sint64>(-64))) &&
                rawVar.Count(LLBC_Variant(static_cast<uint64>(64))) == 0 &&
                    !rawVar.Contains(LLBC_Variant(static_cast<uint64>(64))) &&
                rawVar.Count(LLBC_Variant(3.14f)) == 0 &&
                    !rawVar.Contains(LLBC_Variant(3.14f)) &&
                rawVar.Count(LLBC_Variant(3.14)) == 0 &&
                    !rawVar.Contains(LLBC_Variant(3.14)) &&
                rawVar.Count(LLBC_Variant::Seq{}) == 0 &&
                    !rawVar.Contains(LLBC_Variant::Seq{}) &&
                rawVar.Count(LLBC_Variant::Dict{}) == 0 &&
                    !rawVar.Contains(LLBC_Variant::Dict{}));

    // Str:
    const char *strValPtr = "Hello world Hello world";
    LLBC_Variant strVar(strValPtr);
    LLBC_Expect(strVar.Count("Hello world") == 2 &&
                    strVar.Contains("Hello world")&&
                strVar.Count(LLBC_String("Hello world")) == 2 &&
                    strVar.Contains(LLBC_String("Hello world")) &&
                strVar.Count(LLBC_CString("Hello world")) == 2 &&
                    strVar.Contains(LLBC_CString("Hello world")) &&
                strVar.Count(std::string("Hello world")) == 2 &&
                    strVar.Contains(std::string("Hello world")) &&
                strVar.Count(std::string_view("Hello world")) == 2 &&
                    strVar.Contains(std::string_view("Hello world")));
    LLBC_Expect(strVar.Count("Hey Judy") == 0 &&
                    !strVar.Contains("Hey Judy") &&
                strVar.Count(strValPtr) == 1 &&
                    strVar.Contains(strValPtr) &&
                strVar.Count("") == 0 &&
                    !strVar.Contains(""));

    LLBC_Variant emptyStrVar("");
    LLBC_Expect(emptyStrVar.Count("Hello world") == 0 &&
                    !emptyStrVar.Contains("Hello world") &&
                emptyStrVar.Count("") == 0 &&
                    !emptyStrVar.Contains(""));

    const int intVal = 1234;
    LLBC_Variant numStrVar(LLBC_String().format("%d", intVal) * 3);
    LLBC_Expect(numStrVar.Count(intVal) == 3 &&
                    numStrVar.Contains(intVal) &&
                numStrVar.Count(intVal * 10) == 0 &&
                    !numStrVar.Contains(intVal * 10) &&
                numStrVar.Count(LLBC_Variant(intVal)) == 3 &&
                    numStrVar.Contains(LLBC_Variant(intVal)) &&
                numStrVar.Count(LLBC_Variant(intVal * 10)) == 0 &&
                    !numStrVar.Contains(LLBC_Variant(intVal * 10)) &&
                numStrVar.Count(LLBC_Variant::nil) == 0 &&
                    !numStrVar.Contains(LLBC_Variant::nil));

    return LLBC_OK;
}

int TestCase_Core_Variant::SeqSpecificTest()
{
    LLBC_PrintLn("Sequence specific test:");

    // Non-Seq call NON-CONST sequence specific methods test(implicity convert to Seq).
    LLBC_Variant testVar;
    LLBC_Expect(testVar.SeqBegin() == testVar.SeqEnd() && testVar.Is<LLBC_Variant::Seq>());

    testVar.Become<void>();
    LLBC_Expect(testVar.SeqReverseBegin() == testVar.SeqReverseEnd() && testVar.Is<LLBC_Variant::Seq>());

    testVar.Become<void>();
    testVar.SeqPushBack(3);
    LLBC_Expect(testVar.Is<LLBC_Variant::Seq>() && testVar.Size() == 1);

    testVar.Become<void>();
    testVar.SeqBatchPushBack(false, 3.1415, "Hello World", std::vector<int>{});
    LLBC_Expect(testVar.Is<LLBC_Variant::Seq>() &&
                testVar.Size() == 4 &&
                testVar[0] == false &&
                testVar[1] == 3.1415 &&
                testVar[2] == "Hello World" &&
                testVar[3] == std::vector<int>{});

    testVar.Become<void>();
    testVar.SeqResize(100);
    LLBC_Expect(testVar.Is<LLBC_Variant::Seq>() &&
                testVar.Size() == 100 &&
                std::all_of(testVar.SeqBegin(),
                            testVar.SeqEnd(),
                            [](const auto &elem) { return elem.template Is<void>(); }));
    
    testVar.Become<void>();
    testVar.SeqReserve(200);
    LLBC_Expect(testVar.Is<LLBC_Variant::Seq>() &&
                testVar.Size() == 0 &&
                testVar.Capacity() == 200);

    testVar.Become<void>();
    LLBC_Expect(testVar.SeqErase(3) == 0 &&
                testVar.Is<LLBC_Variant::Seq>());
    LLBC_Expect(testVar.SeqEraseIf([](const auto &elem) { return elem == 3; }) == 0 &&
                testVar.Is<LLBC_Variant::Seq>());

    // Below test code will raise abort.
    // testVar.Become<void>();
    // testVar.SeqInsert(testVar.SeqBegin(), 1, 3);
    // LLBC_Expect(testVar.Is<LLBC_Variant::Seq>());

    // Non-Seq call CONST sequence specific methods test(will not be implicitly converted to Seq).
    const LLBC_Variant constTestVar;
    LLBC_Expect(constTestVar.SeqBegin() == constTestVar.SeqEnd() && constTestVar.Is<void>());
    LLBC_Expect(constTestVar.SeqReverseBegin() == constTestVar.SeqReverseEnd() && constTestVar.Is<void>());
    LLBC_Expect(constTestVar.Size() == 0 && constTestVar.Is<void>());
    LLBC_Expect(constTestVar.Capacity() == 0 && constTestVar.Is<void>());
    LLBC_Expect(constTestVar.Capacity() == 0 && constTestVar.Is<void>());
    LLBC_Expect(!constTestVar.Contains(333) && constTestVar.Is<void>());
    LLBC_Expect(constTestVar.Count(333) == 0 && constTestVar.Is<void>());

    // Empty sequence test.
    LLBC_Variant emptySeqVar;
    const LLBC_Variant &constEmptySeqVar = emptySeqVar;
    emptySeqVar.Become<LLBC_Variant::Seq>();
    LLBC_Expect(emptySeqVar.Is<LLBC_Variant::Seq>() &&
                emptySeqVar.IsEmpty() &&
                emptySeqVar.Size() == 0);
    LLBC_Expect(emptySeqVar.SeqBegin() == emptySeqVar.SeqEnd() &&
                emptySeqVar.SeqEnd() - emptySeqVar.SeqBegin() == 0 &&
                emptySeqVar.SeqReverseBegin() == emptySeqVar.SeqReverseEnd() &&
                emptySeqVar.SeqReverseEnd() - emptySeqVar.SeqReverseBegin() == 0);
    LLBC_Expect(constEmptySeqVar.SeqBegin() == constEmptySeqVar.SeqEnd() &&
                constEmptySeqVar.SeqEnd() - constEmptySeqVar.SeqBegin() == 0 &&
                constEmptySeqVar.SeqReverseBegin() == constEmptySeqVar.SeqReverseEnd() &&
                constEmptySeqVar.SeqReverseEnd() - constEmptySeqVar.SeqReverseBegin() == 0);
    LLBC_Expect(emptySeqVar.Count(333) == 0 && !emptySeqVar.Contains(333));

    // SeqPushBack/SeqBatchPushBack test:
    testVar.Become<LLBC_Variant::Seq>();
    testVar.Clear();
    testVar.SeqPushBack(1);
    LLBC_Expect(testVar.Size() == 1 && testVar[0] == 1);
    testVar.SeqBatchPushBack(false, 3.1415926, "Hello World");
    LLBC_Expect(testVar.Size() == 4 &&
                testVar[0] == 1 &&
                testVar[1] == false &&
                testVar[2] == 3.1415926 &&
                testVar[3] == "Hello World");

    testVar.Clear();
    LLBC_String strElem("Hello World");
    testVar.SeqPushBack(std::move(strElem));
    LLBC_Expect(testVar[0] == "Hello World" && strElem.empty());

    strElem = "Hello World";
    std::vector<int> vecElem{1, 2, 3};
    const auto copyVecElem = vecElem;

    LLBC_String strElem2("Hey Judy");
    testVar.Clear();
    testVar.SeqBatchPushBack(std::move(strElem), std::move(vecElem), strElem2);
    LLBC_Expect(testVar.Size() == 3 &&
                testVar[0] == "Hello World" &&
                testVar[1] == copyVecElem &&
                testVar[2] == "Hey Judy" &&
                strElem.empty() &&
                vecElem.empty() &&
                strElem2 == "Hey Judy",
                "testVar:%s", testVar.ToString().c_str());

    // SeqInsert(SeqIter it, _Ty &&val):
    testVar.Clear();
    strElem = "Hello World";
    testVar.SeqInsert(testVar.SeqBegin(), true);
    testVar.SeqInsert(testVar.SeqEnd(), strElem);
    LLBC_Expect(testVar.Size() == 2 &&
                testVar[0] == true &&
                testVar[1] == "Hello World" &&
                strElem == "Hello World",
                "testVar:%s", testVar.ToString().c_str());

    testVar.SeqInsert(testVar.SeqBegin() + 1, std::move(strElem));
    LLBC_Expect(testVar[1] == testVar[2] &&
                strElem.empty());

    // SeqInsert(SeqIter it, size_t n, const _Ty &val):
    testVar.Clear();
    testVar.SeqInsert(testVar.SeqBegin(), 10, 1);
    LLBC_Expect(testVar.Size() == 10 &&
                std::all_of(testVar.SeqBegin(),
                            testVar.SeqEnd(),
                            [](const auto &elem) { return elem == 1; }));
    testVar.SeqInsert(testVar.SeqBegin() + 5, 10, 2);
    LLBC_Expect(testVar.Size() == 20 &&
                std::all_of(testVar.SeqBegin() + 5,
                            testVar.SeqBegin() + 15,
                            [](const auto &elem) {return elem == 2; }));
    testVar.SeqInsert(testVar.SeqEnd(), 10, 3);
    LLBC_Expect(testVar.Size() == 30 &&
                std::all_of(testVar.SeqBegin() + 20,
                            testVar.SeqEnd(),
                            [](const auto &elem) {return elem == 3; }));

    // SeqInsert(SeqIter it, SeqConstIter begin, SeqConstIter end):
    LLBC_Variant anotherSeq;
    anotherSeq.SeqInsert(anotherSeq.SeqBegin(), 10, 1);
    testVar.SeqInsert(testVar.SeqEnd(), anotherSeq.SeqBegin(), anotherSeq.SeqEnd());
    LLBC_Expect(testVar.Size() == 40 &&
                std::all_of(testVar.SeqBegin() + 30,
                            testVar.SeqEnd(),
                            [](const auto &elem) { return elem == 1; }));

    // SeqBatchInsert(SeqIter it, ...):
    testVar.Clear();
    testVar.SeqBatchInsert(testVar.SeqBegin(), true, 3.1415926, "Hello World");
    LLBC_Expect(testVar.Size() == 3 &&
                testVar[0] == true &&
                testVar[1] == 3.1415926 &&
                testVar[2] == "Hello World",
                "testVar:%s", testVar.ToString().c_str());

    // SeqFront()/SeqBack():
    LLBC_Expect(testVar.SeqFront() == true &&
                testVar.SeqBack() == "Hello World",
                "testVar:%s", testVar.ToString().c_str());

    // SeqPopBack():
    testVar.SeqPopBack();
    LLBC_Expect(testVar.Size() == 2 && testVar.SeqBack() == 3.1415926);

    // SeqErase(SeqIter):
    testVar.Clear();
    testVar.SeqBatchPushBack(1, 2, 3, true, false);
    LLBC_Expect(testVar.SeqErase(testVar.SeqBegin()) == testVar.SeqBegin() &&
                testVar.Size() == 4 &&
                testVar[0] == 2);
    testVar.SeqInsert(testVar.SeqBegin(), 1);
    LLBC_Expect(testVar.SeqErase(static_cast<const LLBC_Variant &>(testVar).SeqBegin() + 1) ==
                    static_cast<const LLBC_Variant &>(testVar).SeqBegin() + 1 &&
                testVar.Size() == 4 &&
                testVar[0] == 1 &&
                testVar[1] == 3);
    testVar.SeqInsert(testVar.SeqBegin() + 1, 2);
    auto seqEraseRet = testVar.SeqErase(testVar.SeqEnd() - 1);
    LLBC_Expect(seqEraseRet == testVar.SeqEnd() &&
                testVar.Size() == 4 &&
                testVar[0] == 1 &&
                testVar[1] == 2 &&
                testVar[2] == 3 &&
                testVar[3] == true);

    // SeqErase(SeqIter begin, SeqIter end):
    testVar.Clear();
    for (int i = 0; i < 10; ++i)
        testVar.SeqPushBack(i);
    seqEraseRet = testVar.SeqErase(testVar.SeqBegin(), testVar.SeqBegin() + 5);
    LLBC_Expect(seqEraseRet == testVar.SeqBegin() &&
                testVar.Size() == 5 &&
                testVar[0] == 5,
                "testVar:%s, testVar.Size():%lu, testVar[0]:%s, seqEraseRet == testVar.SeqBegin()?:%s",
                testVar.ToString().c_str(),
                testVar.Size(),
                testVar[0].ToString().c_str(),
                seqEraseRet == testVar.SeqBegin() ? "true" : "false");
    seqEraseRet = testVar.SeqErase(static_cast<const LLBC_Variant &>(testVar).SeqBegin(),
                                   static_cast<const LLBC_Variant &>(testVar).SeqBegin() + 5);
    LLBC_Expect(seqEraseRet == static_cast<const LLBC_Variant &>(testVar).SeqBegin() &&
                testVar.Size() == 0);

    // SeqErase(val):
    testVar.Clear();
    for (int i = 0; i < 3; ++i)
    {
        for (int i = 0; i < 10; ++i)
            testVar.SeqPushBack(i);
    }

    // [0] 1 2 3 4 5 6 7 8 9 | [0] 1 2 3 4 5 6 7 8 9 | [0] 1 2 3 4 5 6 7 8 9
    size_t valEraseRet = testVar.SeqErase(0);
    LLBC_Expect(valEraseRet == 3 &&
                testVar.Size() == 27 &&
                testVar[0] == 1 &&
                testVar.Count(0) == 0,
                "testVar:%s",
                testVar.ToString().c_str());

    // [1] 2 3 4 5 6 7 8 9 | 1 2 3 4 5 6 7 8 9 | 1 2 3 4 5 6 7 8 9
    valEraseRet = testVar.SeqErase(1, /*eraseCount = */1);
    LLBC_Expect(valEraseRet == 1 &&
                testVar.Size() == 26 &&
                testVar[0] == 2 &&
                testVar.Count(1) == 2,
                "testVar:%s",
                testVar.ToString().c_str());
    
    // 2 3 4 5 6 7 8 9 | 1 2 3 4 5 6 7 8 9 | 1 [2] 3 4 5 6 7 8 9
    valEraseRet = testVar.SeqErase(2, /*eraseCount = */1, /*fromBegin = */false);
    LLBC_Expect(valEraseRet == 1 &&
                testVar.Size() == 25 &&
                testVar[8] == 1 &&
                testVar[16] == 9 &&
                testVar[18] == 3 &&
                testVar.Count(2) == 2,
                "testVar:%s",
                testVar.ToString().c_str());

    // 2 [3] 4 5 6 7 8 9 | 1 2 [3] 4 5 6 7 8 9 | 1 [3] 4 5 6 7 8 9
    valEraseRet = testVar.SeqErase(3, 100);
    LLBC_Expect(valEraseRet == 3 &&
                testVar.Size() == 22 &&
                testVar[7] == 1 &&
                testVar[14] == 9 &&
                testVar[16] == 4 &&
                testVar.Count(3) == 0,
                "testVar:%s",
                testVar.ToString().c_str());

    // 2 [4] 5 6 7 8 9 | 1 2 [4] 5 6 7 8 9 | 1 [4] 5 6 7 8 9
    valEraseRet = testVar.SeqEraseIf([](const auto &elem) { return elem == 4; });
    LLBC_Expect(valEraseRet == 3 &&
                testVar.Size() == 19 &&
                testVar[6] == 1 &&
                testVar[12] == 9 &&
                testVar[14] == 5 &&
                testVar.Count(4) == 0,
                "testVar:%s",
                testVar.ToString().c_str());

    return LLBC_OK;
}

int TestCase_Core_Variant::DictSpecificTest()
{
    LLBC_PrintLn("Dictionary specific test:");

    return LLBC_OK;
}