---
layout: default
title: 日志配置项参考
---

# 日志配置项参考

llbc 日志系统通过 `.cfg` 属性文件（Properties 格式）驱动，每个配置项以 `<loggerName>.<key>=<value>` 的形式出现。根日志器名固定为 `root`，其它日志器可自由命名。所有非根日志器若某项未配置，默认行为由 `notConfigUse` 控制：取 `root` 则继承根日志器同名配置，取 `default` 则使用框架内置默认值。

<div class="callout important" markdown="1">
`logLevel` 已废弃，**不要使用**。请分别用 `consoleLogLevel` 和 `fileLogLevel` 控制各输出器的日志级别。有效的级别字符串（大小写不敏感）：`TRACE`、`DEBUG`、`INFO`、`WARN`、`ERROR`、`FATAL`。`TRACE` 是最低级别。
</div>

## 通用配置项

| 配置键 | 含义 | 默认值 |
|--------|------|--------|
| `notConfigUse` | 未配置项的默认来源：`root`（继承根日志器）或 `default`（框架内置值） | `root` |
| `asynchronous` | 异步输出开关（`true`/`false`） | `false` |
| `independentThread` | 异步模式下是否为该日志器分配独立输出线程（仅 `asynchronous=true` 时有效） | `false` |
| `flushInterval` | 异步模式下日志刷新间隔（毫秒），上限 5000 ms | `200` |
| `addTimestampInJsonLog` | JSON 格式输出时是否添加 `timestamp` 字段 | `false` |
| `takeOver` | 是否接管发往未配置日志器的消息（仅 root 日志器有效） | `true` |

## 控制台输出器配置项

| 配置键 | 含义 | 默认值 |
|--------|------|--------|
| `logToConsole` | 是否输出到控制台 | `true` |
| `consoleLogLevel` | 控制台输出日志级别 | `TRACE`（即级别 0） |
| `consolePattern` | 控制台输出格式，见下方格式符说明 | `%T [%N][%L] - %m%n` |
| `colourfulOutput` | Windows 平台彩色输出开关 | `true` |

## 文件输出器配置项

| 配置键 | 含义 | 默认值 |
|--------|------|--------|
| `logToFile` | 是否输出到文件 | `false` |
| `fileLogLevel` | 文件输出日志级别 | `TRACE`（即级别 0） |
| `filePattern` | 文件输出格式 | `%T %f:%l@[%N][%L] - %m%n` |
| `logDir` | 日志目录；为空时使用当前目录或应用目录（取决于 `forceAppLogPath`） | `""` |
| `logFile` | 日志文件名，支持 `%p`（进程ID）、`%e`（可执行文件名）、`%l`（logger名）、`%L`（智能logger名）等占位符 | `%L` |
| `logFileSuffix` | 日志文件后缀 | `.log` |
| `forceAppLogPath` | 相对路径时以可执行文件所在目录为基准 | `false` |
| `logCodeFilePath` | `%f` 格式符是否输出完整路径（`false` 时仅文件名） | `false` |
| `fileRollingMode` | 文件滚动方式：`NoRolling`/`No`、`HourlyRolling`/`Hourly`、`DailyRolling`/`Daily` | `NoRolling` |
| `maxFileSize` | 单文件最大大小，取值范围 `[1MiB, 10GiB]`，支持 K/KB/KiB/M/MB/MiB/G/GB/GiB 单位 | `1GiB` |
| `maxBackupIndex` | 最大备份文件索引数，0 表示不限制 | `10` |
| `fileBufferSize` | 异步模式下文件写缓冲大小（字节） | `40960`（40 KiB） |
| `lazyCreateLogFile` | 延迟创建日志文件（初始化时不立即创建，适合大量日志器场景） | `false` |

## Page Cache 回收配置项（Linux fadvise，仅文件输出器）

| 配置键 | 含义 | 默认值 |
|--------|------|--------|
| `enableFadviseDiscard` | 是否建议操作系统回收文件 page cache | `false` |
| `fadviseDiscardSize` | 单次建议回收大小，范围 `[1MiB, maxFileSize]` | `100MiB` |
| `fadviseDiscardRetainPercent` | 回收时保留比例（%），范围 `[5, 95]`；实际回收 = `fadviseDiscardSize × (1 - retainPercent/100)` | `50` |

## 日志染色配置项

| 配置键 | 含义 | 默认值 |
|--------|------|--------|
| `requireColorLogTraces` | 染色 trace 列表，格式 `key1:val1,val2\|key2:val1,val2`，key 限 16 字符，每值限 32 字符 | `""` |

## 格式符速查

`consolePattern` / `filePattern` 支持以下格式符（`%[对齐/占位]<字符>`）：

| 格式符 | 说明 |
|--------|------|
| `%T` | 本地化时间（经 time offset 偏移后） |
| `%R` | 进程真实时间 |
| `%L` / `%L{Short}` | 日志级别全称 / 短名（如 `INFO` / `I`） |
| `%m` | 日志消息正文 |
| `%f` | 代码文件名（受 `logCodeFilePath` 影响） |
| `%F` | 函数名 |
| `%l` | 代码行号 |
| `%t` | 线程 ID |
| `%p` | 进程 ID |
| `%N` | Logger 名 |
| `%g` | 消息 tag |
| `%e{PreserveLink}` | 可执行文件名（去扩展名），加 `{PreserveLink}` 保留软链接名 |
| `%E{VAR}` | 环境变量 `VAR` 的值 |
| `%x` | 日志追踪内容（trace） |
| `%n` | 换行符 |
| `%%` | 字面量 `%` |

## 最小配置示例

只记录 INFO 及以上，同时输出控制台与文件，按天滚动：

```cfg
# 根日志器
root.asynchronous=true
root.flushInterval=200
root.takeOver=true

# 控制台
root.logToConsole=true
root.consoleLogLevel=INFO
root.consolePattern=%T [%L] - %m%n

# 文件
root.logToFile=true
root.fileLogLevel=INFO
root.logFile=log/%e
root.logFileSuffix=.log
root.fileRollingMode=Daily
root.maxFileSize=512MiB
root.maxBackupIndex=7
```

<div class="callout note" markdown="1">
文件输出器仅在 `logToFile=true` 时读取文件相关配置项（`logFile`、`fileRollingMode` 等）。控制台输出器同理。两者独立，可单独开关。
</div>

<div class="callout warning" markdown="1">
`independentThread=true` 会为该日志器分配专属输出线程，适合高写入量的日志器。大量日志器都设为 `true` 会造成线程数暴涨，谨慎使用。
</div>

## 参照

- 头文件：`llbc/include/llbc/core/log/LoggerConfigInfo.h`
- 内联实现：`llbc/include/llbc/core/log/LoggerConfigInfoInl.h`
- 实现源码：`llbc/src/core/log/LoggerConfigInfo.cpp`
- 框架默认常量：`llbc/include/llbc/common/Config.h`（`LLBC_CFG_LOG_*` 系列宏）
- 完整配置样例：`tests/func_test/core/log/LogTestCfg.cfg`

## 下一步

- [日志系统概览](../guides/logger.md)
- [快速入门](../getting-started/)
