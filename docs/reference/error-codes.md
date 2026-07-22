---
layout: default
title: 错误码一览
---

# 错误码一览

llbc 使用统一的整型错误码体系。调用失败后，通过 `LLBC_GetLastError()` 取得最近一次错误码，
通过 `LLBC_StrError(no)` 或 `LLBC_FormatLastError()` 取得可读描述字符串。
绝大多数 API 遵守"返回 `LLBC_OK`(0) 成功，返回 `LLBC_FAILED`(-1) 失败"约定。

## 错误码结构

每个错误码是一个 32 位整数，按位段分层编码：

```
位 31-30  Sev   严重级别  00=成功 01=信息 10=警告 11=错误
位 29     C     用户自定义标志
位 28-20  —     保留（必须为 0）
位 19-16  Type  错误来源  0=llbc库 1=C库 2=OS API 3=网络API 4=getaddrinfo
位 15-0   No    错误编号
```

公开宏 `LLBC_ERROR_*` 已将上述字段组合为完整值，直接用于比较即可。

## llbc 库内置错误码

下表列出 `llbc/include/llbc/common/Errno.h` 中全部内置错误码（`__LLBC_ERROR_*` 为内部原始编号，不在公开 API 中直接使用）。

| 错误码宏 | 含义 |
|----------|------|
| `LLBC_ERROR_SUCCESS` | 成功（值为 0） |
| `LLBC_ERROR_UNKNOWN` | 未知错误 |
| `LLBC_ERROR_CLIB` | C 标准库调用失败（子错误见 `errno`） |
| `LLBC_ERROR_OSAPI` | 操作系统 API 调用失败（仅 Windows，子错误见 `GetLastError()`） |
| `LLBC_ERROR_NETAPI` | 网络 API 调用失败（仅 Windows WSA，子错误见 `WSAGetLastError()`） |
| `LLBC_ERROR_GAI` | `getaddrinfo()` 调用失败（子错误见 `LLBC_GetSubErrorNo()`） |
| `LLBC_ERROR_ARG` | 参数错误 |
| `LLBC_ERROR_NOT_IMPL` | 未实现 |
| `LLBC_ERROR_RANGE` | 范围越界 |
| `LLBC_ERROR_EXIST` | 对象/资源已存在 |
| `LLBC_ERROR_NOT_FOUND` | 对象/资源未找到 |
| `LLBC_ERROR_END` | 已到达末尾 |
| `LLBC_ERROR_LIMIT` | 达到数量/容量上限 |
| `LLBC_ERROR_NOT_INIT` | 尚未初始化 |
| `LLBC_ERROR_NOT_OPEN` | 尚未打开 |
| `LLBC_ERROR_INITED` | 已经初始化（重复初始化） |
| `LLBC_ERROR_OPENED` | 已经打开（重复打开） |
| `LLBC_ERROR_PERM` | 权限不足 |
| `LLBC_ERROR_TRUNCATED` | 数据被截断 |
| `LLBC_ERROR_INVALID` | 无效状态或无效数据 |
| `LLBC_ERROR_FORMAT` | 格式错误 |
| `LLBC_ERROR_TIMEOUTED` | 操作超时 |
| `LLBC_ERROR_BUSY` | 资源繁忙 |
| `LLBC_ERROR_REENTRY` | 不允许重入 |
| `LLBC_ERROR_NO_SUCH` | 无此内存/数据/资源 |
| `LLBC_ERROR_WBLOCK` | 操作会阻塞（would block） |
| `LLBC_ERROR_AGAIN` | 请再试一次 |
| `LLBC_ERROR_PENDING` | 操作挂起/排队中 |
| `LLBC_ERROR_ILLEGAL` | 非法操作 |
| `LLBC_ERROR_ACCESS_DENY` | 访问被拒绝 |
| `LLBC_ERROR_REPEAT` | 重复操作 |
| `LLBC_ERROR_ENCODE` | 编码失败 |
| `LLBC_ERROR_DECODE` | 解码失败 |
| `LLBC_ERROR_COMPRESS` | 压缩失败 |
| `LLBC_ERROR_DECOMPRESS` | 解压失败 |
| `LLBC_ERROR_PACK` | 打包失败 |
| `LLBC_ERROR_UNPACK` | 解包失败 |
| `LLBC_ERROR_NOT_ALLOW` | 不允许该操作 |
| `LLBC_ERROR_COMP_INIT_FAILED` | Component 初始化失败 |
| `LLBC_ERROR_COMP_LATE_INIT_FAILED` | Component 延迟初始化失败 |
| `LLBC_ERROR_COMP_START_FAILED` | Component 启动失败 |
| `LLBC_ERROR_COMP_LATE_START_FAILED` | Component 延迟启动失败 |
| `LLBC_ERROR_WSA_SYSNOTREADY` | 底层网络子系统未就绪（仅 Windows） |
| `LLBC_ERROR_WSA_VERNOTSUPPORTED` | 请求的 WinSock 版本不受支持（仅 Windows） |
| `LLBC_ERROR_WSA_EINPROGRESS` | 阻塞式 WinSock 1.1 操作正在进行（仅 Windows） |
| `LLBC_ERROR_WSA_EPROCLIM` | 已达 WinSock 实现支持的任务数上限（仅 Windows） |
| `LLBC_ERROR_IS_LISTEN_SOCKET` | 指定 Socket 为监听 Socket |
| `LLBC_ERROR_NOT_POOL_OBJECT` | 对象不是对象池对象 |
| `LLBC_ERROR_SESSION_SND_BUF_LIMIT` | Session 发送缓冲区达到上限 |
| `LLBC_ERROR_NOT_SUPPORT` | 不支持该操作 |
| `LLBC_ERROR_CANCELLED` | 操作已取消 |
| `LLBC_ERROR_APP_EARLY_START_FAILED` | 应用早期启动阶段失败 |
| `LLBC_ERROR_APP_START_FAILED` | 应用启动失败 |
| `LLBC_ERROR_TIMER_SCHEDULER_DESTROYING` | 定时器调度器正在销毁 |
| `LLBC_ERROR_TIMER_SCHEDULER_CANCELING_ALL` | 定时器调度器正在取消全部定时器 |
| `LLBC_ERROR_EVENT_MANAGER_PRE_FIRE_FAILED` | 事件管理器 pre-fire 失败 |
| `LLBC_ERROR_PARTIAL_PARSED` | 数据仅部分解析 |

