---
layout: default
title: 文档规划
---

# llbc 文档规划（DOC_PLAN）

> 本页是**面向文档贡献者**的内部规划，不面向最终读者，不进入侧边栏导航。
> 目的：统一文档结构、写作模板、内容校验方法与画图规范，保证后续补写的一致与准确。

## 1. 目标与原则

- **准确 > 完整 > 完美**：宁可少写，不可写错。每一处 API、签名、示例都必须能在当前代码中找到出处。
- **代码优先**：能力页以最短可跑示例开场，解释围绕代码展开。
- **单一信息源**：概念/用法人工撰写；API 明细尽量交给 Doxygen 自动生成，避免手写表与代码漂移。
- **旅程式组织**：入门 → 概念 → 使用指南 → 参考 → 更多，读者可线性推进，也可按需检索。

## 2. 目标文档结构（IA）

采用业界主流的 **Diátaxis 四象限**（Tutorial / How-to / Reference / Explanation），
映射到 llbc 现有的四段导航并补全为六段。图例：✅ 已就绪 · 🚧 计划中 · 🟡 已提及待展开。

```
1. 入门 (Tutorial)
   ├─ 安装与构建 ✅   ├─ Hello World ✅   ├─ 第一个 Service ✅
   └─ 第一个网络回显服务 🚧（可选）

2. 核心概念 (Explanation)
   ├─ 架构总览 ✅   ├─ Service 与 Component ✅   ├─ 生命周期与事件 ✅
   ├─ App 与多服务组合 🚧   ├─ 错误处理与返回约定 🚧
   └─ 内存与对象所有权（Object / ObjPool / Stream 关系）🚧

3. 使用指南 · 基础库 (How-to, core)
   ├─ 序列化 Stream ✅   ├─ 变体 Variant ✅   ├─ 委托 Delegate ✅
   ├─ 对象 Object ✅     ├─ 对象池 ObjPool ✅
   ├─ 日志 Logger 🚧    ├─ 定时器 Timer 🚧   ├─ 时间 Time / TimeSpan 🚧
   ├─ 线程与任务 Task / MessageQueue / 锁 🚧   ├─ 事件 EventMgr 🚧
   ├─ 配置 Ini / Properties / Json / Xml 🚧   ├─ 字符串 String / CString 🚧
   ├─ 文件与目录 🚧   ├─ 随机 Random 🚧   ├─ 动态库 Library 🚧
   ├─ 编码转换 Transcoder 🚧   └─ 工具箱 utils（Base64/MD5/Math/Text/…）🚧

4. 使用指南 · 网络 (How-to, comm)
   ├─ Service 消息处理与 Handler 注册 🚧
   ├─ Packet 与 Coder 编解码 🚧
   ├─ Session 与连接管理（Listen / Connect / 异步连接）🚧
   ├─ 协议栈与自定义协议（Codec / Compress / Filter）🚧
   └─ Poller 后端与性能 🚧

5. 参考 (Reference)
   ├─ API 参考（Doxygen 生成）🚧
   ├─ 错误码一览 🚧   ├─ 日志配置项参考 🚧   └─ Service / App 配置项参考 🚧

6. 更多
   ├─ 示例项目（占位）   ├─ 常见问题 🚧   ├─ 贡献指南 🚧   ├─ 更新日志 ✅(外链)
   └─ 版本迁移指南（v1.1.1）🚧   ├─ 术语表 🚧   └─ 性能与压测 🚧
```

## 3. 模块 → 文档映射与优先级

以源码 `llbc/include/llbc/{common,core,comm,testcase,app}` 为准。

