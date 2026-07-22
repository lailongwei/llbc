---
layout: default
title: 字符串 String / CString
---

# 字符串 String / CString

`LLBC_String` 是 llbc 的主力字符串类型，继承自 `std::basic_string<char>`，在完整保留标准库接口的基础上增加了 printf 风格格式化、切分、去空白、大小写转换、转义、UTF-8 字符计数等实用扩展。`LLBC_CString` 是轻量只读字符串包装，仅存储指针和长度，**不分配堆内存**，适合作为 `std::unordered_map` 键或函数入参，以避免临时字符串构造开销。两者定义于 `llbc/include/llbc/common/StringDataType.h`。

## 类型别名速查

```cpp
LLBC_String   // LLBC_BasicString<char>   — 可变字符串（含扩展方法）
LLBC_WString  // LLBC_BasicString<wchar>  — 宽字符变体
LLBC_Strings  // std::vector<LLBC_String> — 常见于 split 返回值
LLBC_CString  // LLBC_BasicCString<char>  — 只读常量字符串
LLBC_CWString // LLBC_BasicCString<wchar> — 宽字符只读变体
```

## 格式化：format / append_format

`format` 以 printf 格式原地重置内容；`append_format` 追加到末尾。两者均内部调用 `vsnprintf` 计算精确长度，只分配一次内存。

```cpp
LLBC_String s;
s.format("player %s, level %d", "Judy", 99);
// s == "player Judy, level 99"

s.append_format(", score %d", 12000);
// s == "player Judy, level 99, score 12000"
```

## 切分：split

按字符串或单字符分隔，返回 `LLBC_Strings`（即 `std::vector<LLBC_String>`）。`max_split` 限制切分次数，`strip_empty` 丢弃空段。

```cpp
LLBC_String path("com.packet.battle.");
LLBC_Strings parts = path.split(".");
// parts == ["com", "packet", "battle", ""]

// 限制切 2 刀，忽略空段
LLBC_Strings p2 = path.split(".", 2, false, true);
// p2 == ["com", "packet", "battle."]
```

`with_elem = true` 时分隔符参数被视为字符集合（按任意一个字符切分）：

```cpp
LLBC_Strings words = LLBC_String("a,b;c").split(",;", -1, true, false);
// words == ["a", "b", "c"]
```

## 去空白：strip / lstrip / rstrip

无参数时去除空白字符（空格、Tab、换行等）；传入字符串参数时只去除该字符集中的字符。成员函数版本原地修改并返回 `*this`；const 版本返回副本。

```cpp
LLBC_String s = "\t  Hello World!  \n";
s.strip();          // s == "Hello World!"

LLBC_String r = s.lstrip("H");  // 非 const 版本：s 原地修改，s 与 r 均为 "ello World!"
```

## 大小写转换：tolower / toupper

返回新字符串，原字符串不变。仅处理 ASCII 范围（0x41–0x5A / 0x61–0x7A）。

```cpp
LLBC_String s = "Hello WoRlD!";
LLBC_String lo = s.tolower();  // "hello world!"
LLBC_String up = s.toupper();  // "HELLO WORLD!"
```

## 前后缀检测：startswith / endswith

```cpp
LLBC_String s = "Hello world!";
s.startswith("Hello");  // true
s.endswith("world!");   // true
s.endswith("World!");   // false  （大小写敏感）
```

## 查找替换：findreplace

原地将所有 `s1` 替换为 `s2`；字符版本替换单个字符。

```cpp
LLBC_String s = "Hello World, Hello World";
s.findreplace("Hel", "HEL");
// s == "HELlo World, HELlo World"
```

## 字符串重复：operator*

```cpp
LLBC_String sep = "-";
LLBC_String line = sep * 20;  // "--------------------"
```

## 字符类型判断

```cpp
LLBC_String::isalpha("abc");   // true — 全字母
LLBC_String::isupper("ABC");   // true — 至少含一个大写字母且不含小写字母（非字母字符被忽略）
LLBC_String::islower("abc");   // true
LLBC_String s("hello123");
s.isdigit();   // false
s.isalpha();   // false
```

## 转义 / 反转义：escape / unescape

```cpp
const LLBC_String sql = "it's a \"test\"";
LLBC_String escaped = sql.escape("\"'", '\\');
// escaped == "it\\'s a \\\"test\\\""

escaped.unescape('\\');
// 还原为原始字符串
```

## UTF-8 支持

`LLBC_String` 存储原始字节；以下方法提供 UTF-8 字符级操作，仅对 `char` 特化有效。

```cpp
LLBC_String utf8 = "\xe4\xbd\xa0\xe5\xa5\xbd";  // "你好"（6 字节）
utf8.length();            // 6（字节数）
utf8.length_with_utf8();  // 2（字符数）

LLBC_Strings chars;
utf8.split_utf8_string(chars);   // chars == {"你", "好"}
```

## LLBC_CString：只读常量字符串

`LLBC_CString` 不拥有内存，仅记录 `const char*` 指针与长度。从字符串字面量构造时在编译期确定长度（模板参数推导，无运行时 `strlen`）；从 `const char*`、`std::string`、`LLBC_String` 构造均支持。

```cpp
// 字面量构造（编译期长度）
LLBC_CString key = "PlayerName";

// 与 LLBC_String 互操作
LLBC_String name = "Judy";
LLBC_CString cname(name);           // 不拷贝
bool eq = (cname == "Judy");        // true
LLBC_String full = name + cname;    // "JudyJudy"
```

<div class="callout note" markdown="1">
`LLBC_CString` 的生命周期与被引用字符串绑定。不要将从临时 `LLBC_String` 构造的 `LLBC_CString` 存入容器或跨函数返回 —— 临时对象析构后指针即悬空。
</div>

### 用作 unordered_map 键

框架为 `LLBC_CString` 提供了 `std::hash` 特化（DJB 算法），可直接用于哈希容器：

```cpp
std::unordered_map<LLBC_CString, int> registry;
registry["HP"] = 100;
registry["MP"] = 50;
auto it = registry.find("HP");   // 无堆分配
```

<div class="callout warning" markdown="1">
**线程安全**：`LLBC_String` 与 `LLBC_CString` 本身不提供内部同步，并发读同一实例安全；并发写或读写混合需调用方加锁。
</div>

## 参照

- 头文件：`llbc/include/llbc/common/BasicString.h`
- 头文件：`llbc/include/llbc/common/BasicCString.h`
- 类型别名：`llbc/include/llbc/common/StringDataType.h`
- 功能测试：`tests/func_test/common/FuncTest_Com_DataType.cpp`

## 下一步

- 序列化与反序列化：[序列化 Stream](stream.md)
- 动态变量类型（含字符串存储）：[Variant](variant.md)
