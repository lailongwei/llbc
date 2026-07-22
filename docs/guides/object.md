---
layout: default
title: 对象 Object
---

# 对象 Object

`LLBC_Object` 是 llbc 中所有“可引用计数管理 / 可自动释放”对象的基类。它提供一套
**非侵入式**的对象生命周期管理机制：对象自己携带引用计数，调用方通过显式的
`Retain()` / `Release()` 约定所有权，也可以把对象交给**自动释放池**（Auto Release Pool）
做延迟释放。引用计数还提供了线程安全的 `SafeRetain()` / `SafeRelease()` 变体。

## 概述

- 每个 `LLBC_Object` 构造时引用计数为 `1`（即“创建者默认持有一份引用”）。
- `Retain()` 使引用计数加 1，`Release()` 使引用计数减 1；当引用计数降到 `0` 时对象自删除（`delete this`）。
- `AutoRelease()` 把对象委托给**当前线程**的自动释放池，由池在合适时机（如 service 帧尾、
  或池析构时）替你释放一份引用。
- 所有权是**概念性**的——由调用方按约定维护，编译器不会强制。这与 `std::shared_ptr` /
  `std::unique_ptr` 的类型级强制所有权不同（见文末对比）。

<div class="callout callout-info">
继承 <code>LLBC_Object</code> 只需 <code>public</code> 派生即可；不要在栈上创建需要引用计数
管理的对象——引用计数模型假定对象在堆上、由 <code>Release()</code> 负责 <code>delete</code>。
栈对象离开作用域时会正常析构，但对它调用 <code>Release()</code> 会导致对栈内存 <code>delete</code>。
</div>

## 使用示例

### 1. 手动 Retain / Release（显式生命周期管理）

```cpp
class MyObj : public LLBC_Object
{
    // ...
};

MyObj *obj = new MyObj;   // 构造后引用计数为 1
obj->Retain();            // 引用计数 -> 2
obj->Release();           // 引用计数 -> 1
obj->Release();           // 引用计数 -> 0，对象在此被 delete
```

跨线程共享同一对象时，用线程安全版本（内部走原子操作）：

```cpp
obj->SafeRetain();        // 原子自增引用计数
obj->SafeRelease();       // 原子自减；减到 0 时 delete
```

### 2. AutoRelease（自动生命周期管理）

`AutoRelease()` 是成员方法，调用后对象被登记到**当前线程**的自动释放池，无需自己再配对
`Release()`：

```cpp
MyObj *obj = new MyObj;   // 引用计数为 1
obj->AutoRelease();       // 交给当前线程的自动释放池托管
// 之后不要再对 obj 调用 Release()：那一份引用已经交给池了
```

在 service 场景里，池由 service 每帧维护，帧尾统一清理。下面是组件中的典型用法
（摘自 `tests/func_test/comm/FuncTest_Comm_ReleasePool.cpp`）：

```cpp
class MyComp : public LLBC_Component
{
public:
    int OnInit(bool &initFinished) override
    {
        TestObj *obj = new TestObj;
        obj->AutoRelease();   // 初始引用计数(1)交给池

        obj->Retain();        // 想多托管一份，就先补一份引用
        obj->AutoRelease();
        return LLBC_OK;
    }
};
```

<div class="callout callout-warning">
每次 <code>AutoRelease()</code> 需要“消耗”一份引用：登记要求当前
<code>autoRef &lt; ref</code>，否则返回 <code>LLBC_FAILED</code>。因此若想对同一对象多次
<code>AutoRelease()</code>，需先用 <code>Retain()</code> 补足引用计数，如上例所示。
</div>

也可以显式创建一个 `LLBC_AutoReleasePool`，它在**析构时**清理其中登记的对象：

```cpp
LLBC_AutoReleasePool *pool = new LLBC_AutoReleasePool;  // 压入当前线程池栈栈顶

LLBC_Object *obj = new TestObj;
obj->AutoRelease();   // 登记到栈顶（即刚创建的这个）池

delete pool;          // 池析构，登记在内的对象在此被释放
```

## 设计与实现

### 设计目标

