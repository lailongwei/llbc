---
layout: default
title: 版本迁移指南 v1.1.1
---

# 版本迁移指南 v1.1.1

v1.1.1 是 llbc 迄今最大的一次集中重构，涉及 Component 事件模型、对象池、Stream 游标、
时间 API、日志配置六个方面。本文按"变更 → 影响 → 改法"逐节说明，CHANGELOG 条目号标注在括号中以供对照。

---

## OnAppConfigReloaded → OnReload（CHANGELOG #26）

**变更**：App reload 语义标准化，`Component::OnAppConfigReloaded()` 已删除，统一用
`OnReload()` 接收配置重载通知。

**影响**：所有重写了 `OnAppConfigReloaded()` 的组件在 v1.1.1 编译时报"no member"错误。

**改法**：将方法名直接替换，无需修改参数或返回值。

```cpp
// v1.1.1 之前
class MyComp : public LLBC_Component {
    void OnAppConfigReloaded() override { /* ... */ }
};

// v1.1.1 之后
class MyComp : public LLBC_Component {
    void OnReload() override {
        // 新配置已写入 GetConfig()，直接读取即可
        const LLBC_Variant &cfg = GetConfig();
        LLBC_PrintLn("reloaded, cfg:\n%s", cfg.ToString().c_str());
    }
};
```

---

## Component 非流程事件收敛到 OnEvent（CHANGELOG #17）

**变更**：Session 建立/销毁、异步连接结果、协议报告等"非流程化"事件，不再各自对应独立的虚函数
（`OnSessionCreate`、`OnSessionDestroy` 等），统一通过 `OnEvent(int eventType, const LLBC_Variant &eventParams)`
分发。事件类型枚举位于 `LLBC_ComponentEventType`。

**影响**：原先重写独立事件方法的代码在 v1.1.1 不再被框架调用。

**改法**：在 `OnEvent` 中按 `eventType` 分支处理，参数通过 `eventParams.As<T *>()` 取指针。

```cpp
void OnEvent(int eventType, const LLBC_Variant &eventParams) override
{
    switch (eventType)
    {
        case LLBC_ComponentEventType::SessionCreate:
        {
            auto &info = *eventParams.As<LLBC_SessionInfo *>();
            LLBC_PrintLn("session %d created", info.GetSessionId());
            break;
        }
        case LLBC_ComponentEventType::SessionDestroy:
        {
            auto &info = *eventParams.As<LLBC_SessionDestroyInfo *>();
            LLBC_PrintLn("session %d destroyed: %s",
                         info.GetSessionId(), info.GetReason().c_str());
            break;
        }
        case LLBC_ComponentEventType::AsyncConnResult:
        {
            auto &r = *eventParams.As<LLBC_AsyncConnResult *>();
            LLBC_PrintLn("async conn result: %s", r.IsConnected() ? "ok" : r.GetReason().c_str());
            break;
        }
        default:
            break;
    }
}
```

<div class="callout note" markdown="1">
业务层自定义事件类型的枚举值应落在 `LLBC_ComponentEventType::LogicBegin`（值 1000）到
`LogicEnd`（值 10000000）之间，避免与框架内置类型冲突。
</div>

真实示例见 `tests/func_test/comm/FuncTest_Comm_SvcBase.cpp`（`OnEvent` 完整实现）。

---

## 旧对象池移除 → LLBC_ObjPool（CHANGELOG #19）

**变更**：框架中所有旧式对象池实现已删除，仅保留全新设计的
`core/objpool/ObjPool.h`（`LLBC_ObjPool`）和 `ThreadSpecObjPool.h`（`LLBC_ThreadSpecObjPool`）。

**影响**：引用旧接口（如 `LLBC_ObjectPool`、`GetPoolInst()`、`GiveBackToPool()` 等旧约定方法）的代码无法编译。

**改法**：使用 `LLBC_ObjPool`，通过 `Acquire<T>()`/`Release(obj)` 管理对象生命周期。
`LLBC_Service` 和 `LLBC_Service::GetThreadSafeObjPool()` 内置了线程安全池，可直接使用。

```cpp
// 独立对象池
LLBC_ObjPool objPool;

// 申请对象
auto *pkt = objPool.Acquire<LLBC_Packet>();

// 使用后归还（池子自动调用 clear/reuse）
objPool.Release(pkt);

// 支持派生类指针归还
BaseCls *obj = objPool.Acquire<DerivedCls>();
objPool.Release(obj);   // 框架内部按真实类型路由

// 查看池统计（JSON 或 CSV）
LLBC_PrintLn("%s", objPool.GetStatistics(LLBC_ObjPoolStatFormat::Json).c_str());
```

<div class="callout important" markdown="1">
`LLBC_ObjPool` 本身**不是线程安全**的；跨线程场景请使用
`LLBC_Service::GetThreadSafeObjPool()` 返回的内置线程安全池，或为每个线程构建独立的
`LLBC_ObjPool`（即 ThreadSpec 模式）。
</div>

真实示例见 `tests/func_test/core/objpool/FuncTest_Core_ObjPool.cpp`。

---

## Stream 读写游标分离（CHANGELOG #1）

**变更**：`LLBC_Stream` 由单一位置游标重构为**读写游标独立**。写入推进写游标，读出推进读游标，
两者互不干扰。

**影响**：旧代码中"写完再回到 pos 0 读"的模式必须改用 `SetReadPos(0)`，否则读游标不会回头。

**改法**：写入完成后显式调用 `SetReadPos(0)` 再读；若需要覆盖写则调用 `SetWritePos(0)`。

