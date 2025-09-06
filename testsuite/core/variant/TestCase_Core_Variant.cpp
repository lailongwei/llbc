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


#include "core/variant/TestCase_Core_Variant.h"

int TestCase_Core_Variant::Run(int argc, char *argv[])
{
    LLBC_Expect(BasicTest() == LLBC_OK);
    LLBC_Expect(BecomeTest() == LLBC_OK);
    LLBC_Expect(StringTest() == LLBC_OK);
    LLBC_Expect(EnumTest() == LLBC_OK);
    LLBC_Expect(CompareTest() == LLBC_OK);
    LLBC_Expect(CompositeKeyTest() == LLBC_OK);
    LLBC_Expect(ArithmeticTest() == LLBC_OK);
    LLBC_Expect(PairTest() == LLBC_OK);
    LLBC_Expect(SeqTest() == LLBC_OK);
    LLBC_Expect(DictTest() == LLBC_OK);
    LLBC_Expect(SerializeTest() == LLBC_OK);
    LLBC_Expect(HashTest() == LLBC_OK);
    LLBC_Expect(ObjTest() == LLBC_OK);

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    return 0;
}

int TestCase_Core_Variant::BasicTest()
{
    LLBC_PrintLn("LLBC Variant Basic test: ");

    // Nil value test.
    LLBC_Variant nilVal;
    LLBC_Expect(nilVal.IsNil() == true);

    // Bool value test.
    LLBC_Variant boolVal1(true);
    LLBC_Expect(boolVal1.IsBool() == true && boolVal1 == true);

    LLBC_Variant boolVal2(false);
    LLBC_Expect(boolVal2.IsBool() == true && boolVal2 == false);

    // Sint8 value test.
    LLBC_Variant int8Val1(std::numeric_limits<sint8>::min());
    LLBC_Expect(int8Val1.IsInt8() == true && int8Val1 == std::numeric_limits<sint8>::min());

    LLBC_Variant int8Val2(std::numeric_limits<sint8>::max());
    LLBC_Expect(int8Val2.IsInt8() == true && int8Val2 == std::numeric_limits<sint8>::max());

    // Uint8 value test.
    LLBC_Variant uint8Val1(std::numeric_limits<uint8>::min());
    LLBC_Expect(uint8Val1.IsUInt8() == true && uint8Val1 == std::numeric_limits<uint8>::min());

    LLBC_Variant uint8Val2(std::numeric_limits<uint8>::max());
    LLBC_Expect(uint8Val2.IsUInt8() == true && uint8Val2 == std::numeric_limits<uint8>::max());

    // Sint16 value test.
    LLBC_Variant int16Val1(std::numeric_limits<sint16>::min());
    LLBC_Expect(int16Val1.IsInt16() == true && int16Val1 == std::numeric_limits<sint16>::min());

    LLBC_Variant int16Val2(std::numeric_limits<sint16>::max());
    LLBC_Expect(int16Val2.IsInt16() == true && int16Val2 == std::numeric_limits<sint16>::max());

    // Uint16 value test.
    LLBC_Variant uint16Val1(std::numeric_limits<uint16>::min());
    LLBC_Expect(uint16Val1.IsUInt16() == true && uint16Val1 == std::numeric_limits<uint16>::min());

    LLBC_Variant uint16Val2(std::numeric_limits<uint16>::max());
    LLBC_Expect(uint16Val2.IsUInt16() == true && uint16Val2 == std::numeric_limits<uint16>::max());

    // Int32 value test.
    LLBC_Variant int32Val1(std::numeric_limits<sint32>::min());
    LLBC_Expect(int32Val1.IsInt32() == true && int32Val1 == std::numeric_limits<sint32>::min());

    LLBC_Variant int32Val2(std::numeric_limits<sint32>::max());
    LLBC_Expect(int32Val2.IsInt32() == true && int32Val2 == std::numeric_limits<sint32>::max());

    // Int64 value test.
    LLBC_Variant int64Val1(std::numeric_limits<sint64>::min());
    LLBC_Expect(int64Val1.IsInt64() == true && int64Val1 == std::numeric_limits<sint64>::min());

    LLBC_Variant int64Val2(std::numeric_limits<sint64>::max());
    LLBC_Expect(int64Val2.IsInt64() == true && int64Val2 == std::numeric_limits<sint64>::max());

    // Uint64 value test.
    LLBC_Variant uint64Val1(std::numeric_limits<uint64>::min());
    LLBC_Expect(uint64Val1.IsUInt64() == true && uint64Val1 == std::numeric_limits<uint64>::min());

    LLBC_Variant uint64Val2(std::numeric_limits<uint64>::max());
    LLBC_Expect(uint64Val2.IsUInt64() == true && uint64Val2 == std::numeric_limits<uint64>::max());

    // Float value test.
    LLBC_Variant floatVal1(std::numeric_limits<float>::min());
    LLBC_Expect(floatVal1.IsFloat() == true && floatVal1 == std::numeric_limits<float>::min());

    LLBC_Variant floatVal2(std::numeric_limits<float>::max());
    LLBC_Expect(floatVal2.IsFloat() == true && floatVal2 == std::numeric_limits<float>::max());

    // Double value test.
    LLBC_Variant doubleVal1(std::numeric_limits<double>::min());
    LLBC_Expect(doubleVal1.IsDouble() == true && doubleVal1 == std::numeric_limits<double>::min());

    LLBC_Variant doubleVal2(std::numeric_limits<double>::max());
    LLBC_Expect(doubleVal2.IsDouble() == true && doubleVal2 == std::numeric_limits<double>::max());

    // Pointer value test.
    void *ptr = reinterpret_cast<void *>(0x12345678ABCDEF00);
    LLBC_Variant ptrVal(ptr);
    LLBC_Expect(ptrVal.IsPtr() == true && ptrVal == ptr);

    // String value test.
    LLBC_Variant strVal("Hello LLBC Variant");
    LLBC_Expect(strVal.IsStr() == true && strVal == "Hello LLBC Variant");

    LLBC_PrintLn("passed");
    return LLBC_OK;

    int intData = 300;
    ptrVal = &intData;
    std::cout << "Pointer type(special) test:" << std::endl;
    std::cout << "intData: " << intData << ", save ptr(0x" << &intData << ") to variant: " << ptrVal << std::endl;

    ptrVal = ptrVal + LLBC_Variant(4);
    ptrVal = ptrVal - LLBC_Variant(4);
    std::cout << "After ptrVal += 4, ptrVal -= 4, value: " << *ptrVal.AsPtr<int>() << std::endl;

    std::cout << "Implicit [const ]char * cast test:" << std::endl;
    strVal = "hello world";
    std::cout << "- After strVal = \"hello world\":" << strVal << std::endl;
    std::cout << "- strVal ? strVal : \"hello\": " << (strVal ? strVal : "hello") << std::endl;

    char mutableHeyStr[4] = { 'H', 'e' , 'y', '\0' };
    strVal = mutableHeyStr;
    std::cout << " - After strVal = mutableHeyStr(Hey):" << strVal << std::endl;
    std::cout << "- strVal ? strVal : mutableHeyStr(Hey): " << (strVal ? strVal : mutableHeyStr) << std::endl;

    LLBC_Variant constructFromCStr("hello world");
    LLBC_Variant constructFromMStr(mutableHeyStr);
    std::cout << "Construct from mutable/immutable test:" << std::endl;
    std::cout << "- construct from cstr:\"hello world\":" << constructFromCStr <<std::endl;
    std::cout << "- construct from mstr:\"" << mutableHeyStr << "\":" << constructFromMStr <<std::endl;
}

