# Log Output Control Items（日志输出控制项）

`llbc` 日志库提供了一套**统一的、按 appender 串行应用的日志输出控制机制**，
用以替代历史上的 `LLBC_LogMuteFilter`。每个 logger 持有一个有序的控制项列表
（`std::vector<LLBC_LogControlItem>`），在 `Logger::OutputLogData()`
内部、对每个 appender 独立、按声明顺序应用。

控制机制由两个设施组成：

- **数据模型**：`LLBC_LogControlItem`（见 `llbc/core/log/LogControl.h`）。
- **执行器**：`LLBC_LogControlMgr`（见 `llbc/core/log/LogControlMgr.h`）。

---

## 1. 控制项三段式结构

每个 `LLBC_LogControlItem` 由三块组成：

### a) Match Rules（匹配规则）

控制项内**已启用**的维度之间为 **AND**（all-of，必须同时命中）；每条规则**自身的取值集合**（如 `matchLineRanges` / `matchFuncs` / `matchThreadIds` / `matchLevels`）则为 **OR**（one-of，任一取值命中即视该条规则命中）。

> 设计取舍：规则间 AND 与 iptables / log4j filter / SQL `WHERE` / Prometheus matcher 的多字段语义一致，符合"过滤器即交集"的工程直觉；如需在多个维度上做 OR，**追加多条控制项**即可。

| 字段 | 标志 | 备注 |
| ---- | ---- | ---- |
| `matchFile` + `matchLineRanges` | `haveFile` | 文件 basename + 行号集合。`matchLineRanges` 是若干 **半开区间** `[begin, end)` 段的并集（与 `LLBC_Stream::Erase / Replace` 等 llbc 范围 API 风格一致）：单行 `N` 内部存为 `[N, N+1)`；为空表示通配整文件。每段必须满足 `0 <= begin && end > begin` |
| `matchFuncs`              | `haveFunc` | 完整函数名列表（不支持通配）；记录的函数名等于其中**任意一个**即命中 |
| `matchThreadIds`          | `haveThreadId` | 原生线程 id 列表；记录的 tid 等于其中**任意一个**即命中 |
| `matchLevels`             | `haveLevel` | 日志级别列表；记录的 level 等于其中**任意一个**即命中。每个元素必须在 `LLBC_LogLevel::Begin..<End` 内 |

`HasAnyMatch()` 校验至少一个维度被启用，否则视为非法控制项（`AddControl` 会
拒绝；配置解析时会静默跳过）。

### b) Appender Scope（应用 appender 集合）

`appenderTypes` 为 `std::vector<int>`，元素为 `LLBC_LogAppenderType::Console / File
/ Network` 之一。**留空 = 对所有 appender 生效**。

### c) Action（生效规则）

| 取值 | 语义 |
| ---- | ---- |
| `LLBC_LogControlAction::Mute`     | 在当前 appender 上丢弃此条记录，并对 `suppressedCount` +1 |
| `LLBC_LogControlAction::SetLevel` | 把记录在当前 appender 链中的 effective level 改写为 `newLevel` |

---

## 2. 应用顺序与作用域

对 **(record, appender)** 这一对：

1. 控制项按声明顺序遍历。
2. `ScopeAllows` 先过 appender 范围（空 = all），不命中则跳过该项。
3. `MatchOne` 在当前 effective level 上做匹配（**已启用规则之间 AND**，每条规则**自身取值集合 OR**）；不命中则跳过该项。
4. 命中后：
   - `Mute`：立刻丢弃该 appender 上的本条记录，`suppressedCount` 原子 +1，
     **停止**遍历当前 appender。
   - `SetLevel`：把 effective level 改写为 `newLevel`；**继续**遍历后续控
     制项（后续控制项可基于改写后的 level 再次匹配，例如先 SetLevel 到
     warn，再用 `level==warn` 的 Mute 项二次过滤）。

> ⚠️ **SetLevel 的改写不会跨 appender 泄漏**。每个 appender 都从
> 记录的原始 level 开始遍历，因此 console 上的 SetLevel 不会影响 file
> appender 看到的 level。

