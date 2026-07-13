---
layout: default
title: Hello World
---

# Hello World

写一个最小的 llbc 程序：初始化库、打印一行、清理退出。这一步的目的是理解 llbc 程序的
**统一入口约定**——任何使用 llbc 的进程都必须成对调用 `LLBC_Startup()` 与 `LLBC_Cleanup()`。

## 完整代码

```cpp
#include "llbc.h"
using namespace llbc;

int main(int argc, char *argv[])
{
    // 初始化 llbc 库
    LLBC_Startup();
    // 用 LLBC_Defer 保证在函数返回时自动清理（等价于在所有 return 前调用 LLBC_Cleanup()）
    LLBC_Defer(LLBC_Cleanup());

    // 打印一行（带换行）
    LLBC_PrintLn("Hello World!");

    return 0;
}
```

运行输出：

```
Hello World!
```

## 逐行说明

- **`#include "llbc.h"`** —— 一个头文件即引入整个框架。它按依赖顺序包含五大模块：
  common → core → comm → testcase → app（见[架构总览](../concepts/architecture.md)）。
- **`using namespace llbc;`** —— 所有公共类型都在 `llbc` 命名空间下（如 `LLBC_Service`、`LLBC_Stream`）。
- **`LLBC_Startup()`** —— 初始化框架。大多数公共 API 都假设库已初始化，**必须先调用它**。
- **`LLBC_Defer(LLBC_Cleanup())`** —— `LLBC_Defer` 会在当前作用域退出时执行传入语句，
  用它注册 `LLBC_Cleanup()` 可避免在每个 `return` 分支手动清理，也不会漏掉。
- **`LLBC_PrintLn(...)`** —— 框架提供的带换行打印宏（printf 风格格式化）。

<div class="callout warning" markdown="1">
**务必成对**：`LLBC_Startup()` 与 `LLBC_Cleanup()` 必须成对出现在进程边界。
未初始化就调用公共 API 属于未定义行为。
</div>

## 参照仓库中的现成例子

这段代码改编自仓库内的快速开始项目：

- `tests/quick_start/hello_world/QuickStart_HelloWorld.cpp` —— Hello World 本体
- `tests/quick_start/QuickStart.cpp` —— `main` 入口与库启停

你可以直接构建并运行 quick_start 看到效果：

```bash
make quick_start config=debug64
./output/gmake2/debug64/quick_start_debug
```

## 下一步

前往 [序列化 Stream](../guides/stream.md)，了解 llbc 的二进制序列化容器 `LLBC_Stream`。
