# 时间&时间跨度 - LLBC_Time & LLBC_TimeSpan
服务器功能开发中，不可避免的跟时间打交道，借鉴c#的一些api设计, llbc提供了两个核心时间相关类，让开发者摆脱时间相关复杂问题的困扰：
* `LLBC_Time`: 时间对象，描述一个绝对时间
* `LLBC_TimeSpan`: 描述一个时间跨度对象, 如: 当前时间离下一次重置时间间隔是多少?

## LLBC_Time
LLBC_Time为世间时间的一个表示, 通过LLBC_Time对象, 可以对世界时间进行各种访问及复杂的计算.
Note: `LLBC_Time`为`immutable`对象, 一旦创建, 不可修改，这也意味着，你不需要对你的变量加`const`修饰。

### 获取当前时间、时间戳
如果需要获取当前时间戳而不是时间对象(有些需求需要满足性能需求, 毕竟创建`LLBC_Time`有额外性能开销), 有以下方式获取:
* 获取秒为单位的`timestamp`:
    - [建议]`time_t LLBC_Time::NowTimestampInSecs()`: 通过`LLBC_Time`的静态方法获得
    - [不建议]`time_t time(nullptr)`: 使用操作系统api`time()`获得
* 获取毫秒为单位的`timestamp`:
    - [建议]`sint64 LLBC_Time::NowTimestampInMillis()`: 通过`LLBC_Time`的静态方法获得
    - [不建议]`sint64 LLBC_GetMilliSeconds()`: 通过llbc的os抽象层函数直接获得, 不建议, 未来可能不再对外暴露
    - [更不建议]`gettimeofday()`: 通过操作系统api获得, 不建议, 需要进行复杂的换算才能得到milli-seconds
* 获取微妙为单位的`timestamp`:
    - [建议]`sint64 LLBC_Time::NowTimestampInMicros()`: 通过`LLBC_Time`的静态方法获得
    - [不建议]`sint64 LLBC_GetMicroSeconds()`: 通过llbc的os抽象层函数直接获得, 不建议, 未来可能不再对外暴露
    - [更不建议]`gettimeofday()`: 通过操作系统api获得, 不建议, 需要进行复杂的换算才能得到micro-seconds

在大部分情况下，并不需要极致的性能，为确保可读性及代码简洁，建议：**大部分情况下，都使用`LLBC_Time`对象来操作时间！**, 以下是用`LLBC_Time`获取当前时间及各单位粒度时间戳的示例代码:
```c++
// 获取当前时间
LLBC_Time now = LLBC_Time::Now();

// 得到各粒度的时间戳:
std::cout <<"now: " <<now <<std::endl;
std::cout <<"now timestamp(in seconds): " <<now.GetTimestampInSecs() <<std::endl;
std::cout <<"now timestamp(in milli-seconds): " <<now.GetTimestampInMillis() <<std::endl;
std::cout <<"now timestamp(in micro-seconds): " <<now.GetTimestampInMicros() <<std::endl;
```

### 以各种方式构建你的时间对象
`LLBC_Time`除了通过`Now()`构建一个当前时间对象外，还支持其它各种格式的时间对象构建，这里构建方法均为`static`：
* 从各不同精度的`timestamp`构建`LLBC_Time`:
    - `static LLBC_Time LLBC_Time::FromSeconds(time_t)`: 通过seconds精度的`timestamp`构建`LLBC_Time`
    - `static LLBC_Time LLBC_Time::FromMillis(sint64)`: 通过milli-seconds精度的`timestamp`构建`LLBC_Time`
    - `static LLBC_Time LLBC_Time::FromMicros(sint64)`: 通过micro-seconds精度的`timestamp`构建`LLBC_Time`
* 从`struct timespec`构建`LLBC_Time`:
    - `static LLBC_Time LLBC_Time::FromTimeSpec(const timespec &)`
    - *Tips: `struct timespec`为second + nano_second组成的时间戳数据结构*
