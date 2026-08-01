# Log Output Control Items（日志输出控制项）

每个 logger 持有一个有序的 `std::vector<LLBC_LogControlItem>`；`Logger::OutputLogData()` 里对每个 appender 独立、按声明顺序应用。控制项**只从配置装载**（`Initialize` / `Reload`）。

实现分三层，源码见 `llbc/core/log/`：

| 层 | 类型 | 职责 |
| --- | --- | --- |
| 配置项 | `LLBC_LogControlItem`（`LogControl.h`） | 纯数据结构，描述一条控制项：一组 match 规则 + appender scope + 一个 action |
| 执行器 | `LLBC_LogControlExecutor`（`LogControlExecutor.h`） | 一条配置项编译后的"匹配 + 动作"最小单元；`SetLogControls` 时**一次性**编译为不可变形态，热路径不再做校验 |
| 管理器 | `LLBC_LogControlMgr`（`LogControlMgr.h`） | 持有 `shared_ptr<const ExecutorList>` 快照，对外提供 `Apply`；`Initialize` / `Reload` 时原子发布新快照，`Apply` 拷指针后**出锁遍历** |

---

## 1. 控制项结构

结构定义位于 `llbc/core/log/LogControl.h`：

```cpp
template <typename T>
struct LLBC_LogMatchSet
{
    bool enabled;
    std::vector<T> values;
};

struct LLBC_LogFileMatch
{
    bool enabled;
    LLBC_String file;                             // file basename
    std::vector<std::pair<int, int>> lineRanges;  // half-open [begin, end)
};

struct LLBC_LogControlItem
{
    // a) match rules; AND-combined when enabled, each rule's values OR-combined.
    LLBC_LogFileMatch               file;
    LLBC_LogMatchSet<LLBC_String>   func;         // exact function names, no wildcard
    LLBC_LogMatchSet<LLBC_ThreadId> threadId;
    LLBC_LogMatchSet<int>           level;        // matched against effective level

    // b) appender scope; empty == all appenders.
    std::vector<int> appenderTypes;               // LLBC_LogAppenderType::{Console, File, Network}

    // c) action; default Unset -> must be assigned explicitly.
    int action;                                   // LLBC_LogControlAction::Mute / SetLevel
    int newLevel;                                 // valid when action == SetLevel

    bool HasAnyMatch() const;                     // 至少一个维度启用，否则非法
};
```

三点关键语义（源码 doc-comment 已述，此处强调）：

- **AND/OR**：已启用的 match 维度之间是 AND；单维度内 `values` / `lineRanges` 是 OR。跨维度 OR 请写多条 `<logControl>`。
- **`file.lineRanges`** 是半开区间 `[begin, end)` 的并集：单行 `N` 存为 `[N, N+1)`，空 = 通配整文件。每段须满足 `0 <= begin < end`。
- **`level` 匹配的是"当前 effective level"**（可与前面 executor 的 `SetLevel` 链式组合），不是 `data.level`。
- **`action` 必须显式赋值**：默认哨兵 `Unset` 会被 `SetLogControls` 拒绝，避免漏写 action 静默变成破坏性 Mute。`Mute` 命中即链遍历终止；`SetLevel` 只改写 level，遍历继续。

---

## 2. 应用顺序

```cpp
LLBC_LogControlApplyResult Apply(int appenderType,
                                 const LLBC_LogData &data,
                                 int originalLevel) const;
// LLBC_LogControlApplyResult { bool muted; int effectiveLevel; }
```

对 `(record, appender)` 这一对：

1. 拷贝 `shared_ptr<const ExecutorList>` 快照后**出锁遍历**；executor 按声明顺序推进。
2. 每个 executor：先过 appender scope（空 == all），再对所有已启用维度 `Match(data, currentLevel)`；任一失败即跳过。level 规则读的是 `currentLevel`，不是 `data.level`。
3. 命中后：`Mute` → `muted = true`，**立即返回**；`SetLevel` → 更新 `currentLevel`，**继续**遍历。

---

## 3. XML 配置：`logControls`

`logControls` 是每个 logger 配置下的**嵌套子树**，每个 `<logControl>` 描述一个控制项。**仅 `.xml` 支持**——`.cfg` 是扁平 `key=value`，无法承载嵌套；若在 `.cfg` 里写 `myLogger.logControls=...` 会 stderr 告警并整段忽略。

```xml
<logControls>
  <logControl>
    <match>
      <!-- file: 每个 <logControl> 至多一个 <file>；多文件请写多条 <logControl>。
           `name` 必填，`lines` 可选：
             缺省 / 空       -> 整文件通配
             "N"             -> 单行, 内部存为 [N, N+1)
             "N-M"           -> 半开区间 [N, M), 需 M > N
             "s1, s2, ..."   -> 多段 OR, 以逗号 ',' 分隔, 段两侧允许空白 -->
      <file name="Foo.cpp" lines="42, 60-65, 100-110"/>

      <!-- 多值 OR：重复子元素 -->
      <func>DoBar</func>
      <func>DoBaz</func>
      <threadId>12345</threadId>
      <level>warn</level>
      <level>error</level>
    </match>

    <appenders>                    <!-- 可选；缺省 / 为空 == 全部 appender -->
      <appender>console</appender>
      <appender>file</appender>
    </appenders>

    <action>                       <!-- mute 无操作数 -->
      <type>mute</type>
    </action>
  </logControl>

  <logControl>
    <match><file name="perf.cpp"/></match>
    <action>                       <!-- setLevel 携带 <newLevel> -->
      <type>setLevel</type>
      <newLevel>warn</newLevel>
    </action>
  </logControl>
</logControls>
```

