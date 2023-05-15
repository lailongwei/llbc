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


#include "common/TestCase_Com_Stream.h"

namespace
{

struct TrivialCls
{
    bool boolVal;

    sint32 int32Val;
    uint32 uint32Val;

    sint64 int64Val;
    uint64 uint64Val;

    float floatVal;
    double doubleVal;

    LLBC_String ToString() const
    {
        LLBC_String str;
        str.append("TrivialCls[");
        str.append_format("\n  - boolVal:%s", boolVal ? "true" : "false");
        str.append_format("\n  - int32Val:%d", int32Val);
        str.append_format("\n  - uint32Val:%u", uint32Val);
        str.append_format("\n  - int64Val:%lld", int64Val);
        str.append_format("\n  - uint64Val:%llu", uint64Val);
        str.append_format("\n  - floatVal:%f", floatVal);
        str.append_format("\n  - doubleVal:%f", doubleVal);
        str.append( "\n]");

        return str;
    }
};

struct SerializableCls
{
    bool bVal;
    sint32 intVal;

    LLBC_String str;
    LLBC_Strings strs;

    std::vector<sint32> int32Vec;
    std::list<sint32> int32List;
    std::set<sint32> int32Set;
    std::unordered_set<sint32> int32USet;
    std::map<sint32, LLBC_String> int32ToStrMap;
    std::unordered_map<sint32, LLBC_String> int32ToStrUMap;

    std::array<LLBC_String, 3> arr;
    std::pair<sint32, LLBC_Strings> pa;

    std::vector<std::list<std::map<sint32, LLBC_String>>> complexVal;

    void Serialize(LLBC_Stream &stream) const
    {
        stream << bVal
            << intVal

            << str
            << strs

            << int32Vec
            << int32List
            << int32Set
            << int32USet
            << int32ToStrMap
            << int32ToStrUMap

            << arr
            << pa

            << complexVal;
    }

    bool DeSerialize(LLBC_Stream &stream)
    {
        LLBC_STREAM_BEGIN_READ(stream, bool, false);

        LLBC_STREAM_READ(bVal);
        LLBC_STREAM_READ(intVal);

        LLBC_STREAM_READ(str);
        LLBC_STREAM_READ(strs);

        LLBC_STREAM_READ(int32Vec);
        LLBC_STREAM_READ(int32List);
        LLBC_STREAM_READ(int32Set);
        LLBC_STREAM_READ(int32USet);
        LLBC_STREAM_READ(int32ToStrMap);
        LLBC_STREAM_READ(int32ToStrUMap);

        LLBC_STREAM_READ(arr);
        LLBC_STREAM_READ(pa);

        LLBC_STREAM_READ(complexVal);

        LLBC_STREAM_END_READ();

        return true;
    }

    LLBC_String ToString() const
    {
        LLBC_String str;
        str.append("SerializableCls[");
        str.append_format("\n  - bVal:%s", bVal ? "true" : "false");
        str.append_format("\n  - intVal:%d", intVal);

        str.append_format("\n  - str:%s", str.c_str());
        str.append_format("\n  - strs:%s", LLBC_Variant(strs).ValueToString().c_str());

        str.append_format("\n  - int32Vec:%s", LLBC_Variant(int32Vec).ValueToString().c_str());
        str.append_format("\n  - int32List:%s", LLBC_Variant(int32List).ValueToString().c_str());
        str.append_format("\n  - int32Set:%s", LLBC_Variant(int32Set).ValueToString().c_str());
        str.append_format("\n  - int32USet:%s", LLBC_Variant(int32USet).ValueToString().c_str());
        str.append_format("\n  - int32ToStrMap:%s", LLBC_Variant(int32ToStrMap).ValueToString().c_str());
        str.append_format("\n  - int32ToStrUMap:%s", LLBC_Variant(int32ToStrUMap).ValueToString().c_str());

        str.append_format("\n  - arr:");
        for (auto &item : arr)
        {
            if (str[str.size() - 1] != ':')
                str.append(1, ',');
            str.append_format("%s", item.c_str());
        }
        str.append_format("\n  - pair: %d, %s", pa.first, LLBC_Variant(pa.second).ValueToString().c_str());

        str.append_format("\n  - complexVal: %s", LLBC_Variant(complexVal).ValueToString().c_str());

        str.append("\n]");

        return str;
    }
};

struct MovableCls
{
    int intVal;
    LLBC_String strVal;

