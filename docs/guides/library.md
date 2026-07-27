---
layout: default
title: 动态库 Library
---

# 动态库 Library

`LLBC_Library` 是 llbc 对跨平台动态库加载的封装，统一了 Linux/macOS 的 `dlopen`/`dlsym`/`dlclose`
与 Windows 的 `LoadLibrary`/`GetProcAddress`/`FreeLibrary`。适用于插件化架构、运行时加载扩展模块、
以及需要按需调用系统库函数的场景。`LLBC_Library` 不可拷贝也不可移动（用户声明的析构函数抑制了隐式移动生成），析构自动关闭句柄。

## 加载动态库

调用 `Open` 传入库文件名，成功返回 `LLBC_OK`，失败返回 `LLBC_FAILED` 并可通过
`LLBC_FormatLastError()` 取错误原因。

```cpp
#include "llbc/llbc.h"

LLBC_Library lib;

#if LLBC_TARGET_PLATFORM_WIN32
const char *libName = "Kernel32.dll";
#elif LLBC_TARGET_PLATFORM_MAC
const char *libName = "libm.dylib";
#else
const char *libName = "libm.so";
#endif

if (lib.Open(libName) != LLBC_OK)
{
    LLBC_PrintLn("打开动态库失败: %s", LLBC_FormatLastError());
    return -1;
}
```

<div class="callout note" markdown="1">
文件名规则与平台一致：Linux 传 `libfoo.so`（或带版本号 `libfoo.so.1`），macOS 传
`libfoo.dylib`，Windows 传 `foo.dll`。相对路径由操作系统 LD 搜索路径决定；建议传绝对路径或仅传
文件名配合环境变量（`LD_LIBRARY_PATH` / `PATH`）。
</div>

## 获取符号地址

`GetProcAddress` 返回 `LLBC_LibraryFun`（函数指针类型），失败返回 `nullptr`。需要手动
`reinterpret_cast` 或 C 风格强转到目标函数签名后再调用。

```cpp
LLBC_LibraryFun fun = lib.GetProcAddress("cos");
if (fun == nullptr)
{
    LLBC_PrintLn("获取符号失败: %s", LLBC_FormatLastError());
    return -1;
}

// 转换为实际函数签名后调用
typedef double (*CosFn)(double);
double result = reinterpret_cast<CosFn>(fun)(2.0);
LLBC_PrintLn("cos(2.0) = %f", result);
```

Windows 示例（取 `Kernel32.dll` 中的 `lstrlenA`）：

```cpp
typedef int (WINAPI *LstrlenFn)(LPCSTR);
LLBC_LibraryFun fun = lib.GetProcAddress("lstrlenA");
int len = reinterpret_cast<LstrlenFn>(fun)("Hello");
LLBC_PrintLn("lstrlenA(\"Hello\") = %d", len);
```

<div class="callout warning" markdown="1">
`LLBC_LibraryFun` 定义为 `int (*)(void)`（非 Win32）或 `int (__cdecl *)()`（Win32），
仅作占位类型使用。**在调用前务必转换到目标签名**，否则传参/返回值解析会出现未定义行为。
</div>

## 加载自身进程

向 `Open` 传入 `nullptr` 可打开调用进程自身的符号表（等价于 `dlopen(nullptr, ...)`），
适合插件框架在宿主进程中按名称查找已注册的工厂函数。

```cpp
LLBC_Library selfLib;
if (selfLib.Open(nullptr) != LLBC_OK)
{
    LLBC_PrintLn("打开自身进程失败: %s", LLBC_FormatLastError());
    return -1;
}
// 随后可用 GetProcAddress 查找本进程导出的符号
```

<div class="callout note" markdown="1">
Windows 上 `Open(nullptr)` 内部调用 `GetModuleHandle(nullptr)`，返回主模块句柄，可枚举
主 EXE 的导出符号，但无法查到未导出（无 `__declspec(dllexport)`）的内部函数。
</div>

## 卸载动态库

`Close` 释放底层句柄并将内部 `_handle` 置为无效值。`LLBC_Library` 析构时若句柄仍有效会自动调用
`Close`，通常无需手动调用，但在需要提前释放资源时可显式调用。

```cpp
if (lib.Close() != LLBC_OK)
    LLBC_PrintLn("关闭动态库失败: %s", LLBC_FormatLastError());
```

<div class="callout warning" markdown="1">
卸载库后，所有通过该库 `GetProcAddress` 取得的函数指针立即失效。继续调用这些指针属于未定义行为，
通常导致程序崩溃。确保在卸载前已不再持有指向该库的函数指针。
</div>

## 线程安全

`LLBC_Library` 实例本身**不是线程安全的**。不要在多个线程间共享同一 `LLBC_Library` 对象。
如需并发访问，在外部加锁或为每个线程维护独立实例。

## 参照

- 头文件：`llbc/include/llbc/core/library/Library.h`
- 底层 OS 封装：`llbc/include/llbc/core/os/OS_Library.h`（`LLBC_LoadLibrary` / `LLBC_GetProcAddress` / `LLBC_CloseLibrary`）
- 类型定义：`llbc/include/llbc/common/BasicDataType.h`（`LLBC_LibraryHandle`、`LLBC_LibraryFun`）
- 功能测试：`tests/func_test/core/library/FuncTest_Core_Library.cpp`
- 快速上手示例（可跑）：`tests/example/core/Example_Core_Library.cpp`

## 下一步

- [文件与目录](file-directory.md) — 跨平台文件/目录操作
- [对象池 ObjPool](objpool.md) — 高性能对象复用
