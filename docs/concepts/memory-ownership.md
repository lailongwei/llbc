---
layout: default
title: 内存与对象所有权
---

# 内存与对象所有权

llbc 提供三种各有边界的内存/所有权模型：基于引用计数的 `LLBC_Object`、基于内存池的
`LLBC_ObjPool`，以及普通的栈分配与 `LLBC_Stream` 缓冲区。选对模型可以避免双重释放、
内存泄漏以及不必要的分配开销。v1.1.1 起旧对象池已删除，`LLBC_ObjPool` 是唯一池化方案。

---

## 模型一：LLBC_Object — 引用计数 + AutoRelease

`LLBC_Object` 维护两个计数器：手动引用计数（`_ref`）和自动释放引用计数（`_autoRef`）。
对象在堆上用 `new` 创建，通过 `Retain`/`Release` 进行手动管理，或通过 `AutoRelease`
委托给当前线程的自动释放池（帧末自动 Purge）。

```cpp
class MyObj final : public LLBC_Object
{
public:
    MyObj()  { m_data = malloc(64); }
    ~MyObj() override { free(m_data); }
private:
    void *m_data;
};

// 手动引用计数
MyObj *obj = new MyObj;   // ref = 1
obj->Retain();            // ref = 2
obj->Release();           // ref = 1
obj->Release();           // ref = 0 → 析构

// 委托给 AutoRelease 池：帧末自动析构
MyObj *obj2 = new MyObj;
obj2->AutoRelease();      // 加入自动释放池
obj2->Retain();           // 多持有一次
obj2->AutoRelease();      // 帧末会 Release 两次
```

<div class="callout warning" markdown="1">
**LLBC_Object 子类不能放入 LLBC_ObjPool。**
`LLBC_ObjPool::Acquire<Obj>()` 内有编译期断言：
`static_assert(!std::is_base_of_v<LLBC_Object, Obj>, ...)` — 对 `LLBC_Object` 派生类
调用 `Acquire` 会直接编译报错。
</div>

<div class="callout note" markdown="1">
`Retain`/`Release` 不是线程安全的；跨线程共享同一个对象时，使用 `SafeRetain`/`SafeRelease`。
</div>

---

## 模型二：LLBC_ObjPool — 值类型池化

`LLBC_ObjPool` 适用于**不继承 `LLBC_Object` 的任意值类型**（包括 STL 容器、`LLBC_Stream`、
`LLBC_Packet` 等）。内存分条带（stripe）预分配，`Acquire` 复用已释放槽位，`Release` 将
对象归还池而不真正析构（如果类型提供 `clear`/`Clear`/`reset`/`Reset`/`reuse`/`Reuse`
方法，会被自动调用以复位状态）。

```cpp
LLBC_ObjPool objPool;                        // 默认非线程安全
// LLBC_ObjPool safePool(true);             // 线程安全版本

// 获取 / 归还
std::string *s = objPool.Acquire<std::string>();
*s = "hello world";
objPool.Release(s);                          // 归还池，不析构

// RAII 守卫：离开作用域自动归还
auto guarded = objPool.AcquireGuarded<std::string>();
guarded->assign("hey judy");
// 离开作用域时自动 Release
```

`LLBC_GuardedPoolObj<T>` 支持移动语义（所有权转移）和 `Detach()` 方法（剥离原始指针，
由调用者负责手动 `Release`）：

```cpp
auto g1 = objPool.AcquireGuarded<std::string>();
auto g2 = std::move(g1);        // g1 变空，g2 持有所有权
std::string *raw = g2.Detach(); // g2 变空，调用者持有 raw
objPool.Release(raw);           // 手动归还
```

<div class="callout note" markdown="1">
**有序析构控制**：当 A 的析构依赖 B 尚未析构时，调用
`objPool.EnsureDeletionBefore<A, B>()` 注册依赖顺序，`LLBC_ObjPool` 析构时按序清理。
</div>

---

## 模型三：栈分配与 LLBC_Stream 缓冲区

生命周期在单个调用栈内结束、无需跨帧传递的对象可以直接栈分配，由析构函数自动释放。
`LLBC_Stream` 本身也是值语义，可以栈上声明，也支持对象池反射（`GetTypedObjPool` /
`SetTypedObjPool`），可从 `LLBC_ObjPool` 分配。

