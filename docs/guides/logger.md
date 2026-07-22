---
layout: default
title: 日志 Logger
---

# 日志 Logger

`LLBC_Logger` / `LLBC_LoggerMgr` 是 llbc 的日志子系统，支持多命名 logger、控制台与文件双输出、
printf 风格格式化、JSON 结构化日志、以及异步滚动写入。v1.1.1 起日志级别顺序调整为
**Trace 为最低级别**（Trace < Debug < Info < Warn < Error < Fatal），
`logLevel` 配置项已废弃，应使用 `consoleLogLevel` / `fileLogLevel` 分别控制各 appender 的输出阈值。

## 初始化与销毁

调用 `LLBC_LoggerMgrSingleton->Initialize` 传入配置文件路径；进程退出前配对调用 `Finalize`。
配置文件支持 `.cfg`（properties）和 `.xml` 两种格式。

```cpp
// 初始化（支持可选时间偏移，常用于跨时区日志对齐）
if (LLBC_LoggerMgrSingleton->Initialize("LogTestCfg.cfg") != LLBC_OK)
{
    LLBC_FilePrintLn(stderr, "logger init failed: %s", LLBC_FormatLastError());
    return -1;
}

// RAII 风格销毁（可配合 LLBC_Defer）
LLBC_Defer(LLBC_LoggerMgrSingleton->Finalize());
```

`Initialize` 也接受第二个可选参数 `LLBC_TimeSpan logTimeOffset`，用于让日志时间戳整体偏移（例如
展示特定时区时间），不影响系统实际时间。

## 日志级别

`LLBC_LogLevel` 枚举从低到高：

| 枚举值 | 含义 |
|--------|------|
| `Trace` | 最低级别，追踪/详细路径 |
| `Debug` | 调试信息 |
| `Info`  | 普通运行信息 |
| `Warn`  | 告警，业务可继续 |
| `Error` | 错误，局部功能受损 |
| `Fatal` | 致命错误，进程应退出 |

<div class="callout warning" markdown="1">
**级别陷阱**：v1.1.1 前 Debug 是最低级别；v1.1.1 起 **Trace 最低**。
如果你的配置里只写了 `consoleLogLevel=DEBUG`，Trace 级别的日志将不会出现在控制台。
</div>

## printf 风格宏（LLOG 系列）

最常用的一组宏，每个级别提供 4 个变体，后缀数字越大参数越多：

| 后缀 | 参数 | 说明 |
|------|------|------|
| 无   | `(fmt, ...)` | 输出到 root logger，无 tag |
| `2`  | `(loggerName, fmt, ...)` | 指定命名 logger，无 tag |
| `3`  | `(tag, fmt, ...)` | 输出到 root logger，附 tag |
| `4`  | `(loggerName, tag, fmt, ...)` | 指定 logger 并附 tag |

```cpp
// root logger
LLOG_TRACE("enter func, param=%d", param);
LLOG_INFO("server started on port %d", port);
LLOG_WARN("slow query: %lld ms", elapsed);
LLOG_ERROR("connect failed: %s", LLBC_FormatLastError());

// 指定命名 logger（对应配置文件中 "net" 段）
LLOG_DEBUG2("net", "recv pkt, len=%zu", pktLen);
LLOG_INFO4("net", "session", "new session, id=%d", sessionId);
```

底层通用宏 `LLOG(loggerName, tag, level, fmt, ...)` 可在运行时传入动态 level，
适合在循环中遍历所有级别：

```cpp
for (int lv = LLBC_LogLevel::Begin; lv != LLBC_LogLevel::End; ++lv)
    LLOG(nullptr, nullptr, lv, "level=%s", LLBC_LogLevel::GetLevelStr(lv).c_str());
```

## JSON 结构化日志（LJLOG 系列）

`LJLOG_<LEVEL>` 返回一个构建器对象，链式调用 `Add` 添加键值对，最后调用 `Finish` 提交：

```cpp
// 结构化记录一次请求
LJLOG_INFO()
    .Add("uid",    playerId)
    .Add("action", "login")
    .Add("ip",     remoteIp)
    .Finish("player login ok");

// 输出到指定 logger 并附 tag
LJLOG_WARN4("billing", "pay")
    .Add("orderId", orderId)
    .Add("amount",  amount)
    .Finish("payment %s", result);
```

后缀规则与 `LLOG` 系列相同（无后缀→root/无tag，`2`→named logger，`3`→tag，`4`→named+tag）。

## 条件日志与控制流宏

这组宏在条件为真时同时记录日志并执行控制流操作，避免每次手写 if + log + return：

```cpp
// 条件为真时记录 Error 并 return LLBC_FAILED
LLBC_ErrorAndReturnIf(ptr == nullptr, LLBC_FAILED, "ptr is null in %s", __FUNCTION__);

// 条件为真时记录 Warn 并 break
LLBC_WarnAndBreakIf(retries > 3, "too many retries: %d", retries);

// 条件为真时记录 Error 并 continue
LLBC_ErrorAndContinueIf(item == nullptr, "skip null item at idx=%d", i);

// 条件为真时执行任意操作
LLBC_LogAndDoIf(err != 0, Error, cleanup(), "cleanup on err=%d", err);
```

每个宏均提供 2/3/4 后缀变体，支持指定 logger 名称或 tag。
通用形式为 `LLBC_LogAndReturnIf(cond, logLv, ret, ...)`，其中 `logLv` 直接写级别名
（`Trace`/`Debug`/`Info`/`Warn`/`Error`/`Fatal`）。