int TestCase_Core_Variant::BecomeTest()
{
    LLBC_PrintLn("LLBC Variant Become test: ");

    LLBC_Variant variant;

    // Nil become test.
    variant.BecomeNil();
    LLBC_Expect(variant.IsNil() == true);

    // Bool become test.
    variant.BecomeBool();
    LLBC_Expect(variant.IsBool() == true && variant == false);

    // Sint8 become test.
    variant.BecomeInt8();
    LLBC_Expect(variant.IsInt8() == true && variant == 0);

    // Uint8 become test.
    variant.BecomeUInt8();
    LLBC_Expect(variant.IsUInt8() == true && variant == 0);

    // Sint16 become test.
    variant.BecomeInt16();
    LLBC_Expect(variant.IsInt16() == true && variant == 0);

    // Uint16 become test.
    variant.BecomeUInt16();
    LLBC_Expect(variant.IsUInt16() == true && variant == 0);

    // Sint32 become test.
    variant.BecomeInt32();
    LLBC_Expect(variant.IsInt32() == true && variant == 0);

    // Uint32 become test.
    variant.BecomeUInt32();
    LLBC_Expect(variant.IsUInt32() == true && variant == 0);

    // Sint64 become test.
    variant.BecomeInt64();
    LLBC_Expect(variant.IsInt64() == true && variant == 0);

    // Uint64 become test.
    variant.BecomeUInt64();
    LLBC_Expect(variant.IsUInt64() == true && variant == 0);

    // Float become test.
    variant.BecomeFloat();
    LLBC_Expect(variant.IsFloat() == true && variant == 0.0f);

    // Double become test.
    variant.BecomeDouble();
    LLBC_Expect(variant.IsDouble() == true && variant == 0.0);

    // Pointer become test.
    variant.BecomePtr();
    LLBC_Expect(variant.IsPtr() == true && variant == nullptr);

    // String become test.
    variant.BecomeStr();
    LLBC_Expect(variant.IsStr() == true && variant == "");

    LLBC_PrintLn("passed");
    return LLBC_OK;
}