| 优先级 | 页面 | 覆盖模块 / 关键类型 | 状态 |
|---|---|---|---|
| — | 序列化 Stream | common `Stream.h` | ✅ |
| — | 变体 / 委托 / 对象 / 对象池 | core `variant`/`utils` `Delegate`/`objbase` `Object`/`objpool` | ✅ |
| **P0** | 日志 Logger | core `log`（`Logger`/`LoggerMgr`/appender/token/滚动） | 🚧 |
| **P0** | 定时器 Timer | core `timer`（`Timer`/`TimerScheduler`） | 🚧 |
| **P0** | 时间 Time / TimeSpan | core `time` | 🚧 |
| **P0** | 线程与任务 | core `thread`（`Task`/`ThreadMgr`/`MessageQueue`/锁/`Tls`/`Guard`） | 🚧 |
| **P0** | Service 消息处理与 Handler | comm `Service`/`Component`/`Coder` | 🚧 |
| **P0** | Packet 与 Coder 编解码 | comm `Packet`/`Coder`/`PacketHeaderAssembler` | 🚧 |
| **P1** | 事件 EventMgr | core `event` | 🚧 |
| **P1** | 配置 | core `config`（`Ini`/`Properties` + rapidjson/tinyxml2） | 🚧 |
| **P1** | 字符串 String / CString | common `BasicString`/`BasicCString` | 🚧 |
| **P1** | Session 与连接管理 | comm `Session`/`SessionOpts`/`Socket` | 🚧 |
| **P1** | 协议栈与自定义协议 | comm `protocol/*` | 🚧 |
| **P1** | App 与多服务组合 | app `App.h` | 🚧 |
| **P1** | 错误处理概念页 + 错误码参考 | common `Errors.h`/`Errno.h` | 🚧 |
| **P2** | 文件/目录、随机、动态库、编码转换、utils 工具箱 | core `file`/`random`/`library`/`transcoder`/`utils` | 🚧 |
| **P2** | objbase Array / Dictionary | core `objbase` | 🚧 |
| **P2** | Poller 后端与性能 | comm `*Poller`/`PollerMgr` | 🚧 |
| **P2** | 日志 / Service / App 配置项参考 | 配置样例 + 源码 | 🚧 |
| **P2** | API 参考(Doxygen)、迁移指南、术语表、FAQ、贡献指南、性能页 | 全局 | 🚧 |
| 保留占位 | 示例项目 | `tests/example`（当前仅 `Example.cpp`，内容不足） | ⏸ |
| 内部/第三方 | os / bundle / iconv / rapidjson / tinyxml2 | — | 不单独成文 |

## 4. 页面写作模板（沿用 stream.md 已确立的风格）

固化为 5 种原型，新页面从对应模板起手。

### A. 能力页 / How-to（基础库、网络指南主力）
```
---\nlayout: default\ntitle: <能力名>\n---
# <能力名>
<一段：是什么 + 何时用 + 关键版本变更（若有）>
## 快速上手      # 最短可跑代码
## <按能力分节>  # 每节：一句说明 + cpp 代码块（≤20 行）
## 注意事项      # callout：陷阱 / 线程安全 / 生命周期 / 废弃项
## 参照          # 头文件路径 + tests/ 真实示例（必须存在）
（可选）## 下一步  # 指向相关页
```

### B. 概念页 / Explanation（architecture 类）
```
引子（解决什么问题）→ ## 心智模型（图/表）→ ## 关键角色（各类型职责表）
→ ## 数据流 / 控制流（时序或状态图）→ ## 与其它模块关系（交叉链接）→ ## 参照
```
少代码、多图与表。

### C. 教程页 / Tutorial（hello-world / first-service 类）
```
目标一句话 → ## 完整代码（可直接跑）→ ## 逐行说明（bullet）
→ callout（必守约定）→ ## 构建与运行（make 命令）→ ## 下一步
```

### D. 参考页 / Reference
- **API 参考**：Doxygen 自动生成，人工只维护头注释。
- **错误码 / 配置项**：纯表格 `名称 | 含义 | 默认值 | 备注`，一页一主题，便于检索。

### E. 元信息页（FAQ / 迁移 / 术语 / 贡献）
- FAQ：Q/A 列表；迁移指南：按版本分节「变更 → 影响 → 改法」，直接引用 `CHANGELOG` 条目号；术语表：`术语 | 释义 | 相关页`。

## 5. 内容准确性方法论（如何吃透设计意图与用法）

写每一页前后都要走这套流程——它在本轮把 PDF 笔记里的多处 API 错名纠正了过来（如
`IsInt32()`→`Is<T>()`、`GuaredPoolObj`→`LLBC_GuardedPoolObj`、`AutoReleasePool::Current()->Add()`→`obj->AutoRelease()`）。

### 5.1 信息源优先级（冲突时以上为准）
1. **头文件 `*.h`** —— 公开接口的唯一真相；以此确定类名、方法名、签名、默认参数。
2. **`*Inl.h`** —— 模板/内联实现，确认真实模板特化与重载（签名常在这里才看得全）。
3. **`tests/`** —— 可编译的权威用法示例。取例优先级：`unit_test`（自动化）> `func_test` > `example`（入门/上手）。
4. **`CHANGELOG`** —— **设计意图与变更缘由**的权威来源（为什么这样改、废弃了什么）。
5. **类/头顶部注释** —— 作者写下的动机与约束。
6. **既有笔记 / PDF / wiki** —— 只用于**选题和结构**，所有结论必须回到 1–3 校验。