## 配置文件：控制台与文件 appender

配置文件以 `<loggerName>.<key>=<value>` 格式组织，`root` 是根 logger。

```properties
# root logger 示例（摘自 tests/func_test/core/log/LogTestCfg.cfg）
root.asynchronous=true
root.flushInterval=500

# 控制台 appender — 使用 consoleLogLevel（勿用废弃的 logLevel）
root.logToConsole=true
root.consoleLogLevel=TRACE
root.consolePattern=%T [tag:%g][%-5L][%f:%l,%F] - %m%n
root.colourfulOutput=true

# 文件 appender — 使用 fileLogLevel（勿用废弃的 logLevel）
root.logToFile=true
root.fileLogLevel=TRACE
root.logDir=.
root.logFile=log/llbc_%p_%e_%L
root.logFileSuffix=.txt
root.filePattern=%T [%-5L][%f:%l]<%x> - %m%n
```

<div class="callout important" markdown="1">
**废弃项**：`logLevel` 配置键已废弃，不要使用。
控制台输出阈值使用 `consoleLogLevel`，文件输出阈值使用 `fileLogLevel`，两者可独立配置。
</div>

常用 pattern 占位符：

| 占位符 | 含义 |
|--------|------|
| `%T`  | 本地时间戳 |
| `%L`  | 级别（如 `DEBUG`）；`%L{Short}` 输出 `D` |
| `%m`  | 日志消息正文 |
| `%f`  | 代码文件名 |
| `%l`  | 代码行号 |
| `%F`  | 函数名 |
| `%g`  | tag |
| `%N`  | logger 名 |
| `%t`  | 线程 ID |
| `%x`  | log trace 附加内容 |
| `%n`  | 换行 |

## 异步模式与文件滚动

异步模式下日志写入由后台线程完成，通常可大幅提升调用方吞吐量：

```properties
root.asynchronous=true
# 高负载 logger 可申请独立线程，避免和其他 logger 竞争
perftest.independentThread=true
root.flushInterval=200         # 异步刷盘间隔，毫秒

# 文件滚动模式：NoRolling | Hourly | Daily
root.fileRollingMode=Daily
root.maxFileSize=10.5MiB
root.maxBackupIndex=20
```

<div class="callout note" markdown="1">
**线程安全**：`LLOG_*` 宏在同步和异步模式下均线程安全，可在任意线程直接调用。
异步模式下，消息先入队，由后台线程批量落盘；`flushInterval` 控制最大延迟。
</div>

## 按名获取 Logger

通过单例直接获取指定 logger 的指针，随后可调用其成员方法：

```cpp
// 获取 root logger
LLBC_Logger *root = LLBC_LoggerMgrSingleton->GetRootLogger();

// 获取命名 logger（名称对应配置文件中的段名）
LLBC_Logger *net = LLBC_LoggerMgrSingleton->GetLogger("net");
if (net == nullptr)
{
    LLOG_WARN("logger 'net' not found");
}

// 运行时动态调整某个 appender 的日志级别
net->SetAppenderLogLevel(LLBC_LogAppenderType::Console, LLBC_LogLevel::Warn);
```

## 日志 Hook

可为 logger 的指定级别注册回调，在日志消息产生时同步触发（适合告警上报、日志监控等）：

```cpp
LLBC_Logger *root = LLBC_LoggerMgrSingleton->GetRootLogger();

// 注册 hook：同时监听 Error 和 Fatal
root->SetLogHook(
    { LLBC_LogLevel::Error, LLBC_LogLevel::Fatal },
    [](const LLBC_LogData *data) {
        AlertSystem::Report(data->logger->GetLoggerName(), data->msg);
    });

// 清除 hook（传 nullptr）
root->SetLogHook({ LLBC_LogLevel::Error, LLBC_LogLevel::Fatal }, nullptr);
```

## 配置热重载

运行时可不重启进程地重新加载日志配置（仅已有 logger 的参数变更生效，不支持新增 logger）：

```cpp
// 使用原始配置文件重载
LLBC_ErrorAndReturnIf(
    LLBC_LoggerMgrSingleton->Reload() != LLBC_OK,
    LLBC_FAILED,
    "reload failed: %s", LLBC_FormatLastError());

// 也可以切换到新配置文件
LLBC_LoggerMgrSingleton->Reload("NewLogConfig.cfg");
```

## 参照

头文件：
- `llbc/include/llbc/core/log/LogLevel.h` — 级别枚举与字符串互转
- `llbc/include/llbc/core/log/Logger.h` / `LoggerInl.h` — `LLBC_Logger` 类接口
- `llbc/include/llbc/core/log/LoggerMgr.h` / `LoggerMgrInl.h` — 单例、宏定义、条件宏
- `llbc/include/llbc/core/log/LoggerConfigInfo.h` — 配置信息（内部类）

真实用例：
- `tests/func_test/core/log/FuncTest_Core_Log.cpp` — 全功能演示（初始化、宏、JSON、条件宏、hook、trace、重载）
- `tests/func_test/core/log/LogTestCfg.cfg` — 完整配置文件示例

## 下一步

- [Variant](variant.md) — 日志消息中经常用到的动态类型，可直接在格式化参数中使用
- [Stream](stream.md) — 若启用 `LLBC_CFG_LOG_USING_WITH_STREAM`，`LSLOG_*` 宏可用 `<<` 拼接日志