int TestCase_Core_Variant::StringTest()
{
    LLBC_PrintLn("LLBC Variant String test: ");

    // Construct test.
    LLBC_Variant v1("Hello world");
    LLBC_Expect(v1.IsStr() == true && v1 == "Hello world");

    LLBC_Variant v2(std::string("Hello world"));
    LLBC_Expect(v2.IsStr() == true && v2 == "Hello world");

    LLBC_Variant v3(LLBC_String("Hello world"));
    LLBC_Expect(v3.IsStr() == true && v3 == "Hello world");

    LLBC_Variant v4(LLBC_CString("Hello world"));
    LLBC_Expect(v4.IsStr() == true && v4 == "Hello world");

    // Assignment operator test.
    LLBC_Variant v5;
    v5 = "Hello world";
    LLBC_Expect(v5.IsStr() == true && v5 == "Hello world");

    LLBC_Variant v6;
    v6 = std::string("Hello world");
    LLBC_Expect(v6.IsStr() == true && v6 == "Hello world");

    LLBC_Variant v7;
    v7 = LLBC_String("Hello world");
    LLBC_Expect(v7.IsStr() == true && v7 == "Hello world");

    LLBC_Variant v8;
    v8 = LLBC_CString("Hello world");
    LLBC_Expect(v8.IsStr() == true && v8 == "Hello world");

    // Copy construct test.
    LLBC_Variant v9(v1);
    LLBC_Expect(v9.IsStr() == true && v9 == "Hello world");

    // Copy assignment test.
    LLBC_Variant v10;
    v10 = v1;
    LLBC_Expect(v10.IsStr() == true && v10 == "Hello world");

    return LLBC_OK;
}

int TestCase_Core_Variant::EnumTest()
{
    LLBC_PrintLn("LLBC Variant Enum test: ");

    enum TraditionalStyleEnum
    {
        A = 3,
        B = 4,
        C = 5
    };

    // Traditional style enum test.
    // Traditional style enum construct test.
    LLBC_Variant tse1(TraditionalStyleEnum::A);
    LLBC_Expect(tse1 == TraditionalStyleEnum::A);

    // Traditional style enum AsEnum<> test.
    LLBC_Variant tse2(4);
    LLBC_Expect(tse2.AsEnum<TraditionalStyleEnum>() == TraditionalStyleEnum::B);

    // Traditional style enum operator<Enum> test.
    TraditionalStyleEnum tse3 = tse1;
    LLBC_Expect(tse3 == TraditionalStyleEnum::A);

    // Traditional style enum operator=(Enum) test.
    LLBC_Variant tse4;
    tse4 = TraditionalStyleEnum::C;
    LLBC_Expect(tse4 == TraditionalStyleEnum::C);


    enum class ClassStyleEnum
    {
        X = -3,
        Y = -2,
        Z = -1
    };

    // Class style enum test.
    // Class style enum construct test.
    LLBC_Variant cste1(ClassStyleEnum::X);
    LLBC_Expect(cste1 == ClassStyleEnum::X);

    // Class style enum AsEnum<> test.
    LLBC_Variant cste2(-2);
    LLBC_Expect(cste2.AsEnum<ClassStyleEnum>() == ClassStyleEnum::Y);

    // Class style enum operator<Enum> test.
    ClassStyleEnum cste3 = cste1;
    LLBC_Expect(cste3 == ClassStyleEnum::X);

    // Class style enum operator=(Enum) test.
    LLBC_Variant cste4;
    cste4 = ClassStyleEnum::Z;
    LLBC_Expect(cste4 == ClassStyleEnum::Z);

    return LLBC_OK;
}