### 5.2 写作前检查清单
- [ ] 读完该能力的主头文件 + 对应 `*Inl.h`，列出真实公开 API。
- [ ] 在 `tests/` 中 grep 该类型，找到至少一个真实用例作为示例底本。
- [ ] 在 `CHANGELOG` 搜索该模块，确认有无 v1.1.1 级别的变更/废弃（见 5.4）。
- [ ] 明确「设计意图」：这个能力**为什么存在**、**何时该用/不该用**（写进引子与"注意事项"）。

### 5.3 写作后校验清单
- [ ] 每个出现的 API 名、方法签名都能在头文件里逐一对上（不臆造、不改名）。
- [ ] 代码示例风格贴近 `tests/` 用法；能大致心算其可编译性。
- [ ] `参照` 里引用的头文件与 `tests/` 路径**真实存在**（可用脚本核验，见 5.5）。
- [ ] 交叉链接指向的 `.md` 均存在。

### 5.4 已知的"过时陷阱"（易写错，务必回避）
- 日志：不要配置 `logLevel`（已废弃），改用 `consoleLogLevel` / `fileLogLevel`；`Trace` 是最低级别（非 `Debug`）。
- 组件：非流程事件走 `OnEvent()`、配置重载走 `OnReload()`；`OnAppConfigReloaded()` 已删除（CHANGELOG 17/26）。
- 对象池：旧对象池已删除，仅 `core/objpool/ObjPool.h` 与 `ThreadSpecObjPool.h`。
- Stream：v1.1.1 起**读写游标分离**，勿假设单一游标。
- Time：`LLBC_Time`/`LLBC_TimeSpan` 在 v1.1.1 简化，优先用新构造/工厂，勿手写旧解析路径。

### 5.5 校验脚本（提交前跑）
```bash
# 交叉链接是否可解析 + 参照的源码/示例路径是否存在
cd docs && for f in $(find . -name '*.md' -not -path './_site/*'); do
  d=$(dirname "$f")
  grep -oE '\]\(([^)]+\.md)\)' "$f" | sed -E 's/\]\(//;s/\)//' | \
    while read l; do [ -f "$d/$l" ] || echo "BROKEN link: $f -> $l"; done
done
```

## 6. 画图规范（手写 SVG）

### 6.1 为什么是手写 SVG
评估过 Mermaid（观感一般）、D2（需 CLI 预渲染）、Excalidraw / draw.io（需 GUI 手绘，无法用代码完全生成）后，
最终统一用**手写 SVG**：像素级可控（接近 Figma 的精细度）、**零工具链 / 零构建步骤**、
可随源码 diff/评审、GitHub Pages 原生渲染。图直接作为静态资源提交，页面按普通图片引用。

- 存放：`docs/assets/diagrams/<主题>.svg`
- 引用：`![图题](../assets/diagrams/<主题>.svg)`（相对路径按页面所在目录）
- 已落地范例：`architecture-modules.svg`、`service-component.svg`、`lifecycle-hooks.svg`、`objpool-structure.svg`

### 6.2 统一视觉系统（所有图共用）
**按模块归色**（承载"属于哪个模块"的语义，不随意上色）：

| 语义 | fill | stroke | text |
|---|---|---|---|
| common | `#f1f5f9` | `#64748b` | `#334155` |
| core | `#dbeafe` | `#2563eb` | `#1e40af` |
| comm | `#d1fae5` / `#ecfdf5` | `#059669` | `#065f46` |
| app | `#fff7ed` / `#ffedd5` | `#ea580c` | `#9a3412` |
| testcase | `#ede9fe` | `#7c3aed` | `#5b21b6` |
| 中性 / 构造析构 | `#e2e8f0` | `#94a3b8` | `#0f172a` |
| 连线 / 箭头 | — | `#475569` | `#475569` |

**字体**（CJK 安全，避免中文缺字）：

```
font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", "PingFang SC",
             "Microsoft YaHei", "Hiragino Sans GB", sans-serif;
```

**通用骨架**（每个 SVG 起手复制）：

```svg
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 W H" width="W" height="H"
     role="img" aria-labelledby="t d">
  <title id="t">图题</title>
  <desc id="d">一句话说明（无障碍/SEO）</desc>
  <defs>
    <style>
      .lbl { font-family: -apple-system, "PingFang SC", "Microsoft YaHei", sans-serif; }
      /* .name/.role/.cap 等按需定义字号字重 */
    </style>
    <marker id="arr" markerWidth="9" markerHeight="9" refX="7" refY="3" orient="auto">
      <path d="M0,0 L7,3 L0,6 Z" fill="#475569"/>
    </marker>
  </defs>
  <!-- 节点用 rect rx=10~12；连线 <line ... marker-end="url(#arr)"/> -->
</svg>
```

