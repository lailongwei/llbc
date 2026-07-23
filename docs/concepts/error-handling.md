---
layout: default
title: 错误处理与返回约定
---

# 错误处理与返回约定

llbc 采用**返回值 + 线程局部错误槽**的两阶段错误传递模型：绝大多数公开 API 返回
`LLBC_OK`（0）或 `LLBC_FAILED`（-1），失败时把原因写入线程局部的"最后错误"槽，调用者
立即用 `LLBC_GetLastError()` / `LLBC_FormatLastError()` 读取。这套约定适用于全部
模块；不要在自己的代码里发明 ad-hoc 整型返回码——需要新语义时请扩展
`llbc/include/llbc/common/Errno.h`。

## 基础约定：LLBC_OK / LLBC_FAILED

```cpp
// 调用任何返回 int 的 llbc API 的正确姿势
if (someApi(args) == LLBC_FAILED)
{
    // 失败后立即读取原因，跨调用后原因可能被覆盖
    LLBC_PrintLn("操作失败: %s", LLBC_FormatLastError());
    return LLBC_FAILED;
}
```

`LLBC_OK` 定义为 `0`，`LLBC_FAILED` 定义为 `-1`，均来自
`llbc/include/llbc/common/Macro.h`。

## 读取与格式化错误

| API | 签名 | 说明 |
|-----|------|------|
| `LLBC_GetLastError` | `int LLBC_GetLastError()` | 返回当前线程最后错误号 |
| `LLBC_FormatLastError` | `const char *LLBC_FormatLastError()` | 返回可打印的错误描述字符串 |
| `LLBC_StrError` | `const char *LLBC_StrError(int no)` | 将任意错误号转为描述，不依赖当前线程状态 |
| `LLBC_StrErrorEx` | `const char *LLBC_StrErrorEx(int no, int subErrno)` | 带子错误号的描述 |

```cpp
// 读取并打印错误
int err = LLBC_GetLastError();
LLBC_PrintLn("错误号: 0x%x, 描述: %s", err, LLBC_StrError(err));

// 直接格式化最后一次错误（更常见的写法）
LLBC_PrintLn("失败原因: %s", LLBC_FormatLastError());
```

## 设置错误

内部实现或上层封装需要向调用者报告失败时，通过 `LLBC_SetLastError` 写入错误槽，
然后返回 `LLBC_FAILED`：

```cpp
// 报告库内置错误码
LLBC_SetLastError(LLBC_ERROR_ARG);
return LLBC_FAILED;

// 携带可选的覆盖描述串（覆盖默认文字，调用者看到的是该字符串）
LLBC_SetLastError(LLBC_ERROR_FORMAT, "字段 'id' 必须为正整数");
return LLBC_FAILED;

// 枚举类型重载（enum class 同样适用）
LLBC_SetLastError(MyErrEnum::NotReady);
return LLBC_FAILED;
```

## 库错误码 vs C 库 / OS 错误

llbc 错误号是 32 位结构化值（字段：Severity、Custom 标志、Type、No），`LLBC_StrError`
会根据 Type 字段自动抓取正确的描述来源：

| Type | 宏常量 | 描述来源 |
|------|--------|---------|
| `0x0000` | `LLBC_ERROR_TYPE_LIB` | llbc 内置文本 |
| `0x0001` | `LLBC_ERROR_TYPE_CLIB` | `strerror(errno)` |
| `0x0002` | `LLBC_ERROR_TYPE_OSAPI` | `FormatMessage`（Windows 专有）|
| `0x0003` | `LLBC_ERROR_TYPE_NETAPI` | WSA 错误文本（Windows 专有）|
| `0x0004` | `LLBC_ERROR_TYPE_GAI` | `gai_strerror(subErrno)` |

```cpp
// C 库错误：先设置 errno，再设置错误槽
errno = EINVAL;
LLBC_SetLastError(LLBC_ERROR_CLIB);
LLBC_PrintLn("C库错误: %s", LLBC_FormatLastError());

// GAI 错误：需要配合子错误号
LLBC_SetLastError(LLBC_ERROR_GAI);
LLBC_SetSubErrorNo(EAI_NODATA);
LLBC_PrintLn("GAI错误: %s", LLBC_FormatLastError());
```

## 自定义错误码

当业务层需要自定义语义时，用 `LLBC_BuildCustomErrno` 生成编号（自动置位 Custom 标志），
再通过 `LLBC_AddCustomErrno` 注册描述文本：

```cpp
// 定义并注册自定义错误码（通常在模块初始化时做一次）
const int ERR_ROOM_FULL  = LLBC_BuildCustomErrno(1);
const int ERR_ROOM_CLOSE = LLBC_BuildCustomErrno(2);

LLBC_AddCustomErrno(ERR_ROOM_FULL,  "房间已满");
LLBC_AddCustomErrno(ERR_ROOM_CLOSE, "房间已关闭");

// 使用
LLBC_SetLastError(ERR_ROOM_FULL);
return LLBC_FAILED;

// 模块销毁时清理（或按需移除）
LLBC_RemoveCustomErrno(ERR_ROOM_FULL);
LLBC_RemoveAllCustomErrnos();
```

<div class="callout warning" markdown="1">
**不要自造 ad-hoc 返回码。** 直接 `return -2` / `return 3` 会破坏调用链的错误传播约定；
调用者无法用统一接口解析原因。如果内置错误码不够用，通过 `LLBC_BuildCustomErrno` +
`LLBC_AddCustomErrno` 注册，或向 `Errno.h` 提交新的内置码。
</div>

## 异常类（辅助用途）

`llbc/include/llbc/common/Exceptions.h` 提供一组继承自 `std::logic_error` 的异常类，
供需要以异常风格传递错误的场景使用（库核心不抛出异常）：

| 类名 | 适用场景 |
|------|---------|
| `LLBC_InvalidArgException` | 非法参数 |
| `LLBC_NullPtrException` | 空指针 |
| `LLBC_LengthException` | 长度越界 |
| `LLBC_RangeException` | 值域错误 |
| `LLBC_OverflowException` | 溢出 |
| `LLBC_UnderflowException` | 下溢 |
| `LLBC_IOException` | I/O 错误 |

```cpp
if (!ptr)
    throw LLBC_NullPtrException("ptr must not be null");
```

<div class="callout note" markdown="1">
**线程安全。** 错误槽（last error、sub error no）是**线程局部**存储，各线程互不干扰。
但 `LLBC_AddCustomErrno` / `LLBC_RemoveCustomErrno` 修改全局注册表，需在单线程阶段
（初始化/销毁）调用，或自行加锁。
</div>

## 参照

- 头文件：
  - `llbc/include/llbc/common/Errors.h` — 错误操作 API
  - `llbc/include/llbc/common/Errno.h` — 内置错误码定义与位域说明
  - `llbc/include/llbc/common/Exceptions.h` — 异常类
  - `llbc/include/llbc/common/Macro.h` — `LLBC_OK` / `LLBC_FAILED`
- 真实示例：`tests/func_test/common/FuncTest_Com_Error.cpp`
- 快速上手示例（可跑）：`tests/example/common/Example_Common_ErrorHandling.cpp`

## 下一步

- 架构总览：[架构概述](architecture.md)
- 服务与组件：[服务与组件](service-component.md)
