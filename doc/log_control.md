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

控制项内**已启用**的维度之间为 **AND**（all-of，必须同时命中）；每条规则**自身的取值集合**（如 `file.lineRanges` / `func.values` / `threadId.values` / `level.values`）则为 **OR**（one-of，任一取值命中即视该条规则命中）。

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

每个 logger 配置中可以提供一个名为 `logControls` 的**嵌套 XML 子树**，子树
下每个 `<item>` 描述一个控制项。

> **配置载体限制**：`logControls` **仅 XML 格式 (`.xml`) 支持**。`.cfg`
> (property-style) 是扁平的 `key=value` 格式，无法承载 `item -> match ->
> file/func/threadId/level/...` 这种嵌套结构；若在 `.cfg` 中写
> `myLogger.logControls=...` 会被 LoggerConfigurator 检测到并触发 stderr
> 警告，然后整段忽略。

### XML schema

本 schema 刻意**只用子元素、不用 XML 属性**：所有 payload 一律走内联文本或
子元素，多值规则（OR）用同名重复子元素承载。这样 XML 与未来的 YAML/JSON
loader 结构完全对齐（每个元素 == map 条目；重复子元素 == list），无需在
"该字段做 attribute 还是做 key"上分裂。`<action>` 作为 sum type（mute 无操
作数；setLevel 携带 `<newLevel>` 操作数），其判别 tag 也作为 `<type>` 子元素
承载——保持"操作数与判别 tag 都在 `<action>` 子序列里"的一致性，也便于将来
扩展更多 action 变体。

```xml
<logControls>
  <item>
    <match>
      <!-- 文件名 + 行号 (多个 <file> 同一 item 内只取第一个) -->
      <file><name>Foo.cpp</name></file>                     <!-- 通配整文件 -->
      <file>
        <name>Foo.cpp</name>
        <line>42</line>                                     <!-- 单行 -->
      </file>
      <file>
        <name>Foo.cpp</name>
        <line>10-12</line>                                  <!-- 半开区间 [10, 12) -->
      </file>
      <file>
        <name>Foo.cpp</name>
        <line>100</line>                                    <!-- 多段: 重复 <line> -->
        <line>102-112</line>
      </file>

      <!-- 函数名 (重复多次以表达 OR) -->
      <func>DoBar</func>
      <func>DoBaz</func>

      <!-- 线程 id (重复多次以表达 OR) -->
      <threadId>12345</threadId>
      <threadId>23456</threadId>

      <!-- 日志级别 (重复多次以表达 OR; 字符串大小写不敏感, 或整型) -->
      <level>warn</level>
      <level>error</level>
    </match>

    <appenders>                                             <!-- 可选; 缺省 == 全部 appender -->
      <appender>console</appender>
      <appender>file</appender>
    </appenders>

    <action>                                                <!-- 变体 1: 静音, 无操作数 -->
      <type>mute</type>
    </action>
  </item>

  <item>
    <match><file><name>perf.cpp</name></file></match>
    <action>                                                <!-- 变体 2: 提级, 操作数为 <newLevel> -->
      <type>setLevel</type>
      <newLevel>warn</newLevel>
    </action>
  </item>
</logControls>
```

> 行号语法采用**左闭右开** `[begin, end)`——`begin` 包含、`end` 不包含——
> 这与 llbc 内 `LLBC_Stream::Erase / Replace` 等使用 `[n0, n1)` 半开区间的
> 风格保持一致。例如 `<line>10-12</line>` 命中第 10、11 行，**不**命中第
> 12 行；多段写法 `<line>100</line><line>102-112</line>` 命中第 100 行 与
> 第 102..111 行，**不**命中 101 / 112。

### 解析规则（`ParseLogControls`）

下文给出 `LoggerConfigInfo::ParseLogControls` 把上述 XML 子树物化为
`std::vector<LLBC_LogControlItem>` 的**逐字段、逐分支**的精确规则。它是
schema 的执行级补充——schema 描述"长什么样"，本节描述"按什么顺序解析、
谁先谁后、哪些情况静默跳过、哪些情况拒掉整个 `<item>`"。

#### 遍历框架

1. 仅当 `cfg` 是 Dict、且其 `Children` 是 Seq 时才会进入遍历；否则视为
   "没有配置任何控制项"，**静默返回**——这覆盖了 `.cfg` 扁平配置、XML 中
   缺 `<logControls>` 节点等所有"无控制项"形态。