---

## 3. 配置驱动：`logControls`

每个 logger 配置中可以提供一个名为 `logControls` 的字符串字段，**值是一个 JSON
数组**，数组每个元素描述一个控制项。Schema：

```json
[
  {
    "match": {
      "file": "Foo.cpp",                          // 字符串, 通配整文件
      // 或: "file": "Foo.cpp:42"                  // 字符串, 单行
      // 或: "file": "Foo.cpp:10-12"               // 字符串, 行号半开区间 [10, 12)
      // 或: "file": "Foo.cpp:100, 102-112"        // 字符串, 多段 (单行 / 范围混用, 逗号分隔)
      // 或: "file": {"name": "Foo.cpp", "line": 12}                      // 对象, 单行
      // 或: "file": {"name": "Foo.cpp", "line": 10, "lineEnd": 12}       // 对象, 半开区间 [10, 12)
      // 或: "file": {"name": "Foo.cpp", "lines": "100, 102-112"}         // 对象, 多段; 优先级高于 line/lineEnd
      "func": "DoBar",                              // 字符串单值
      // 或: "func": ["DoBar", "DoBaz"]              // 字符串数组多值 (任一命中即匹配)
      "threadId": 12345,                            // 整型单值
      // 或: "threadId": [12345, 23456]              // 整型数组多值 (任一命中即匹配)
      "level": "warn"                               // 字符串大小写不敏感, 或整型
      // 或: "level": ["warn", "error"]              // 字符串/整型数组多值 (任一命中即匹配)
    },
    "appenders": ["console", "file", "network"],  // 缺省 == 全部 appender
    "action": "mute",                             // "mute" 或 "setlevel"
    "newLevel": "error"                           // action == "setlevel" 时必填
  }
]
```

> 行号语法采用**左闭右开** `[begin, end)`——`begin` 包含、`end` 不包含——
> 这与 llbc 内 `LLBC_Stream::Erase / Replace` 等使用 `[n0, n1)` 半开区间的
> 风格保持一致。例如 `"Foo.cpp:10-12"` 命中第 10、11 行，**不**命中第 12 行；
> `"Foo.cpp:100, 102-112"` 命中第 100 行 与 第 102..111 行，**不**命中 101 / 112。

### 解析容错

下列情况**静默跳过单个控制项**，不影响其他合法控制项：

- 元素不是对象 / `match` 不是对象。
- 没有任何启用的 match 维度（`HasAnyMatch()` 失败）。
- `match.file` 既不是字符串也不是 `{name[, line, lineEnd | lines]}` 对象、或
  `name` 为空、或字符串语法无法解析（任一段为空 / 数字部分非纯数字 /
  `end <= begin` / 整体没有 `:` 分隔但有冒号前缀等）、或对象的 `line < 0`、
  `lineEnd < 0`、`line/lineEnd` 同时给出但 `lineEnd <= line`、`lines` 不是字符
  串或解析失败。
- `match.func` 不是字符串、也不是字符串数组；若是数组则其中的非字符串 / 空字符串
  条目被静默丢弃；若最终没有任何有效条目，整个 `func` 规则被忽略（控制项可能因
  此变成"无任何匹配维度"被整体跳过）。
- `match.threadId` 不是整型、也不是整型数组；若是数组则其中非整型条目被静默丢
  弃；若最终没有任何有效条目，整个 `threadId` 规则被忽略。
- `match.level` 不是合法 level、也不是合法 level 数组；若是数组则其中非法 level
  条目被静默丢弃；若最终没有任何有效条目，整个 `level` 规则被忽略。
- `action` 不是已知字符串（不区分大小写：`"mute" / "setlevel"`）。
- `action == "setlevel"` 但 `newLevel` 缺失或非法。

下列情况**静默跳过单条 appender**，控制项整体保留：

- `appenders` 数组中含未识别的字符串。

整个 `logControls` 的 JSON 解析失败（`HasParseError() || !IsArray()`）也是
**静默跳过**，等价于"没有配置任何控制项"。

