---
layout: default
title: App 与多服务组合
---

# App 与多服务组合

`LLBC_App` 是进程级生命周期的抽象容器：它负责初始化 llbc 库、编排任意数量的
`LLBC_Service`、统一处理配置重载，并响应操作系统信号（`SIGINT`/`SIGUSR2` 等）。
典型的游戏或服务器进程继承 `LLBC_App` 后，在 `OnStart` 中创建并启动若干 Service，
各 Service 各自跑独立逻辑线程，互不干扰。

`LLBC_App` 是可选层——裸用 `LLBC_Service` 同样合法；但一旦需要多 Service
协同、统一配置或热重载，推荐引入 `LLBC_App`。

> **信号默认配置**（Linux/macOS）：停止信号为 `SIGINT`/`SIGTERM`/`SIGQUIT`，热重载信号为 `SIGUSR2`；Windows 无热重载信号。

## 继承 LLBC_App

只需实现两个纯虚钩子，其余均可按需覆写：

```cpp
class MyApp final : public LLBC_App
{
public:
    // 必须覆写：进程启动逻辑
    int OnStart(int argc, char *argv[], bool &startFinished) override;
    // 必须覆写：进程停止逻辑
    void OnStop(bool &stopFinished) override;
};
```

## 完整生命周期钩子

App 的生命周期钩子按调用顺序排列：

| 钩子 | 签名 | 说明 |
|------|------|------|
| `OnEarlyStart` | `int OnEarlyStart(int argc, char *argv[], bool &earlyStartFinished)` | 最早执行；返回 `LLBC_FAILED` 终止启动 |
| `OnStart` | `int OnStart(int argc, char *argv[], bool &startFinished)` | **纯虚**；可多帧完成（将 `startFinished` 置 `false` 则下帧继续调用） |
| `OnLateStart` | `void OnLateStart(int argc, char *argv[])` | 所有钩子均完成后最后执行一次 |
| `OnUpdate` | `void OnUpdate()` | 主循环每帧调用 |
| `OnReload` | `void OnReload()` | 配置重载成功后调用 |
| `OnReloadFailed` | `void OnReloadFailed()` | 配置重载失败后调用 |
| `OnEarlyStop` | `void OnEarlyStop(bool &earlyStopFinished)` | 停止流程最早执行 |
| `OnStop` | `void OnStop(bool &stopFinished)` | **纯虚**；同样可多帧完成 |
| `OnLateStop` | `void OnLateStop()` | 停止流程最后执行 |

<div class="callout note" markdown="1">
`OnStart` / `OnStop` 的 `bool &` 出参默认值为 `true`（单帧完成）。
若启动或停止需要等待异步操作，将其置 `false`，框架会在下一帧再次调用，
直到置为 `true` 为止。`OnEarlyStart` / `OnEarlyStop` 同理。
</div>

## 主函数入口

`Start()` 阻塞主线程直到 App 停止（收到停止信号或调用 `Stop()`），
返回 `0` 表示正常退出，`-1` 表示启动失败。

```cpp
int main(int argc, char *argv[])
{
    LLBC_Startup(); // 可选：App 内部也会自动调用

    MyApp app;
    app.SetConfigPath("./server.ini"); // 可选配置路径
    const int ret = app.Start(argc, argv, "MyServer");

    LLBC_Cleanup();
    return ret;
}
```

## 在 OnStart 中创建多个 Service

`LLBC_Service::Create()` 创建堆对象；调用 `Start()` 后 Service 即在自己的线程中运行。
App 停止时会自动停止所有已注册的 Service，**不要在 OnStop 中再次 delete**。

```cpp
int MyApp::OnStart(int argc, char *argv[], bool &startFinished) override
{
    // 网关 Service：处理客户端连接
    auto gatewaySvc = LLBC_Service::Create("GatewaySvc");
    gatewaySvc->AddComponent<GatewayComp>();
    if (gatewaySvc->Start() != LLBC_OK)
    {
        LLBC_PrintLn("GatewaySvc start failed: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    // 逻辑 Service：处理业务逻辑
    auto logicSvc = LLBC_Service::Create("LogicSvc");
    logicSvc->AddComponent<LogicComp>();
    if (logicSvc->Start() != LLBC_OK)
    {
        LLBC_PrintLn("LogicSvc start failed: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

void MyApp::OnStop(bool &stopFinished) override
{
    // Service 由 App 自动停止，此处无需手动 delete
}
```