int TestCase_Core_Variant::CompareTest()
{
    LLBC_PrintLn("LLBC Variant Compare test: ");

    // Nil compare test.
    LLBC_Variant nil;
    LLBC_Expect((nil < nil) == false);
    LLBC_Expect((nil > nil) == false);
    LLBC_Expect((nil == nil) == true);

    // Bool compare test.
    LLBC_Variant boolTrue(true);
    LLBC_Variant boolFalse(false);

    LLBC_Expect((boolTrue < boolTrue) == false);
    LLBC_Expect((boolTrue > boolTrue) == false);
    LLBC_Expect((boolTrue == boolTrue) == true);

    LLBC_Expect((boolFalse < boolFalse) == false);
    LLBC_Expect((boolFalse > boolFalse) == false);
    LLBC_Expect((boolFalse == boolFalse) == true);

    LLBC_Expect((boolTrue < boolFalse) == false);
    LLBC_Expect((boolTrue > boolFalse) == true);
    LLBC_Expect((boolTrue == boolFalse) == false);
    LLBC_Expect((boolFalse < boolTrue) == true);
    LLBC_Expect((boolFalse > boolTrue) == false);
    LLBC_Expect((boolFalse == boolTrue) == false);

    // Int compare test.
    LLBC_Variant int0(0);
    LLBC_Variant int1(1);
    LLBC_Variant int2(-1);

    LLBC_Expect((int0 < int0) == false);
    LLBC_Expect((int0 > int0) == false);
    LLBC_Expect((int0 == int0) == true);

    LLBC_Expect((int1 < int1) == false);
    LLBC_Expect((int1 > int1) == false);
    LLBC_Expect((int1 == int1) == true);

    LLBC_Expect((int2 < int2) == false);
    LLBC_Expect((int2 > int2) == false);
    LLBC_Expect((int2 == int2) == true);

    LLBC_Expect((int0 < int1) == true);
    LLBC_Expect((int0 > int1) == false);
    LLBC_Expect((int0 == int1) == false);

    LLBC_Expect((int1 < int0) == false);
    LLBC_Expect((int1 > int0) == true);
    LLBC_Expect((int1 == int0) == false);

    LLBC_Expect((int2 < int0) == true);
    LLBC_Expect((int2 > int0) == false);
    LLBC_Expect((int2 == int0) == false);

    LLBC_Expect((int0 < int2) == false);
    LLBC_Expect((int0 > int2) == true);
    LLBC_Expect((int0 == int2) == false);

    // Double compare test.
    LLBC_Variant dbl10010(10010.0);
    LLBC_Variant dbl10086(10086.0);

    LLBC_Expect((dbl10010 < dbl10010) == false);
    LLBC_Expect((dbl10010 > dbl10010) == false);
    LLBC_Expect((dbl10010 == dbl10010) == true);

    LLBC_Expect((dbl10010 < dbl10086) == true);
    LLBC_Expect((dbl10010 > dbl10086) == false);
    LLBC_Expect((dbl10010 == dbl10086) == false);

    LLBC_Expect((dbl10086 < dbl10010) == false);
    LLBC_Expect((dbl10086 > dbl10010) == true);
    LLBC_Expect((dbl10086 == dbl10010) == false);

    // String compare test.
    LLBC_Variant str1_FromStringLiteral("Hello world(string literal)");
    LLBC_Variant str1_FromStlString(std::string("Hello world(stl string)"));
    LLBC_Variant str1_FromLLBCString(LLBC_String("Hello world(llbc string)"));

    LLBC_Expect((str1_FromStringLiteral < str1_FromStringLiteral) == false);
    LLBC_Expect((str1_FromStringLiteral > str1_FromStringLiteral) == false);
    LLBC_Expect((str1_FromStringLiteral == str1_FromStringLiteral) == true);
    LLBC_Expect((str1_FromStringLiteral < str1_FromStlString) == false);
    LLBC_Expect((str1_FromStringLiteral > str1_FromStlString) == true);
    LLBC_Expect((str1_FromStringLiteral == str1_FromStlString) == false);
    LLBC_Expect((str1_FromStlString < str1_FromStringLiteral) == true);
    LLBC_Expect((str1_FromStlString > str1_FromStringLiteral) == false);
    LLBC_Expect((str1_FromStlString == str1_FromStringLiteral) == false);

    // Sequence compare test.
    LLBC_Variant seq1;
    LLBC_Variant seq2;
    LLBC_Variant seq2Clone;
    seq1.SeqPushBack(LLBC_Variant::nil);
    seq2.SeqPushBack(1);
    seq2Clone = seq2;
    LLBC_Expect((seq1 < seq1) == false);
    LLBC_Expect((seq1 > seq1) == false);
    LLBC_Expect((seq1 == seq1) == true);

    LLBC_Expect((seq1 < seq2) == true);
    LLBC_Expect((seq1 > seq2) == false);
    LLBC_Expect((seq1 == seq2) == false);

    LLBC_Expect((seq2 < seq2Clone) == false);
    LLBC_Expect((seq2 > seq2Clone) == false);
    LLBC_Expect((seq2 == seq2Clone) == true);

    // Dictionary compare test.
    LLBC_Variant dict1;
    LLBC_Variant dict2;
    dict1["Key1"] = "Value1";
    dict2["Key1"] = "Value2";

    LLBC_Expect((dict1 < dict1) == false);
    LLBC_Expect((dict1 > dict1) == false);
    LLBC_Expect((dict1 == dict1) == true);

    LLBC_Expect((dict1 < dict2) == true);
    LLBC_Expect((dict1 > dict2) == false);
    LLBC_Expect((dict1 == dict2) == false);

    return LLBC_OK;
}

