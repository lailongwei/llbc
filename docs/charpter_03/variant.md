## 变体 - LLBC_Variant

### 概述

LLBC_Variant是 LLBC 框架提供的通用“变体”数据容器, 用于承载多种不同类型的数据, 并提供统一的访问接口:
- 基本数值类型(bool / int / uint / float / double / enum / pointer)
- 字符串(char[] / string / LLBC_String / LLBC_CString)
- 序列(pair / vector / list / deque / queue / set)
- 字典(map / unordered_map)
- 变体(variant)


### 使用示例

1. 构造与赋值
    ```cpp
    LLBC_Variant v1 = 123;
    LLBC_Variant v2 = 3.14;
    LLBC_Variant v3 = "hello";
    LLBC_Variant v4 = std::vector<int>{1, 2, 3};
    LLBC_Variant v5 = std::map<int, std::string>{{1, "a"}};
    ```
   所有构造函数 explicit(拷贝除外), 避免隐式类型污染.

2. 类型判断
    ```cpp
    v.IsNil();
    v.IsRaw();
    v.IsInt32();
    v.IsStr();
    v.IsSeq();
    v.IsDict();
    ```
    反射接口无 RTTI, 复杂度 O(1).

3. 取值
    ```cpp
    auto i = v.AsInt32();
    auto d = v.AsDouble();
    auto s = v.AsStr();
    auto e = v.AsEnum<MyEnum>();
   ```
4. 自动类型转换(operator T)
    ```cpp
    int i = v;
    double d = v;
    std::string s = v;
    ```
    自动类型转换是为了 脚本 / 配置 / DSL 场景提供便利性, 在核心逻辑中建议显式 AsXXX().

5. 序列(Seq)

    ```cpp
    LLBC_Variant v;
    v.SeqPushBack(1, 2, 3, "hello");
    for (auto &e : v.AsSeq())
        // ...
    ```
    Seq 实现是基于 std::vector<LLBC_Variant>, 支持 foreach 迭代访问和随机访问, 同时支持以下常用操作:
    
    Begin / End / ReverseBegin / ReverseEnd / Front / Back / Insert / PushBack / PopBack / Resize / Reserve / Erase.

6. 字典(Dict)
    ```cpp
    LLBC_Variant v;
    v.BecomeDict();
    
    v["id"] = 123;
    v["name"] = "llbc";
    
    int id = v["id"];
    ```
    Dict 是 LLBC_Variant 最常用的使用方式, 支持通过 operator[] 访问和修改 Key / Value. 支持多层嵌套使用.
    Dict 实现是基于 std::map<LLBC_Variant, LLBC_Variant>, 支持 foreach 迭代访问, 同时支持以下常用操作:
    Operator[] / Begin / End / ReverseBegin / ReverseEnd / Insert / Find / Erase.


### 设计与实现

```cpp
// 16 bit 类型编码
LLBC_VariantType::ENUM type;

// 数据存储
union DataType
{
    union RawType
    {
        sint64 int64Val;
        uint64 uint64Val;
        double doubleVal;
    } raw;

    union ObjType
    {
        Str str;
        Dict dict;
        Seq seq;
    } obj;

    DataType() {}
    ~DataType() {};
} data;
```

1. **类型编码**

    LLBC_Variant 的类型由 LLBC_VariantType::ENUM 描述, 采用 16bit 编码: `[first type(8bit)][second type(8bit)]`

    - **First Type(大类):**
      - RAW: 原始值
      - STR: 字符串
      - SEQ: 序列
      - DICT: 字典
      - NIL: 空值

    - **Raw 子类型:**
      - 有符号 / 无符号整数(8/16/32/64)
      - float / double
      - bool
      - pointer
      - enum

  通过类型编码, 可以快速判断数据所属大类, 进而决定数据存储位置(data.raw / data.obj)及访问方式. 无 RTTI, 无虚函数, 无 typeid.

2. **数据存储**

    LLBC_Variant 采用 union + placement new 方式存储数据, 以最大限度减少内存开销及提升访问性能.
      - RawType: 统一使用 64bit 存储.
      - ObjType: 使用 placement new 管理 String / Seq / Dict 复杂对象的生命周期.
    
    没有虚表, 没有堆分配的小对象包装, 大部分 raw 类型是纯值语义.