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


#include "core/variant/TestCase_Core_VariantTest.h"

int TestCase_Core_VariantTest::Run(int argc, char *argv[])
{
    std::cout << "LLBC_Variant test:" << std::endl;

    BasicTest();
    std::cout << std::endl;

    CompareTest();
    std::cout << std::endl;

    CompositeKeyTest();
    std::cout << std::endl;

    ArithmeticTest();
    std::cout << std::endl;

    SeqTest();
    std::cout << std::endl;

    DictTest();
    std::cout << std::endl;

    SerializeTest();
    std::cout << std::endl;

    HashTest();
    std::cout << std::endl;

    ConvertToUnurderedStlContainerTest();

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    return 0;
}

void TestCase_Core_VariantTest::BasicTest()
{
    std::cout << "Basic test:" << std::endl;

    LLBC_Variant nilVal;
    LLBC_Variant boolVal(true);
    LLBC_Variant int8Val((sint8)-8);
    LLBC_Variant uint8Val((uint8)8);
    LLBC_Variant int16Val((sint16)-16);
    LLBC_Variant uint16Val((uint16)16);
    LLBC_Variant int32Val((sint32)-32);
    LLBC_Variant uint32Val((uint32)32);
    LLBC_Variant longVal((long)-10086);
    LLBC_Variant ulongVal((LLBC_NS ulong)10086);
    uint64 ptrAddr = 0x80000000;
    void *ptr; memcpy(&ptr, &ptrAddr, sizeof(void *));
    LLBC_Variant ptrVal(ptr);
    LLBC_Variant int64Val((sint64)-64);
    LLBC_Variant uint64Val((uint64)64);
    LLBC_Variant floatVal((float)88.0);
    LLBC_Variant doubleVal((double)-88.0);
    LLBC_Variant strVal("Hello");
    LLBC_Variant looseBoolVal1("   true ");
    LLBC_Variant looseBoolVal2("   yes ");
    LLBC_Variant looseBoolVal3("   false ");
    LLBC_Variant looseBoolVal4("   -4 ");
    LLBC_Variant looseBoolVal5("   0 ");
    LLBC_Variant looseBoolVal6("   0.0 ");
    LLBC_Variant looseBoolVal7("   1.3 ");

    std::cout << "nil val, isNil: "
        << (nilVal.IsNil() ? "true" : "false") << ", ToString: " << nilVal << std::endl;
    std::cout << "bool val, isBool: "
        << (boolVal.IsBool() ? "true" : "false") << ", ToString: " << boolVal << std::endl;
    std::cout << "int8Val, isInt8: "
        << (int8Val.IsInt8() ? "true" : "false") << ", ToString: " << int8Val << std::endl;
    std::cout << "uint8Val, isUInt8: "
        << (uint8Val.IsUInt8() ? "true" : "false") << ", ToString: " << uint8Val << std::endl;
    std::cout << "int16Val, isInt16: "
        << (int16Val.IsInt16() ? "true" : "false") << ", ToString: " << int16Val << std::endl;
    std::cout << "uint16Val, isUInt16: "
        << (uint16Val.IsUInt16() ? "true" : "false") << ", ToString: " << uint16Val << std::endl;
    std::cout << "int32Val, isInt32: "
        << (int32Val.IsInt32() ? "true" : "false") << ", ToString: " << int32Val << std::endl;
    std::cout << "uint32Val, isUInt32: "
        << (uint32Val.IsUInt32() ? "true" : "false") << ", ToString: " << uint32Val << std::endl;
    std::cout << "longVal, isLong: "
        << (longVal.IsLong() ? "true" : "false") << ", ToString: " << longVal << std::endl;
    std::cout << "ulongVal, isULong: "
        << (ulongVal.IsULong() ? "true" : "false") << ", ToString: " << ulongVal << std::endl;
    std::cout << "ptrVal, isPtr: "
        << (ptrVal.IsPtr() ? "true" : "false") << ", ToString: " << ptrVal << std::endl;
    std::cout << "int64Val, isInt64: "
        << (int64Val.IsInt64() ? "true" : "false") << ", ToString: " << int64Val << std::endl;
    std::cout << "uint64Val, isUInt64: "
        << (uint64Val.IsUInt64() ? "true" : "false") << ", ToString: " << uint64Val << std::endl;
    std::cout << "floatVal, isFloat: "
        << (floatVal.IsFloat() ? "true" : "false") << ", ToString: " << floatVal << std::endl;
    std::cout << "doubleVal, isDouble: "
        << (doubleVal.IsDouble() ? "true" : "false") << ", ToString: " << doubleVal << std::endl;
    std::cout << "strVal, isStr: "
        << (strVal.IsStr() ? "true" : "false") << ", ToString: " << strVal << std::endl;
    std::cout << "looseBoolVal1: "
        << looseBoolVal1 << ", AsBool(): " << looseBoolVal1.AsLooseBool() << std::endl;
    std::cout << "looseBoolVal2: "
        << looseBoolVal2 << ", AsBool(): " << looseBoolVal2.AsLooseBool() << std::endl;
    std::cout << "looseBoolVal3: "
        << looseBoolVal3 << ", AsBool(): " << looseBoolVal3.AsLooseBool() << std::endl;
    std::cout << "looseBoolVal4: "
        << looseBoolVal4 << ", AsBool(): " << looseBoolVal4.AsLooseBool() << std::endl;
    std::cout << "looseBoolVal5: "
        << looseBoolVal5 << ", AsBool(): " << looseBoolVal5.AsLooseBool() << std::endl;
    std::cout << "looseBoolVal6: "
        << looseBoolVal6 << ", AsBool(): " << looseBoolVal6.AsLooseBool() << std::endl;
    std::cout << "looseBoolVal7: "
        << looseBoolVal7 << ", AsBool(): " << looseBoolVal7.AsLooseBool() << std::endl;
    std::cout << std::endl;

    LLBC_Variant become(3);
    std::cout << "origin value: " << become << std::endl;
    become.BecomeStr();
    std::cout << "become to Sring: " << become << std::endl;
    become.BecomeBool();
    std::cout << "become bool: " << become << std::endl;
    become.BecomeInt8();
    std::cout << "become int8: " << become << std::endl;
    become.BecomeUInt8();
    std::cout << "become uint8: " << become << std::endl;
    become.BecomeInt16();
    std::cout << "become int16: " << become << std::endl;
    become.BecomeUInt16();
    std::cout << "become uint16: " << become << std::endl;
    become.BecomeInt32();
    std::cout << "become int32: " << become << std::endl;
    become.BecomeUInt32();
    std::cout << "become uint32: " << become << std::endl;
    become.BecomeLong();
    std::cout << "become long: " << become << std::endl;
    become.BecomeULong();
    std::cout << "become ulong: " << become << std::endl;
    become.BecomePtr();
    std::cout << "become ptr: " << become << std::endl;
    become.BecomeInt64();
    std::cout << "become int64: " << become << std::endl;
    become.BecomeUInt64();
    std::cout << "become uint64: " << become << std::endl;
    become.BecomeFloat();
    std::cout << "become float: " << become << std::endl;
    become.BecomeDouble();
    std::cout << "become double: " << become << std::endl;
    become.BecomeNil();
    std::cout << "become Nil: " << become << std::endl;
    become.BecomeBool();
    std::cout << "become bool: " << become << std::endl;

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

void TestCase_Core_VariantTest::CompareTest()
{
    std::cout << "Compare test: " << std::endl;

    LLBC_Variant nil;
    LLBC_Variant boolTrue(true);
    LLBC_Variant boolFalse(false);
    std::cout << "nil < nil?: " << (nil < nil) << ", nil == nil?: " << (nil == nil) << std::endl;
    std::cout << "nil < boolTrue?: " << (nil < boolTrue) << ", nil == boolTrue?: " << (nil == boolTrue) << std::endl;
    std::cout << "nil < boolFalse?: " << (nil < boolFalse) << ", nil == boolFalse?: " << (nil == boolFalse) << std::endl;
    std::cout << "boolTrue < boolTrue?: " << (boolTrue < boolTrue) << ", boolTrue == boolTrue?: " << (boolTrue == boolTrue) << std::endl;
    std::cout << "boolFalse < boolFalse?: " << (boolFalse < boolFalse) << ", boolFalse == boolFalse?: " << (boolFalse == boolFalse) << std::endl;
    std::cout << "boolTrue < boolFalse?: " << (boolTrue < boolFalse) << ", boolTrue == boolFalse?: " << (boolTrue == boolFalse) << std::endl;
    std::cout << "boolFalse < boolTrue?: " << (boolFalse < boolTrue) << ", boolFalse == boolTrue?: " << (boolFalse == boolTrue) << std::endl;

    LLBC_Variant int0(0);
    LLBC_Variant int1(1);
    LLBC_Variant int10010(10010);
    LLBC_Variant int10086(10086);
    std::cout << "nil < int10010?: " << (nil < int10010) << ", nil == int10010?: " << (nil == int10010) << std::endl;
    std::cout << "int0 < boolFalse?: " << (int0 < boolFalse) << ", int0 == boolFalse?: " << (int0 == boolFalse) << std::endl;
    std::cout << "int1 < boolTrue?: " << (int1 < boolTrue) << ", int1 == boolTrue?: " << (int1 == boolTrue) << std::endl;
    std::cout << "int10010 < int10086?: " << (int10010 < int10086) << ", int10010 == int10086?: " << (int10010 == int10086) << std::endl;
    std::cout << "int10086 < int10010?: " << (int10086 < int10010) << ", int10086 == int10010?: " << (int10086 == int10010) << std::endl;
    std::cout << "int10086 < int10086?: " << (int10086 < int10086) << ", int10086 == int10086?: " << (int10086 == int10086) << std::endl;

    LLBC_Variant dbl10010(10010.0);
    LLBC_Variant dbl10086(10086.0);
    std::cout << "nil < dbl10010?: " << (nil < dbl10010) << ", nil == dbl10010?:" << (nil == dbl10010) << std::endl;
    std::cout << "int0 < dbl10010?: " << (int0 < dbl10010) << ", int0 == dbl10010?: " << (int0 == dbl10010) << std::endl;
    std::cout << "int10010 < dbl10010?: " << (int10010 < dbl10010) << ", int10010 == dbl10010?: " << (int10010 == dbl10010) << std::endl;
    std::cout << "dbl10010 < int10010?: " << (dbl10010 < int10010) << ", dbl10010 == int10010?: " << (dbl10010 == int10010) << std::endl;
    std::cout << "dbl10010 < dbl10086?: " << (dbl10010 < dbl10086) << ", dbl10010 == dbl10086?: " << (dbl10010 == dbl10086) << std::endl;
    std::cout << "dbl10086 < dbl10086?: " << (dbl10086 < dbl10086) << ", dbl10086 == dbl10086?: " << (dbl10086 == dbl10086) << std::endl;

    LLBC_Variant str1_FromStringLiteral("Hello world(string literal)");
    LLBC_Variant str1_FromStlString(std::string("Hello world(stl string)"));
    LLBC_Variant str1_FromLLBCString(LLBC_String("Hello world(llbc string)"));
    LLBC_Variant str2_FromStringLiteral("Hello world(string literal)");
    std::cout << "str1_FromStringLiteral: " << str1_FromStringLiteral << std::endl;
    std::cout << "str2_FromStringLiteral: " << str2_FromStringLiteral << std::endl;
    std::cout << "str1_FromStlString: " << str1_FromStlString << std::endl;
    std::cout << "str1_FromLLBCString: " << str1_FromLLBCString << std::endl;
    std::cout << "str1_FromStringLiteral == str1_FromStlString?: " << (str1_FromStringLiteral == str1_FromStlString) << std::endl;
    std::cout << "str1_FromStringLiteral == str2_FromStringLiteral?: " << (str1_FromStringLiteral == str2_FromStringLiteral) << std::endl;
    std::cout << "str1_FromStringLiteral == dbl10086?: " << (str1_FromStringLiteral == dbl10086) << std::endl;

    LLBC_Variant seq1;
    LLBC_Variant seq2;
    LLBC_Variant seq2Clone;
    seq1.SeqPushBack(LLBC_Variant::nil);
    seq2.SeqPushBack(1);
    seq2Clone = seq2;
    std::cout << "nil < seq1?: " << (nil < seq1) << ", nil == seq1?: " << (nil == seq1) << std::endl;
    std::cout << "boolTrue < seq1?: " << (boolTrue < seq1) << ", boolTrue == seq1?: " << (boolTrue == seq1) << std::endl;
    std::cout << "int10086 < seq1?: " << (int10086 < seq1) << ", int10086 == seq1?: " << (int10086 == seq1) << std::endl;
    std::cout << "dbl10086 < seq1?: " << (dbl10086 < seq1) << ", dbl10086 == seq1?: " << (dbl10086 == seq1) << std::endl;
    std::cout << "seq1 < seq1?: " << (seq1 < seq1) << ", seq1 == seq1?: " << (seq1 == seq1) << std::endl;
    std::cout << "seq1 < seq2?: " << (seq1 < seq2) << ", seq1 == seq2?: " << (seq1 == seq2) << std::endl;
    std::cout << "seq2 < seq2Clone?: " << (seq2 < seq2Clone) << ", seq2 == seq2Clone?: " << (seq2 == seq2Clone) << std::endl;

    LLBC_Variant dict1;
    LLBC_Variant dict2;
    dict1["Key1"] = "Value1";
    dict2["Key1"] = "Value2";
    std::cout << "nil < ?: dict1?: " << (nil < dict1) << ", nil == dict1?: " << (nil == dict1) << std::endl;
    std::cout << "boolTrue < dict1?: " << (boolTrue < dict1) << ", boolTrue == dict1?: " << (boolTrue == dict1) << std::endl;
    std::cout << "int10086 < dict1?: " << (int10086 < dict1) << ", int10086 == dict1?: " << (int10086 == dict1) << std::endl;
    std::cout << "dbl10086 < dict1?: " << (dbl10086 < dict1) << ", dbl10086 == dict1?: " << (dbl10086 == dict1) << std::endl;
    std::cout << "seq1 < dict1?: " << (seq1 < dict1) << ", seq1 == dict1?: " << (seq1 == dict1) << std::endl;
    std::cout << "dict1 < dict1?: " << (dict1 < dict1) << ", dict1 == dict1?: " << (dict1 == dict1) << std::endl;
    std::cout << "dict1 < dict2?: " << (dict1 < dict2) << ", dict1 == dict2?: " << (dict1 == dict2) << std::endl;
}

void TestCase_Core_VariantTest::CompositeKeyTest()
{
    std::cout <<"Composite key test:" <<std::endl;

    LLBC_Variant v;
    v["The str key 1"] = "The str value 1";
    v[100] = 100;
    v[-100] = -100;
    v[true] = true;
    v[false] = false;
    v[LLBC_Variant::nil] = LLBC_Variant::nil;
    v[LLBC_Variant::nil] = "The nil value";

    LLBC_Variant dictKey;
    dictKey["embedded key 1"] = "embedded value 1";
    dictKey["embedded key 2"] = "embedded value 2";
    v[dictKey] = "The dict value";

    std::cout <<"The composite variant: " <<v <<std::endl;
}

void TestCase_Core_VariantTest::ArithmeticTest()
{
    std::cout <<"Arithmetic test:" <<std::endl;

    LLBC_Variant left((sint32)3);
    LLBC_Variant right((sint64)-5);
    std::cout <<"[" <<left <<"] + [" <<right <<"] = " <<left + right <<std::endl;
    std::cout <<"[" <<left <<"] - [" <<right <<"] = " <<left - right <<std::endl;
    std::cout <<"[" <<left <<"] * [" <<right <<"] = " <<left * right <<std::endl;
    right = right.AsDouble();
    std::cout <<"[" <<left <<"] / [" <<right <<"] = " <<left / right <<std::endl;
    std::cout <<std::endl;

    left = -15;
    right = 32;
    std::cout <<"[" <<left <<"] + [" <<right <<"] = " <<left + right <<std::endl;
    std::cout <<"[" <<left <<"] - [" <<right <<"] = " <<left - right <<std::endl;
    std::cout <<"[" <<left <<"] * [" <<right <<"] = " <<left * right <<std::endl;
    right = right.AsDouble();
    std::cout <<"[" <<left <<"] / [" <<right <<"] = " <<left / right.BecomeDouble() <<std::endl;
    std::cout <<std::endl;

    left = 3;
    right = -4;
    std::cout <<"[" <<left <<"] += [" <<right <<"] = ";
    std::cout <<(left += right) <<std::endl;
    std::cout <<"[" <<left <<"] -= [" <<right <<"] = ";
    std::cout <<(left -= right) <<std::endl;
    std::cout <<"[" <<left <<"] *= [" <<right <<"] = ";
    std::cout <<(left *= right) <<std::endl;
    std::cout <<"[" <<left <<"] /= [" <<right <<"] = ";
    std::cout <<(left /= right) <<std::endl;
    std::cout <<std::endl;

    left = "hello ";
    right = "world!";
    std::cout <<"[" <<left <<"] + [" <<right <<"] = " <<left + right <<std::endl;
    std::cout <<"[" <<left <<"] - [" <<right <<"] = " <<left - right <<std::endl;
    std::cout <<"[" <<left <<"] * [" <<right <<"] = " <<left * right <<std::endl;
    std::cout <<"[" <<left <<"] / [" <<right <<"] = " <<left / right <<std::endl;
    std::cout <<std::endl;

    left = "he333llo";
    right = 333;
    std::cout <<"[" <<left <<"] + [" <<right <<"] = " <<left + right <<std::endl;
    std::cout <<"[" <<left <<"] - [" <<right <<"] = " <<left - right <<std::endl;
    std::cout <<"[" <<left <<"] * [" <<right <<"] = " <<left * right <<std::endl;
    std::cout <<"[" <<left <<"] / [" <<right <<"] = " <<left / right <<std::endl;
    std::cout <<std::endl;
    std::cout <<"[" <<right <<"] + [" <<left <<"] = " <<right + left <<std::endl;
    std::cout <<"[" <<right <<"] - [" <<left <<"] = " <<right - left <<std::endl;
    std::cout <<"[" <<right <<"] * [" <<left <<"] = " <<right * left <<std::endl;
    std::cout <<"[" <<right <<"] / [" <<left <<"] = " <<right / left <<std::endl;
    std::cout <<std::endl;
}

void TestCase_Core_VariantTest::SeqTest()
{
    std::cout << "Sequence test" << std::endl;

    // Construct default sequence.
    LLBC_Variant subSeq;
    subSeq["Hello"] = 3;
    subSeq[LLBC_Variant::nil] = "The nil key value";

    LLBC_Variant seq1;
    seq1.SeqPushBack(3);
    seq1.SeqPushBack(false);
    seq1.SeqPushBack("Hello World");
    seq1.SeqPushBack(subSeq);
    std::cout << "seq1 data:" << seq1 << std::endl;

    // Assignment test.
    LLBC_Variant seq2 = seq1;
    std::cout << "seq2 = seq1, seq2: " << seq2 << std::endl;

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
    std::cout << "Perform sequence arithmetic test, left:" << left << ", right:" << right << std::endl;
    std::cout << "left + right: " << left + right << std::endl;
    std::cout << "left - right: " << left - right << std::endl;
    std::cout << "left * right: " << left * right << std::endl;
    std::cout << "left / right: " << left / right << std::endl;

    // Sequence + - * / non sequence test.
    LLBC_Variant seq10;
    seq10.SeqPushBack(10);
    seq10.SeqPushBack("Hello world");
    seq10.SeqPushBack(false);
    std::cout << "Perform sequence +-*/ non seuqnce test:" << std::endl;
    std::cout << "seq10: " << seq10 << std::endl;
    std::cout << "seq10 + 10086: " << seq10 + 10086 << std::endl;
    std::cout << "seq10 + nil: " << seq10 + LLBC_Variant::nil << std::endl;
    std::cout << "seq10 + hello world: " << seq10 + "hello world" << std::endl;
    std::cout << "seq10 - false: " << seq10 - false << std::endl;

    // Sequence batch push back test.
    std::cout <<"Sequence batch push back test:" <<std::endl;
    LLBC_Variant batchPushBackTestSeq;
    batchPushBackTestSeq.SeqPushBack(true, false, 1, LLBC_Variant(2), "Hello world", 5.8, LLBC_Variant::nil);
    std::cout <<"- After batch push back:" <<batchPushBackTestSeq <<std::endl;
}

void TestCase_Core_VariantTest::DictTest()
{
    std::cout <<"Dict test" <<std::endl;

    // Raw key type dictionary test.
    LLBC_Variant seqKey;
    seqKey = 3;
    seqKey = "Hello World";
    seqKey = LLBC_Variant::Dict();
    seqKey.SeqPushBack(3);
    seqKey.SeqPushBack(4);
    seqKey.SeqPushBack(false);
    seqKey.SeqPushBack("Hello World");
    seqKey.SeqInsert(seqKey.SeqBegin() + 3, LLBC_Variant::nil);

    LLBC_Variant rawDict1;
    rawDict1.DictInsert(1, "Hello");
    rawDict1.DictInsert(2, "World");
    rawDict1.DictInsert(3, "Hey");
    rawDict1.DictInsert(4, "Judy");
    rawDict1.DictInsert(5.0, "Jack");
    rawDict1.DictInsert(false, 34);
    rawDict1[seqKey] = "The seq key value";
    std::cout <<"rawDict1 data: " <<rawDict1 <<std::endl;

    LLBC_Variant rawDict2(rawDict1);
    std::cout <<"rawDict2(copy from rawDict1) data: " <<rawDict2 <<std::endl;
    rawDict2[1] = "hello";
    rawDict2[2] = "worlld";
    rawDict2[3] = "hey";
    rawDict2[4] = "judy";
    rawDict2[5.0] = "jack";
    rawDict2[false] = 35;
    std::cout <<"rawDict2(change idx:1 value) data: " <<rawDict2 <<std::endl;

    std::cout <<"rawDict1 == rawDict2 ? " <<(rawDict1 == rawDict2) <<std::endl;
    std::cout <<"rawDict1 < rawDict2 ? " <<(rawDict1 < rawDict2) <<std::endl;

    // Dictionary collection operation test.
    LLBC_Variant set1;
    LLBC_Variant set2;
    set1[1] = true;
    set1[2] = true;
    set2[2] = true;
    set2[3] = true;
    std::cout << "Dictionary collection operation test:" << std::endl;
    std::cout <<"union op: set1[" <<set1 <<"] + set2[" <<set2 <<"] = " <<set1 + set2 <<std::endl;
    std::cout <<"difference op: set1[" <<set1 <<"] - set2[" <<set2 <<"] = " <<set1 - set2 <<std::endl;
    std::cout <<"intersection op: set1[" <<set1 <<"] * set2[" <<set2 <<"] = " <<set1 * set2 <<std::endl;
    std::cout <<"union difference op: set1[" <<set1 <<"] / set2[" <<set2 <<"] = " <<set1 / set2 <<std::endl;

    // Use dictionary as key(low-performance)
    LLBC_Variant lowPerfDict;
    lowPerfDict[rawDict1] = "Hello";
    lowPerfDict[rawDict2] = "World";
    std::cout <<"lowPerfDict: " <<lowPerfDict <<std::endl;

    // Batch erase test.
    LLBC_Variant batchEraseTestDict;
    batchEraseTestDict["a"] = 3;
    batchEraseTestDict[false] = "Hello world";
    batchEraseTestDict[true] = "Hey Judy";
    batchEraseTestDict[6] = true;
    batchEraseTestDict[5] = true;
    std::cout <<"Batch erase dict test:" <<std::endl;
    std::cout <<"- Before erase(\"a\", false, 5):" <<batchEraseTestDict <<std::endl;
    auto batchEraseRet = batchEraseTestDict.DictErase("a", false, LLBC_Variant(5));
    std::cout <<"- After erase:" <<batchEraseTestDict <<std::endl;
    std::cout <<"- Erase return:" <<batchEraseRet <<std::endl;

    batchEraseTestDict.DictErase(batchEraseTestDict.DictFind(true));
    std::cout <<"- After call DictErase(iter(true)):" <<batchEraseTestDict <<std::endl;

    batchEraseTestDict.DictErase(6);
    std::cout <<"- After call DictErase(6):" <<batchEraseTestDict <<std::endl;
}

void TestCase_Core_VariantTest::SerializeTest()
{
    std::cout <<"Serialize test" <<std::endl;

    // Serialize & Deserialize raw type.
    LLBC_Stream stream;
    LLBC_Variant raw(-64);
    stream <<raw;
    std::cout <<"Raw obj[" <<raw <<"] serialized size: " <<stream.GetPos() <<std::endl;

    LLBC_Variant deserRaw;
    stream.SetPos(0);
    stream>> deserRaw;
    std::cout <<"Deserialized from stream: [" <<deserRaw <<"]" <<std::endl;

    // Serialize & Deserialize string type.
    LLBC_Variant str("Hello World!");
    stream.SetPos(0);
    stream <<str;
    std::cout <<"String obj[" <<str <<"] serialized size: " <<stream.GetPos() <<std::endl;

    LLBC_Variant deserStr;
    stream.SetPos(0);
    stream>> deserStr;
    std::cout <<"Deserialized from stream: [" <<deserStr <<"]" <<std::endl;

    // Serialize & Deserialize seq type.
    LLBC_Variant seq;
    seq.SeqPushBack(1);
    seq.SeqPushBack(false);
    seq.SeqPushBack("Hello world");
    stream.SetPos(0);
    stream <<seq;
    std::cout << "Seq obj[" << seq << "] serialized size: " << stream.GetPos() << std::endl;

    LLBC_Variant deserSeq;
    stream.SetPos(0);
    stream >> deserSeq;
    std::cout << "Deserialized from stream:[" << deserSeq << "]" << std::endl;

    // Serialize & Deserialize dict type.
    LLBC_Variant dict;
    dict[1] = "Hello World!";
    dict[2] = "Hey Judy!";
    stream.SetPos(0);
    stream <<dict;
    std::cout <<"Dict obj[" <<dict <<"] serialized size: " <<stream.GetPos() <<std::endl;

    LLBC_Variant deserDict;
    stream.SetPos(0);
    stream>> deserDict;
    std::cout <<"Deserialized from stream: [" <<deserDict <<"]" <<std::endl;
}

void TestCase_Core_VariantTest::HashTest()
{
    std::cout <<"Hash test" <<std::endl;

    auto doHash = [](const LLBC_Variant &v)
    {
        std::cout <<"Hash " <<v <<": " <<std::hash<LLBC_Variant>()(v) <<std::endl;
    };

    doHash(LLBC_Variant::nil); 
    doHash(LLBC_Variant(true));
    doHash(LLBC_Variant(false));
    doHash(LLBC_Variant(0));
    doHash(LLBC_Variant(1));
    doHash(LLBC_Variant(2));
    doHash(LLBC_Variant(-1));
    doHash(LLBC_Variant(-2));
    doHash(LLBC_Variant(3.1415926f));
    doHash(LLBC_Variant(6.18));
    doHash(LLBC_Variant("Hello world"));
    doHash(LLBC_Variant(""));

    LLBC_Variant seq;
    seq.SeqPushBack(LLBC_Variant(true));
    seq.SeqPushBack(LLBC_Variant(9527));
    seq.SeqPushBack(LLBC_Variant(3.1415926));
    seq.SeqPushBack(LLBC_Variant());
    seq[seq.Size() - 1].DictInsert("hello world", 6.18);
    doHash(seq);
}

void TestCase_Core_VariantTest::ConvertToUnurderedStlContainerTest()
{
    std::cout <<"Convert to unordered stl container test:" <<std::endl;

    LLBC_Variant v;
    v["hello world"] = 3;
    v[false] = "hey judy";
    v[LLBC_Variant::nil] = LLBC_Variant::nil;
    v[3.1415926] = 9527;

    std::unordered_set<LLBC_Variant> us = v;
    std::cout <<"Variant: " <<v <<std::endl;
    std::cout <<"Convert to " <<LLBC_GetTypeName(us) <<"(size:" <<us.size() <<"):" <<std::endl;
    for (auto &elem : us)
        std::cout <<"- " <<elem <<std::endl;
}