int TestCase_Core_Variant::CompositeKeyTest()
{
    LLBC_PrintLn("LLBC Variant Composite Key test: ");

    // Composite key test.
    LLBC_Variant v;
    v["The str key 1"] = "The str value 1";
    LLBC_Expect(v["The str key 1"] == "The str value 1");

    v[100] = 100;
    LLBC_Expect(v[100] == 100);

    v[-100] = -100;
    LLBC_Expect(v[-100] == -100);

    v[true] = true;
    LLBC_Expect(v[true] == true);

    v[false] = false;
    LLBC_Expect(v[false] == false);

    v[LLBC_Variant::nil] = LLBC_Variant::nil;
    LLBC_Expect(v[LLBC_Variant::nil] == LLBC_Variant::nil);

    v[LLBC_Variant::nil] = "The nil value";
    LLBC_Expect(v[LLBC_Variant::nil] == "The nil value");

    // Nested structure test: sequence inside dict and dict inside dict.
    LLBC_Variant nested;
    nested["inner_seq"].BecomeSeq();
    nested["inner_seq"].SeqPushBack(1);
    nested["inner_seq"].SeqPushBack("two");
    nested["inner_seq"].SeqPushBack(false);

    nested["inner_dict"]["subkey"] = 3.14;
    nested["inner_dict"]["subdict"]["deep"] = "deep value";

    // Composite key built from a variant sequence.
    LLBC_Variant complexKey;
    complexKey.SeqPushBack("k1");
    complexKey.SeqPushBack(42);
    complexKey.SeqPushBack(LLBC_Variant::nil);

    v[complexKey] = nested;

    // Verify nested contents.
    LLBC_Expect(v[complexKey]["inner_seq"][0] == 1);
    LLBC_Expect(v[complexKey]["inner_seq"][1] == "two");
    LLBC_Expect(v[complexKey]["inner_seq"][2] == false);

    LLBC_Expect(v[complexKey]["inner_dict"]["subkey"] == 3.14);
    LLBC_Expect(v[complexKey]["inner_dict"]["subdict"]["deep"] == "deep value");

    // Copy / assignment of nested variant.
    LLBC_Variant recovered = v[complexKey];
    LLBC_Expect(recovered.IsDict());
    LLBC_Expect(recovered["inner_seq"].Size() == 3);
    LLBC_Expect(recovered["inner_dict"]["subdict"]["deep"] == "deep value");

    // Use a dict (with nested seq) as a key.
    LLBC_Variant dictKey;
    dictKey["a"] = 1;
    dictKey["b"].BecomeSeq();
    dictKey["b"].SeqPushBack("x");
    dictKey["b"].SeqPushBack("y");

    v[dictKey] = "mixed key value";
    LLBC_Expect(v[dictKey] == "mixed key value");

    return LLBC_OK;
}

int TestCase_Core_Variant::ArithmeticTest()
{
    LLBC_PrintLn("LLBC Variant Arithmetic test: ");

    #if LLBC_CUR_COMP == LLBC_COMP_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Woverflow"
    #endif

    constexpr auto leftValue1 = std::numeric_limits<sint64>::min();
    LLBC_Variant left1(leftValue1);

    constexpr auto rightValue1 = std::numeric_limits<sint64>::max();
    LLBC_Variant right1(rightValue1);

    LLBC_Expect(left1 + right1 == leftValue1 + rightValue1);
    LLBC_Expect(right1 + left1 == rightValue1 + leftValue1);
    LLBC_Expect(left1 - right1 == leftValue1 - rightValue1);
    LLBC_Expect(right1 - left1 == rightValue1 - leftValue1);
    LLBC_Expect(left1 * right1 == leftValue1 * rightValue1);
    LLBC_Expect(right1 * left1 == rightValue1 * leftValue1);
    LLBC_Expect(left1 / right1 == leftValue1 / rightValue1);
    LLBC_Expect(right1 / left1 == rightValue1 / leftValue1);
    LLBC_Expect(left1 % right1 == leftValue1 % rightValue1);
    LLBC_Expect(right1 % left1 == rightValue1 % leftValue1);

    constexpr auto leftValue2 = "hello, ";
    LLBC_Variant left2(leftValue2);
    constexpr auto rightValue2 = "world!";
    LLBC_Variant right2(rightValue2);

    LLBC_Expect(left2 + right2 == LLBC_String(leftValue2) + LLBC_String(rightValue2));
    LLBC_Expect(right2 + left2 == LLBC_String(rightValue2) + LLBC_String(leftValue2));

    #if LLBC_CUR_COMP == LLBC_COMP_GCC
    #pragma GCC diagnostic pop
    #endif

    return LLBC_OK;
}

int TestCase_Core_Variant::PairTest()
{
    LLBC_PrintLn("LLBC Variant Pair test: ");

    LLBC_Variant var1(std::make_pair(std::make_pair(1, "hello"), std::make_pair(2, "world")));
    LLBC_Expect(var1.IsSeq() && var1.Size() == 2);
    LLBC_Expect(LLBC_Variant(var1[0]) == std::make_pair(1, "hello"));
    LLBC_Expect(LLBC_Variant(var1[1]) == std::make_pair(2, "world"));

    std::pair<std::vector<int>, std::map<int, int>> var2;
    for (int i = 0; i < 10; ++i)
    {
        var2.first.push_back(i);
        var2.second.emplace(i, i * 100);
    }
    LLBC_Variant var3(var2);
    LLBC_Expect(var3.IsSeq() && var3.Size() == 2);
    LLBC_Expect(LLBC_Variant(var3[0]).IsSeq() && LLBC_Variant(var3[0]).Size() == 10);
    for (int i = 0; i < 10; ++i)
        LLBC_Expect(LLBC_Variant(var3[0])[i] == i);
    LLBC_Expect(LLBC_Variant(var3[1]).IsDict() && LLBC_Variant(var3[1]).Size() == 10);
    for (int i = 0; i < 10; ++i)
        LLBC_Expect(LLBC_Variant(var3[1])[i] == i * 100);
    LLBC_Expect(var3 == var2);

    return LLBC_OK;
}