* 从`struct timeval`构建`LLBC_Time`:
    - `static LLBC_Time LLBC_Time::FromTimeVal(const timeval &)`
    - *Tips: `struct timeval`为second + micro_second缓存的时间戳数据结构*
* 从时间字符串构建`LLBC_Time`(如*2024-12-10 10:00:00*):
    - `static LLBC_Time LLBC_Time::FromTimeStr(timeStr)`
    - *Tips: 时间字符串解析性能在llbc上作了极致优化, 可放心使用, 包括参数适配及解析实现*
* 从`struct tm`构建`LLBC_Time`:
    - `static LLBC_Time LLBC_Time::FromTimeStruct(const tm &)`
* 从时间各部分分量构建`LLBC_Time`:
    - `static LLBC_Time LLBC_Time::FromTimeParts(int year, int month, int day, ...)`: 你可以通过各部分分量直接构建时间，会比通过`struct tm`结构构建来得简单

通过以上近乎所有方式都支持的一系列构建函数`FromXXX()`，你可以很轻松构建出一个时间对象，而不必再每次遇到时间对象构建、转换时，都要查阅一堆文档及思考，特别在编写跨平台代码时。

### 取得时间各部分分量
通过以下接口，可以得到时间各部分的分量:
- `int LLBC_Time::GetYear() const`: 得到年
- `int LLBC_Time::GetMonth() const`: 得到月, 取值范围:[1, 12], 即: 1->1月分、2->2月份、...、12->12月份
- `int LLBC_Time::GetDay() const`: 得到月内日期, 取值范围:[1, 31], 即: 1->1号、2->2号、...、31->31号
- `int LLBC_Time::GetDayOfWeek() const`: 取得星期几, 取值范围:[0, 6], 映射：0->周日、1->周一、...、6->周六
- `int LLBC_Time::GetDayOfMonth() const`: 取得月内第几天, 取值范围:[1, 31], **注意: 唯一一个以1起始的接口**
- `int LLBC_Time::GetDayOfYear() const`: 取得年内第几天, 取值范围:[0, 365]
- `int LLBC_Time::GetHour() const`: 获取天内的小时, 取值范围:[0, 23]
- `int LLBC_Time::GetMinute() const`: 获取天内的分钟, 取值范围:[0, 59]
- `int LLBC_Time::GetSecond() const`: 获取天内的秒数, 取值范围:[0, 59]
- `int LLBC_Time::GetMillisecond() const`: 获取秒内的毫秒数, 取值范围:[0, 999]
- `int LLBC_Time::GetMicrosecond() const`: 获取毫秒内的微秒数, 取值范围:[0, 999]

### GetDate & 周期内时间偏移
项目开发时，经常需要获得当天零点的时间, 在`LLBC_Time`中，通过`LLBC_Time LLBC_Time::GetDate() const`即可获得:
```c++
// 获得当前时间
LLBC_Time now = LLBC_Time::Now();
// 获取当天零点时间
LLBC_Time nowDate = now.GetDate(); // 如当前时间为 2024-12-11 21:51:08, GetDate()将返回2024-12-11 00:00:00的时间
```

除`GetDate()`需求外, 还会经常遇到: **获取天内时间偏移、小时内时间偏移、本周时间偏移**这类周期内时间偏移的需求, 这些需求`LLBC_Time`都提供了支持:
- `LLBC_TimeSpan LLBC_Time::GetTimeOfHour() const` - 获得小时内时间偏移，即：得到时间的分钟及后面的值
- `LLBC_TimeSpan LLBC_Time::GetTimeOfDay() const` - 获得天内时间偏移，即：得到时间的小时及后面的值（大白话：今天几时几分几秒）
- `LLBC_TimeSpan LLBC_Time::GetTimeOfWeek() const` - 获得周内时间偏移
- `LLBC_TimeSpan LLBC_Time::GetTimeOfMonth() const` - 获得月内时间偏移

### 格式化LLBC_Time
`LLBC_Time`提供了`Format()`函数来格式化时间, 参数格式同`[strftime()](https://man7.org/linux/man-pages/man3/strftime.3.html)`, 如想得到`YYYY/MM/DD`字符串，只需要`now.Format("%Y/%m/%d")`即可