要点：矩形圆角 `rx` 10–12；描边 1.6–2；`role="img"` + `<title>`/`<desc>` 保证无障碍与检索；
`viewBox` 决定坐标系、`width/height` 给出固有尺寸（站点 `.markdown-body img{max-width:100%}` 会自适应缩放）。

### 6.3 各页推荐图意
| 页面 | 画什么 | 布局 |
|---|---|---|
| 架构总览 ✅ | 五模块依赖 common→core→comm→testcase→app | 竖向分层，箭头示依赖 + 初始化顺序 |
| Service 与 Component ✅ | App→Service→Component 组合与多服务 | 容器嵌套 |
| 生命周期与事件 ✅ | 组件钩子调用顺序（含每帧循环、OnEvent/OnReload 穿插） | 竖向分阶段泳道 + 回环箭头 |
| 对象池 ObjPool ✅ | ObjPool→TypedObjPool→Stripe 结构 | 三段式 + 槽位图 |
| Service 消息处理 🚧 | 收包→解码→Handler→发包 | 横向时序 |
| 协议栈 🚧 | Codec/Compress/… 分层管道 | 横向管道，上/下行双箭头 |
| 定时器 / 线程 🚧 | 调度与触发时序 | 横向时序 |
| Stream 🚧 | 读游标 / 写游标分离 | 一条 buffer + 两游标标记 |

### 6.4 作图风格约定
- 单图聚焦一个概念，节点 **≤ 12 个**；过大则拆图。
- 方向统一：依赖/流程用自上而下或自左而右，同类页面保持一致。
- 标签用中文 + 关键类型英文名（如 `会话 Session`）。
- 颜色只承载"模块归属"语义，遵循 6.2 调色板。
- 图**辅助**文字，不替代文字；每张图前后要有一句话点题。
- 时序图标出关键返回值/错误分支（呼应 `LLBC_OK`/`LLBC_FAILED` 约定）。
- 提交前用浏览器实际打开一次，确认中文不缺字、无文字溢出框。

## 7. 执行计划（分批）

- **第 1 批（P0）**：日志、定时器、时间、线程与任务、Service 消息处理、Packet/Coder。
  复用 §6.2 的 SVG 视觉系统作图，新页配套时序/结构图。
- **第 2 批（P1）**：事件、配置、字符串、Session、协议栈、App 组合、错误处理 + 错误码参考。
- **第 3 批（P2）**：其余基础库能力页、Poller、各配置项参考、API 参考(Doxygen)、迁移/术语/FAQ/贡献/性能。

每批可多 sub-agent 并行，每页严格执行 §5 校验流程；合并前跑 §5.5 脚本。

## 8. 进度看板（全量建档 · 单一进度源）

状态：✅ 已完成 · 🟡 进行中 · ⬜ 计划中 · ⏸ 暂缓。**完成定义（DoD）见 §5.2/§5.3**。

### 入门 (Tutorial)
| 页面 | 文件 | 主要源头 | 图 | 状态 |
|---|---|---|---|---|
| 安装与构建 | `getting-started/build.md` | CMakeLists / config.cmake / WinPreBuild.bat | — | ✅ |
| Hello World | `getting-started/hello-world.md` | example/common/Example_Common_HelloWorld | — | ✅ |
| 第一个 Service | `getting-started/first-service.md` | func_test/comm/SvcBase | — | ✅ |
| 第一个网络回显服务 | `getting-started/echo-service.md` | comm/Service.h · func_test SvcBase | — | ✅ |

### 核心概念 (Explanation)
| 页面 | 文件 | 主要源头 | 图 | 状态 |
|---|---|---|---|---|
| 架构总览 | `concepts/architecture.md` | 五模块 / llbc.h | ✅ | ✅ |
| Service 与 Component | `concepts/service-component.md` | comm/Service.h · Component.h | ✅ | ✅ |
| 生命周期与事件 | `concepts/lifecycle-event.md` | comm/Component.h · CHANGELOG 17/26 | ✅ | ✅ |
| App 与多服务组合 | `concepts/app-multiservice.md` | app/App.h | — | ✅ |
| 错误处理与返回约定 | `concepts/error-handling.md` | common/Errors.h · Errno.h | — | ✅ |
| 内存与对象所有权 | `concepts/memory-ownership.md` | objbase/Object · objpool/ObjPool · Stream | — | ✅ |

