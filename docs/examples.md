---
layout: default
title: 示例项目
---

# 示例项目（example）

`tests/example` 是 llbc 的**入门 + 各特性用法**示例项目：一个菜单驱动的交互式程序，
按模块（common → core → comm → app）组织，每个主题对应一段最短可跑、可直接照抄到自己
工程里的代码。示例与本文档站点一一对应——每篇「使用指南 / 入门 / 核心概念」页面底部的
`参照` 都链接到这里的对应示例源码。

## 构建与运行

```bash
# premake → make（Linux/macOS）
make example config=debug64          # 产物：output/gmake2/debug64/example_debug

# 运行后按菜单序号选择要跑的示例，输入 0 退出
./output/gmake2/debug64/example_debug
```

> CMake 路径同样会构建 `example` 目标（产物在 `output/cmake/`）。

新增示例：在对应模块目录下新建 `Example_<Module>_<Topic>.{h,cpp}`（继承
`LLBC_BaseTestCase`，只重写 `Run()`，**不要**在 `Run()` 内调用 `LLBC_Startup()/LLBC_Cleanup()`
——框架已由菜单主程序统一启停），再到 `ExampleDef.h` 用 `__DEFINE_EXAMPLE("<菜单名>", <类名>)`
注册（注册行必须连续，勿插空行/整行注释）。

## 示例索引

菜单序号与下表一致。

### common
| # | 示例 | 源码 | 对应文档 |
|---|------|------|----------|
| 1 | Hello World | `tests/example/common/Example_Common_HelloWorld.cpp` | [Hello World](getting-started/hello-world.md) |
| 2 | String | `tests/example/common/Example_Common_String.cpp` | [字符串 String / CString](guides/string.md) |
| 3 | Error Handling | `tests/example/common/Example_Common_ErrorHandling.cpp` | [错误处理与返回约定](concepts/error-handling.md) |

### core
| # | 示例 | 源码 | 对应文档 |
|---|------|------|----------|
| 4 | Stream | `tests/example/core/Example_Core_Stream.cpp` | [序列化 Stream](guides/stream.md) |
| 5 | Variant | `tests/example/core/Example_Core_Variant.cpp` | [变体 Variant](guides/variant.md) |
| 6 | Delegate | `tests/example/core/Example_Core_Delegate.cpp` | [委托 Delegate](guides/delegate.md) |
| 7 | Object | `tests/example/core/Example_Core_Object.cpp` | [对象 Object](guides/object.md) |
| 8 | ObjPool | `tests/example/core/Example_Core_ObjPool.cpp` | [对象池 ObjPool](guides/objpool.md) |
| 9 | Objbase Collections | `tests/example/core/Example_Core_ObjbaseCollections.cpp` | [对象容器 Array / Dictionary](guides/objbase-collections.md) |
| 10 | Logger | `tests/example/core/Example_Core_Logger.cpp` | [日志 Logger](guides/logger.md) |
| 11 | Timer | `tests/example/core/Example_Core_Timer.cpp` | [定时器 Timer](guides/timer.md) |
| 12 | Time | `tests/example/core/Example_Core_Time.cpp` | [时间 Time / TimeSpan](guides/time.md) |
| 13 | Thread | `tests/example/core/Example_Core_Thread.cpp` | [线程与任务](guides/thread.md) |
| 14 | Event | `tests/example/core/Example_Core_Event.cpp` | [事件 EventMgr](guides/event.md) |
| 15 | Config | `tests/example/core/Example_Core_Config.cpp` | [配置 Ini / Properties / Json / Xml](guides/config.md) |
| 16 | File & Directory | `tests/example/core/Example_Core_FileDirectory.cpp` | [文件与目录](guides/file-directory.md) |
| 17 | Random | `tests/example/core/Example_Core_Random.cpp` | [随机 Random](guides/random.md) |
| 18 | Library | `tests/example/core/Example_Core_Library.cpp` | [动态库 Library](guides/library.md) |
| 19 | Transcoder | `tests/example/core/Example_Core_Transcoder.cpp` | [编码转换 Transcoder](guides/transcoder.md) |
| 20 | Utils | `tests/example/core/Example_Core_Utils.cpp` | [工具箱 utils](guides/utils.md) |

### comm
| # | 示例 | 源码 | 对应文档 |
|---|------|------|----------|
| 21 | First Service | `tests/example/comm/Example_Comm_FirstService.cpp` | [第一个 Service](getting-started/first-service.md) |
| 22 | Echo Service | `tests/example/comm/Example_Comm_EchoService.cpp` | [第一个网络回显服务](getting-started/echo-service.md) |
| 23 | Service Component | `tests/example/comm/Example_Comm_ServiceComponent.cpp` | [Service 与 Component](concepts/service-component.md) |
| 24 | Lifecycle & Event | `tests/example/comm/Example_Comm_LifecycleEvent.cpp` | [生命周期与事件](concepts/lifecycle-event.md) |
| 25 | Service Messaging | `tests/example/comm/Example_Comm_ServiceMessaging.cpp` | [Service 消息处理与 Handler](guides/service-messaging.md) |
| 26 | Packet & Coder | `tests/example/comm/Example_Comm_PacketCoder.cpp` | [Packet 与 Coder 编解码](guides/packet-coder.md) |
| 27 | Session | `tests/example/comm/Example_Comm_Session.cpp` | [Session 与连接管理](guides/session.md) |
| 28 | Protocol | `tests/example/comm/Example_Comm_Protocol.cpp` | [协议栈与自定义协议](guides/protocol.md) |
| 29 | Poller | `tests/example/comm/Example_Comm_Poller.cpp` | [Poller 后端与性能](guides/poller.md) |

### app
| # | 示例 | 源码 | 对应文档 |
|---|------|------|----------|
| 30 | Multi-Service App | `tests/example/app/Example_App_MultiService.cpp` | [App 与多服务组合](concepts/app-multiservice.md) |