    MovableCls()
    : intVal()
    {
    }

    MovableCls(const MovableCls &other)
    : intVal(other.intVal)
    , strVal(other.strVal)
    {
        LLBC_PrintLn("- MovableCls::MovableCls(const MovableCls &) called!");
    }

    MovableCls(MovableCls &&other)
    : intVal(other.intVal)
    , strVal(std::move(other.strVal))
    {
        other.intVal = 0;
        LLBC_PrintLn("- MovableCls::MovableCls(MovableCls &&) called!");
    }

    MovableCls &operator=(const MovableCls &other)
    {
        intVal = other.intVal;
        strVal = other.strVal;

        LLBC_PrintLn("- MovableCls &MovableCls::operator=(const MovableCls &) called!");

        return *this;
    }

    MovableCls &operator=(MovableCls &&other)
    {
        intVal = other.intVal;
        other.intVal = 0;
        strVal = std::move(other.strVal);

        LLBC_PrintLn("- MovableCls &MovableCls::operator=(MovableCls &&) called!");

        return *this;
    }

    void Serialize(LLBC_Stream &stream) const
    {
        stream << intVal << strVal;
    }

    bool Deserialize(LLBC_Stream &stream)
    {
        return stream.Read(intVal) && stream.Read(strVal);
    }
};

}

