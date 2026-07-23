---
layout: default
title: 变体 Variant
---

# 变体 Variant

`LLBC_Variant` 是 llbc 提供的通用“变体”数据容器，用统一接口承载多种类型的数据：

- **原始值（RAW）**：`bool` / 有符号·无符号整数（8/16/32/64）/ `long` / `float` / `double` / `enum` / 指针
- **字符串（STR）**：`char[]` / `std::string` / `LLBC_String` / `LLBC_CString`
- **序列（SEQ）**：由 `pair` / `vector` / `list` / `deque` / `queue` / `set` 等构造
- **字典（DICT）**：由 `map` / `unordered_map` 等构造
- **变体（Variant）**：可嵌套 `LLBC_Variant`，构成任意层级的树形结构

它常用于配置、脚本桥接、DSL、动态数据结构等需要“弱类型”表达的场景。

## 构造与赋值

除拷贝/移动外，各类型构造均可从原生类型直接推导，无需显式指定变体类型：

```cpp
LLBC_Variant v1 = 123;                                  // RAW(sint32)
LLBC_Variant v2 = 3.14;                                 // RAW(double)
LLBC_Variant v3 = "hello";                              // STR
LLBC_Variant v4 = std::vector<int>{1, 2, 3};            // SEQ
LLBC_Variant v5 = std::map<int, std::string>{ {1, "a"} }; // DICT

LLBC_Variant v6;          // 默认构造：NIL（空值）
v6 = v1;                  // 拷贝赋值
v6 = std::move(v4);       // 移动赋值
```

## 类型判断

类型判断走模板方法 `Is<T>()`，内部只比较 16 位类型编码，**不依赖 RTTI**，复杂度 O(1)：

```cpp
v.Is<void>();                 // 是否为 NIL
v.Is<bool>();                 // 是否为 bool
v.Is<sint32>();               // 是否为 sint32
v.Is<LLBC_Variant::Str>();    // 是否为字符串
v.Is<LLBC_Variant::Seq>();    // 是否为序列
v.Is<LLBC_Variant::Dict>();   // 是否为字典

// 也支持一次判断多个候选类型（任一命中即为 true）
v.Is<sint32, sint64>();

// 大类判断（不区分子类型）
v.IsRaw();                    // 是否为原始值大类
v.IsSigned();                 // 原始值且为有符号
v.IsUnsigned();               // 原始值且为无符号
```

<div class="callout note" markdown="1">
`IsNil()` / `IsStr()` / `IsSeq()` / `IsDict()` 这几个旧成员已标记为 **deprecated**，请分别改用
`Is<void>()` / `Is<LLBC_Variant::Str>()` / `Is<LLBC_Variant::Seq>()` / `Is<LLBC_Variant::Dict>()`。
`GetType()` 可取回完整的 `LLBC_VariantType::ENUM`，`GetFirstType()` 只取大类。
</div>

## 取值

显式取值统一走模板方法 `As<T>()`，返回目标类型的值（必要时做类型转换）：

```cpp
auto i = v.As<sint32>();          // 取为 sint32
auto d = v.As<double>();          // 取为 double
auto s = v.As<LLBC_String>();     // 取为字符串
auto e = v.As<MyEnum>();          // 枚举属于 RAW，直接 As<枚举类型>()

const auto &seq  = v.As<LLBC_Variant::Seq>();   // 取序列引用（非 SEQ 时返回空序列）
const auto &dict = v.As<LLBC_Variant::Dict>();  // 取字典引用（非 DICT 时返回空字典）
```

<div class="callout note" markdown="1">
`As<std::string_view>()` / `As<LLBC_CString>()` / `As<const char *>()` 返回的是**视图/裸指针**，
其有效期仅限源变体未被修改期间，不要跨修改保存。
</div>

## 自动类型转换（operator T）

`LLBC_Variant` 提供了通用的隐式转换 `operator T()`，等价于调用 `As<T>()`：

```cpp
int         i = v;
double      d = v;
std::string s = v;
```

<div class="callout warn" markdown="1">
自动转换是为脚本 / 配置 / DSL 场景提供便利。在核心业务逻辑中建议显式使用 `As<T>()`，
避免隐式转换带来的可读性与二义性问题。
</div>

## 序列（Seq）

序列底层是 `std::vector<LLBC_Variant>`，支持 `foreach` 迭代与随机访问，并提供一组成员操作：
`SeqBegin` / `SeqEnd` / `SeqReverseBegin` / `SeqReverseEnd` / `SeqFront` / `SeqBack` /
`SeqInsert` / `SeqPushBack` / `SeqPopBack` / `SeqResize` / `SeqReserve` / `SeqErase`。

