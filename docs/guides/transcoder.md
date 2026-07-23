---
layout: default
title: 编码转换 Transcoder
---

# 编码转换 Transcoder

`LLBC_Transcoder` 是 llbc 提供的文本编码转换工具类，所有方法均为静态方法，无需实例化。
它覆盖三类转换路径：多字节编码互转（如 UTF-8 ↔ GBK）、多字节转宽字符（UTF-16）、
宽字符转多字节，每类均支持内存字符串与文件两种输入/输出形式。
常见使用场景：接入 GBK 编码的旧协议、Windows 平台写文件前的编码规范化、日志落盘的字符集统一。

## 多字节互转（MultiByteToMultiByte）

在两种多字节编码之间直接转换，无需经过中间宽字符层。

```cpp
LLBC_String gbkStr = /* GBK 编码原始串 */ "";
LLBC_String utf8Str;

// GBK -> UTF-8（出参版本，返回 LLBC_OK/LLBC_FAILED）
if (LLBC_Transcoder::MultiByteToMultiByte("GBK", gbkStr, "UTF-8", utf8Str) != LLBC_OK)
{
    LLBC_PrintLn("转换失败: %s", LLBC_FormatLastError());
    return -1;
}

// 返回值版本（无出参，失败时返回空串，同样可通过 LLBC_GetLastError 查询）
LLBC_String result = LLBC_Transcoder::MultiByteToMultiByte("UTF-8", utf8Str, "GBK");
```

## 多字节转宽字符（MultiByteToWideChar）

将任意多字节编码的字符串转为宽字符串（`LLBC_WString`，即 `std::wstring`）。

```cpp
LLBC_String utf8Str = /* UTF-8 串 */ "";
LLBC_WString wideStr;

if (LLBC_Transcoder::MultiByteToWideChar("UTF8", utf8Str, wideStr) != LLBC_OK)
{
    LLBC_PrintLn("MultiByteToWideChar 失败: %s", LLBC_FormatLastError());
    return -1;
}

// 返回值版本
LLBC_WString ws = LLBC_Transcoder::MultiByteToWideChar("GBK", gbkStr);
```

## 宽字符转多字节（WideCharToMultiByte）

将宽字符串转回指定编码的多字节字符串。

```cpp
LLBC_WString wideStr = /* 宽字符串 */ L"";
LLBC_String utf8Out;

if (LLBC_Transcoder::WideCharToMultiByte("UTF8", wideStr, utf8Out) != LLBC_OK)
{
    LLBC_PrintLn("WideCharToMultiByte 失败: %s", LLBC_FormatLastError());
    return -1;
}

// 返回值版本
LLBC_String s = LLBC_Transcoder::WideCharToMultiByte("GBK", wideStr);
```

## 文件 I/O 变体

每类转换都有四种签名：字符串↔字符串（基本形式）、文件→字符串、字符串→文件、文件→文件。
文件变体直接读写磁盘，适合批量转换大文件。

```cpp
// UTF-16 文件 -> UTF-8 文件（宽字符文件 -> 多字节文件）
if (LLBC_Transcoder::WideCharFileToMultiByteFile(
        "UTF8", "input_utf16.txt", "output_utf8.txt") != LLBC_OK)
{
    LLBC_PrintLn("文件转换失败: %s", LLBC_FormatLastError());
    return -1;
}

// UTF-16 文件 -> GBK 文件
if (LLBC_Transcoder::WideCharFileToMultiByteFile(
        "GBK", "input_utf16.txt", "output_gbk.txt") != LLBC_OK)
{
    LLBC_PrintLn("文件转换失败: %s", LLBC_FormatLastError());
    return -1;
}
```

文件变体的完整签名对照（以 `WideChar*` 系列为例，`MultiByte*` 系列结构相同）：

| 签名 | 输入 | 输出 |
|---|---|---|
| `WideCharToMultiByte(toCode, src, dest)` | `LLBC_WString` | `LLBC_String` |
| `WideCharFileToMultiByte(toCode, srcFile, dest)` | 文件路径 | `LLBC_String` |
| `WideCharToMultiByteFile(toCode, src, destFile)` | `LLBC_WString` | 文件路径 |
| `WideCharFileToMultiByteFile(toCode, srcFile, destFile)` | 文件路径 | 文件路径 |

## 编码名称写法

编码名称底层经由 `iconv`（Linux/macOS）或 Windows 代码页 API 解析，大小写通常不敏感，
但建议使用测试中已验证的形式：

| 编码 | 推荐写法 | 测试中出现的等价写法 |
|---|---|---|
| UTF-8 | `"UTF-8"` | `"UTF8"` |
| GBK（GB2312 超集） | `"GBK"` | — |
| BIG5 | `"BIG5"` | — |

<div class="callout note" markdown="1">
代码页名称由底层 `iconv` 或平台 API 匹配，不同操作系统支持的别名集合略有差异。
跨平台代码建议统一用标准写法（`"UTF-8"`、`"GBK"`），并在 CI 各平台均验证转换结果。
</div>

## 错误处理

所有出参版本转换失败时返回 `LLBC_FAILED`（即 `-1`），错误原因通过标准接口查询：

```cpp
if (LLBC_Transcoder::MultiByteToMultiByte("GBK", src, "UTF-8", dest) != LLBC_OK)
{
    // LLBC_FormatLastError() 返回人可读的错误描述
    LLBC_PrintLn("转换失败: %s", LLBC_FormatLastError());
}
```

返回值版本在失败时返回空串（`LLBC_String()` 或 `LLBC_WString()`），同样可用 `LLBC_GetLastError()` 查询错误码。

<div class="callout warning" markdown="1">
`LLBC_Transcoder` 的构造函数为 `private`，不可实例化，只能通过静态方法调用。
所有方法均为线程安全（无共享可变状态），可在多线程中并发调用。
</div>

## 参照

- 头文件：`llbc/include/llbc/core/transcoder/Transcoder.h`
- func_test 示例：`tests/func_test/core/transcoder/FuncTest_Core_Transcoder.cpp`
- 快速上手示例（可跑）：`tests/example/core/Example_Core_Transcoder.cpp`

## 下一步

- [序列化 Stream](stream.md) — 若需把转换结果写入网络包，可直接送入 `LLBC_Stream`
- [日志系统 Logger](logger.md) — 日志落盘前的字符集统一处理
