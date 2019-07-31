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
    std::cout <<"LLBC_Variant test:" <<std::endl;

    BasicTest();
    std::cout <<std::endl;

    CompositeKeyTest();
    std::cout <<std::endl;

    ArithmeticTest();
    std::cout <<std::endl;

    DictTtest();
    std::cout <<std::endl;

    SerializeTest();

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    return 0;
}

void TestCase_Core_VariantTest::BasicTest()
{
    std::cout <<"Basic test:" <<std::endl;

    LLBC_Variant nilVal;
    LLBC_Variant boolVal(true);
    LLBC_Variant int8Val( (sint8)-8 );
    LLBC_Variant uint8Val( (uint8)8 );
    LLBC_Variant int16Val( (sint16)-16 );
    LLBC_Variant uint16Val( (uint16)16 );
    LLBC_Variant int32Val( (sint32)-32 );
    LLBC_Variant uint32Val( (uint32)32 );
    LLBC_Variant longVal( (long)-10086 );
    LLBC_Variant ulongVal( (LLBC_NS ulong)10086 );
    uint64 ptrAddr = 0x80000000;
    void *ptr; ::memcpy(&ptr, &ptrAddr, sizeof(void *));
    LLBC_Variant ptrVal(ptr);
    LLBC_Variant int64Val( (sint64)-64 );
    LLBC_Variant uint64Val( (uint64)64 );
    LLBC_Variant floatVal( (float)88.0 );
    LLBC_Variant doubleVal( (double)-88.0 );
    LLBC_Variant strVal("Hello");

    std::cout <<"nil val, isNil: " 
        <<(nilVal.IsNil() ? "true" : "false") <<", ToString: " <<nilVal <<std::endl;
    std::cout <<"bool val, isBool: "
        <<(boolVal.IsBool() ? "true" : "false") <<", ToString: " <<boolVal <<std::endl;
    std::cout <<"int8Val, isInt8: " 
        <<(int8Val.IsInt8() ? "true" : "false") <<", ToString: " <<int8Val <<std::endl;
    std::cout <<"uint8Val, isUInt8: " 
        <<(uint8Val.IsUInt8() ? "true" : "false") <<", ToString: " <<uint8Val <<std::endl;
    std::cout <<"int16Val, isInt16: "
        <<(int16Val.IsInt16() ? "true" : "false") <<", ToString: " <<int16Val <<std::endl;
    std::cout <<"uint16Val, isUInt16: "
        <<(uint16Val.IsUInt16() ? "true" : "false") <<", ToString: " <<uint16Val <<std::endl;
    std::cout <<"int32Val, isInt32: "
        <<(int32Val.IsInt32() ? "true" : "false") <<", ToString: " <<int32Val <<std::endl;
    std::cout <<"uint32Val, isUInt32: "
        <<(uint32Val.IsUInt32() ? "true" : "false") <<", ToString: " <<uint32Val <<std::endl;
    std::cout <<"longVal, isLong: " 
        <<(longVal.IsLong() ? "true" : "false") <<", ToString: " <<longVal <<std::endl;
    std::cout <<"ulongVal, isULong: " 
        <<(ulongVal.IsULong() ? "true" : "false") <<", ToString: " <<ulongVal <<std::endl;
    std::cout <<"ptrVal, isPtr: " 
        <<(ptrVal.IsPtr() ? "true" : "false") <<", ToString: " <<ptrVal <<std::endl;
    std::cout <<"int64Val, isInt64: " 
        <<(int64Val.IsInt64() ? "true" : "false") <<", ToString: " <<int64Val <<std::endl;
    std::cout <<"uint64Val, isUInt64: " 
        <<(uint64Val.IsUInt64() ? "true" : "false") <<", ToString: " <<uint64Val <<std::endl;
    std::cout <<"floatVal, isFloat: " 
        <<(floatVal.IsFloat() ? "true" : "false") <<", ToString: " <<floatVal <<std::endl;
    std::cout <<"doubleVal, isDouble: " 
        <<(doubleVal.IsDouble() ? "true" : "false") <<", ToString: " <<doubleVal <<std::endl;
    std::cout <<"strVal, isStr: " 
        <<(strVal.IsStr() ? "true" : "false") <<", ToString: " <<strVal <<std::endl;
    std::cout <<std::endl;

    LLBC_Variant become(3);
    std::cout <<"origin value: " <<become <<std::endl;
    become.BecomeStr();
    std::cout <<"become to Sring: " <<become <<std::endl;
    become.BecomeBool();
    std::cout <<"become bool: " <<become <<std::endl;
    become.BecomeInt8();
    std::cout <<"become int8: " <<become <<std::endl;
    become.BecomeUInt8();
    std::cout <<"become uint8: " <<become <<std::endl;
    become.BecomeInt16();
    std::cout <<"become int16: " <<become <<std::endl;
    become.BecomeUInt16();
    std::cout <<"become uint16: " <<become <<std::endl;
    become.BecomeInt32();
    std::cout <<"become int32: " <<become <<std::endl;
    become.BecomeUInt32();
    std::cout <<"become uint32: " <<become <<std::endl;
    become.BecomeLong();
    std::cout <<"become long: " <<become <<std::endl;
    become.BecomeULong();
    std::cout <<"become ulong: " <<become <<std::endl;
    become.BecomePtr();
    std::cout <<"become ptr: " <<become <<std::endl;
    become.BecomeInt64();
    std::cout <<"become int64: " <<become <<std::endl;
    become.BecomeUInt64();
    std::cout <<"become uint64: " <<become <<std::endl;
    become.BecomeFloat();
    std::cout <<"become float: " <<become <<std::endl;
    become.BecomeDouble();
    std::cout <<"become double: " <<become <<std::endl;
    become.BecomeNil();
    std::cout <<"become Nil: " <<become <<std::endl;
    become.BecomeBool();
    std::cout <<"become bool: " <<become <<std::endl;

    int intData = 300;
    ptrVal = &intData;
    std::cout <<"Pointer type(special) test:" <<std::endl;
    std::cout <<"intData: " <<intData <<", save ptr(0x" <<&intData << ") to variant: " <<ptrVal <<std::endl;

    ptrVal = ptrVal + LLBC_Variant(4);
    ptrVal = ptrVal - LLBC_Variant(4);
    std::cout <<"After ptrVal += 4, ptrVal -= 4, value: " <<*ptrVal.AsPtr<int>() <<std::endl;
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

    LLBC_Variant left( (sint32)3 );
    LLBC_Variant right( (sint64)-5 );
    LLBC_Variant result = left + right;
    std::cout <<"[" <<left <<"] + [" <<right <<"] = " <<left + right <<std::endl;
    std::cout <<"[" <<left <<"] - [" <<right <<"] = " <<left - right <<std::endl;
    std::cout <<"[" <<left <<"] * [" <<right <<"] = " <<left * right <<std::endl;
    std::cout <<"[" <<left <<"] / [" <<right <<"] = " <<left / right.BecomeDouble() <<std::endl;
    std::cout <<std::endl;

    left = -15;
    right = 32;
    std::cout <<"[" <<left <<"] + [" <<right <<"] = " <<left + right <<std::endl;
    std::cout <<"[" <<left <<"] - [" <<right <<"] = " <<left - right <<std::endl;
    std::cout <<"[" <<left <<"] * [" <<right <<"] = " <<left * right <<std::endl;
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
}

