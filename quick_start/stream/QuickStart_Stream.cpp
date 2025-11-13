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

#include "stream/QuickStart_Stream.h"

// 玩家性别enum
enum class PlayerSex
{
    Male,
    Female,
};

// 玩家数据, 可通过编译期探测进行序列化
struct PlayerData
{
    // 玩家Id
    uint64 playerId;
    // 玩家名
    LLBC_String playerName;

    // 玩家等级
    int level;
    // 玩家性别
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

    // 序列化支持
    bool Serialize(LLBC_Stream &stream) const
    {
        LLBC_STREAM_BEGIN_WRITE(stream, false);
        LLBC_STREAM_BATCH_WRITE(playerId, playerName, level, sex);
        LLBC_STREAM_END_WRITE_RET(true);
    }

    // 反序列化支持
    bool Deserialize(LLBC_Stream &stream)
    {
        LLBC_STREAM_BEGIN_READ(stream, false);
        LLBC_STREAM_BATCH_READ(playerId, playerName, level, sex);
        LLBC_STREAM_END_READ_RET(true);
    }
};

int QuickStart_Stream::Run(int argc, char *argv[])
{
    // 初始化llbc框架
    LLBC_Startup();
    // Defer清理llbc框架
    LLBC_Defer(LLBC_Cleanup());

    // 定义一个流对象
    LLBC_Stream stream;
    // 通过ToString()方法看默认Stream信息
    LLBC_PrintLn("Default stream: %s", stream.ToString().c_str());

    // 设置字节序为networld byte order, 默认为LLBC_DefaultEndian, 可通过Config.h中的配置修改默认字节序
    stream.SetEndian(LLBC_Endian::NetEndian);

    // ========================= 基础数据类型序列化, 反序列化 =========================
    LLBC_PrintLn("Basic datatypes serialize/deserialize:");
    // 序列化numbers/string数据
    stream << true // bool
           << -32 // int32
           << 32u // uint32
           << -64ll // int64
           << 64llu // uint64
           << 3.14f // float
           << 6.28 // double
           << "Hello World!"; // string
    // 打印stream信息
    LLBC_PrintLn("- After serialize numbers/string, stream: %s", stream.ToString().c_str());

    // 轻量级构造stream(attach from buffer or another stream).
    LLBC_Stream stream2;
    stream2.Attach(stream);
    LLBC_PrintLn("- Attach from stream, stream2: %s", stream2.ToString().c_str());

    // 反序列化numbers/string数据
    bool boolVal;
    sint32 int32Val;
    uint32 uint32Val;
    sint64 int64Val;
    uint64 uint64Val;
    float floatVal;
    double doubleVal;
    LLBC_String strVal;
    stream2 >> boolVal
            >> int32Val
            >> uint32Val
            >> int64Val
            >> uint64Val
            >> floatVal
            >> doubleVal
            >> strVal;
    // 打印stream信息及反序列化后的各numbers/string值(使用streaming方式输出)
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

    // ========================= stl容器序列化/反序列化 =========================
    LLBC_PrintLn("Stl container serialize/deserialize:");
    std::vector<std::map<LLBC_String, std::set<int> > > stlContainer = {
        {
            { "Key1", {1, 2, 3} },
            { "Key2", {4, 5, 6} }
        }
    };

    // reset stream读指针
    // stream.SetReadPos(0); // 无需reset read指针, SetWritePos(0)时, read指针将自动规整
    // reset stream写指针
    stream.SetWritePos(0);

    // 执行读写
    decltype(stlContainer) stlContainer2;
    stream << stlContainer >> stlContainer2;
    // Dump重新反序列化出来的结果(通过LLBC_Variant打印复杂容器)
    LLBC_PrintLn("- After write & read stl container: %s, container:", LLBC_GetTypeName(stlContainer));
    LLBC_PrintLn("  - %s", LLBC_Variant(stlContainer2).ValueToString().c_str());
    LLBC_PrintLn("\n");

    // ========================= 用户自定义类序列化 =========================
    LLBC_PrintLn("User define class/struct serialize/deserialize:");
    PlayerData playerData {9527llu, "Judy", 99, PlayerSex::Female};

    stream.SetWritePos(0);
    PlayerData playerData2;
    stream << playerData >> playerData2;
    LLBC_PrintLn("- After write & read player data, player data:");
    LLBC_PrintLn("  - %s", playerData2.ToString().c_str());

    return LLBC_OK;
}