```cpp
LLBC_Stream stream;
stream << 42 << LLBC_String("hello");

// 必须重置读游标后才能读出
stream.SetReadPos(0);

int n;
LLBC_String s;
stream >> n >> s;   // n==42, s=="hello"
```

<div class="callout warning" markdown="1">
v1.1.1 之前对 `LLBC_Stream` 有"位置单一"假设的代码（例如依赖写完后 pos 自动归零）在新版本中将读取到错误数据或触发 assert。迁移时全局搜索 `SetPos`/`GetPos` 旧接口并替换为 `SetReadPos`/`GetReadPos`/`SetWritePos`/`GetWritePos`。
</div>

---

## Time / TimeSpan 接口简化（CHANGELOG #16）

**变更**：`LLBC_Time` 与 `LLBC_TimeSpan` 全面优化接口，特别是字符串解析路径。
新增了一批 `constexpr` 工厂方法（`FromDays`/`FromHours`/…），旧的繁琐构造方式不再推荐。

**改法**：优先使用工厂方法构造，避免手动 format 解析。

```cpp
// LLBC_TimeSpan 工厂（constexpr，编译期计算）
constexpr auto t1 = LLBC_TimeSpan::FromDays(1, 2, 3, 4, 5, 6);   // 1d 2h 3m 4s 5ms 6us
constexpr auto t2 = LLBC_TimeSpan::FromSeconds(30);
constexpr auto t3 = LLBC_TimeSpan::FromMillis(500);

// LLBC_Time 工厂
LLBC_Time now    = LLBC_Time::Now();
LLBC_Time t4     = LLBC_Time::FromSeconds(time_t(0));    // UTC epoch
LLBC_Time t5     = LLBC_Time::FromMillis(1700000000000LL);
LLBC_Time t6     = LLBC_Time::FromTimeStr("2024-01-01 00:00:00.000000");
LLBC_Time t7     = LLBC_Time::FromTimeParts(2024, 1, 1, 0, 0, 0);

// 获取时间戳
sint64 ms        = LLBC_Time::NowTimestampInMillis();
sint64 us        = LLBC_Time::NowTimestampInMicros();
```

<div class="callout note" markdown="1">
`LLBC_TimeSpan` 构造函数的单参数形式接受**微秒**（`sint64`）；如果你原来传的是毫秒，
需要乘以 1000 或改用 `FromMillis()`。
</div>

真实示例见 `tests/func_test/core/time/FuncTest_Core_Time_Time.cpp`。

---

## 日志级别调整 / logLevel 废弃（CHANGELOG #15）

**变更**有两条：

1. **Debug / Trace 对调**：v1.1.1 起 `Trace` 是最低级别（最详细），`Debug` 在 `Trace` 之上。
   级别顺序：`Trace < Debug < Info < Warn < Error < Fatal`。
2. **logLevel 配置项废弃**：旧的 `logLevel` 被移除，需明确使用 `consoleLogLevel`（控制台输出级别）
   和 `fileLogLevel`（文件输出级别）来分别控制各 appender 的日志级别。

**影响**：

- 若代码或配置中依赖 `logLevel` 设置，升级后该字段被忽略，日志级别将回退到继承自 root 的默认值。
- 若原本期望 `Debug` 是最低级别（捕获所有输出），升级后最低级别是 `Trace`，`Debug` 级别的日志
  不受影响，但需确认 `Trace` 级日志不会意外出现。

**改法**：将配置文件中的 `logLevel` 替换为对应 appender 的明确配置。

```ini
# v1.1.1 之前（已废弃）
root.logLevel=DEBUG

# v1.1.1 之后——分别控制控制台与文件两个 appender
root.consoleLogLevel=TRACE
root.fileLogLevel=INFO

# 某个命名 logger 只开文件 WARN 以上
myservice.fileLogLevel=WARN
myservice.consoleLogLevel=DEBUG
```

<div class="callout warning" markdown="1">
`Trace` 级别在高频路径下输出量极大，生产环境建议将 `consoleLogLevel` / `fileLogLevel` 设为
`Info` 或更高，否则可能因日志 IO 成为性能瓶颈。
</div>

真实配置示例见 `tests/func_test/core/log/LogTestCfg.cfg`。

---

## 参照

### 头文件

| 变更点 | 头文件 |
|--------|--------|
| Component / OnEvent / OnReload | `llbc/include/llbc/comm/Component.h` |
| LLBC_ObjPool | `llbc/include/llbc/core/objpool/ObjPool.h` |
| LLBC_ThreadSpecObjPool | `llbc/include/llbc/core/objpool/ThreadSpecObjPool.h` |
| LLBC_Stream（读写游标） | `llbc/include/llbc/common/Stream.h` |
| LLBC_Time / LLBC_TimeSpan | `llbc/include/llbc/core/time/Time.h`、`TimeSpan.h` |
| LLBC_LogLevel / 日志配置 | `llbc/include/llbc/core/log/LogLevel.h`、`LoggerConfigInfo.h` |

### 真实测试示例

- `tests/func_test/comm/FuncTest_Comm_SvcBase.cpp` — `OnEvent` 完整示例
- `tests/func_test/app/FuncTest_App_AppCfgTest.cpp` — `OnReload` 示例
- `tests/func_test/core/objpool/FuncTest_Core_ObjPool.cpp` — `LLBC_ObjPool` 完整示例
- `tests/func_test/core/time/FuncTest_Core_Time_Time.cpp` — `LLBC_TimeSpan` 工厂方法示例
- `tests/func_test/core/log/LogTestCfg.cfg` — 日志配置示例（consoleLogLevel / fileLogLevel）

---

## 下一步

- [序列化 Stream](guides/stream.md) — Stream 游标分离的完整 API 说明
- CHANGELOG — 查看 v1.1.1 各条目完整说明