int TestCase_Core_Variant::SeqTest()
{
    LLBC_PrintLn("LLBC Variant Basic test: ");

    LLBC_Variant subSeq;
    subSeq["Hello"] = 3;
    LLBC_Expect(subSeq["Hello"] == 3);

    subSeq[LLBC_Variant::nil] = "The nil key value";
    LLBC_Expect(subSeq[LLBC_Variant::nil] == "The nil key value");

    LLBC_Variant seq1;
    seq1.SeqPushBack(3);
    LLBC_Expect(seq1[0] == 3);

    seq1.SeqPushBack(false);
    LLBC_Expect(seq1[1] == false);

    seq1.SeqPushBack("Hello World");
    LLBC_Expect(seq1[2] == "Hello World");

    seq1.SeqPushBack(subSeq);
    LLBC_Expect(seq1[3]["Hello"] == 3);
    LLBC_Expect(seq1[3][LLBC_Variant::nil] == "The nil key value");
    LLBC_Expect(seq1.Size() == 4);

    // Non-Const sequence subscript access test.
    LLBC_Variant seqForSubscriptTest;
    for (int i = 0; i < 10; ++i)
        seqForSubscriptTest.SeqPushBack(i);
    LLBC_Expect(seqForSubscriptTest.Size() == 10);
    LLBC_Expect(seqForSubscriptTest[10086] == LLBC_Variant::nil);

    // Const sequence subscript access test.
    const LLBC_Variant constSeqForSubscriptTest;
    LLBC_Expect(constSeqForSubscriptTest.IsNil() && constSeqForSubscriptTest.Size() == 0);
    LLBC_Expect(constSeqForSubscriptTest[10086] == LLBC_Variant::nil);

    // Assignment test.
    LLBC_Variant seq2 = seq1;
    LLBC_Expect(seq2.Size() == seq1.Size());
    for (size_t i = 0; i < seq1.Size(); ++i)
        LLBC_Expect(seq2[i] == seq1[i]);
    LLBC_Expect(seq2 == seq1);

    // Arithmetic test.
    LLBC_Variant left;
    left.SeqPushBack(1);
    left.SeqPushBack(2);
    left.SeqPushBack(2);
    left.SeqPushBack(3);

    LLBC_Variant right;
    right.SeqPushBack(2);
    right.SeqPushBack(3);
    right.SeqPushBack(3);
    right.SeqPushBack(4);

    LLBC_Expect(left != right);
    LLBC_Expect(left + right == LLBC_Variant(std::vector{1, 2, 2, 3, 2, 3, 3, 4}));
    LLBC_Expect(left - right == LLBC_Variant(std::vector{1}));
    LLBC_Expect(left * right == LLBC_Variant(std::vector{2, 2, 3}));
    LLBC_Expect(left / right == LLBC_Variant(std::vector{1, 2, 2, 3}));
    LLBC_Expect(left % right == LLBC_Variant(std::vector{1, 2, 2, 3}));

    // Sequence + - * / non sequence test.
    LLBC_Variant seq10;
    seq10.SeqPushBack(10);
    seq10.SeqPushBack("Hello world");
    seq10.SeqPushBack(false);
    LLBC_Variant seq11 = seq10;
    seq11.SeqPushBack(10086);
    LLBC_Expect(seq10 + 10086 == seq11);

    seq11.SeqPopBack();
    seq11.SeqPushBack(LLBC_Variant::nil);
    LLBC_Expect(seq10 + LLBC_Variant::nil == seq11);

    seq11.SeqPopBack();
    seq11.SeqPushBack("hello world");
    LLBC_Expect(seq10 + "hello world" == seq11);

    // Sequence batch push back test.
    LLBC_Variant batchPushBackTestSeq;
    batchPushBackTestSeq.SeqPushBack(10, "Hello world", false, 5.8, LLBC_Variant::nil);
    seq11 = seq10;
    seq11.SeqPushBack(5.8);
    seq11.SeqPushBack(LLBC_Variant::nil);
    LLBC_Expect(batchPushBackTestSeq == seq11);

    return LLBC_OK;
}

