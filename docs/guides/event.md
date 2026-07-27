---
layout: default
title: 事件 EventMgr
---

# 事件 EventMgr

`LLBC_EventMgr` 是 llbc `core` 模块提供的进程内事件总线，用于在同一线程（或同一逻辑域）内解耦生产者与消费者。典型使用场景：游戏逻辑模块间通知、定时任务完成广播、配置热更触发等。

> **与 comm 层的区别**：`LLBC_EventMgr` 是纯 core 层工具，不依赖网络服务。`LLBC_Service` 在 comm 层封装了一套同名 API（`SubscribeEvent` / `UnsubscribeEvent` / `FireEvent` / `BeginFireEvent`），底层同样基于 EventMgr，但具备跨组件线程安全保证，适合 Service+Component 架构。参见 [../concepts/service-component.md](../concepts/service-component.md)。

---

## LLBC_Event：带参事件对象

`LLBC_Event` 是携带键值参数的事件载体，继承自 `LLBC_PoolObj`（对象池支持）。参数键为字符串类型（`const char *`、`std::string`、`LLBC_CString` 均可），值为 `LLBC_Variant`，支持 `operator[]` 下标访问。

```cpp
// 直接构造并设参
LLBC_Event ev(MyEventIds::PlayerLogin);
ev.SetParam("playerId", 9527);
ev.SetParam("playerName", LLBC_String("Alice"));

// 等价下标写法
ev["level"] = 99;

// 读取参数
int id   = ev.GetParam("playerId").As<int>();
auto &lv = ev["level"];   // LLBC_Variant &
```

<div class="callout note" markdown="1">
`dontDelAfterFire`（第二个构造参数，默认 `false`）控制 `Fire()` 后是否自动回收对象。若需在触发后继续持有 `LLBC_Event` 指针，应传 `true`，并手动 `delete`（或 `LLBC_Recycle`）。默认为 `false`，即 `Fire()` 后由 EventMgr 负责释放。
</div>

---

## 订阅事件：AddListener

`AddListener` 支持三种形式，均返回 `LLBC_ListenerStub`（退订凭据）；失败时返回 `0`（无效 stub）。

```cpp
LLBC_EventMgr evMgr;

// 1. lambda / 函数对象
auto stub1 = evMgr.AddListener(MyEventIds::PlayerLogin,
    [](LLBC_Event &ev) {
        LLBC_PrintLn("Player %d logged in", ev["playerId"].As<int>());
    });

// 2. 成员函数指针
class MyMgr {
public:
    void OnPlayerLogin(LLBC_Event &ev) { /* ... */ }
};
MyMgr mgr;
auto stub2 = evMgr.AddListener(MyEventIds::PlayerLogin, &mgr, &MyMgr::OnPlayerLogin);

// 3. LLBC_EventListener 子类（适合需要多态的场景）
class LoginListener : public LLBC_EventListener {
public:
    void Invoke(LLBC_Event &ev) override { /* ... */ }
};
auto stub3 = evMgr.AddListener(MyEventIds::PlayerLogin, new LoginListener);
```

<div class="callout note" markdown="1">
同一个事件 id 可绑定多个监听器，触发时按注册顺序依次回调。
</div>

---

## 触发事件：Fire 与 BeginFire

**直接 Fire**：手动创建 `LLBC_Event` 后传入，所有权转移给 EventMgr（`dontDelAfterFire=false` 时会被自动释放）。

```cpp
auto *ev = new LLBC_Event(MyEventIds::PlayerLogin);
ev->SetParam("playerId", 1001);
evMgr.Fire(ev);   // ev 触发后被释放，勿再访问
```

**链式 BeginFire**：更简洁，推荐优先使用。`BeginFire` 内部通过对象池分配 `LLBC_Event`，`SetParam` 返回 `LLBC_EventFirer &`，最终 `.Fire()` 触发并释放。

```cpp
evMgr.BeginFire(MyEventIds::PlayerLogin)
    .SetParam("playerId", 1001)
    .SetParam("playerName", "Alice")
    .Fire();
```

<div class="callout note" markdown="1">
`Fire()` 返回 `int`：`LLBC_OK` 表示成功，`LLBC_FAILED` 表示失败。若启用了死循环检测（`LLBC_CFG_CORE_ENABLE_EVENT_FIRE_DEAD_LOOP_DETECTION`），事件递归触发同一 id 时会返回 `LLBC_FAILED` 并将 `LLBC_GetLastError()` 置为 `LLBC_ERROR_REPEAT`。
</div>

