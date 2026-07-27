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

#include "core/Example_Core_Stream.h"

// Player sex enum
enum class PlayerSex
{
    Male,
    Female,
};

// Player data, serializable via compile-time detection
struct PlayerData
{
    // Player id
    uint64 playerId;
    // Player name
    LLBC_String playerName;

    // Player level
    int level;
    // Player sex
    PlayerSex sex;

    // ToString
    LLBC_String ToString() const
    {
        return LLBC_String().format(
            "PlayerData[%llu, name:%s, level:%d, sex:%s]",
            playerId,
            playerName.c_str(),
            level,
            sex == PlayerSex::Female ? "Female" : "Male");
    }

    // Serialization support
    void Serialize(LLBC_Stream &stream) const
    {
        stream << playerId
               << playerName
               << level
               << sex;
    }

    // Deserialization support
    bool Deserialize(LLBC_Stream &stream)
    {
        stream >> playerId
               >> playerName
               >> level
               >> sex;

        return true;
    }
};

int Example_Core_Stream::Run(int argc, char *argv[])
{
    // Define a stream object
    LLBC_Stream stream;
    // Inspect the default stream info via ToString()
    LLBC_PrintLn("Default stream: %s", stream.ToString().c_str());

    // Set byte order to network byte order; defaults to LLBC_DefaultEndian, which can be changed via the config in Config.h
    stream.SetEndian(LLBC_Endian::NetEndian);

    // ========================= Basic datatype serialize/deserialize =========================
    LLBC_PrintLn("Basic datatypes serialize/deserialize:");
    // Serialize numbers/string data
    stream << true // bool
           << -32 // int32
           << 32u // uint32
           << -64ll // int64
           << 64llu // uint64
           << 3.14f // float
           << 6.28 // double
           << "Hello World!"; // string
    // Print stream info
    LLBC_PrintLn("- After serialize numbers/string, stream: %s", stream.ToString().c_str());

    // Lightweight stream construction (attach from buffer or another stream).
    LLBC_Stream stream2;
    stream2.Attach(stream);
    LLBC_PrintLn("- Attach from stream, stream2: %s", stream2.ToString().c_str());

    // Deserialize numbers/string data
    bool boolVal = false;
    sint32 int32Val = 0;
    uint32 uint32Val = 0;
    sint64 int64Val = 0;
    uint64 uint64Val = 0;
    float floatVal = 0.0f;
    double doubleVal = 0.0;
    LLBC_String strVal;
    stream2 >> boolVal
            >> int32Val
            >> uint32Val
            >> int64Val
            >> uint64Val
            >> floatVal
            >> doubleVal
            >> strVal;
    // Print stream info and each deserialized number/string value (output via streaming)
    std::cout << "- After deserialize numbers/string, stream: " << stream << std::endl;
    LLBC_PrintLn("  - boolVal: %d", boolVal);
    LLBC_PrintLn("  - int32Val: %d", int32Val);
    LLBC_PrintLn("  - uint32Val: %d", uint32Val);
    LLBC_PrintLn("  - int64Val: %lld", int64Val);
    LLBC_PrintLn("  - uint64Val: %llu", uint64Val);
    LLBC_PrintLn("  - floatVal: %f", floatVal);
    LLBC_PrintLn("  - doubleVal: %f", doubleVal);
    LLBC_PrintLn("  - strVal: %s", strVal.c_str());
    LLBC_PrintLn("\n");

    // ========================= Stl container serialize/deserialize =========================
    LLBC_PrintLn("Stl container serialize/deserialize:");
    std::vector<std::map<LLBC_String, std::set<int> > > stlContainer = {
        {
            { "Key1", {1, 2, 3} },
            { "Key2", {4, 5, 6} }
        }
    };

    // reset stream read pointer
    // stream.SetReadPos(0); // no need to reset the read pointer; on SetWritePos(0) the read pointer is auto-clamped
    // reset stream write pointer
    stream.SetWritePos(0);

    // Perform read/write
    decltype(stlContainer) stlContainer2;
    stream << stlContainer >> stlContainer2;
    // Dump the re-deserialized result (print the complex container via LLBC_Variant)
    LLBC_PrintLn("- After write & read stl container: %s, container:", LLBC_GetTypeName(stlContainer));
    LLBC_PrintLn("  - %s", LLBC_Variant(stlContainer2).ValueToString().c_str());
    LLBC_PrintLn("\n");

    // ========================= User-defined class serialize =========================
    LLBC_PrintLn("User define class/struct serialize/deserialize:");
    PlayerData playerData {9527llu, "Judy", 99, PlayerSex::Female};

    stream.SetWritePos(0);
    PlayerData playerData2;
    stream << playerData >> playerData2;
    LLBC_PrintLn("- After write & read player data, player data:");
    LLBC_PrintLn("  - %s", playerData2.ToString().c_str());

    return LLBC_OK;
}
