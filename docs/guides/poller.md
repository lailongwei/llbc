---
layout: default
title: Poller 后端与性能
---

# Poller 后端与性能

Poller 是 llbc 网络层的 I/O 事件驱动核心，每个 `LLBC_Service` 启动时会按配置创建若干 Poller 线程，
负责 accept / connect / recv / send 等所有套接字事件。llbc 提供三种 Poller 实现——
`SelectPoller`、`EpollPoller`、`IocpPoller`——在**编译期**由宏 `LLBC_CFG_COMM_POLLER_MODEL` 选定，
运行时不支持切换。Poller 类本身标注为 `LLBC_HIDDEN`，用户代码无需也不应直接实例化，
所有交互均通过 `LLBC_Service` 完成。

## 三种后端一览

| 后端 | 可用平台 | 底层机制 | 编译期默认值 |
|------|----------|----------|-------------|
| `SelectPoller` | 全平台 | `select()` / `fd_set` | iOS、macOS、Android、其它 |
| `EpollPoller` | Linux、Android | `epoll_wait()` | Linux |
| `IocpPoller` | Windows | IOCP 完成端口 | Windows |

`LLBC_PollerType` 枚举对应的整数值：

```cpp
// llbc/include/llbc/comm/PollerType.h
LLBC_PollerType::SelectPoller  // = 0，全平台起点
LLBC_PollerType::EpollPoller   // Linux / Android 可用
LLBC_PollerType::IocpPoller    // Win32 可用
```

辅助函数可在日志或调试中使用：

```cpp
int type = LLBC_PollerType::Str2Type(LLBC_CFG_COMM_POLLER_MODEL); // 字符串 → 枚举
const LLBC_String &name = LLBC_PollerType::Type2Str(type);        // 枚举 → 字符串
```

## 编译期选择：LLBC_CFG_COMM_POLLER_MODEL

`llbc/include/llbc/common/Config.h` 按平台自动设置该宏，也可在编译命令行覆盖：

```cpp
// Config.h 中的默认值（摘录）
#if LLBC_TARGET_PLATFORM_LINUX
 #define LLBC_CFG_COMM_POLLER_MODEL  "EpollPoller"
#elif LLBC_TARGET_PLATFORM_WIN32
 #define LLBC_CFG_COMM_POLLER_MODEL  "IocpPoller"
#elif LLBC_TARGET_PLATFORM_IPHONE
 #define LLBC_CFG_COMM_POLLER_MODEL  "SelectPoller"
#elif LLBC_TARGET_PLATFORM_MAC
 #define LLBC_CFG_COMM_POLLER_MODEL  "SelectPoller"
#else
 #define LLBC_CFG_COMM_POLLER_MODEL  "SelectPoller"
#endif
```

若需在 Linux 上强制使用 `SelectPoller`（调试用途），可在编译时追加：

```bash
-DLLBC_CFG_COMM_POLLER_MODEL='"SelectPoller"'
```

<div class="callout warning" markdown="1">
**宏值是字符串字面量**，赋值时必须带引号（含外层双引号），否则编译报错。
</div>

## pollerCount：控制 Poller 线程数

`LLBC_ServiceStartArgs::pollerCount` 决定为该 Service 启动多少个 Poller 线程，默认值为 `1`。
每个 Session 在建立时被哈希分配到一个固定的 Poller，同一 Session 的所有收发事件由同一线程串行处理，
天然保证 Session 内顺序，无需额外加锁。

```cpp
// 启动 Service 时指定 pollerCount
LLBC_ServiceStartArgs startArgs;
startArgs.pollerCount = 4;   // 4 个 Poller 线程，适合多核高并发场景
svc->Start(startArgs);

// 不指定则使用默认值 1
svc->Start();
```

真实压测示例中（`tests/func_test/comm/FuncTest_Comm_Svc.cpp`）使用了 `pollerCount = 8`。

<div class="callout note" markdown="1">
**线程数建议**：`pollerCount` 不是越大越好。单 Service 典型推荐值为 `1`～`CPU 核数的 1/2`。
过多 Poller 线程会增加跨线程消息队列竞争，反而降低吞吐。对大多数游戏服务端业务，
`1` 或 `2` 个 Poller 已足够。
</div>

## 各平台首选与限制

### Linux / Android（首选 EpollPoller）

`EpollPoller` 使用 `epoll_wait()` 水平触发模式，并额外启动一个监控线程（`LLBC_PollerMonitor`）
负责异步连接状态轮询。每次 `epoll_wait` 最多处理 `LLBC_CFG_COMM_MAX_EVENT_COUNT`（默认 100）个事件。
这是 Linux 生产部署的首选，支持数万并发连接而不受 `FD_SETSIZE` 限制。

### Windows（首选 IocpPoller）

`IocpPoller` 基于 Windows IOCP 完成端口，同样配有监控线程。IOCP 为真正的异步 I/O 模型，
内核在 I/O 完成后主动投递完成包，CPU 占用低于轮询方式。Windows 生产部署应始终使用 IOCP 后端。

### macOS / iOS（仅 SelectPoller，性能受限）

macOS 和 iOS 上只有 `SelectPoller` 可用，底层调用 `select()` 遍历 `fd_set`。
`fd_set` 在多数系统上上限为 1024 个文件描述符，且每次调用需线性扫描全部监听 fd，
随连接数增长性能急剧下降。

<div class="callout warning" markdown="1">
**macOS 不建议用于生产**：llbc 官方定位 macOS 为二级平台（tier-2），网络性能明显低于 Linux 与 Windows。
仅建议用于本地开发与调试。生产部署请选择 Linux 或 Windows。
</div>

## 在代码中区分当前后端

如需在运行时针对不同后端调整行为（如限制并发连接数），推荐方式：

```cpp
const int pollerType = LLBC_PollerType::Str2Type(LLBC_CFG_COMM_POLLER_MODEL);
int clientLimit;
if (pollerType == LLBC_PollerType::SelectPoller)
    clientLimit = 50;   // select 受 FD_SETSIZE 限制
else
    clientLimit = 200;  // epoll/iocp 可承载更多
```

## 参照

- 后端类型枚举：`llbc/include/llbc/comm/PollerType.h`
- 编译期配置宏：`llbc/include/llbc/common/Config.h`（第 314–328 行）
- 服务启动参数：`llbc/include/llbc/comm/Service.h`（`LLBC_ServiceStartArgs`）
- 真实用例：`tests/func_test/comm/FuncTest_Comm_Svc.cpp`

## 下一步

- [Service 与 Component](../concepts/service-component.md) — 了解 Service 完整生命周期
- [序列化 Stream](stream.md) — Poller 收到的数据通过 Stream 反序列化
