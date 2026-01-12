## 流操作 - LLBC_Stream

## 简述
日常开发过程中，对字节流操作、对象序列化、反序列化非常高频使用, llbc提供了非常强大的流对象`LLBC_Stream`来帮助开发者完成：字节流操作、对象序列化、反序列化。特别地，对于对象序列化、反序列化: `LLBC_Stream`将自动化进行字节序转换操作及序列化方法的编译期探测。

源码 - [quick_start - stream](https://github.com/lailongwei/llbc/tree/master/quick_start/stream)

## 代码及输出
```cpp
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
    void Serialize(LLBC_Stream &stream) const
    {
        stream << playerId
               << playerName
               << level
               << sex;
    }

    // 反序列化支持
    bool Deserialize(LLBC_Stream &stream)
    {
        stream >> playerId
               >> playerName
               >> level
               >> sex;

        return true;
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
```

输出:

![02-stream-output](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/quick_start/02-stream-output.png)

## 引入的知识点
`LLBC_Stream`核心设计:
* 内部核心成员:
    * `void *_buf`: buffer指针
    * `size_t _cap`: stream buffer容量
    * `size_t _wpos`: stream写指针, 要求[0, _cap], 在重新调整`_cap`时将自动规整
    * `size_t _rpos`: stream读指针, 要求[0, _wpos], 在更新`_wpos`时将自动规整

* QuickSart完成的功能:
    * 展示`LLBC_Stream`的基础序列化能力、字节序自动支持能力
    * 特别地: 展示了基于方法探测的序列化能力(`PlayerData`)

* QuickStart引入的知识点:
    * `LLBC_Stream`可自动化完成所有基础数据类型的序列化(int/double/string/...)
    * `LLBC_Stream`可自动化完成绝大部分stl containers的序列化及嵌套容器序列化, 如: `std::vector<std::list<std::map<int, std::string>>>`
    * `LLBC_Stream`可自动探测目标类是否有实现序列化方法、反序列化方法, 并在编译期探测
    * [示例代码未展示]`LLBC_Stream`在序列化用户自定义类时, 如果无序列化、反序列化方法被探测到, 将进行默认的`memcpy(buf, this, sizeof(*this))`操作
    * [示例代码未展示]`LLBC_Stream`可探测目标对象是`protobuf 2` or `protobuf 3`的`message`, 进行正确的序列化、反序列化
    * [示例代码未展示]对以上所有支持的序列化方式的类的任何组合、嵌套, `LLBC_Stream`均可完成序列化