## 常用错误处理模式

```cpp
// 调用失败后读取错误
if (LLBC_FAILED == someApi())
{
    LLBC_PrintLn("failed: %s", LLBC_FormatLastError());
    // 或者：
    int err = LLBC_GetLastError();
    LLBC_PrintLn("errNo=%d desc=%s", err, LLBC_StrError(err));
}
```

## 带子错误码的系统错误

`LLBC_ERROR_CLIB`、`LLBC_ERROR_OSAPI`、`LLBC_ERROR_NETAPI`、`LLBC_ERROR_GAI`
这四个错误码在格式化时会携带子错误的可读描述。使用方式：

```cpp
// C 库错误：先设 errno，再设 llbc 错误
errno = EINVAL;
LLBC_SetLastError(LLBC_ERROR_CLIB);
LLBC_PrintLn("%s", LLBC_FormatLastError());  // 输出 C 库错误描述

// getaddrinfo 错误：用子错误号传 gai 错误码
LLBC_SetLastError(LLBC_ERROR_GAI);
LLBC_SetSubErrorNo(EAI_NODATA);
LLBC_PrintLn("%s", LLBC_FormatLastError());  // 输出 gai_strerror 描述
```

## 自定义错误码

业务层可注册自定义错误码，通过 `LLBC_BuildCustomErrno` 宏构建合法编号：

```cpp
// 注册
const int ERR_MY_LOGIC = LLBC_BuildCustomErrno(1001);
LLBC_AddCustomErrno(ERR_MY_LOGIC, "my business logic error");

// 使用
LLBC_SetLastError(ERR_MY_LOGIC);
LLBC_PrintLn("%s", LLBC_FormatLastError());  // "my business logic error"

// 注销（单个 / 全部）
LLBC_RemoveCustomErrno(ERR_MY_LOGIC);
LLBC_RemoveAllCustomErrnos();
```

<div class="callout note" markdown="1">
`LLBC_BuildCustomErrno` 只取传入值的低 16 位，并强制设置 Sev=Error、Custom=1。
不同模块请自行划分自定义编号区间，避免冲突。
</div>

## 设置错误时附加自定义描述字符串

`LLBC_SetLastError` 接受可选的 `customErrStr` 参数，用于在标准描述之外附加上下文信息：

```cpp
LLBC_SetLastError(LLBC_ERROR_FORMAT, "field 'userId' missing in packet");
LLBC_PrintLn("%s", LLBC_FormatLastError());
// 输出中包含 "field 'userId' missing in packet"
```

<div class="callout warning" markdown="1">
`customErrStr` 指针的生命周期必须覆盖 `LLBC_FormatLastError()` 调用，不要传入临时字符串字面量以外的栈变量。
</div>

## 下一步

- 了解 API 返回值约定：参见 [../guides/stream.md](../guides/stream.md)（返回值约定贯穿全库）
- 组件生命周期中的错误处理：参见 [../concepts/](../concepts/)

## 参照

- 头文件：`llbc/include/llbc/common/Errno.h`（错误码宏定义）
- 头文件：`llbc/include/llbc/common/Errors.h`（API 声明）
- 头文件：`llbc/include/llbc/common/ErrorsInl.h`（模板重载实现）
- 真实示例：`tests/func_test/common/FuncTest_Com_Error.cpp`
