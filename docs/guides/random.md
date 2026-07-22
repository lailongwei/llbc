---
layout: default
title: 随机 Random
---

# 随机 Random

`LLBC_Random` 是 llbc 对 Mersenne Twister（`std::mt19937`）的封装，提供整数区间随机、
浮点随机、布尔判定、随机选取与洗牌等常用操作。框架同时提供一组以 `LLBC_` 开头的
全局函数供快速使用；如需在多线程中各自持有独立状态，请使用 `LLBC_Random` 实例。

## 播种

构造时可指定初始种子（默认 `0`），运行期可调用 `Seed()` 重置：

```cpp
LLBC_Random rng(42);   // 构造时播种
rng.Seed(12345);       // 重置种子

// 全局随机器也可单独播种
LLBC_SeedRand(99u);
```

## 整数区间随机

三个重载覆盖不同区间约定：

```cpp
LLBC_Random rng;

int a = rng.Rand();          // [-2147483648, 2147483647]
int b = rng.Rand(100);       // [0, 100)，若 end < 0 则 [end, 0)
int c = rng.Rand(-10, 10);   // [-10, 10)，若 begin > end 则区间倒置

// 等价的全局版本
int d = LLBC_Rand(100);
int e = LLBC_Rand(-10, 10);
```

## 按权重随机下标

传入 `vector`/`list`/`std::array`/C 数组，按元素权重返回下标 `[0, size)`，适合掉落概率表：

```cpp
LLBC_Random rng;

// 概率比 1:2:3:4:5，返回下标 0~4
std::vector<int> weights{1, 2, 3, 4, 5};
int idx = rng.Rand(weights);

// C 数组同样支持
int arr[5] = {10, 20, 30, 20, 10};
int idx2 = rng.Rand(arr);
```

<div class="callout note" markdown="1">
权重值会被 `static_cast<int>` 累加（累计器类型为 `int`），支持 `int` 等整数类型；
传入 `sint64` 等大整数时，若权重值超过 `INT_MAX` 会导致累加溢出，请确保单个权重及总和均在 `int` 范围内。
权重之和必须大于 0，否则行为未定义。
</div>

## 随机浮点数

`RandReal()` 返回 `[0.0, 1.0)` 的 `double`：

```cpp
LLBC_Random rng;
double r = rng.RandReal();    // 例如 0.736124
double g = LLBC_RandReal();   // 全局版本
```

## 布尔判定

`BoolJudge()` 以 50% 概率返回 `true` / `false`：

```cpp
LLBC_Random rng;
bool coin = rng.BoolJudge();
bool coin2 = LLBC_BoolJudge();  // 全局版本
```

## 随机选取

`Choice()` 接受一对随机访问迭代器，等概率返回其中一个元素的迭代器；
若范围为空，返回 `end`：

```cpp
LLBC_Random rng;
std::vector<int> v = {10, 20, 30, 40, 50};
auto it = rng.Choice(v.begin(), v.end());
if (it != v.end())
    LLBC_PrintLn("picked: %d", *it);
```

## 洗牌

`Shuffle()` 对 `[begin, end)` 做 Fisher-Yates 原地随机置换：

```cpp
LLBC_Random rng;
std::vector<int> deck = {1, 2, 3, 4, 5, 6, 7, 8};
rng.Shuffle(deck.begin(), deck.end());
// deck 已随机重排
```

## 全局函数一览

全局函数底层共享同一个进程级 `LLBC_Random` 实例，适合脚本式快速调用：

| 函数 | 对应方法 |
|------|---------|
| `LLBC_SeedRand(seed)` | `Seed()` |
| `LLBC_Rand()` | `Rand()` |
| `LLBC_Rand(end)` | `Rand(int end)` |
| `LLBC_Rand(begin, end)` | `Rand(int begin, int end)` |
| `LLBC_RandReal()` | `RandReal()` |
| `LLBC_BoolJudge()` | `BoolJudge()` |

`Choice()` 与 `Shuffle()` 以及权重版 `Rand()` **没有**全局函数，须使用 `LLBC_Random` 实例。

## 线程使用注意

<div class="callout warning" markdown="1">
`LLBC_Random` 内部不加锁，**非线程安全**。全局函数同样共享同一内部实例。

多线程场景下，每个线程应创建独立的 `LLBC_Random` 实例，避免数据竞争：

```cpp
// 每个线程各自持有实例
thread_local LLBC_Random tl_rng(static_cast<uint32>(std::hash<std::thread::id>{}(
    std::this_thread::get_id())));
```
</div>

## 参照

- 头文件：`llbc/include/llbc/core/random/Random.h`
- 内联实现：`llbc/include/llbc/core/random/RandomInl.h`
- 功能测试：`tests/func_test/core/random/FuncTest_Core_Random.cpp`

## 下一步

- [定时器 Timer](timer.md) — 与随机配合实现抖动延迟
- [对象池 ObjPool](objpool.md) — 高频随机对象分配场景
- [Variant](variant.md) — 将随机结果序列化或存入动态容器