int TestCase_Core_Variant::DictTest()
{
    LLBC_PrintLn("LLBC Variant Dictionary test: ");

    // Raw key type dictionary test.
    LLBC_Variant seqKey;
    seqKey = 3;
    seqKey = "Hello World";
    seqKey = LLBC_Variant::Dict();
    seqKey.SeqPushBack(3);
    seqKey.SeqPushBack(4);
    seqKey.SeqPushBack(false);
    seqKey.SeqPushBack("Hello World");
    LLBC_Expect(seqKey.IsSeq() && seqKey.Size() == 4);

    LLBC_Variant rawDict1;
    rawDict1.DictInsert(1, "Hello");
    rawDict1.DictInsert(2, "World");
    rawDict1.DictInsert(3, "Hey");
    rawDict1.DictInsert(4, "Judy");
    rawDict1.DictInsert(5.0, "Jack");
    rawDict1.DictInsert(false, 34);
    rawDict1[seqKey] = "The seq key value";
    LLBC_Expect(rawDict1.Size() == 7);
    LLBC_Expect(rawDict1[1] == "Hello");
    LLBC_Expect(rawDict1[2] == "World");
    LLBC_Expect(rawDict1[3] == "Hey");
    LLBC_Expect(rawDict1[4] == "Judy");
    LLBC_Expect(rawDict1[5.0] == "Jack");
    LLBC_Expect(rawDict1[false] == 34);
    LLBC_Expect(rawDict1[seqKey] == "The seq key value");

    LLBC_Variant rawDict2(rawDict1);
    LLBC_Expect(rawDict2.Size() == rawDict1.Size());
    for (auto it = rawDict1.DictBegin(); it != rawDict1.DictEnd(); ++it)
    {
        LLBC_Expect(rawDict2.DictFind(it->first) != rawDict2.DictEnd());
        LLBC_Expect(rawDict2.DictFind(it->first)->second == it->second);
    }
    LLBC_Expect(rawDict2 == rawDict1);

    // Dictionary collection operation test.
    LLBC_Variant set1;
    LLBC_Variant set2;
    set1[1] = true;
    set1[2] = true;
    set2[2] = true;
    set2[3] = true;
    LLBC_Variant setPlus;
    setPlus[1] = true;
    setPlus[2] = true;
    setPlus[3] = true;
    LLBC_Expect(set1 + set2 == setPlus);

    LLBC_Variant setMinus;
    setMinus[1] = true;
    LLBC_Expect(set1 - set2 == setMinus);

    LLBC_Variant setIntersect;
    setIntersect[2] = true;
    LLBC_Expect(set1 * set2 == setIntersect);

    LLBC_Variant setUnionDiff;
    setUnionDiff[1] = true;
    setUnionDiff[2] = true;
    LLBC_Expect(set1 / set2 == setUnionDiff);

    // Use dictionary as key(low-performance)
    LLBC_Variant lowPerfDict;
    lowPerfDict[rawDict1] = "Hello";
    LLBC_Expect(lowPerfDict[rawDict1] == "Hello");
    lowPerfDict[rawDict2] = "World";
    LLBC_Expect(lowPerfDict[rawDict2] == "World");

    // Batch erase test.
    LLBC_Variant batchEraseTestDict;
    batchEraseTestDict["a"] = 3;
    batchEraseTestDict[false] = "Hello world";
    batchEraseTestDict[true] = "Hey Judy";
    batchEraseTestDict[6] = true;
    batchEraseTestDict[5] = true;
    batchEraseTestDict.DictErase("a", false, LLBC_Variant(5));
    LLBC_Variant expectedAfterErase;
    expectedAfterErase[true] = "Hey Judy";
    expectedAfterErase[6] = true;
    LLBC_Expect(batchEraseTestDict == expectedAfterErase);

    return LLBC_OK;
}

int TestCase_Core_Variant::ObjTest()
{
    LLBC_PrintLn("LLBC Variant Obj test: ");

    LLBC_Variant obj;
    obj.BecomeSeq();
    LLBC_Expect(obj.IsSeq());
    LLBC_Expect(obj.AsSeq() == LLBC_Variant::Seq());

    obj.BecomeBool();
    LLBC_Expect(obj.IsBool());
    LLBC_Expect(obj.IsDict() == false);
    LLBC_Expect(obj.IsStr() == false);
    LLBC_Expect(obj.IsSeq() == false);

    obj.BecomeSeq();
    obj.BecomeDict();
    obj.BecomeStr();
    obj.BecomeDict();
    obj.BecomeSeq();
    obj.BecomeStr();

    obj = std::vector<int>{1, 2, 3};
    LLBC_Expect(obj.IsSeq());
    LLBC_Expect(obj.IsDict() == false);
    LLBC_Expect(obj.IsStr() == false);
    LLBC_Expect(obj.AsSeq() == LLBC_Variant(std::vector<int>{1, 2, 3}).AsSeq());

    obj.BecomeBool();
    LLBC_Expect(obj.IsBool());
    LLBC_Expect(obj.IsDict() == false);
    LLBC_Expect(obj.IsStr() == false);
    LLBC_Expect(obj.IsSeq() == false);

    obj.BecomeSeq();
    obj.BecomeDict();
    obj.BecomeStr();
    obj.BecomeDict();
    obj.BecomeSeq();
    obj.BecomeStr();

    obj = "hello world";
    LLBC_Expect(obj.IsStr());
    LLBC_Expect(obj.AsStr() == "hello world");
    LLBC_Expect(obj.IsStr());
    LLBC_Expect(obj.IsSeq() == false);
    LLBC_Expect(obj.IsDict() == false);

    obj.BecomeBool();
    LLBC_Expect(obj.IsBool());
    LLBC_Expect(obj.IsDict() == false);
    LLBC_Expect(obj.IsStr() == false);
    LLBC_Expect(obj.IsSeq() == false);

    obj.BecomeSeq();
    obj.BecomeDict();
    obj.BecomeStr();
    obj.BecomeDict();
    obj.BecomeSeq();
    obj.BecomeStr();

    obj = std::map<int, int>{{1, 100}, {2, 200}, {3, 300}};
    LLBC_Expect(obj.IsDict());
    LLBC_Expect(obj.AsDict() == LLBC_Variant(std::map<int, int>{{1, 100}, {2, 200}, {3, 300}}).AsDict());
    LLBC_Expect(obj.IsDict());
    LLBC_Expect(obj.IsStr() == false);
    LLBC_Expect(obj.IsSeq() == false);

    return LLBC_OK;
}