### 配置文件写法

`.cfg`（property-style）和 `.xml` 都支持，**一定要把 JSON 写在一行内**
（`.cfg` 的 parser 不支持反斜杠续行）：

```ini
mylogger.logControls=[{"match":{"func":"NoisyFunc"},"action":"mute"},{"match":{"file":"perf.cpp"},"action":"setlevel","newLevel":"warn"}]
```

```xml
<mylogger>
  <logControls>[{"match":{"func":"NoisyFunc"},"action":"mute"}]</logControls>
</mylogger>
```

---

## 4. 控制 API（`LLBC_Logger`）

控制项**只从配置安装**。生产代码不应直接调用控制 API；下列接口仅供以下
两条内部路径以及测试代码使用：

- `Logger::Initialize`：从 `LoggerConfigInfo::GetLogControls()` 一次性装载。
- `LoggerMgr::Reload`：重新解析配置文件后，对每个 logger 调用一次完整替换。

```cpp
int    SetLogControls(const std::vector<LLBC_LogControlItem> &items);
size_t GetLogControlCount() const;
void   GetLogControls(std::vector<LLBC_LogControlItem> &out) const;
uint64 GetLogControlSuppressedCount() const;
void   ResetLogControlSuppressedCount();
```

- `SetLogControls(items)`：**原子整体替换**当前控制项列表。先全量校验
  `items`，任一项非法则整体失败 (`LLBC_ERROR_INVALID`)、当前快照保持不变；
  校验通过后单次原子发布新快照——正在 `OutputLogData()` 中的并发记录要么
  完整看到旧快照、要么完整看到新快照，**不会观察到中间空状态**。
  - 传空向量会发布一个空快照，`IsEmpty()` 快路径会跳过整段控制逻辑。
  - `SuppressedCount` 不会被该操作清零（如需归零请显式调用
    `ResetLogControlSuppressedCount()`）。
  - 失败原因 (`LLBC_GetLastError`) `LLBC_ERROR_INVALID`：`HasAnyMatch()` 失
    败、`haveFile && matchFile.empty()`、`matchLineRanges` 中存在 `begin < 0`
    或 `end <= begin` 的非法段、`haveFunc && matchFuncs.empty()`、
    `matchFuncs` 中存在空字符串、`haveThreadId && matchThreadIds.empty()`、
    `haveLevel && matchLevels.empty()`、`matchLevels` 中存在非法 level、
    非法 `action`、`SetLevel` 但 `newLevel` 非法、`appenderTypes` 中含
    无效类型。
- `GetLogControlSuppressedCount()`：**按 (record, appender) 计数**——一条
  记录被 N 个 appender 静音就计 N。

设计注记：之所以**不**提供运行时的 `Add / Remove / Clear` 增量 API——
log control 是**配置项**，"哪些日志应当被静音/提级"应当跟配置文件同步演
进（包括代码审查、版本管理、reload）。开放运行时增量入口会诱导跳过这些
约束，违背 "可观测性配置即代码" 的初衷。所有读路径接口 (`GetLogControls`,
`GetLogControlCount`, `Get/ResetLogControlSuppressedCount`) 始终可用。

`SetLogControls` 与 `Apply` 共用 `LLBC_SpinLock`（仅在写侧之间互斥，
读侧 lock-free）；`IsEmpty()` 走 `volatile bool` fast-path，用于无控制项
时完全跳过整段控制逻辑。

---

## 5. 典型用法

控制项的"正常"装载入口是**配置文件**。下面每个例子先给等价的 XML/JSON 片段，
再附一段"手动用 `SetLogControls` 安装"的代码——后者主要面向单测、自检工具
或诊断小程序，正式业务代码请优先走配置 + `LLBC_LoggerMgrSingleton->Reload()`。

每次 `SetLogControls` 都是**整体替换**：要"在原列表上加一条"必须自行先
`GetLogControls(...)` 取回当前列表、push 后整体回写。

### 5.1 仅 console 上静音某个吵闹函数