### 使用指南 · 基础库 (How-to, core)
| 页面 | 文件 | 主要源头 | 图 | 状态 |
|---|---|---|---|---|
| 序列化 Stream | `guides/stream.md` | common/Stream.h | ✅ | ✅ |
| 变体 Variant | `guides/variant.md` | core/variant/Variant.h | — | ✅ |
| 委托 Delegate | `guides/delegate.md` | core/utils/Util_Delegate.h | — | ✅ |
| 对象 Object | `guides/object.md` | core/objbase/Object.h · AutoReleasePool.h | — | ✅ |
| 对象池 ObjPool | `guides/objpool.md` | core/objpool/ObjPool.h | ✅ | ✅ |
| 日志 Logger | `guides/logger.md` | core/log/Logger.h · LoggerMgr.h · LogLevel.h | — | ✅ |
| 定时器 Timer | `guides/timer.md` | core/timer/Timer.h · TimerScheduler.h | ✅ | ✅ |
| 时间 Time / TimeSpan | `guides/time.md` | core/time/Time.h · TimeSpan.h | — | ✅ |
| 线程与任务 | `guides/thread.md` | core/thread/Task.h · ThreadMgr.h · MessageQueue.h · 锁 · Tls.h | ✅ | ✅ |
| 事件 EventMgr | `guides/event.md` | core/event/Event.h · EventMgr.h · EventFirer.h | — | ✅ |
| 配置 Ini/Properties/Json/Xml | `guides/config.md` | core/config/Ini.h · Properties.h · rapidjson · tinyxml2 | — | ✅ |
| 字符串 String / CString | `guides/string.md` | common/BasicString.h · BasicCString.h | — | ✅ |
| 文件与目录 | `guides/file-directory.md` | core/file/File.h · Directory.h | — | ✅ |
| 随机 Random | `guides/random.md` | core/random/Random.h | — | ✅ |
| 动态库 Library | `guides/library.md` | core/library/Library.h | — | ✅ |
| 编码转换 Transcoder | `guides/transcoder.md` | core/transcoder/Transcoder.h | — | ✅ |
| 工具箱 utils | `guides/utils.md` | core/utils/Util_Base64/MD5/Math/Text/Network/Debug.h | — | ✅ |
| 对象容器 Array / Dictionary | `guides/objbase-collections.md` | core/objbase/Array.h · Dictionary.h | — | ✅ |

### 使用指南 · 网络 (How-to, comm)
| 页面 | 文件 | 主要源头 | 图 | 状态 |
|---|---|---|---|---|
| Service 消息处理与 Handler | `guides/service-messaging.md` | comm/Service.h · Packet.h · Coder.h | ✅ | ✅ |
| Packet 与 Coder 编解码 | `guides/packet-coder.md` | comm/Packet.h · Coder.h · PacketHeaderAssembler.h | — | ✅ |
| Session 与连接管理 | `guides/session.md` | comm/Session.h · SessionOpts.h · Socket.h | — | ✅ |
| 协议栈与自定义协议 | `guides/protocol.md` | comm/protocol/* | ✅ | ✅ |
| Poller 后端与性能 | `guides/poller.md` | comm/*Poller.h · PollerMgr.h | — | ✅ |

### 参考 (Reference)
| 页面 | 文件 | 主要源头 | 图 | 状态 |
|---|---|---|---|---|
| 错误码一览 | `reference/error-codes.md` | common/Errors.h | — | ✅ |
| 日志配置项参考 | `reference/log-config.md` | core/log/LoggerConfigInfo.h · 样例 .cfg | — | ✅ |
| Service / App 配置项参考 | `reference/service-app-config.md` | comm/Service.h · app/App.h · 样例 .cfg | — | ✅ |
| API 参考（Doxygen） | — | 头注释自动生成 | — | ⏸（工具化，单独设） |

### 更多 (Meta)
| 页面 | 文件 | 主要源头 | 图 | 状态 |
|---|---|---|---|---|
| 常见问题 FAQ | `faq.md` | 综合 | — | ✅ |
| 贡献指南 | `contributing.md` | 构建/子模块/命名约定/PR | — | ✅ |
| 版本迁移指南 v1.1.1 | `migration-v1.1.1.md` | 仓库根 CHANGELOG | — | ✅ |
| 术语表 | `glossary.md` | 全局 | — | ✅ |
| 示例项目 | `examples.md` | tests/example（30 例，菜单+traits，按模块分组） | — | ✅ |
| 更新日志 | `changelog.md` | 外链 CHANGELOG | — | ✅ |

> "图"列：✅ 已配图 · 🚧 待配图（手写 SVG，见 §6）· —— 无需配图。新增 `getting-started` / `concepts` / `guides` / `reference` 页写完后，需在 `_data/navigation.yml` 与 `index.md` 路线图登记（新增「使用指南 · 网络」「参考」两个分区）。
</content>