```cpp
LLBC_Variant v;
v.SeqPushBack(1);
v.SeqPushBack("hello");
v.SeqBatchPushBack(2, 3, 4);   // 一次追加多个元素

for (auto &e : v.As<LLBC_Variant::Seq>())
{
    // 逐个访问元素（每个元素仍是 LLBC_Variant）
}
```

<div class="callout note" markdown="1">
单值追加用 `SeqPushBack(val)`；变参一次追加多个用 `SeqBatchPushBack(vals...)`（`SeqInsert` 亦有对应的
`SeqBatchInsert`）。二者不要混用。
</div>

## 字典（Dict）

字典是 `LLBC_Variant` 最常用的形态，底层是 `std::map<LLBC_Variant, LLBC_Variant>`，
可通过 `operator[]` 读写 Key/Value，支持多层嵌套。除 `operator[]` 外还提供：
`DictBegin` / `DictEnd` / `DictReverseBegin` / `DictReverseEnd` / `DictInsert` / `DictFind` / `DictErase`。

```cpp
LLBC_Variant v;
v.Become<LLBC_Variant::Dict>();   // 显式转为字典（也可直接用 operator[] 写入）

v["id"]   = 123;
v["name"] = "llbc";

int id = v["id"];                 // 隐式转换取值

// 嵌套
v["attr"]["hp"] = 100;

for (auto &it : v.As<LLBC_Variant::Dict>())
{
    const LLBC_Variant &key = it.first;
    const LLBC_Variant &val = it.second;
}
```

## 通用操作与序列化

不区分具体类型的通用方法：

```cpp
v.Clear();                 // 按当前类型清空（RAW 归零 / STR·SEQ·DICT 清空）
v.IsEmpty();               // STR/SEQ/DICT 是否为空（NIL/RAW 恒为 true）
v.Size();                  // STR/SEQ/DICT 的元素数（NIL/RAW 恒为 0）
v.Contains(key);           // STR 子串 / SEQ 元素 / DICT 键 是否存在
v.Count(key);              // 匹配计数

LLBC_String s = v.ToString();          // 值的字符串表示
const LLBC_String &t = v.TypeToString(); // 类型的字符串表示

// 与 LLBC_Stream 互转
LLBC_Stream stream;
v.Serialize(stream);
LLBC_Variant v2;
stream.SetReadPos(0);
v2.Deserialize(stream);
```

## 设计与实现

### 1. 类型编码

变体类型由 `LLBC_VariantType::ENUM` 描述，采用 **16 位编码**：`[first type(高 8 位)][second type(低 8 位)]`。

First type（大类）：

- `RAW`：原始值
- `STR`：字符串
- `SEQ`：序列
- `DICT`：字典
- `NIL`：空值（编码为 `0x0000`）

Second type 用于细分子类型。以 RAW 为例，低 8 位进一步区分具体标量类型，
并用最低位（`MASK_RAW_SIGNED`）标记有无符号：

```cpp
// 位视图（RAW 大类）：[first type][raw type][signed/unsigned]
//                        8 bits     7 bits       1 bit
RAW_BOOL    = 0x0102,
RAW_SINT32  = 0x010d,   // 有符号
RAW_UINT32  = 0x010e,   // 无符号
RAW_DOUBLE  = 0x011d,
// ...
```

由此，只需对类型编码做一次位运算（`GetFirstType()` = `type & MASK_FIRST_TYPE`）即可判断大类，
进而决定数据的存储位置与访问方式。**无 RTTI、无虚函数、无 `typeid`**，类型判断为 O(1)。

### 2. 数据存储

`LLBC_Variant` 用 **union + placement new** 存储数据，兼顾内存开销与访问性能：

```cpp
union Data
{
    union RawType          // 原始值：统一按 64 位存储
    {
        sint64 i64;
        uint64 ui64;
        double dbl;
    } raw;

    union ObjType          // 复杂对象：placement new 管理生命周期
    {
        Str  str;
        Seq  seq;
        Dict dict;
        ~ObjType() { }
    } obj;

    Data()  { }
    ~Data() { }
};
```

- **RawType**：所有标量统一用 64 位承载（`i64` / `ui64` / `dbl`），纯值语义，无堆分配。
- **ObjType**：`Str` / `Seq` / `Dict` 这类复杂对象通过 placement new 在 union 内原地构造，
  由变体在类型切换/析构时显式管理其生命周期（union 自身不自动析构成员）。

没有虚表，也没有为小对象额外做堆分配包装，绝大多数 RAW 类型开销等同于一个 64 位整数加一个类型标记。

## 参照

- 头文件：`llbc/include/llbc/core/variant/Variant.h`
- 功能测试：`tests/func_test/core/variant/FuncTest_Core_Variant.cpp`
- 单元测试：`tests/unit_test/core/variant/UnitTest_Core_Variant.cpp`
- 快速上手示例（可跑）：`tests/example/core/Example_Core_Variant.cpp`
