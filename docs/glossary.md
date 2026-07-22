---
layout: default
title: 术语表
---

# 术语表

本页汇总 llbc 框架中最常见的核心概念与类型，每条目给出一句话释义及指向详细文档的链接。
阅读任何指南页前，可先在此快速定位陌生术语。

| 术语 | C++ 类型 | 释义 | 相关页 |
|------|----------|------|--------|
| **App** | `LLBC_App` | 进程级应用入口，管理 start / reload / stop 完整生命周期，并将多个 Service 组合在一起统一运作。 | [App 与多 Service](concepts/app-multiservice.md) |
| **Service** | `LLBC_Service` | 独立的网络 + 事件循环单元，拥有若干 Session 和 Component，是 llbc 网络编程的核心调度器。 | [Service 与 Component](concepts/service-component.md)、[消息收发](guides/service-messaging.md) |
| **Component** | `LLBC_Component` | 业务逻辑单元，挂载到 Service 上；生命周期钩子为 `OnInit` / `OnStart` / `OnStop`，非流程事件走 `OnEvent`，配置热重载走 `OnReload`。 | [Service 与 Component](concepts/service-component.md)、[生命周期与事件](concepts/lifecycle-event.md) |
| **Session** | `LLBC_Session` | 一条 TCP 连接的抽象句柄，用整型 `sessionId` 唯一标识；连接建立 / 断开时框架分别向组件派发 `LLBC_ComponentEventType::SessionCreate` / `LLBC_ComponentEventType::SessionDestroy` 事件，组件通过 `OnEvent()` 钩子接收。 | [Session](guides/session.md) |
| **Packet** | `LLBC_Packet` | 网络消息的最小交换单位，携带 sessionId、opcode、长度与载荷 Stream；收发均通过 `<<` / `>>` 操作符读写。 | [Packet 与 Coder](guides/packet-coder.md) |
| **Coder** | `LLBC_Coder` | 数据包编解码接口；继承后实现 `Encode(LLBC_Packet &)` / `Decode(LLBC_Packet &)` 即可为指定 opcode 注册自定义序列化逻辑。 | [Packet 与 Coder](guides/packet-coder.md) |
| **Poller** | `LLBC_BasePoller` | Service 内部的 I/O 多路复用后端；按平台自动选择：`SelectPoller`（全平台）、`EpollPoller`（Linux/Android）、`IocpPoller`（Windows）。 | [Poller](guides/poller.md) |
| **ObjPool** | `LLBC_ObjPool` / `LLBC_TypedObjPool<T>` | 线程感知对象池；`Acquire()` 取出对象，`Release()` 归还以供复用，显著减少高频分配/释放的堆压力。 | [对象池](guides/objpool.md) |
| **Stripe** | `_ObjStripe`（内部） | `LLBC_TypedObjPool` 的内部分配条带，即一块连续内存 slab，固定容纳若干个包装对象，以降低单对象元数据开销；可通过 `GetStripeCapacity()` 定制容量。 | [对象池](guides/objpool.md) |
| **Variant** | `LLBC_Variant` | 动态类型值容器，支持 nil / bool / 整型 / 浮点 / 字符串 / 序列（Seq）/ 字典（Dict）；常用于配置传递、事件参数和跨组件数据交换。 | [Variant](guides/variant.md) |
| **Delegate** | `LLBC_Delegate<Rtn(Args...)>` | 统一可调用对象包装器，兼容 C 函数指针、lambda、`std::function` 以及成员函数指针，消除回调绑定的样板代码。 | [Delegate](guides/delegate.md) |
| **Object** | `LLBC_Object` | 引用计数基类；`Retain()` 增加引用、`Release()` 减少引用并在归零时析构，`AutoRelease()` 将对象托管给当前线程的 AutoReleasePool。 | [Object 与引用计数](guides/object.md) |
| **AutoReleasePool** | `LLBC_AutoReleasePool` | 延迟释放桶，收集通过 `AutoRelease()` 托管的 `LLBC_Object` 子类实例；调用 `Purge()` 一次性释放所有收集的对象。 | [Object 与引用计数](guides/object.md) |
| **Stream** | `LLBC_Stream` | 二进制序列化容器，读写游标独立（v1.1.1 起），`<<` 写入、`>>` 读出，支持基础类型、STL 容器及实现 `Serialize`/`Deserialize` 的自定义类型。 | [序列化 Stream](guides/stream.md) |

<div class="callout note" markdown="1">
**命名约定**：所有公开类型与宏统一以 `LLBC_` 前缀开头（如 `LLBC_Service`、`LLBC_OK`）；
双下划线前缀 `__LLBC_` 或 `LLBC_INTERNAL_` 表示内部符号，不应出现在业务代码中。
</div>

<div class="callout warning" markdown="1">
**已废弃项速查**

- 日志：`logLevel` 已废弃，使用 `consoleLogLevel` / `fileLogLevel`；`Trace` 是最低日志级别（低于 `Debug`）。
- 组件：`OnAppConfigReloaded()` 已删除，配置重载改用 `OnReload()`。
- 对象池：旧对象池已删除，仅保留 `core/objpool/ObjPool.h` 与 `ThreadSpecObjPool.h`。
- Stream：v1.1.1 前的单一位置游标假设已失效，读写游标现在各自独立。
</div>

## 下一步

- 了解整体设计理念：[架构概览](concepts/architecture.md)
- 动手实践：[Hello World](getting-started/hello-world.md)、[第一个 Service](getting-started/first-service.md)
- 深入网络层：[Service 与 Component](concepts/service-component.md)、[消息收发](guides/service-messaging.md)