2. 遍历 `<logControls>` 的直接子序列，仅识别 tag 名等于 `"item"` 的子元素；
   其他 tag（含偶然写入的 `<foo/>`）**静默跳过**。
3. 对每个 `<item>`，按 **match → appenders → action** 的顺序解析，并按
   "解析到非法形态即 `continue` 整 item"的策略保持 all-or-nothing 单元语义。
   只有走完全部三段、未被任何分支 `continue` 的 item 才会 `push_back` 到
   输出列表。声明顺序在输出列表中被**完整保留**（后续 `LogControlMgr` 按
   顺序应用，链式语义依赖该顺序）。

#### 1) match 段

- 在 `<item>` 的直接子序列里查找第一个 tag 为 `"match"` 的子元素，**只取
  第一个**，其余 `<match>` 兄弟静默忽略。`<match>` 缺失或非 Dict 时，
  match 段什么都不解析，最终在 `HasAnyMatch()` 校验处被丢弃。
- match 内部按 `file → func → threadId → level` 的顺序解析；它们彼此独立，
  解析顺序仅影响**错误检出顺序**，不影响最终语义。

  | 维度 | 子元素形态 | 多值/单值 | 规则 |
  | ---- | ---- | ---- | ---- |
  | `file` | `<file><name>...</name>[<line>...</line>...]</file>` | 同 item 内**只取第一个** | 多个 `<file>` 时只解析第一个；第一个若非法（`<name>` 缺失/为空、任一 `<line>` 非法等）→ **整 item `continue`**。要表达"多个文件"应当写多个 `<item>` |
  | `func` | `<func>name</func>`，可重复 | 多值（OR） | 取 `tolower` 前的原始文本；空/全空白项**静默丢弃**；最终列表非空才启用该规则 |
  | `threadId` | `<threadId>n</threadId>`，可重复 | 多值（OR） | 每项必须能被 `ParseThreadIdStr` 解析为合法 tid；非法项**静默丢弃**；最终列表非空才启用 |
  | `level` | `<level>warn</level>`，可重复 | 多值（OR） | 接受字符串（不区分大小写）或整型；越界/非法项**静默丢弃**；最终列表非空才启用 |

- `<file>` 的 `<line>` 子元素可 0 至多个：0 个表示整文件通配；每个 `<line>` 的内联文本是**一段** `N` 或 `N-M`（半开区间，详见 `ParseLineToken`），任一 `<line>` 非法整 item 被跳过。多段 OR 通过重复 `<line>` 表达，与 `<func>` / `<threadId>` / `<level>` 的多值语义完全对称。
- 单个维度的取值集合**在去掉非法元素后为空**等价于"该维度未启用"，不会
  让整 item 失败；但若所有维度都未启用，会在 match 段末尾的 `HasAnyMatch()`
  处统一 `continue`。
- 任何一个**已启用维度**自身仍然是 OR；维度之间是 AND——这一行为是
  `LogControlMgr::MatchOne` 的运行时语义，`ParseLogControls` 只负责"启用
  对应维度并写入取值列表"。

#### 2) appenders 段

- 可选；遍历 `<item>` 直接子序列，命中第一个 `<appenders>` 后**只解析它一个**。
- `<appender>` 文本通过 `ParseAppenderTypeStr` 翻译为 `LLBC_LogAppenderType`
  整型；未识别字符串**静默丢弃**，整个 item **不会被** `continue`——这是
  schema 中"未知 appender 名静默忽略，控制项整体保留"的来源（与 match 段
  对非法 `<file/>` 的强约束**刻意不一致**）。
- `appenderTypes` 为空向量在运行时含义为"对所有 appender 生效"。

#### 3) action 段

- 必选；遍历 `<item>` 直接子序列，命中第一个 `<action>` 后**只解析它一个**，
  其余 `<action>` 静默忽略。`<action>` 缺失 → 整 item `continue`。