### 字段解析规则（对照 `ParseLogControls`）

| 字段 | 允许形态 | 值语义 |
| --- | --- | --- |
| `<file name="..." lines="..."/>` | attr 形式；`name` 必填非空；`lines` 可选字符串 | `name` 是 basename；`lines` 段间以 `,` 分隔，段两侧空白 strip 后忽略；单段 `N` 存为 `[N, N+1)`，`N-M` 存为 `[N, M)`（要求 `0 <= N < M < INT_MAX`） |
| `<func>` | 单个或重复子元素 | 每项 strip 后非空即收；OR |
| `<threadId>` | 单个或重复子元素 | 十进制整数，允许前导 `+` / `-`；范围 `LLBC_ThreadId`；OR |
| `<level>` | 单个或重复子元素 | 名称（`Debug` / `Info` / … 大小写不敏感）**或**十进制整数；必须落在 `LLBC_LogLevel::Begin..End` 内；OR |
| `<appenders><appender>` | 容器 + 重复子元素 | 名称：`console` / `file` / `network`，大小写不敏感；未知名**静默丢弃**（不拒 item）；容器缺失 / 全部无效 == 全部 appender |
| `<action><type>` | 必填 | 大小写不敏感；仅 `mute` / `setlevel` |
| `<action><newLevel>` | 仅 `setLevel` 时读取 | 同 `<level>` 值语义 |

### 错误处理速查

`ParseLogControls` 的三条原则：**未知即静默丢弃、可降级维度自动禁用、结构性错误拒整条 logControl**。

| 触发条件 | 处理 |
| --- | --- |
| `<logControls>` 缺失 / 非 Dict / 出现在 `.cfg` 中 | 静默忽略整段（`.cfg` 会额外发一条 stderr 告警） |
| `<logControl>` 下的未知 tag、多余的 `<match>` / `<file>` / `<appenders>` / `<action>` | 静默忽略；同名标量子元素只取第一次出现的值 |
| `<func>` / `<threadId>` / `<level>` 个别项非法或为空 | 只丢该项；若该维度最终无有效值，则该维度自动不启用；item 保留 |
| `<appender>` 未知名 | 静默丢弃；若全部无效，等价于 appenders 缺失（== 全部 appender） |
| `<file>` 存在但 `name` 空 **或** `lines` 任一段非法 | **拒整条 logControl** |
| `<action>` 缺失 / 非 Dict、`<type>` 未知、`setLevel` 缺 `<newLevel>` 或 `<newLevel>` 非法 | **拒整条 logControl** |
| 所有 match 维度均未启用（`HasAnyMatch()` 失败） | **拒整条 logControl** |

被拒的 logControl 不影响同一 `<logControls>` 里其它 logControl 的解析。

解析阶段**不做去重、不做跨 item 校验**：`link.cpp` 先 `SetLevel` 再按 `level==warn` `Mute` 这种链式构造由声明顺序保证。

---

## 4. 典型用法

### 4.1 静音某函数（只在 console）

用 `<func>` 精确命中 + `<appenders>` 收窄作用域：file appender 仍然照常记录。

```xml
<logControls>
  <logControl>
    <match><func>NoisyFunc</func></match>
    <appenders><appender>console</appender></appenders>
    <action><type>mute</type></action>
  </logControl>
</logControls>
```

### 4.2 链式：先提级再按级别静音

跨 logControl 交互：**声明顺序即应用顺序**，两条必须在同一份配置里。

```xml
<logControls>
  <logControl>
    <match><file name="link.cpp"/></match>
    <action><type>setLevel</type><newLevel>warn</newLevel></action>
  </logControl>
  <logControl>
    <match><level>warn</level></match>
    <action><type>mute</type></action>
  </logControl>
</logControls>
<!-- link.cpp 上的 info 记录: 先被第 1 条改写为 warn, 再被第 2 条 level==warn 的 mute 命中 -->
```

---

## 5. Reload

`LLBC_LoggerMgrSingleton->Reload(cfgPath)` 会重新解析配置并对每个 logger 调一次 `SetLogControls` **整体重建**列表（不增量、不合并）。

- 某个 logger 的新配置非法 → 该 logger 保留旧 logControls，`Reload()` 返回 `LLBC_FAILED`。

```cpp
if (LLBC_LoggerMgrSingleton->Reload() != LLBC_OK)
    fprintf(stderr, "reload failed: %s\n", LLBC_FormatLastError());
```
