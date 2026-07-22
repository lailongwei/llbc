---
layout: default
title: 常见问题
---

# 常见问题

使用 llbc 时遇到的高频问题汇总。每条简答后附交叉链接，方便深入阅读。

---

## Q1：`LLBC_Startup()` 和 `LLBC_Cleanup()` 为什么必须成对调用？

`LLBC_Startup()` 按顺序初始化 common → core → comm → testcase → app 五个子模块（每个子模块有独立的 `__LLBC_<Module>Startup()` 入口）；`LLBC_Cleanup()` 按相反顺序清理它们。未初始化就调用公共 API 会导致未定义行为（崩溃或断言失败），未清理则会泄漏全局资源（线程、定时器队列、日志后台线程等）。

推荐在 `main()` 入口处直接配对：

```cpp
#include <llbc.h>
using namespace llbc;

int main()
{
    LLBC_Startup();

    // ... 业务逻辑 ...

    LLBC_Cleanup();
    return 0;
}
```

unit_test 的惯例是通过 gtest `Environment` 保证配对（见 `tests/unit_test/UnitTest.cpp`）：

```cpp
class TestEnv_LLBCIniter : public ::testing::Environment
{
public:
    void SetUp() override   { LLBC_Startup();  }
    void TearDown() override { LLBC_Cleanup(); }
};
```

---

## Q2：克隆仓库后封装层（pyllbc/lullbc）或 unit_test 编译报找不到文件，为什么？

仓库含三个 git 子模块，普通 `git clone` 不会自动拉取：

| 子模块路径 | 用途 |
|-----------|------|
| `wrap/pyllbc/cpython` | Python 封装编译依赖 |
| `wrap/lullbc/lua` | Lua 封装编译依赖 |
| `tests/3rdparty/googletest` | unit_test 的 gtest/gmock |

子模块目录存在但为空时，封装层会**静默产出损坏的制品**，CMake 的 `unit_test` 目标则会在链接 gtest 时失败。

正确做法：克隆时加 `--recurse-submodules`，或在已有克隆中补全：

```bash
# 方式 1：克隆时一次性拉取
git clone --recurse-submodules https://github.com/lailongwei/llbc.git

# 方式 2：已克隆后补全
git submodule update --init --recursive
```

---

## Q3：日志配置文件里写了 `logLevel` 但不生效，怎么回事？

`logLevel` 已在 **v1.1.1** 废弃，框架不再读取该字段。请改用各 appender 级别的独立配置项：

| 废弃字段 | 替代字段 | 含义 |
|---------|---------|------|
| `logLevel` | `consoleLogLevel` | 控制台输出的最低级别 |
| `logLevel` | `fileLogLevel` | 文件输出的最低级别 |

同时注意：v1.1.1 对调了 Debug 与 Trace 的顺序，**Trace 现在是最低级别**（比 Debug 更详细）。

```ini
# 正确写法（ini 格式示例）
[root]
consoleLogLevel = Debug
fileLogLevel    = Trace
```

<div class="callout warning" markdown="1">
若日志文件或控制台没有输出，先确认 `consoleLogLevel` / `fileLogLevel` 已设置，且级别不高于实际打印调用的级别（Trace < Debug < Info < Warn < Error < Fatal）。
</div>

---

## Q4：macOS 上网络性能很差，是 bug 吗？

不是 bug，是已知限制。llbc 的平台支持分两档：

- **一档（生产推荐）**：Linux（EpollPoller）、Windows（IocpPoller）
- **二档（开发调试可用，不推荐生产）**：macOS（SelectPoller）

macOS 上网络层使用 `SelectPoller`，性能显著低于 Linux 的 epoll 和 Windows 的 IOCP，在高连接数或高吞吐场景下会成为瓶颈。**macOS 仅适合本地开发和功能调试，生产环境请部署到 Linux。**

---

## Q5：`LLBC_ObjPool::Acquire<MyObj>()` 编译报 `static_assert` 失败，但 `MyObj` 继承了 `LLBC_Object`，这是为什么？

`LLBC_ObjPool` 明确**禁止**管理 `LLBC_Object` 派生类（`ObjPool.h` 中有编译期断言）：

```cpp
// ObjPool.h 内部（已简化）
template <typename Obj>
Obj *Acquire()
{
    static_assert(
        !std::is_base_of_v<LLBC_Object, Obj>,
        "Obj can not create by ObjPool, as is derived form LLBC_Object");
    // ...
}
```

`LLBC_Object` 有自己的引用计数 + 自动释放池（`Retain()`/`Release()`/`AutoRelease()`）生命周期模型，与 `ObjPool` 的 Acquire/Release 归还模型根本冲突，混用会导致双重释放。

如需在池中使用 `LLBC_Object` 派生类，应让对象继承 `LLBC_PoolObj`（而非 `LLBC_Object`），并通过 `ObjPool` 管理。

---

## Q6：应该选 `LLBC_Object`（引用计数）还是 `LLBC_ObjPool`（对象池）？

两者解决不同问题，选择依据如下：

| 维度 | `LLBC_Object` | `LLBC_ObjPool` / `LLBC_PoolObj` |
|------|--------------|--------------------------------|
| 核心机制 | 引用计数 + 自动释放池 | Acquire/Release 归还复用 |
| 适用场景 | 共享所有权、生命周期不确定的对象 | 高频创建销毁、生命周期明确的对象 |
| 性能 | 引用计数有原子操作开销 | 池化避免频繁 malloc/free，性能更高 |
| 线程安全 | `SafeRetain()`/`SafeRelease()` 显式选择 | `LLBC_ObjPool(threadSafe=true)` 或线程专属池 |
| 归还方式 | `Release()` / `AutoRelease()` | `pool->Release(obj)` 或 `LLBC_GuardedPoolObj<T>` RAII 自动归还 |

**经验规则**：网络层的 `Packet`、业务消息、临时数据结构等短生命周期对象优先用 `ObjPool`；需要在多处共享引用的业务对象（如场景中的实体）考虑 `LLBC_Object`。

对象池的 RAII 用法示例：

```cpp
LLBC_ObjPool pool;
{
    // AcquireGuarded 返回 LLBC_GuardedPoolObj，离开作用域自动归还
    auto guardedObj = pool.AcquireGuarded<MyData>();
    guardedObj->DoSomething();
} // 此处自动 Release 回池
```

每个 llbc 线程还内置了线程专属池（无锁，性能最高）：

```cpp
// 当前线程的非线程安全池（性能最佳，只能在本线程用）
auto *obj = LLBC_ThreadSpecObjPool::UnsafeAcquire<MyData>();
// ...
LLBC_ThreadSpecObjPool::UnsafeRelease(obj);
```

---

## 参照

- 头文件：`llbc/include/llbc/core/objpool/ObjPool.h`
- 头文件：`llbc/include/llbc/core/objpool/ThreadSpecObjPool.h`
- 头文件：`llbc/include/llbc/core/objbase/Object.h`
- 对象池真实用例：`tests/func_test/core/objpool/FuncTest_Core_ObjPool.cpp`
- Startup/Cleanup 范例：`tests/unit_test/UnitTest.cpp`
- 变更说明：`CHANGELOG`（v1.1.1 第 15、19 条）

## 下一步

- 对象池深入：[对象池](guides/objpool.md)
- 日志配置：[日志](guides/logger.md)
- Stream 序列化：[序列化 Stream](guides/stream.md)
- 组件生命周期：[Service 与 Component](concepts/service-component.md)