## 按名/按 ID 检索 Service

`LLBC_App` 通过内部 `LLBC_ServiceMgr` 维护所有已启动的 Service，提供按名和按 ID 两种检索方式：

```cpp
// 按名检索（Service 在 Start 后自动注册到 ServiceMgr）
LLBC_Service *gw = LLBC_App::ThisApp()->GetService("GatewaySvc");
if (gw)
    gw->Post(...); // 跨 Service 投递任务

// 按 ID 检索（ID 由 Service 启动时自动分配）
LLBC_Service *svc2 = LLBC_App::ThisApp()->GetService(2);
```

<div class="callout note" markdown="1">
`GetService()` 委托给全局单例 `LLBC_ServiceMgrSingleton`，线程安全。
未找到时返回 `nullptr`，调用方自行判空。
</div>

## 停止单个 Service

App 运行期间可按需停止并释放某个 Service：

```cpp
// 按名停止并删除（del=true 为默认值）
LLBC_App::ThisApp()->StopService("GatewaySvc");

// 按 ID 停止，保留指针（del=false）让调用方自行管理生命周期
LLBC_App::ThisApp()->StopService(2, false);
```

## 统一配置重载

调用 `LLBC_App::Reload()` 或发送 `SIGUSR2` 信号均可触发重载流程。
重载成功后框架按顺序回调：所有 `LLBC_Component::OnReload()` → `LLBC_App::OnReload()`；
失败则调用 `LLBC_App::OnReloadFailed()`，同时每个 Component 会收到
`LLBC_ComponentEventType::AppReloadFailed` 事件。

```cpp
void MyApp::OnReload() override
{
    // GetConfig() 已返回新配置（线程安全）
    LLBC_PrintLn("App reloaded, new config: %s",
                 GetConfig().ToString().c_str());
}

void MyApp::OnReloadFailed() override
{
    LLBC_PrintLn("Reload failed, keeping old config");
}
```

## OnUpdate 驱动主线程逻辑

App 主循环每帧调用一次 `OnUpdate()`，可在此执行需要在主线程运行的轻量逻辑：

```cpp
void MyApp::OnUpdate() override
{
    // 检查控制台命令、刷新监控指标等
}
```

<div class="callout warning" markdown="1">
`OnUpdate()` 在主线程中运行，与 Service 的工作线程不在同一线程。
跨线程操作 Service 数据时需使用 `Service::Post()` 将任务投递到 Service 线程。
</div>

## 不使用 App 的场景

若只需一个 Service，可以完全跳过 `LLBC_App`，手动管理生命周期：

```cpp
LLBC_Startup();

auto svc = LLBC_Service::Create("Standalone");
svc->AddComponent<MyComp>();
svc->Start();
svc->Wait(); // 阻塞，直到 Service 停止

delete svc;
LLBC_Cleanup();
```

<div class="callout note" markdown="1">
绕过 `LLBC_App` 时，热重载、统一信号处理等功能需要自行实现。
多 Service 场景建议始终使用 `LLBC_App`。
</div>

## 参照

- 头文件：`llbc/include/llbc/app/App.h`、`llbc/include/llbc/comm/Service.h`、`llbc/include/llbc/comm/ServiceMgr.h`
- 真实示例：
  - `tests/func_test/app/FuncTest_App_AppTest.cpp`（单 Service + 全部钩子）
  - `tests/func_test/app/FuncTest_App_AppCfgTest.cpp`（多 Service + 配置重载）
  - `tests/func_test/app/FuncTest_App_ReloadFailedTest.cpp`（重载失败处理）
  - `tests/func_test/app/FuncTest_App_AppPhaseWaitingTest.cpp`（多帧启停等待）

## 下一步

- [Service 与 Component](service-component.md) — 了解 Service 驱动模式与 Component 生命周期钩子
- [生命周期与事件](lifecycle-event.md) — Component 事件类型与 `OnEvent()` 分发机制