1. **统一生命周期管理模型**——所有需要引用管理的对象共享同一套 `Retain`/`Release`/`AutoRelease` 接口。
2. **支持自动释放池**——允许把释放时机延迟到帧尾或作用域末尾，简化临时对象管理。
3. **可扩展、可继承**——基类接口均为 `virtual`，派生类可覆写 `Clone()` / `ToString()` /
   `Serialize()` / `Deserialize()` 等。
4. **跨线程 / 跨模块安全**——引用计数提供 `SafeRetain()` / `SafeRelease()` 原子版本；
   自动释放池按线程隔离（每个线程有独立的池栈）。

### 核心成员

`LLBC_Object` 的关键内部状态（均为私有，此处按语义描述）：

| 成员 | 含义 |
|------|------|
| 引用计数 `_ref` | 手动 `Retain` / `Release`（含 Safe 版本）维护的引用计数；降到 0 触发 `delete this`。 |
| 自动释放计数 `_autoRef` | 该对象当前被自动释放池登记的次数。 |
| 池栈指针 `_poolStack` | 指向对象所属的、当前线程的自动释放池栈（`LLBC_AutoReleasePoolStack`）。 |

### Retain / Release 机制

- `Retain()`：表示对象被某处显式持有，引用计数加 1。
- `Release()`：释放一份显式引用，引用计数减 1。
- 当引用计数减到 `0` 时，对象自删除（`delete this`）。
- `SafeRetain()` / `SafeRelease()` 是对应的线程安全版本，使用原子操作维护引用计数，
  用于对象可能被多个线程同时增减引用的场景。

### AutoRelease 机制

`AutoRelease()` 把释放动作延迟交给自动释放池。运行流程：

1. 首次调用时，对象通过线程本地存储（TLS）定位到**当前线程**的自动释放池栈
   `LLBC_AutoReleasePoolStack`，并记住它（`_poolStack`）。
2. 把对象登记进栈顶那个池：自动释放计数加 1，同时**立刻**执行一次 `Release()`——
   相当于把“创建者持有的那份引用”转交给了池。（这也是为什么每次 `AutoRelease()`
   前都要保证有一份可供转交的引用。）
3. 当池被**清理（Purge）**时——service 每帧帧尾会调用池栈的 `Purge()`，独立池则在析构时
   清理——池遍历登记对象、递减其自动释放计数，并释放池对这些对象持有的引用，从而完成延迟释放。

<div class="callout callout-info">
service 会在每帧结束时对当前线程的池栈做一次 <code>Purge()</code>，并在服务停止时再清理一次。
因此在组件回调（如 <code>OnUpdate()</code>）中 <code>AutoRelease()</code> 的临时对象，会在该帧
处理完成后统一释放，无需手动 <code>Release()</code>。
</div>

## LLBC_Object vs std::shared_ptr / std::unique_ptr

| 维度 | `LLBC_Object` | `std::shared_ptr` | `std::unique_ptr` |
|------|---------------|-------------------|-------------------|
| 引用计数位置 | 对象自身携带（侵入式基类） | 独立控制块 | 无（唯一所有权） |
| 所有权 | 概念性，由调用方约定 | 类型系统强制、可共享 | 类型系统强制、唯一 |
| 释放时机 | 显式 `Retain`/`Release` 归零；可选 `AutoRelease` 延迟释放 | 最后一个持有者析构（计数归 0） | 离开作用域 / 显式 `reset` |
| 线程安全 | 计数需用 `SafeRetain`/`SafeRelease` | 控制块计数原子、天然安全 | 无共享，无需同步 |
| 适用场景 | 需接入 llbc 池 / service 帧生命周期的对象 | 通用共享所有权 | 通用独占所有权 |

简言之：`LLBC_Object` 更贴近 llbc 的 service / 自动释放池模型，适合与框架生命周期
（帧尾统一回收临时对象）配合；若只是普通业务对象、无需接入池机制，优先使用标准库的
智能指针。

## 参照

- 头文件：`llbc/include/llbc/core/objbase/Object.h`
- 自动释放池：`llbc/include/llbc/core/objbase/AutoReleasePool.h`、
  `llbc/include/llbc/core/objbase/AutoReleasePoolStack.h`
- 手动 Retain/Release 示例：`tests/func_test/core/objbase/FuncTest_Core_ObjBase_Object.cpp`
- AutoRelease + service 池示例：`tests/func_test/comm/FuncTest_Comm_ReleasePool.cpp`