```xml
<mylogger>
  <logControls>
    [{"match":{"func":"NoisyFunc"},"appender":"console","action":"mute"}]
  </logControls>
</mylogger>
```

```cpp
LLBC_LogControlItem item;
item.haveFunc = true;
item.matchFuncs.emplace_back("NoisyFunc");
item.appenderTypes.push_back(LLBC_LogAppenderType::Console);
item.action = LLBC_LogControlAction::Mute;
logger->SetLogControls({item});
```

### 5.2 文件级"提级"——所有低于 warn 的日志都改写到 warn 落地

```xml
<logControls>
  [{"match":{"file":"perf_hot_path.cpp"},"action":"setlevel","newLevel":"warn"}]
</logControls>
```

```cpp
LLBC_LogControlItem item;
item.haveFile = true;
item.matchFile = "perf_hot_path.cpp";
item.action = LLBC_LogControlAction::SetLevel;
item.newLevel = LLBC_LogLevel::Warn;
logger->SetLogControls({item});
```

### 5.3 行号范围 / 多段静音 —— 半开区间 `[begin, end)`

```xml
<!-- 单段: file=hot.cpp 上 [120, 160) -->
<logControls>[{"match":{"file":"hot.cpp:120-160"},"action":"mute"}]</logControls>
<!-- 多段: 50, 60-65, 70 -->
<logControls>[{"match":{"file":"hot.cpp:50, 60-65, 70"},"action":"mute"}]</logControls>
```

```cpp
LLBC_LogControlItem item;
item.haveFile  = true;
item.matchFile = "hot.cpp";
item.matchLineRanges.emplace_back(50, 51);   // 单行 50
item.matchLineRanges.emplace_back(60, 65);   // [60, 65)
item.matchLineRanges.emplace_back(70, 71);   // 单行 70
item.action    = LLBC_LogControlAction::Mute;
logger->SetLogControls({item});
```

> 推荐统一通过 `matchLineRanges` 表达单行 / 范围 / 多段三种情况；单行 `N`
> 用 `emplace_back(N, N+1)`；空 `matchLineRanges` 表示通配整个文件。

### 5.4 多值 func / threadId —— 任一命中即匹配

```xml
<logControls>
  [
    {"match":{"func":["NoisyA","NoisyB"]},"action":"mute"},
    {"match":{"threadId":[12345,23456]},"action":"mute"}
  ]
</logControls>
```

```cpp
LLBC_LogControlItem byFunc;
byFunc.haveFunc = true;
byFunc.matchFuncs.emplace_back("NoisyA");
byFunc.matchFuncs.emplace_back("NoisyB");
byFunc.action = LLBC_LogControlAction::Mute;

LLBC_LogControlItem byTid;
byTid.haveThreadId = true;
byTid.matchThreadIds.push_back(12345);
byTid.matchThreadIds.push_back(23456);
byTid.action = LLBC_LogControlAction::Mute;

logger->SetLogControls({byFunc, byTid});
```

### 5.5 链式：先提级再按级别静音

链式两条规则必须 **在同一个 `SetLogControls` 调用里按声明序安装**，因为
chain 顺序由列表顺序决定，`SetLogControls` 是原子整体替换。

```xml
<logControls>
  [
    {"match":{"file":"link.cpp"},"action":"setlevel","newLevel":"warn"},
    {"match":{"level":"warn"},"action":"mute"}
  ]
</logControls>
```

```cpp
LLBC_LogControlItem setlvl;
setlvl.haveFile = true;
setlvl.matchFile = "link.cpp";
setlvl.action   = LLBC_LogControlAction::SetLevel;
setlvl.newLevel = LLBC_LogLevel::Warn;

LLBC_LogControlItem mute;
mute.haveLevel  = true;
mute.matchLevels.push_back(LLBC_LogLevel::Warn);
mute.action     = LLBC_LogControlAction::Mute;

logger->SetLogControls({setlvl, mute});
// link.cpp 上的 INFO 记录: 先被改写为 WARN, 再被 level==WARN 的 mute 命中.
```

