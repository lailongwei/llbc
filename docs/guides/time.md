---
layout: default
title: 时间 Time / TimeSpan
---

# 时间 Time / TimeSpan

`LLBC_Time` 表示一个时间点（精度：微秒），`LLBC_TimeSpan` 表示一段时间差（精度：微秒）。
两者配合用于获取当前时间、格式化输出、日期运算、以及跨"小时/日/周/月"边界统计。
v1.1.1 对构造与字符串解析路径做了简化，新代码应使用工厂方法与 `FromTimeParts`，
不要手写旧的字符串解析逻辑。

---

## 获取当前时间

```cpp
// 获取当前时间对象（本地时区）
LLBC_Time now = LLBC_Time::Now();

// 仅需原始时间戳时，可跳过对象构造
time_t  secs   = LLBC_Time::NowTimestampInSecs();
sint64  millis = LLBC_Time::NowTimestampInMillis();
sint64  micros = LLBC_Time::NowTimestampInMicros();
```

---

## 构造时间点

工厂方法均标记 `[[nodiscard]]`，勿忽略返回值。

```cpp
// 从时间戳构造
LLBC_Time t1 = LLBC_Time::FromSeconds(1719820800);
LLBC_Time t2 = LLBC_Time::FromMillis(1719820800000LL);
LLBC_Time t3 = LLBC_Time::FromMicros(1719820800000000LL);

// 从日期时间字符串构造（默认分隔符："-" "/" ":" "."）
LLBC_Time t4 = LLBC_Time::FromTimeStr("2024-07-01 08:00:00.000000");

// 从各部分构造
LLBC_Time t5 = LLBC_Time::FromTimeParts(
    2024, 7, 1,   // year, month(1-based), day
    8, 0, 0,      // hour, minute, second
    123, 456      // millisecond, microsecond (optional)
);
```

<div class="callout warning" markdown="1">
`FromTimeParts` 的 `month` 参数为 **1 起算**（1=January），但 `GetMonth()` 返回值为 **0 起算**。
两者基准不同，混用时需加 1/减 1。
</div>

---

## 读取日期与时间分量

```cpp
LLBC_Time now = LLBC_Time::Now();

int year  = now.GetYear();         // 完整年份，如 2024
int month = now.GetMonth();        // 0 起算：0=一月, 11=十二月
int day   = now.GetDayOfMonth();   // 1 起算：1..31
int hour  = now.GetHour();         // [0, 23]
int min   = now.GetMinute();       // [0, 59]
int sec   = now.GetSecond();       // [0, 59]
int ms    = now.GetMillisecond();  // [0, 999]
int us    = now.GetMicrosecond();  // [0, 999]

int weekday = now.GetDayOfWeek();  // 默认 0=Sunday；传 false 则 0=Monday
int yday    = now.GetDayOfYear();  // 0 起算
```

---

## 格式化输出

`Format` / `FormatAsGmt` 接受 `strftime` 格式串；传 `nullptr` 使用默认格式
`YYYY-MM-DD HH:MM:SS`（不含微秒）。若需带微秒的字符串，用 `ToString()`（`Format() + ".%06d"`）。

```cpp
LLBC_Time now = LLBC_Time::Now();

LLBC_String local = now.Format();              // "2024-07-01 08:00:00"
LLBC_String utc   = now.FormatAsGmt();         // UTC 同格式
LLBC_String ym    = now.Format("%Y-%m");       // "2024-07"
LLBC_String full  = now.ToString();            // "2024-07-01 08:00:00.000123"（带微秒）

// 直接从时间戳格式化（静态版）
LLBC_String s = LLBC_Time::Format(time(nullptr), "%Y/%m/%d");
```

---

## GMT 与本地时间结构体

```cpp
LLBC_Time now = LLBC_Time::Now();

tm gmtStruct   = now.GetGmtTime();      // UTC struct tm（拷贝）
const tm &local = now.GetLocalTime();   // 本地 struct tm（引用，零拷贝）
```

---

## 日期加减运算

所有 `AddXXX` 方法均返回新的 `LLBC_Time` 对象，原对象不变，线程安全。

```cpp
LLBC_Time now = LLBC_Time::Now();

LLBC_Time nextYear    = now.AddYears(1);
LLBC_Time lastMonth   = now.AddMonths(-1);
LLBC_Time tomorrow    = now.AddDays(1);
LLBC_Time in3Hours    = now.AddHours(3);
LLBC_Time in30Mins    = now.AddMinutes(30);
LLBC_Time after500ms  = now.AddMillis(500);
```

---

## LLBC_TimeSpan：时间差

`LLBC_TimeSpan` 内部以微秒 `sint64` 存储，工厂方法全部是 `constexpr`，可用于编译期常量。

```cpp
// 工厂方法（首选）
constexpr LLBC_TimeSpan oneDay    = LLBC_TimeSpan::FromDays(1);
constexpr LLBC_TimeSpan twoHours  = LLBC_TimeSpan::FromHours(2);
constexpr LLBC_TimeSpan t30s      = LLBC_TimeSpan::FromSeconds(30);
constexpr LLBC_TimeSpan t500ms    = LLBC_TimeSpan::FromMillis(500);

// 便利常量
LLBC_TimeSpan zero    = LLBC_TimeSpan::zero;
LLBC_TimeSpan oneWeek = LLBC_TimeSpan::oneWeek;

// 从字符串构造，格式：[DD ][HH:]MM:SS[.microseconds]
LLBC_TimeSpan span = LLBC_TimeSpan::FromSpanStr("05 01:02:03.123456");
```