- `<action>` 内的 `<type>` 子元素文本通过 `tolower` 标准化，识别两种变体：
  - `<type>mute</type>`：写入 `action = Mute`，不再读任何操作数子元素。
  - `<type>setLevel</type>`：写入 `action = SetLevel`，并要求**`<action>` 自身**
    的直接子序列里出现至少一个合法的 `<newLevel>`：
    - 文本通过 `ParseLevelStr` 解析；
    - **只取第一个**，无论其合法与否；
    - 第一个合法 → 写入 `item.newLevel`；
    - 第一个非法 或 完全没有 `<newLevel>` → 整 item `continue`。
  - 其他 `<type>`（包括 `<type>` 缺失/空文本、未知字符串如 `"unknownaction"`）
    → 整 item `continue`。

#### 静默跳过 vs 整项拒绝（速查）

下列情况**静默跳过单个控制项**，不影响同列表中的其他合法控制项：

| 触发条件 | 说明 |
| ---- | ---- |
| 元素 tag 不是 `<item>` | 整段 schema 只接受 `<item>` 作为子元素 |
| `HasAnyMatch()` 失败 | 包括 `<match/>` 空块，以及所有维度取值集合在丢弃非法项后变空 |
| 第一个 `<file>` 非法 | `<name>` 缺失/为空、任一 `<line>` 非法（非数字、负数、区间空/翻转等） |
| `<action>` 缺失 | item 必须显式声明 action |
| `<action>` 内 `<type>` 不是已知变体 | 含 `<type>` 缺省、未知字符串 |
| `<action>` 内 `<type>setLevel</type>` 缺 `<newLevel>` 或值非法 | setLevel 操作数硬要求 |

下列情况**静默跳过单条子元素**，控制项**整体保留**：

| 触发条件 | 影响 |
| ---- | ---- |
| `<func>` / `<threadId>` / `<level>` 个别项非法或为空 | 仅丢弃该项；若该维度因此空了，则该维度自动不启用 |
| `<appender>` 文本未识别 | 仅丢弃该项；其余 appender 仍生效；空集合等价于"全部 appender" |
| `<logControls>` / `<item>` 下出现未知 tag | 直接忽略，不会让宿主 item 失败 |

整个 `<logControls>` 子树本身缺失 / 不是合法 Dict（如 `.cfg` 配置不支持
该子树）也是**静默跳过**，等价于"未配置任何控制项"。

#### 显式忽略策略（不是 bug）

- 同 `<item>` 内出现多个 `<match>` / `<file>` / `<appenders>` / `<action>`
  时**只取第一个**：这条规则统一了"标量字段在 XML 中误写多份"的歧义；想
  表达"多文件 mute / 多 appender 限定"应当通过**多个 `<item>`** 或同名重
  复子元素（`<func>` / `<threadId>` / `<level>`）来表达。
- 解析阶段**不做去重**：完全相同的两条 `<item>` 会原样进入输出列表；这是
  刻意保留的——`LogControlMgr` 按顺序应用，重复项会在统计层多算一次，但
  语义上等价；强行去重反而会丢失"重复即压力测试"这类用例。
- 解析阶段**不做跨 item 校验**：例如"link.cpp 先 SetLevel 到 warn、再用
  level==warn Mute"这种链式构造完全合法，由顺序保证。

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
    <item>
      <match><func>NoisyFunc</func></match>
      <appenders><appender>console</appender></appenders>
      <action><type>mute</type></action>
    </item>
  </logControls>
</mylogger>
```

```cpp
LLBC_LogControlItem item;
item.func.enabled = true;
item.func.values.emplace_back("NoisyFunc");
item.appenderTypes.push_back(LLBC_LogAppenderType::Console);
item.action = LLBC_LogControlAction::Mute;
logger->SetLogControls({item});
```

### 5.2 文件级"提级"——所有低于 warn 的日志都改写到 warn 落地

```xml
<logControls>
  <item>
    <match><file><name>perf_hot_path.cpp</name></file></match>
    <action><type>setLevel</type><newLevel>warn</newLevel></action>
  </item>
</logControls>
```

```cpp
LLBC_LogControlItem item;
item.file.enabled = true;
item.file.file = "perf_hot_path.cpp";
item.action = LLBC_LogControlAction::SetLevel;
item.newLevel = LLBC_LogLevel::Warn;
logger->SetLogControls({item});
```

### 5.3 行号范围 / 多段静音 —— 半开区间 `[begin, end)`

```xml
<!-- 单段: file=hot.cpp 上 [120, 160) -->
<logControls>
  <item>
    <match>
      <file>
        <name>hot.cpp</name>
        <line>120-160</line>
      </file>
    </match>
    <action><type>mute</type></action>
  </item>