### 时间偏移操作
项目开发时，往往会有`3.5小时后的时间是多少`这类需求，在llbc中，非常容易做到，以下为三种实现方式：
```c++
LLBC_Time now = LLBC_Time::Now();
// 通过为now加一个span值, 可直接得到3.5小时后的时间对象
LLBC_Time laterTime = now + LLBC_TimeSpan::FromHours(3, 30);
// 这也是一种写法，开发者可根据自己的喜好选择
// LLBC_Time laterTime = now + LLBC_TimeSpan::oneHour * 3 + LLBC_TimeSpan::oneMin * 30;
// 这也是一种写法, 通过LLBC_Time的原始AddXXX()方法而不是通过LLBC_TimeSpan进行, 不建议, 因为不够直观
// LLBC_Time laterTime = now.AddHours(1).AddMinutes(30);
```

通过为`LLBC_Time`对象加减一个`LLBC_TimeSpan`对象即可得到你要的时间

### 高阶: 获取离最近的天内时刻的偏移
项目开发中，Timer被高频使用，其中一个需求就是业务需要启动周期性的定时器，如每天8:00:00的定时器，开发者需要算第一次定时器超时值，因为第一次大概率不是24小时，这个时候就产生了一个需求：**我离最近一天8:00:00有多远？**, 这在llbc框架中是很容易实现的, 示例代码:
```c++
// 得到当前时间
LLBC_Time now = LLBC_Time::Now();

// 通过GetIntervalToTimeOfDay()得到离最近一天8:00:00的时间偏移
LLBC_TimeSpan span = now.GetTintervalToTimeOfDay(LLBC_TimeSpan::oneHour * 8);

// 启动定时器, so easy!
// - 参数1 - firstPeriod: 第一次超时时间
// - 参数2 - period:      第二次及之后的超时时间
timer.Schedule(span, LLBC_TimeSpan::oneDay);
```

泛化一下:
- `LLBC_Time`同样提供`离最近的小时内时刻的偏移`, 用于完成*启动Hourly定时器*需求的工作
- `LLBC_Time`同样提供`离最近的周内时刻的偏移`, 用于完成*启动Weekly定时器*需求的工作

### 高阶: 时间跨越判断
项目开发中，同样有一个高频需求: 一些玩法系统是按天重置, 在玩家登录时，需要根据当前时间、上一次重置时间来判断，是否跨越了重置点，如果跨越了，强制重置，否则不重置。
这种**是否跨越了重置点**的判断其实非常复杂，`LLBC_Time`直接提供了简单接口判断，以下是示例代码:
```c++
// 得到: 当前时间, 上一次重置时间
LLBC_Time now = LLBC_Time::Now();
LLBC_Time lastResetTime = LLBC_Time::FromMicros(data.lastResetTime);

// 判断是否过了重置点(是否跨天), 假定重置点是8:00:00
if (LLBC_Time::IsCrossedDay(lastRestTime, now, LLBC_TimeSpan::oneHour * 8))
{
    // 执行重置
    // ... ...

    // 更新最后重置时间
    data.lastResetTime = now.GetTimestampInMicros();
}

// 启动重置定时器
resetTimer.Schedule(now.GetIntervalToTimeOfDay(LLBC_TimeSpan::oneHour * 8), LLBC_TimeSpan::oneDay);
```

## LLBC_TimeSpan
`LLBC_TimeSpan`表示了一个时间跨度，用于:
* 解决复杂且容易出错的各种不同精度的时间偏移计算、换算
* 对`LLBC_Time`进行时间偏移时使用

通过`LLBC_TimeSpan::FromXXX()`来构造Span对象，也可使用`LLBC_TimeSpan`内建的各精度单位常量来构建Span对象