int TestCase_Core_Variant::SerializeTest()
{
    LLBC_PrintLn("LLBC Variant Serialize test: ");

    // Serialize & Deserialize raw type.
    LLBC_Stream stream;
    LLBC_Variant raw(-64);
    stream << raw;
    LLBC_Expect(stream.GetWritePos() == 10);


    LLBC_Variant deserRaw;
    stream >> deserRaw;
    LLBC_Expect(deserRaw == raw);

    // Serialize & Deserialize string type.
    LLBC_Variant str("Hello World!");
    stream.SetReadPos(0);
    stream.SetWritePos(0);
    stream << str;
    LLBC_Expect(stream.GetWritePos() == 18);

    LLBC_Variant deserStr;
    stream >> deserStr;
    LLBC_Expect(deserStr == str);

    // Serialize & Deserialize seq type.
    LLBC_Variant seq;
    seq.SeqPushBack(1);
    seq.SeqPushBack(false);
    seq.SeqPushBack("Hello world");
    stream.SetReadPos(0);
    stream.SetWritePos(0);
    stream << seq;
    LLBC_Expect(stream.GetWritePos() == 43);
    std::cout << "Seq obj[" << seq << "] serialized size: " << stream.GetWritePos() << std::endl;

    LLBC_Variant deserSeq;
    stream >> deserSeq;
    LLBC_Expect(deserSeq == seq);

    // Serialize & Deserialize dict type.
    LLBC_Variant dict;
    dict[1] = "Hello World!";
    dict[2] = "Hey Judy!";
    stream.SetReadPos(0);
    stream.SetWritePos(0);
    stream << dict;
    LLBC_Expect(stream.GetWritePos() ==  59);

    LLBC_Variant deserDict;
    stream >> deserDict;
    LLBC_Expect(deserDict == dict);

    return LLBC_OK;
}

int TestCase_Core_Variant::HashTest()
{
    LLBC_PrintLn("LLBC Variant Hash test: ");

    LLBC_Expect(std::hash<LLBC_Variant>()(LLBC_Variant::nil) == std::hash<LLBC_Variant>()(LLBC_Variant::nil));
    LLBC_Expect(std::hash<LLBC_Variant>()(LLBC_Variant(true)) == std::hash<LLBC_Variant>()(LLBC_Variant(true)));
    LLBC_Expect(std::hash<LLBC_Variant>()(LLBC_Variant(false)) == std::hash<LLBC_Variant>()(LLBC_Variant(false)));
    LLBC_Expect(std::hash<LLBC_Variant>()(LLBC_Variant(0)) == std::hash<LLBC_Variant>()(LLBC_Variant(0)));
    LLBC_Expect(std::hash<LLBC_Variant>()(LLBC_Variant(1)) == std::hash<LLBC_Variant>()(LLBC_Variant(1)));
    LLBC_Expect(std::hash<LLBC_Variant>()(LLBC_Variant(2)) == std::hash<LLBC_Variant>()(LLBC_Variant(2)));
    LLBC_Expect(std::hash<LLBC_Variant>()(LLBC_Variant(-1)) == std::hash<LLBC_Variant>()(LLBC_Variant(-1)));
    LLBC_Expect(std::hash<LLBC_Variant>()(LLBC_Variant(-2)) == std::hash<LLBC_Variant>()(LLBC_Variant(-2)));
    LLBC_Expect(std::hash<LLBC_Variant>()(LLBC_Variant(3.1415926f)) == std::hash<LLBC_Variant>()(LLBC_Variant(3.1415926f)));
    LLBC_Expect(std::hash<LLBC_Variant>()(LLBC_Variant(6.18)) == std::hash<LLBC_Variant>()(LLBC_Variant(6.18)));
    LLBC_Expect(std::hash<LLBC_Variant>()(LLBC_Variant("Hello world")) == std::hash<LLBC_Variant>()(LLBC_Variant("Hello world")));
    LLBC_Expect(std::hash<LLBC_Variant>()(LLBC_Variant("")) == std::hash<LLBC_Variant>()(LLBC_Variant("")));

    LLBC_Variant seq;
    seq.SeqPushBack(LLBC_Variant(true));
    seq.SeqPushBack(LLBC_Variant(9527));
    seq.SeqPushBack(LLBC_Variant(3.1415926));
    seq.SeqPushBack(LLBC_Variant());
    seq[seq.Size() - 1].DictInsert("hello world", 6.18);
    LLBC_Variant seq2 = seq;
    LLBC_Expect(std::hash<LLBC_Variant>()(seq) == std::hash<LLBC_Variant>()(seq2));

    LLBC_Variant dict;
    dict["bool"] = true;
    dict["int"] = 9527;
    dict["double"] = 3.1415926;
    dict["obj"] = LLBC_Variant();
    dict["obj"]["hello world"] = 6.18;
    LLBC_Variant dict2 = dict;;
    LLBC_Expect(std::hash<LLBC_Variant>()(dict) == std::hash<LLBC_Variant>()(dict2));

    return LLBC_OK;
}