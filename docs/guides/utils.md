---
layout: default
title: 工具箱 utils
---

# 工具箱 utils

llbc `core/utils/` 模块提供了一组独立的小工具：Base64 编解码、MD5 摘要、通用绝对值、
数字字符串互转、IPv4 地址校验、hex dump 与高精度秒表。这些工具均为静态方法或自由函数，
无需额外初始化，在 `LLBC_Startup()` 之后即可直接使用。

## Base64 编解码

`LLBC_Base64` 提供字节序列到 Base64 字符串的对称编解码。返回值重载（直接返回字符串）
适合单次调用；`int` 重载把结果写入已有缓冲区，适合批量/热路径。

```cpp
#include "llbc/core/utils/Util_Base64.h"

std::string plain  = "hello, world!@#$%^&*()_+`";
std::string encoded, decoded;

// 编码
LLBC_Base64::Encode(plain, encoded);      // encoded = "aGVsbG8sIHdvcmxkIUAjJCVeJiooKV8rYA=="

// 解码
LLBC_Base64::Decode(encoded, decoded);   // decoded == plain

// 也可直接返回字符串（内部分配）
LLBC_String enc = LLBC_Base64::Encode(LLBC_String("llbc"));
```

<div class="callout note" markdown="1">
`Encode` / `Decode` 成功返回 `LLBC_OK`（0），失败返回 `LLBC_FAILED`（-1）并设置
`LLBC_GetLastError()`。空输入合法，返回空字符串。
</div>

## MD5 摘要

`LLBC_MD5` 支持对字符串、原始字节块、文件三种来源计算 MD5。`Digest()` 返回原始 16 字节，
`HexDigest()` 返回 32 位小写十六进制字符串。

```cpp
#include "llbc/core/utils/Util_MD5.h"

LLBC_String str = "Hello World";

// 对字符串求 hex 摘要
LLBC_String hex = LLBC_MD5::HexDigest(str);
// hex == "b10a8db164e0754105b7a99be72e3fe5"

// 对原始字节求摘要
const char buf[] = {0x01, 0x02, 0x03};
LLBC_String raw = LLBC_MD5::Digest(buf, sizeof(buf));   // 16 字节二进制

// 对文件求 hex 摘要
LLBC_String fileHex = LLBC_MD5::FileHexDigest("config.ini");
```

## 绝对值 LLBC_Abs

模板自由函数，支持所有算术类型，对 `float` / `double` / `ldouble` 特化为
`fabsf` / `fabs` / `fabsl`，避免截断。

```cpp
#include "llbc/core/utils/Util_Math.h"

int   a = LLBC_Abs(-42);       // 42
float b = LLBC_Abs(-3.14f);    // 3.14f
```

## 数字字符串互转

`Util_Text.h` 提供三组自由函数，覆盖整型、浮点、枚举、指针。

```cpp
#include "llbc/core/utils/Util_Text.h"

// 数字 → 字符串
LLBC_String s  = LLBC_Num2Str(12345u);             // "12345"
LLBC_String sx = LLBC_Num2Str<uint32, true>(255u); // "0xff"

// 数字 → const char*（结果存于 TLS 缓冲区，无额外分配，性能更高）
size_t len;
const char *cs = LLBC_Num2Str2(-100, &len);        // "-100"

// 字符串 → 数字（返回 0 时需检查 LLBC_GetLastError()，0 也可能是合法结果）
sint64 n = LLBC_Str2Num<sint64>("10086");

// 宽松 bool 解析：识别 "true" / "yes" / 非零数字
bool b = LLBC_Str2LooseBool("yes");   // true
bool c = LLBC_Str2LooseBool("0");     // false
```

<div class="callout note" markdown="1">
`LLBC_Str2Num` 在转换**成功**且返回值非 0 时不修改 `LLBC_GetLastError()`；
返回 0 时需调用 `LLBC_GetLastError()` 确认是否出错（0 本身也是合法值）。
</div>

## IPv4 地址校验

`LLBC_IsIPv4Addr` 仅校验**格式**（四段点分十进制），不做 DNS 解析。

```cpp
#include "llbc/core/utils/Util_Network.h"

LLBC_IsIPv4Addr("127.0.0.1");       // true
LLBC_IsIPv4Addr("255.255.255.255"); // true
LLBC_IsIPv4Addr("www.baidu.com");   // false
LLBC_IsIPv4Addr("10.1.1.1.222");    // false
```

## Hex Dump 与秒表

### LLBC_Byte2Hex

把字节数组格式化为十六进制字符串，常用于协议报文调试。

```cpp
#include "llbc/core/utils/Util_Debug.h"

char buf[16];
for (int i = 0; i < 16; ++i) buf[i] = static_cast<char>(i);

// byteSep=' ', lineWidth=8：每字节空格隔开，每 8 字节换行
LLBC_String hex = LLBC_Byte2Hex(buf, 16, ' ', 8);
// "00 01 02 03 04 05 06 07\n08 09 0a 0b 0c 0d 0e 0f"
```

### LLBC_Stopwatch

高精度秒表，构造时默认自动开始计时。支持 `Pause` / `Resume` / `Reset` / `Restart`。
在支持 RDTSC 的平台（x86/x64）上会启用硬件计数器。

```cpp
#include "llbc/core/utils/Util_Debug.h"

LLBC_Stopwatch sw;           // 构造即开始计时

DoSomework();

sw.Pause();
// ... 暂停期间的耗时不被计入 ...
sw.Resume();

LLBC_TimeSpan elapsed = sw.Elapsed();      // 以 LLBC_TimeSpan 返回
uint64       nanos    = sw.ElapsedNanos(); // 纳秒数
LLBC_PrintLn("耗时: %s", sw.ToString().c_str());
```

### trace / traceline 宏

仅在 `LLBC_DEBUG` 宏定义时生效，Release 构建自动变为空语句，不产生任何开销。

```cpp
trace("value = %d", x);        // 不换行
traceline("value = %d", x);    // 自动换行
```

<div class="callout warning" markdown="1">
`trace` / `traceline` 是条件编译宏，仅 Debug 构建有输出。不要在 Release 路径依赖这两个宏
的副作用（如函数调用的返回值）——它们在 Release 下被完全剔除。
</div>

## 参照

头文件：
- `llbc/include/llbc/core/utils/Util_Base64.h`
- `llbc/include/llbc/core/utils/Util_MD5.h`
- `llbc/include/llbc/core/utils/Util_Math.h`
- `llbc/include/llbc/core/utils/Util_Text.h`
- `llbc/include/llbc/core/utils/Util_Network.h`
- `llbc/include/llbc/core/utils/Util_Debug.h`

tests/ 真实示例：
- `tests/func_test/core/utils/FuncTest_Core_Utils_Base64.cpp`
- `tests/func_test/core/utils/FuncTest_Core_Utils_MD5.cpp`
- `tests/func_test/core/utils/FuncTest_Core_Utils_Text.cpp`
- `tests/func_test/core/utils/FuncTest_Core_Utils_Network.cpp`
- `tests/func_test/core/utils/FuncTest_Core_Utils_Debug.cpp`
- 快速上手示例（可跑）：`tests/example/core/Example_Core_Utils.cpp`

## 下一步

- [序列化 Stream](stream.md) — 与 `LLBC_Byte2Hex` 配合调试二进制数据帧
- [时间 Time](time.md) — `LLBC_TimeSpan` 与 `LLBC_Stopwatch` 的计时单位一致
- [字符串 String](string.md) — `LLBC_String` 作为工具函数的主要返回类型