int TestCase_Com_Stream::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Stream test:");

    LLBC_ReturnIf(BasicTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(ConstructAndAssignmentTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(AttachTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(DetachTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(RecapTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(SwapTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(EndianTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(PODTypeSerTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(TrivialClsSerTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(SerializableClsSerTest() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(MovableReadTest() != LLBC_OK, LLBC_FAILED);

    return LLBC_OK;
}

int TestCase_Com_Stream::BasicTest()
{
    LLBC_PrintLn("BasicTest:");

    LLBC_Stream stream1;
    LLBC_PrintLn("- Default stream:%s", stream1.ToString().c_str());

    LLBC_Stream stream2(1024);
    LLBC_PrintLn("- Has 1024 cap stream:%s", stream2.ToString().c_str());
    stream2.Recap(2048);
    LLBC_PrintLn("- After recap to 2048:%s", stream2.ToString().c_str());
    stream2.Recap(1024);
    LLBC_PrintLn("- After recap to 1024:%s", stream2.ToString().c_str());
    stream2.SetAttach(true);
    LLBC_PrintLn("- After set attach flag to true:%s", stream2.ToString().c_str());
    stream2.Recap(4096);
    LLBC_PrintLn("- After set stream to 4096:%s", stream2.ToString().c_str());
    stream2.SetAttach(false);
    LLBC_PrintLn("- After set attach flag to false:%s", stream2.ToString().c_str());

    stream2 << true << 128 << 1.32 << "Hello world";
    LLBC_PrintLn("- After write:%s", stream2.ToString().c_str());

    bool bVal;
    int intVal;
    double dblVal = 0.0;
    char strVal[20];
    stream2.SetPos(0);
    stream2 >> bVal >> intVal >> dblVal >> strVal;
    LLBC_PrintLn("- After read, bVal:%d, intVal:%d, dblVal:%f, strVal:%s, stream:%s",
                 bVal, intVal, dblVal, strVal, stream2.ToString().c_str());

    return LLBC_OK;
}

int TestCase_Com_Stream::ConstructAndAssignmentTest()
{
    LLBC_PrintLn("Copy and Assignment test:");

    auto readStreamCnt = [](LLBC_Stream &stream, const char *streamName)
    {
        bool bVal;
        int intVal;
        double dblVal;
        LLBC_String strVal;
        stream.SetPos(0);
        stream >> bVal >> intVal >> dblVal >> strVal;
        LLBC_PrintLn("- %s content:bVal:%d, intVal:%d, dblVal:%f, strVal:%s, stream2:%s",
                     streamName, bVal, intVal, dblVal, strVal.c_str(), stream.ToString().c_str());
    };

    LLBC_Stream stream1(128);
    stream1 << true << 128 << 1.32 << "Hello world";

    LLBC_Stream stream2(stream1);
    LLBC_PrintLn("- After Copy construct:\n- stream1:%s\n- stream2:%s",
                 stream1.ToString().c_str(), stream2.ToString().c_str());
    readStreamCnt(stream2, "Stream2");

    LLBC_Stream stream3;
    stream3 = stream1;
    LLBC_PrintLn("- After Copy assignment, stream3:%s", stream3.ToString().c_str());
    readStreamCnt(stream3, "Stream3");

    LLBC_Stream stream4(stream3, true);
    LLBC_PrintLn("After copy construct(attach), stream4:%s", stream4.ToString().c_str());
    readStreamCnt(stream4, "Stream4");

    LLBC_Stream stream5(std::move(stream1));
    LLBC_PrintLn("- After move construct(stream1 -> stream5): stream1:%s, stream5:%s",
                 stream1.ToString().c_str(), stream5.ToString().c_str());
    readStreamCnt(stream5, "Stream5");

    LLBC_Stream stream6;
    stream6 = std::move(stream2);
    LLBC_PrintLn("- After move assignment(stream2 -> stream6): stream2:%s, stream6:%s",
                 stream2.ToString().c_str(), stream2.ToString().c_str());
    readStreamCnt(stream6, "Stream6");

    LLBC_Stream stream7;
    stream7 = std::move(stream4);
    LLBC_PrintLn("- After move assign attached stream(stream4 - > stream7): stream4:%s, stream7:%s",
                 stream4.ToString().c_str(), stream7.ToString().c_str());
    readStreamCnt(stream7, "Stream7");

    return LLBC_OK;
}

int TestCase_Com_Stream::AttachTest()
{
    LLBC_PrintLn("Attach test:");

    // Attach construct:
    LLBC_Stream stream1;
    stream1 << "Hello world";
    LLBC_PrintLn("- Original stream:%s", stream1.ToString().c_str());
    LLBC_Stream stream2(stream1, false);
    LLBC_PrintLn("- Stream copy construct(non-attach):origStream:%s, stream:%s",
                 stream1.ToString().c_str(), stream2.ToString().c_str());
    LLBC_Stream stream3(stream1, true);
    LLBC_PrintLn("- Stream copy construct(attach): origStream:%s, stream:%s",
                 stream1.ToString().c_str(), stream3.ToString().c_str());

    // Attach(stream):
    LLBC_Stream stream4;
    stream4.Attach(stream3);
    LLBC_PrintLn("- Stream.Attach(): origStream:%s, stream:%s",
                 stream3.ToString().c_str(), stream4.ToString().c_str());

    // Attach(buf, size):
    char buf[128];
    LLBC_Stream stream5(buf, sizeof(buf), true);
    stream5 << "Hello world";
    LLBC_PrintLn("- Stream::Stream(buf, size):%s", stream5.ToString().c_str());

    return LLBC_OK;
}

int TestCase_Com_Stream::DetachTest()
{
    LLBC_PrintLn("Detach test:");

    LLBC_Stream stream;
    stream << "Hello world";
    LLBC_PrintLn("- Begin detach stream:%s", stream.ToString().c_str());

    const size_t cap = stream.GetCap();
    const size_t pos = stream.GetPos();
    auto ptr = stream.Detach();
    LLBC_PrintLn("- Detached, pos:%lu, cap:%lu, ptr:%p, stream:%s",
                 pos, cap, ptr, stream.ToString().c_str());

    free(ptr);

    return LLBC_OK;
}

int TestCase_Com_Stream::RecapTest()
{
    LLBC_PrintLn("Recap test:");
    LLBC_Stream stream;
    LLBC_PrintLn("- Default construct stream:%s", stream.ToString().c_str());

    LLBC_PrintLn("- Incr recap:");
    for (int i = 1; i <= 10000; ++i)
    {
        stream.Recap(i);
        if (i % 100 == 0)
            LLBC_PrintLn(" - After recap %d bytes:%s", i, stream.ToString().c_str());
    }

    stream.Recap(50);
    LLBC_PrintLn("- After recap to 50, stream:%s", stream.ToString().c_str());

    return LLBC_OK;
}

int TestCase_Com_Stream::SwapTest()
{
    LLBC_PrintLn("Swap test:");

    LLBC_Stream stream1(1024);
    LLBC_Stream stream2(2048);
    stream1 << 3;
    stream2 << "Hello World";

    LLBC_PrintLn("- Begin swap %s, %s",
                 stream1.ToString().c_str(), stream2.ToString().c_str());

    stream1.Swap(stream2); // Or std::swap(stream1, stream2);
    LLBC_PrintLn("- After swap:%s, %s",
                 stream1.ToString().c_str(), stream2.ToString().c_str());

    return LLBC_OK;
}

int TestCase_Com_Stream::EndianTest()
{
    LLBC_PrintLn("Endian test:");

    LLBC_PrintLn("- Default endian:%s", LLBC_Endian::Type2Str(LLBC_DefaultEndian));

    auto testEndian = [](LLBC_Stream &stream, const char *prefix)
    {
        stream << 1 << 3 << 5 << 7;
        LLBC_PrintLn("- %s: stream:%s, bytes:%s",
                     prefix,
                     stream.ToString().c_str(),
                     LLBC_Byte2Hex(stream.GetBuf(), stream.GetPos()).c_str());

        int intVals[4];
        stream.SetPos(0);
        stream >> intVals;
        LLBC_PrintLn("- %s: read int vals:%d %d %d %d",
                     prefix,
                     intVals[0], intVals[1], intVals[2], intVals[3]);
    };

    LLBC_Stream stream1;
    testEndian(stream1, "Default Endian");

    LLBC_Stream stream2;
    stream2.SetEndian(LLBC_Endian::LittleEndian);
    testEndian(stream2, "Little Endian");

    LLBC_Stream stream3;
    stream3.SetEndian(LLBC_Endian::BigEndian);
    testEndian(stream3, "Big Endian");

    return LLBC_OK;
}

int TestCase_Com_Stream::PODTypeSerTest()
{
    LLBC_PrintLn("PODType ser test:");

    LLBC_PrintLn("- Arithmetic type ser test:");
    LLBC_Stream stream;
    stream << true // bool
        << 'X' // sint8
        << static_cast<uint8>(8) // uint8
        << static_cast<sint16>(-16) // sint16
        << static_cast<uint16>(16) // uint16
        << static_cast<sint32>(-32) // sint32
        << static_cast<uint32>(32) // uint32
        << static_cast<long>(-48) // long
        << static_cast<ulong>(48) // ulong
        << static_cast<sint64>(-64) // sint64
        << static_cast<uint64>(64) // uint64
        << 1.234f // float
        << 6.789 // double
        << (void *)0x1234567; // ptr
    LLBC_PrintLn("  - After ser, stream:%s", stream.ToString().c_str());

    stream.SetPos(0);
    LLBC_PrintLn("  - Read arithmetic values from stream:");
    LLBC_PrintLn("    - bool: %d", stream.Read<bool>());
    LLBC_PrintLn("    - sint8: %d", stream.Read<sint8>());
    LLBC_PrintLn("    - uint8: %d", stream.Read<uint8>());
    LLBC_PrintLn("    - sint16: %d", stream.Read<sint16>());
    LLBC_PrintLn("    - uint16: %d", stream.Read<uint16>());
    LLBC_PrintLn("    - sint32: %d", stream.Read<sint32>());
    LLBC_PrintLn("    - uint32: %d", stream.Read<uint32>());
    LLBC_PrintLn("    - long: %ld", stream.Read<long>());
    LLBC_PrintLn("    - ulong: %lu", stream.Read<ulong>());
    LLBC_PrintLn("    - long: %lld", stream.Read<sint64>());
    LLBC_PrintLn("    - ulong: %llu", stream.Read<uint64>());
    LLBC_PrintLn("    - long: %f", stream.Read<float>());
    LLBC_PrintLn("    - ulong: %f", stream.Read<double>());
    LLBC_PrintLn("    - ptr: 0x%p", stream.Read<void *>());

    // =============================================================
    LLBC_PrintLn("- Arithmetic array type ser test:");
    {
        LLBC_Stream _;
        std::swap(stream, _);
    }

    uint8 uint8Arr[4]{ 8, 18, 28, 38 };
    sint16 sint16Arr[4]{ -16, -106, -1006, -10006 };
    uint16 uint16Arr[4]{ 16, 106, 1006, 10006 };
    sint32 sint32Arr[4]{ -32, -302, -3002, -30002 };
    uint32 uint32Arr[4]{ 32, 302, 3002, 30002 };
    sint64 sint64Arr[4]{ -64, -604, -6004, -60004 };
    uint64 uint64Arr[4]{ 64, 604, 6004, 60004 };
    float floatArr[4]{ 1.0f, 2.0f, 3.0f, 4.0f };
    double dblArr[4]{ 1.1f, 2.1f, 3.1f, 4.1f };
    stream << "Hello World" // sint8[]
        << uint8Arr // uint8[]
        << sint16Arr // sint16[]
        << uint16Arr // uint16[]
        << sint32Arr // sint32[]
        << uint32Arr // uint32[]
        << sint64Arr // sint64[]
        << uint64Arr // uint64[]
        << floatArr // float[]
        << dblArr; // double[]
    LLBC_PrintLn("  - After ser, stream:%s", stream.ToString().c_str());

    stream.SetPos(0);
    LLBC_String sint8Arr;
    memset(uint8Arr, 0, sizeof(uint8Arr));
    memset(sint16Arr, 0, sizeof(sint16Arr));
    memset(uint16Arr, 0, sizeof(uint16Arr));
    memset(sint32Arr, 0, sizeof(sint32Arr));
    memset(uint32Arr, 0, sizeof(uint32Arr));
    memset(sint64Arr, 0, sizeof(sint64Arr));
    memset(uint64Arr, 0, sizeof(uint64Arr));
    memset(floatArr, 0, sizeof(floatArr));
    memset(dblArr, 0, sizeof(dblArr));
    stream >> sint8Arr
        >> uint8Arr
        >> sint16Arr
        >> uint16Arr
        >> sint32Arr
        >> uint32Arr
        >> sint64Arr
        >> uint64Arr
        >> floatArr
        >> dblArr;

    LLBC_PrintLn("  - Read arithmetic array values from stream:");
    LLBC_PrintLn("    - sint8Arr: %s", sint8Arr.c_str());
    LLBC_PrintLn("    - uint8Arr: %u %u %u %u",
                 uint8Arr[0], uint8Arr[1], uint8Arr[2], uint8Arr[3]);
    LLBC_PrintLn("    - sint16Arr: %d %d %d %d",
                 sint16Arr[0], sint16Arr[1], sint16Arr[2], sint16Arr[3]);
    LLBC_PrintLn("    - uint16Arr: %u %u %u %u",
                 uint16Arr[0], uint16Arr[1], uint16Arr[2], uint16Arr[3]);
    LLBC_PrintLn("    - sint32Arr: %d %d %d %d",
                 sint32Arr[0], sint32Arr[1], sint32Arr[2], sint32Arr[3]);
    LLBC_PrintLn("    - uint32Arr: %u %u %u %u",
                 uint32Arr[0], uint32Arr[1], uint32Arr[2], uint32Arr[3]);
    LLBC_PrintLn("    - sint64Arr: %lld %lld %lld %lld",
                 sint64Arr[0], sint64Arr[1], sint64Arr[2], sint64Arr[3]);
    LLBC_PrintLn("    - uint64Arr: %llu %llu %llu %llu",
                 uint64Arr[0], uint64Arr[1], uint64Arr[2], uint64Arr[3]);
    LLBC_PrintLn("    - floatArr: %f %f %f %f",
                 floatArr[0], floatArr[1], floatArr[2], floatArr[3]);
    LLBC_PrintLn("    - dblArr: %f %f %f %f",
                 dblArr[0], dblArr[1], dblArr[2], dblArr[3]);

    return LLBC_OK;
}

int TestCase_Com_Stream::TrivialClsSerTest()
{
    LLBC_PrintLn("Trivial type ser test:");

    TrivialCls trivialObj;
    trivialObj.boolVal = true;
    trivialObj.int32Val = -32;
    trivialObj.uint32Val = 32;
    trivialObj.int64Val = -64;
    trivialObj.uint64Val = 64;
    trivialObj.floatVal = 1.2345f;
    trivialObj.doubleVal = 5.4321;

    LLBC_Stream stream;
    LLBC_PrintLn("- Serialize trivial obj:%s", trivialObj.ToString().c_str());

    stream << trivialObj;
    LLBC_PrintLn("- After serialize, stream:%s", stream.ToString().c_str());

    stream.SetPos(0);
    memset(&trivialObj, 0, sizeof(trivialObj));
    stream >> trivialObj;
    LLBC_PrintLn("- After deserialize, trivial obj:%s", trivialObj.ToString().c_str());

    return LLBC_OK;
}

int TestCase_Com_Stream::SerializableClsSerTest()
{
    LLBC_PrintLn("Serializable type ser test:");

    LLBC_Random rander;

    SerializableCls serObj1;
    serObj1.bVal = rander.BoolJudge();
    serObj1.intVal = rander.Rand();

    GenRandStr(serObj1.str, rander, { 50, 200 });
    for (int i = 0; i < rander.Rand(1, 10); ++i)
        serObj1.strs.push_back(std::move(GenRandStr(rander, {50, 100})));

    for (int i = 0; i < rander.Rand(1, 10); ++i)
    {
        serObj1.int32Vec.push_back(rander.Rand());
        serObj1.int32List.push_back(rander.Rand());
        serObj1.int32Set.insert(rander.Rand());
        serObj1.int32USet.insert(rander.Rand());
        serObj1.int32ToStrMap.emplace(rander.Rand(), GenRandStr(rander, { 50, 100 }));
        serObj1.int32ToStrUMap.emplace(rander.Rand(), GenRandStr(rander, { 50, 100 }));
    }

    serObj1.arr[0] = GenRandStr(rander, { 50, 100 });
    serObj1.arr[1] = GenRandStr(rander, { 50, 100 });
    serObj1.arr[2] = GenRandStr(rander, { 50, 100 });

    serObj1.pa.first = rander.Rand();
    for (int i = 0; i < rander.Rand(1, 10); ++i)
        serObj1.pa.second.push_back(GenRandStr(rander, { 50, 100 }));

    for (int i = 0; i < rander.Rand(1, 10); ++i)
    {
        serObj1.complexVal.push_back(std::list<std::map<sint32, LLBC_String>>());

        auto &nest1Obj = serObj1.complexVal[i];
        for (int j = 0; j < rander.Rand(1, 10); ++j)
        {
            nest1Obj.push_back(std::map<sint32, LLBC_String>());
            auto &nest2Obj = *nest1Obj.rbegin();
            for (int k = 0; k < rander.Rand(1, 10); ++k)
                nest2Obj.emplace(rander.Rand(), GenRandStr(rander, { 50, 100 }));
        }
    }

    LLBC_PrintLn("- Original serializable obj:\n%s", serObj1.ToString().c_str());

    LLBC_Stream s;
    s << serObj1;
    LLBC_PrintLn("- Serialized to stream:%s", s.ToString().c_str());

    s.SetPos(0);
    SerializableCls serObj2;
    if (!s.Read(serObj2))
    {
        LLBC_FilePrintLn(stderr, "- Deserialize from stream failed");
        LLBC_SetLastError(LLBC_ERROR_UNKNOWN);
        return LLBC_FAILED;
    }

    LLBC_PrintLn("- Deserialized serializable obj:\n%s", serObj2.ToString().c_str());

    return LLBC_OK;
}

int TestCase_Com_Stream::MovableReadTest()
{
    LLBC_PrintLn("Movable read test:");

    LLBC_Random rander;

    MovableCls movableObj1;
    movableObj1.intVal = rander.Rand();
    movableObj1.strVal = GenRandStr(rander, { 50, 100 });

    LLBC_Stream stream;
    stream << movableObj1;
    LLBC_PrintLn("- After ser movable obj:");
    LLBC_PrintLn("  - stream:%s", stream.ToString().c_str());
    LLBC_PrintLn("  - movableObj1:%d %s", movableObj1.intVal, movableObj1.strVal.c_str());

    stream.SetPos(0);
    auto movableObj2 = stream.Read<MovableCls>();

    LLBC_PrintLn("- After called Read<MovableCls>():");
    LLBC_PrintLn("  - stream:%s", stream.ToString().c_str());
    LLBC_PrintLn("  - movableObj2:%d %s", movableObj2.intVal, movableObj2.strVal.c_str());

    return LLBC_OK;
}

LLBC_FORCE_INLINE void TestCase_Com_Stream::GenRandStr(
    LLBC_String &str, LLBC_Random &rander, const std::pair<int, int> &strLenRange)
{
    const int randStrLen = rander.Rand(strLenRange.first, strLenRange.second);
    for (int i = 0; i < randStrLen; ++i)
        str.append(1, static_cast<char>(rander.Rand(32, 127))); // [<space>, '~']
}

LLBC_FORCE_INLINE LLBC_String TestCase_Com_Stream::GenRandStr(
    LLBC_Random &rander, const std::pair<int, int> &strLenRange)
{
    LLBC_String randStr;
    GenRandStr(randStr, rander, strLenRange);

    return randStr;
}