---

## 嵌套触发

EventMgr 支持在监听器回调中触发其他事件（或相同事件），操作安全：

```cpp
evMgr.AddListener(MyEventIds::Event1, [&evMgr](LLBC_Event &ev) {
    // 在 Event1 处理函数中触发 Event2
    evMgr.BeginFire(MyEventIds::Event2)
        .SetParam("source", "Event1")
        .Fire();
});
```

---

## 退订：RemoveListener

```cpp
// 按事件 id 退订该 id 下所有监听器
evMgr.RemoveListener(MyEventIds::PlayerLogin);

// 按 stub 退订单个监听器
evMgr.RemoveListener(stub1);

// 按 stub 退订并自动将 stub 清零（防悬空）
evMgr.RemoveListenerX(stub2);   // stub2 置为 0

// 清空全部监听器
evMgr.RemoveAllListeners();
```

<div class="callout warning" markdown="1">
**在 Fire 回调中调用 Remove** 时，操作不会立即生效，而是标记为 Pending，`RemoveListener` 会返回 `LLBC_FAILED` 且 `LLBC_GetLastError() == LLBC_ERROR_PENDING`。这是预期行为，Fire 结束后操作会自动完成，无需重试。
</div>

---

## EventFirer 链式接口

`BeginFire` 返回的 `LLBC_EventFirer` 对象持有待触发的事件，支持链式 `SetParam`：

```cpp
LLBC_EventFirer firer = evMgr.BeginFire(MyEventIds::ScoreUpdate);
firer.SetParam("delta", 100)
     .SetParam("reason", "kill")
     .Fire();
```

<div class="callout note" markdown="1">
`LLBC_EventFirer` 析构时若尚未调用 `Fire()`，持有的事件对象会被自动回收（`LLBC_Recycle`），不会触发监听器。务必显式调用 `.Fire()` 才能派发事件。
</div>

---

## 在 Service/Component 中使用

`LLBC_Service` 对 EventMgr 封装了线程安全的对外接口，在 `LLBC_Component` 中应优先使用 Service API 而非直接操作 `LLBC_EventMgr`：

```cpp
class MyComp : public LLBC_Component {
public:
    int OnInit(bool &initFinished) override {
        _stub = GetService()->SubscribeEvent(
            MyEventIds::PlayerLogin, this, &MyComp::OnPlayerLogin);
        return LLBC_OK;
    }

    void OnUpdate() override {
        GetService()->BeginFireEvent(MyEventIds::PlayerLogin)
            .SetParam("playerId", 42)
            .Fire();
    }

    void OnPlayerLogin(LLBC_Event &ev) {
        // 处理
        GetService()->UnsubscribeEvent(_stub);
    }

private:
    LLBC_ListenerStub _stub;
};
```

<div class="callout important" markdown="1">
**Component 内的非流程事件**（业务自定义事件）走 `GetService()->SubscribeEvent/FireEvent`，而不是 `OnEvent()`。`LLBC_Component::OnEvent()` 是框架内部保留回调（非流程生命周期通知），勿用于业务事件路由。
</div>

---

## 参照

- 头文件
  - `llbc/include/llbc/core/event/Event.h`
  - `llbc/include/llbc/core/event/EventMgr.h`
  - `llbc/include/llbc/core/event/EventFirer.h`
- 真实用例
  - `tests/func_test/core/event/FuncTest_Core_Event.cpp`（基础订阅/触发/退订、嵌套 Fire、死循环检测）
  - `tests/func_test/comm/FuncTest_Comm_EventInSvc.cpp`（Service 层 SubscribeEvent / BeginFireEvent 用法）
- 快速上手示例（可跑）：`tests/example/core/Example_Core_Event.cpp`

---

## 下一步

- [variant.md](variant.md) — 理解事件参数值类型 `LLBC_Variant`
- [delegate.md](delegate.md) — 了解 `LLBC_Delegate` 与成员函数绑定原理
- [objpool.md](objpool.md) — `LLBC_Event` 继承自 `LLBC_PoolObj`，了解对象池回收机制