```cpp
// 栈上使用 LLBC_Stream（短生命周期序列化）
void SerializeLocally()
{
    LLBC_Stream stream;
    stream << 42 << "hello" << 3.14;
    stream.SetReadPos(0);
    int n; LLBC_String s; double d;
    stream >> n >> s >> d;
}   // stream 离开作用域，内部 buffer 自动释放

// 从 ObjPool 获取 LLBC_Stream（频繁创建场景）
LLBC_ObjPool objPool;
auto *stream = objPool.Acquire<LLBC_Stream>();
*stream << "pooled data";
LLBC_Recycle(stream);   // 归还池并复位（调用 clear/Clear/Reset...）
```

<div class="callout note" markdown="1">
`LLBC_Stream` 读写游标分离：`SetReadPos` 和 `SetWritePos` 彼此独立。
从池中归还再重新 `Acquire` 时，`Clear` 方法会被自动调用，游标和缓冲长度均清零。
</div>

---

## 通用归还：LLBC_Recycle / LLBC_ObjReflector::RecycleX

`LLBC_ObjReflector::Recycle<T>(obj)` 是通用回收入口，可以统一处理三种情况：

| 对象来源 | 行为 |
|---------|------|
| 继承 `LLBC_Object` | 调用 `AutoRelease()`，委托帧末 GC |
| 从 `LLBC_ObjPool` 获取（持有池指针） | 归还到来源池 |
| 普通 `new` | 直接 `delete` |

`LLBC_Recycle(obj)` 宏等价于 `LLBC_ObjReflector::Recycle(obj)`（**不**置 `nullptr`；若需归还后将指针清空，可直接调用 `LLBC_ObjReflector::RecycleX(obj)`）。

```cpp
// obj 可以是池对象、LLBC_Object 子类，或普通 new 的对象
LLBC_Recycle(obj);   // 统一归还，不必区分来源
```

---

## 选型决策

| 场景 | 推荐模型 |
|------|---------|
| 框架内置对象（`LLBC_Array`、`LLBC_Dictionary` 等） | `LLBC_Object` 引用计数 |
| 业务数据包、事件对象、高频临时容器 | `LLBC_ObjPool` |
| 单帧/单函数内的序列化缓冲 | 栈上 `LLBC_Stream` |
| 跨帧传递的序列化缓冲 | `LLBC_ObjPool` 获取 `LLBC_Stream` |
| 需要跨线程安全引用计数 | `LLBC_Object` + `SafeRetain`/`SafeRelease` |
| 需要跨线程安全池化 | `LLBC_ObjPool(true)` |

<div class="callout important" markdown="1">
**关键约束汇总**

- `LLBC_Object` 子类 **禁止** 放入 `LLBC_ObjPool`，编译期强制检查。
- `LLBC_Object` 析构由引用计数驱动，**不要** 对 ref > 0 的对象直接 `delete`。
- `LLBC_ObjPool` 中的对象 **不要** 在 `Release` 后继续访问，也不要 `Release` 两次。
- `LLBC_Stream` 从池获取后若调用 `Attach`，析构不释放外部缓冲，注意生命周期。
</div>

---

## 参照

**头文件**
- `llbc/include/llbc/core/objbase/Object.h` — `LLBC_Object` 引用计数接口
- `llbc/include/llbc/core/objbase/AutoReleasePool.h` — `LLBC_AutoReleasePool`
- `llbc/include/llbc/core/objpool/ObjPool.h` — `LLBC_ObjPool` / `LLBC_PoolObj` / `LLBC_GuardedPoolObj`
- `llbc/include/llbc/common/Stream.h` — `LLBC_Stream`

**真实示例**
- `tests/func_test/core/objbase/FuncTest_Core_ObjBase_Object.cpp` — `LLBC_Object` Retain/Release
- `tests/func_test/core/objpool/FuncTest_Core_ObjPool.cpp` — 池化 Acquire/Release/Guarded/Recycle
- `tests/func_test/comm/FuncTest_Comm_ReleasePool.cpp` — `AutoRelease` 在组件中的用法

---

## 下一步

- [LLBC_Object 引用计数](../guides/object.md) — 完整 API 与 AutoRelease 池栈说明
- [LLBC_ObjPool 对象池](../guides/objpool.md) — 池反射、有序析构、线程安全配置
- [序列化 Stream](../guides/stream.md) — 读写游标、Attach、自定义类型序列化