---

## 读取时间差分量

```cpp
constexpr LLBC_TimeSpan ts = LLBC_TimeSpan::FromDays(1, 2, 3, 4, 5, 6);
//                                                    d  h  m  s  ms us

// 各分量（不进位）
ts.GetDays();     // 1
ts.GetHours();    // 2
ts.GetMinutes();  // 3
ts.GetSeconds();  // 4
ts.GetMillis();   // 5
ts.GetMicros();   // 6

// 总量（全部折算）
ts.GetTotalHours();    // int
ts.GetTotalSeconds();  // int
ts.GetTotalMillis();   // sint64
ts.GetTotalMicros();   // sint64
```

---

## 时间差运算

```cpp
LLBC_Time now   = LLBC_Time::Now();
LLBC_Time later = now.AddHours(3);

LLBC_TimeSpan diff = later - now;  // operator-(LLBC_Time) → LLBC_TimeSpan

LLBC_TimeSpan a = LLBC_TimeSpan::FromHours(1);
LLBC_TimeSpan b = LLBC_TimeSpan::FromMinutes(30);

LLBC_TimeSpan sum    = a + b;    // 1.5 hours
LLBC_TimeSpan sub    = a - b;    // 0.5 hours
LLBC_TimeSpan scaled = a * 2.0;  // 2 hours
LLBC_TimeSpan half   = a / 2.0;  // 0.5 hours
LLBC_TimeSpan neg    = -a;       // -1 hour
```

---

## 日/周期边界工具

获取当前时间点在其所在小时/天/周/月内的起始时刻或已过偏移量：

```cpp
LLBC_Time now = LLBC_Time::Now();

LLBC_Time  beginOfDay    = now.GetBeginTimeOfDay();
LLBC_TimeSpan offsetOfDay = now.GetOffsetTimeOfDay();  // 今天已过时长

// 周，默认以周日为第一天；传 false 则以周一为第一天
LLBC_Time  beginOfWeek = now.GetBeginTimeOfWeek(false);
```

跨边界次数统计（常用于游戏跨天/跨周刷新判断）：

```cpp
LLBC_Time from = LLBC_Time::FromTimeStr("2024-01-01 00:00:00");
LLBC_Time to   = LLBC_Time::FromTimeStr("2024-01-04 03:00:00");

// 从 from 到 to 经过了多少个自然日边界（0 点为刷新点）
int days = LLBC_Time::GetCrossedDays(from, to, LLBC_TimeSpan::zero); // 3

// 以每天 08:00 为刷新点
int days8 = LLBC_Time::GetCrossedDays(
    from, to, LLBC_TimeSpan::FromHours(8));
```

<div class="callout note" markdown="1">
`GetCrossedXxx` 的第三个参数 `timeOfXxx` 指定该周期内的刷新点偏移量（相对周期起始）。
传 `LLBC_TimeSpan::zero` 表示以自然边界（0 点）为刷新点。负值会被自动折算为正方向偏移。
</div>

---

## 距下一刷新点的间隔

```cpp
LLBC_Time now = LLBC_Time::Now();

// 距今天下一个 08:00 还有多长时间
LLBC_TimeSpan toNext8 = now.GetIntervalToTimeOfDay(LLBC_TimeSpan::FromHours(8));

// 距本周下一个周五 08:00 还有多长时间（startOnSunday=true：0=Sun...5=Fri）
LLBC_TimeSpan toFri8 = now.GetIntervalToTimeOfWeek(
    LLBC_TimeSpan::FromDays(5, 8));
```

---

## 时间常量（LLBC_TimeConst）

```cpp
// 常用换算常量
int    sPerDay  = LLBC_TimeConst::numOfSecondsPerDay;   // 86400
sint64 usPerSec = LLBC_TimeConst::numOfMicrosPerSecond; // 1000000

// 星期名称字符串（从 Sunday 起）
const char *name  = LLBC_TimeConst::dayOfWeekDesc[now.GetDayOfWeek()];  // "Monday" 等
const char *brief = LLBC_TimeConst::GetDayOfWeekDesc(now.GetDayOfWeek(), true); // "Mon"
```

---

## 自定义分隔符

时间字符串默认格式为 `YYYY-MM-DD HH:MM:SS.microseconds`。
若来源数据使用不同分隔符，可通过 `LLBC_TimeSep` 指定：

```cpp
LLBC_TimeSep sep;
sep.YMDSep    = '/';
sep.HMSSep    = '.';
sep.microSecSep = '#';
// 解析 "2024/07/01 20.05.08#123456"
LLBC_Time t = LLBC_Time::FromTimeStr("2024/07/01 20.05.08#123456", sep);
```

---

## 参照

- 头文件：`llbc/include/llbc/core/time/Time.h`
- 头文件：`llbc/include/llbc/core/time/TimeSpan.h`
- 头文件：`llbc/include/llbc/core/time/TimeConst.h`
- 功能测试：`tests/func_test/core/time/FuncTest_Core_Time_Time.cpp`
- 快速上手示例（可跑）：`tests/example/core/Example_Core_Time.cpp`

## 下一步

- [序列化 Stream](stream.md) — `LLBC_Time` / `LLBC_TimeSpan` 均实现 `Serialize`/`Deserialize`，可直接用 `<<`/`>>` 写入 `LLBC_Stream`
- [对象池 ObjPool](objpool.md) — 高频创建时间对象时可配合对象池