</logControls>
<!-- 多段: 50, 60-65, 70 -->
<logControls>
  <item>
    <match>
      <file>
        <name>hot.cpp</name>
        <line>50</line>
        <line>60-65</line>
        <line>70</line>
      </file>
    </match>
    <action><type>mute</type></action>
  </item>
</logControls>
```

```cpp
LLBC_LogControlItem item;
item.file.enabled = true;
item.file.file    = "hot.cpp";
item.file.lineRanges.emplace_back(50, 51);   // 单行 50
item.file.lineRanges.emplace_back(60, 65);   // [60, 65)
item.file.lineRanges.emplace_back(70, 71);   // 单行 70
item.action       = LLBC_LogControlAction::Mute;
logger->SetLogControls({item});
```

> 推荐统一通过 `file.lineRanges` 表达单行 / 范围 / 多段三种情况；单行 `N`
> 用 `emplace_back(N, N+1)`；空 `file.lineRanges` 表示通配整个文件。

### 5.4 多值 func / threadId —— 任一命中即匹配

```xml
<logControls>
  <item>
    <match>
      <func>NoisyA</func>
      <func>NoisyB</func>
    </match>
    <action><type>mute</type></action>
  </item>
  <item>
    <match>
      <threadId>12345</threadId>
      <threadId>23456</threadId>
    </match>
    <action><type>mute</type></action>
  </item>
</logControls>
```

```cpp
LLBC_LogControlItem byFunc;
byFunc.func.enabled = true;
byFunc.func.values.emplace_back("NoisyA");
byFunc.func.values.emplace_back("NoisyB");
byFunc.action = LLBC_LogControlAction::Mute;

LLBC_LogControlItem byTid;
byTid.threadId.enabled = true;
byTid.threadId.values.push_back(12345);
byTid.threadId.values.push_back(23456);
byTid.action = LLBC_LogControlAction::Mute;

logger->SetLogControls({byFunc, byTid});
```

### 5.5 链式：先提级再按级别静音

链式两条规则必须 **在同一个 `SetLogControls` 调用里按声明序安装**，因为
chain 顺序由列表顺序决定，`SetLogControls` 是原子整体替换。

```xml
<logControls>
  <item>
    <match><file><name>link.cpp</name></file></match>
    <action><type>setLevel</type><newLevel>warn</newLevel></action>
  </item>
  <item>
    <match><level>warn</level></match>
    <action><type>mute</type></action>
  </item>
</logControls>
```

```cpp
LLBC_LogControlItem setlvl;
setlvl.file.enabled = true;
setlvl.file.file    = "link.cpp";
setlvl.action       = LLBC_LogControlAction::SetLevel;
setlvl.newLevel     = LLBC_LogLevel::Warn;

LLBC_LogControlItem mute;
mute.level.enabled = true;
mute.level.values.push_back(LLBC_LogLevel::Warn);
mute.action        = LLBC_LogControlAction::Mute;

logger->SetLogControls({setlvl, mute});
// link.cpp 上的 INFO 记录: 先被改写为 WARN, 再被 level==WARN 的 mute 命中.
```

### 5.6 多级别 OR + 多维度 AND —— 仅静音特定函数的 warn / error

```xml
<logControls>
  <item>
    <match>
      <func>Stub</func>
      <level>warn</level>
      <level>error</level>
    </match>
    <action><type>mute</type></action>
  </item>
</logControls>
```

```cpp
LLBC_LogControlItem item;
item.func.enabled = true;
item.func.values.emplace_back("Stub");                // func 维度: 等于 "Stub"
item.level.enabled = true;
item.level.values.push_back(LLBC_LogLevel::Warn);     // level 维度: warn 或 error
item.level.values.push_back(LLBC_LogLevel::Error);
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
| 配置 | 三个独立字符串 key + 自定义分隔符（`.cfg` / `.xml` 都支持） | 单一嵌套 XML 子树（仅 `.xml` 支持） |
| API 命名 | `*LogMute*` | `*LogControl*` |
| 计数语义 | 每条 record 计 1 次 | 每条 record × 命中 appender 计 1 次 |

旧 API 已**全部移除**，业务层若仍在使用旧 `*LogMute*` 系列接口，须按新 API
重构。
