---
layout: default
title: llbc 文档
---

# llbc 用户指南

**llbc**（Low Level Base Component）是一套简洁、高效、跨平台、多语言支持的服务端开发框架，
秉持「麻雀虽小，五脏俱全」的理念，以 **App → Service → Component** 的方式组织业务，
让开发者以极低的学习、使用与接入成本完成服务端底层搭建与功能开发。底层由 C++17 实现，
广泛用于游戏与通用服务端开发。

> 注：llbc 框架本身不包含具体业务组件；较为通用的组件由独立仓库
> [llbc_comps](https://github.com/lailongwei/llbc_comps) 提供，欢迎贡献。

## llbc 提供了什么

在一个库中集成了服务端开发常用的一整套子系统，并尽量减少对第三方库的依赖：

- **高性能网络层** —— 可插拔 Poller 后端（Linux `epoll` / Windows `IOCP` / 通用 `select`），协议栈可定制。
- **一站式核心库** —— 线程、定时器、日志、对象池、序列化（`LLBC_Stream`）、配置（ini/xml/properties）、
  时间、随机、`LLBC_Variant`、委托（`LLBC_Delegate`）、事件、文件/目录、动态库加载、文本编码转换、
  调试工具、错误码等常用能力开箱即用。
- **多语言封装** —— 在 C++ 核心之上提供 Python / C# / Lua 封装，脚本层可直接复用底层能力。

## 适用场景

游戏服务端、长连接网关、房间/对局服、通用 TCP 服务，以及任何需要
「组件化业务 + 高性能网络 + 完整基础库」的 C++ 服务端。

## 从这里开始

本文档按**学习旅程**线性组织，覆盖入门、核心概念、基础库与网络使用指南、参考与更多。

1. **入门**
   - [安装与构建](getting-started/build.md)：用 CMake（Linux/macOS）或 Visual Studio（Windows）编译核心库。
   - [Hello World](getting-started/hello-world.md)：最小可运行程序，理解 `LLBC_Startup`/`LLBC_Cleanup`。
   - [第一个 Service](getting-started/first-service.md)：创建 Service、挂载 Component、启动运行。
   - [第一个网络回显服务](getting-started/echo-service.md)：监听、订阅 opcode、收发网络包。
2. **核心概念**
   - [架构总览](concepts/architecture.md)、[Service 与 Component](concepts/service-component.md)、[生命周期与事件](concepts/lifecycle-event.md)、[App 与多服务组合](concepts/app-multiservice.md)、[错误处理与返回约定](concepts/error-handling.md)、[内存与对象所有权](concepts/memory-ownership.md)。
3. **使用指南 · 基础库**
   - [Stream](guides/stream.md)、[Variant](guides/variant.md)、[Delegate](guides/delegate.md)、[Object](guides/object.md)、[ObjPool](guides/objpool.md)、[Array/Dictionary](guides/objbase-collections.md)、[日志](guides/logger.md)、[定时器](guides/timer.md)、[时间](guides/time.md)、[线程与任务](guides/thread.md)、[事件](guides/event.md)、[配置](guides/config.md)、[字符串](guides/string.md)、[文件与目录](guides/file-directory.md)、[随机](guides/random.md)、[动态库](guides/library.md)、[编码转换](guides/transcoder.md)、[工具箱 utils](guides/utils.md)。
4. **使用指南 · 网络**
   - [Service 消息处理](guides/service-messaging.md)、[Packet 与 Coder](guides/packet-coder.md)、[Session 与连接](guides/session.md)、[协议栈与自定义协议](guides/protocol.md)、[Poller 后端](guides/poller.md)。
5. **参考** —— [错误码一览](reference/error-codes.md)、[日志配置项](reference/log-config.md)、[Service / App 配置项](reference/service-app-config.md)。
6. **更多** —— [常见问题](faq.md)、[贡献指南](contributing.md)、[版本迁移指南 v1.1.1](migration-v1.1.1.md)、[术语表](glossary.md)、[更新日志](changelog.md)。

## 支持平台

| 平台 | 支持级别 | 说明 |
|------|----------|------|
| **Linux** | 一级 | 稳定，特性最新，可用于线上生产环境 |
| **Windows** | 一级 | 稳定，特性最新，可用于线上生产环境 |
| **macOS** | 二级 | 稳定，但网络部分性能较低，不建议线上生产 |
| **iOS** | 尽力 | 理论支持，已不再主动维护 |
| **Android** | 尽力 | 理论支持，已不再主动维护 |

## 构建状态

[![License](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)
[![Docs](https://img.shields.io/badge/docs-online-blue.svg)](https://lailongwei.github.io/llbc/)

| 平台 | 编译器 | 类型 | 状态 |
|------|--------|------|------|
| **Linux**   | g++, clang++  | Build, ~~Test~~, ~~Deploy~~ | [![Build Status](https://github.com/lailongwei/llbc/actions/workflows/linux-build.yml/badge.svg?branch=master)](https://github.com/lailongwei/llbc/actions/workflows/linux-build.yml) |
| **macOS**   | g++, clang++  | Build, ~~Test~~, ~~Deploy~~ | [![Build Status](https://github.com/lailongwei/llbc/actions/workflows/macos-build.yml/badge.svg?branch=master)](https://github.com/lailongwei/llbc/actions/workflows/macos-build.yml) |
| **Windows** | Visual Studio | Build, ~~Test~~, ~~Deploy~~ | [![Build status](https://github.com/lailongwei/llbc/actions/workflows/windows-build.yml/badge.svg?branch=master)](https://github.com/lailongwei/llbc/actions/workflows/windows-build.yml) |

## 商业案例

|[onemt--苏丹的复仇](http://www.ltjianhe.com/koh.html)|[onemt--苏丹传奇](https://play.google.com/store/apps/details?id=com.onemt.wolves.sos&hl=zh&gl=US)|[onemt--帝国时代](http://www.ltjianhe.com/boe.html)|[onemt--永恒之战](http://www.ltjianhe.com/woe.html)|[onemt--诸王黎明](http://www.ltjianhe.com/rok.html)|
|:--------:|:---------:|:-------:|:--------:|:--------:|
|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__onemt__苏丹的复仇.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__onemt__苏丹传奇.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__onemt__帝国时代.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__onemt__永恒之战.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__onemt__诸王黎明.png)|
|[onemt--阿拉伯传奇](http://www.ltjianhe.com/dk.html)|[Seasun--天域幻想](https://www.925g.com/game01/20197.html)|[Seasun--剑仙奇侠传四](https://v.17173.com/zt/zjqbd/xj4/)|[游爱--剑仙传](https://baike.baidu.com/item/%E5%89%91%E4%BB%99%E4%BC%A0/3333767?fr=aladdin)|[游爱--掌上飞仙]()|
|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__onemt__阿拉伯传奇.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__seasun__天域幻想.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__seasun__仙剑4.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__游爱__剑仙传.png)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase__游爱__掌上飞仙.png)|
|[37--天启之门](https://www.xiaogouh5.com/syrfrc/subuxyc.html)|||||
|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/showcases/showcase_37wan_天启之门.png)|||||

## 使用此框架的公司

|[腾讯](https://www.tencent.com/zh-cn/)|[ONEMT](http://www.onemt.com/)|[西山居](https://www.xishanju.com/)|[祺曜互娱](http://www.7yao.top/)|[37wan](https://www.37.com/)|
|:--------------------------:|:----------------------------:|:--------------------------------:|:------------------------------:|:----------------------:|
|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/company_logos/tencent-logo.jpg)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/company_logos/onemt-logo.jpg)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/company_logos/seasun-logo.jpg)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/company_logos/qiyao-logo.jpg)|![](https://raw.githubusercontent.com/wiki/lailongwei/llbc/static/company_logos/37-logo.jpg)|

## 获取帮助

- GitHub：<https://github.com/lailongwei/llbc>
- Discord：<https://discord.gg/YmtjKkNgaN>
- QQ 交流群：662590231
- 作者邮箱：lailongwei@126.com