示例：
```c++
// 得到一天后的时间
LLBC_Time oneDayLater = LLBC_Time::Now() + LLBC_Time::oneDay;
// 得到一天前的时间
LLBC_Time oneDayBefore = LLBC_Time::Now() - LLBC_Time::oneDay;

// 得到2小时30分15秒的span
LLBC_TimeSpan span1 = LLBC_TimeSpan::FromHours(2, 30, 15);
// 得到2小时30分钟的span的另外一种方式
LLBC_TimeSpan span2 = LLBC_TimeSpan::oneHour * 2 + LLBC_TimeSpan::oneMiute * 30 + LLBC_TimeSpan::oneSec * 15;

// 得到span的值:
// - 得到总秒数: 2 * 3600 + 30 * 60 + 15
LLBC_PrintLn("- Total span seconds:%d", span1.GetTotalSeconds());
// - 得到秒部分的值: 15
LLBC_PrintLn("- Span seconds:%d", span1.GetSeconds());
```

## 综合示例
以下代码展示了`LLBC_Time`、`LLBC_TimeSpan`其中一部分功能:
```c++
#include <llbc.h>
using namespace llbc;

int main(int argc, char *argv[])
{
    // 初始化llbc
    LLBC_Startup();
    LLBC_Defer(LLBC_Cleanup());

    // 示例1: 获取当前时间(时间对象的时间精度:us)
    LLBC_Time now = LLBC_Time::Now();
    std::cout <<"Now: " <<now <<std::endl;

    // 示例2: 从字符串时间转换回时间对象
    LLBC_Time timeFromStr = LLBC_Time::FromTimeStr("2024-10-01 10:01:00");
    std::cout <<"Construct from time str:" <<timeFromStr <<std::endl;

    // 示例3: 从各部分时间分量转换回时间对象
    LLBC_Time timeFromParts = LLBC_Time::FromTimeParts(2024, 10, 1, 10, 1, 0);
    std::cout <<"Construct from time parts:" <<timeFromParts <<std::endl;

    // 示例4: 得到5天后的时间
    LLBC_Time fiveDaysLaterTime = LLBC_Time::Now() + LLBC_TimeSpan::oneDay * 5;
    std::cout <<"Five days later time:" <<fiveDaysLaterTime <<std::endl;

    // 示例5: 得到离最近的8:00:00的时间间隔
    // - 如当前天内时间在9:00:00, 将返回23 hours
    // - 如当前天内时间在7:00:00, 将返回 1 hours
    // Note: GetIntervalToTimeOfXXX()可获得指定周期内时刻的间隔
    LLBC_TimeSpan spanTime = LLBC_Time::Now().GetIntervalToTimeOfDay(LLBC_TimeSpan::oneHour * 8);
    std::cout <<"Interval to time of day[8:00:00]:" <<spanTime <<std::endl;

    // 示例6: 得到两个时间点是否跨越了天内8点
    // Note: IsCrossedXXX()可确认两个时刻间是否跨越了周期内的某个时间点
    LLBC_Time time1 = LLBC_Time::FromTimeStr("2024-10-01 7:00:00");
    LLBC_Time time2 = LLBC_Time::FromTimeStr("2024-10-01 9:00:00");
    std::cout <<"IsCrossed time of day[8:00:00]:" <<LLBC_Time::IsCrossedDay(time1, time2, LLBC_TimeSpan::oneHour * 8);

    // 示例7: 计算两个时间的偏差
    spanTime = time2 - time1;
    std::cout <<"time2 - time1:" <<spanTime <<std::endl;

    // 示例8: 得到下周的今天
    LLBC_Time nextWeekNow = LLBC_Time::Now() + LLBC_TimeSpan::oneWeek;
    std::cout <<"Next week now:" <<nextWeekNow <<std::endl;

    return 0;   
}
```

以上代码输出(参考):
```
Now: 2024-12-10 22:14:57.943330
Construct from time str:2024-10-01 10:01:00.000000
Construct from time parts:2024-10-01 10:01:00.000000
Five days later time:2024-12-15 22:14:57.943330
Interval to time of day[8:00:00]:09:45:02.056670
IsCrossed time of day[8:00:00]:1
time2 - time1:02:00:00.000000
Next week now:2024-12-17 22:14:57.943330
```