### 5.6 多级别 OR + 多维度 AND —— 仅静音特定函数的 warn / error

```xml
<logControls>
  [{"match":{"func":"Stub","level":["warn","error"]},"action":"mute"}]
</logControls>
```

```cpp
LLBC_LogControlItem item;
item.haveFunc = true;
item.matchFuncs.emplace_back("Stub");                 // func 维度: 等于 "Stub"
item.haveLevel = true;
item.matchLevels.push_back(LLBC_LogLevel::Warn);      // level 维度: warn 或 error
item.matchLevels.push_back(LLBC_LogLevel::Error);
item.action = LLBC_LogControlAction::Mute;
logger->SetLogControls({item});
// 命中条件: 函数名 == "Stub" 且 (level == warn 或 level == error).
// 同函数的 info / 其它函数的 warn / error 都不会被静音.
```

---

## 6. 配置 Reload

`LLBC_LoggerMgrSingleton->Reload(newCfgFilePath)` 会把已存在的 logger 配置
（默认重新加载首次 `Initialize` 用的那份配置文件）应用到 live logger 上。
对 `logControls` 而言：

- 配置文件解析仍走标准管线：非法元素静默丢弃，同 `Initialize` 完全一致。
- 内部对每个 logger 调一次 `Logger::SetLogControls(...)`：**整体重建**控制项
  列表。这是 reload 时 `logControls` 唯一的更新方式——既不增量打补丁，也不
  跨快照合并；旧快照在新快照原子发布后被整体淘汰。
- 某条 reload 失败（任一项非法）时，对应 logger 的旧 logControls 保持有效
  （`SetLogControls` 的 all-or-nothing 语义），整个 `Reload()` 返回
  `LLBC_FAILED` 并设置 `LastError`。
- `SuppressedCount` 不会被 reload 清零。如需归零请在 reload 后显式调用
  `ResetLogControlSuppressedCount()`。
- 已知限制（继承自 `LoggerMgr::Reload`）：仅刷新已存在的 logger；`Reload`
  期间新增的 logger 段不会被创建。

```cpp
// 改了配置文件，希望热生效:
if (LLBC_LoggerMgrSingleton->Reload() != LLBC_OK)
    fprintf(stderr, "reload failed: %s\n", LLBC_FormatLastError());

// 也可重新指向另一份配置文件:
if (LLBC_LoggerMgrSingleton->Reload("/etc/myapp/log.alt.xml") != LLBC_OK)
    fprintf(stderr, "reload failed: %s\n", LLBC_FormatLastError());
```

> 控制项不提供"绕过配置文件、由程序内代码动态注入"的稳定入口；如确需在
> 测试 / 诊断工具里手动装入一组规则，可直接调用 `Logger::SetLogControls`，
> 但生产代码不应依赖此路径——它存在的目的是供 `Initialize` / `Reload` 复用，
> 不属于对外稳定 API。

---

## 7. 与旧 `LLBC_LogMuteFilter` 的差异

| 维度 | 旧 LogMuteFilter | 新 LogControlMgr |
| ---- | ---------------- | ------------------- |
| 维度组合 | 三类 mute 规则平铺，互不交叉 | 单控制项内多维度 AND，每条规则的取值集合内部 OR；多控制项按声明顺序串联 |
| 应用范围 | 整 logger（所有 appender 共享） | 按 appender 单独应用（可显式限定 appender） |
| 动作 | 仅 Mute（含"提级到 minLevel 的隐式 Mute"） | Mute / SetLevel 显式分开；SetLevel 在链中可被后续匹配 |
| 配置 | 三个独立字符串 key + 自定义分隔符 | 单一 JSON 数组字符串 |
| API 命名 | `*LogMute*` | `*LogControl*` |
| 计数语义 | 每条 record 计 1 次 | 每条 record × 命中 appender 计 1 次 |

旧 API 已**全部移除**，业务层若仍在使用旧 `*LogMute*` 系列接口，须按新 API
重构。