void TestCase_Core_VariantTest::DictTtest()
{
    std::cout <<"Dict test" <<std::endl;

    // Raw key type dictionary test.
    LLBC_Variant rawDict1;
    rawDict1.Insert(1, "Hello");
    rawDict1.Insert(2, "World");
    rawDict1.Insert(3, "Hey");
    rawDict1.Insert(4, "Judy");
    rawDict1.Insert(5.0, "Jack");
    rawDict1.Insert(false, 34);
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
    std::cout <<"union op: set1[" <<set1 <<"] + set2[" <<set2 <<"]: = " <<set1 + set2 <<std::endl;
    std::cout <<"difference op: set1[" <<set1 <<"] - set2[" <<set2 <<"]: = " <<set1 - set2 <<std::endl;
    std::cout <<"intersection op: set1[" <<set1 <<"] * set2[" <<set2 <<"]: = " <<set1 * set2 <<std::endl;
    std::cout <<"union difference op: set1[" <<set1 <<"] / set2[" <<set2 <<"]: = " <<set1 / set2 <<std::endl;

    // Use dictionary as key(low-performance)
    LLBC_Variant lowPerfDict;
    lowPerfDict[rawDict1] = "Hello";
    lowPerfDict[rawDict2] = "World";
    std::cout <<"lowPerfDict: " <<lowPerfDict <<std::endl;
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
