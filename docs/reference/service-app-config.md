---
layout: default
title: Service / App 配置项参考
---

# Service / App 配置项参考

本页汇总 `LLBC_Service` 与 `LLBC_App` 的所有可调参数——包括 Service 启动参数结构体
`LLBC_ServiceStartArgs`、驱动模式枚举 `LLBC_ServiceDriveMode`、帧率控制，以及 App 配置
文件自动发现机制与 FPS 设置。所有默认值均来自
`llbc/include/llbc/common/Config.h`（`LLBC_CFG_*` 宏）。

---

## LLBC_ServiceStartArgs：Service 启动参数

调用 `LLBC_Service::Start()` 时可传入 `LLBC_ServiceStartArgs`，也可直接使用默认值
`LLBC_ServiceStartArgs::dft`。

| 字段 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `pollerCount` | `int` | `1` | 启动的 IO Poller 线程数。增大可提升高并发场景的 IO 吞吐，但会增加线程开销；通常 1–4 已足够。 |
| `loadSampleTime` | `LLBC_TimeSpan` | `FromSeconds(0)`（禁用） | 负载采样时间窗口。`0` 表示禁用负载采样；设置正值后可通过 `GetRecentLoadInfo()` 查询帧利用率。最大值受 `LLBC_CFG_COMM_MAX_SERVICE_LOAD_SAMPLE_TIME`（3600 秒）约束。 |

```cpp
// 方式一：使用默认值（pollerCount=1，不开启负载采样）
svc->Start();

// 方式二：自定义启动参数
LLBC_ServiceStartArgs startArgs;
startArgs.pollerCount   = 2;
startArgs.loadSampleTime = LLBC_TimeSpan::FromSeconds(30);
svc->Start(startArgs);
```

<div class="callout note" markdown="1">
`pollerCount` 在 `Start()` 调用后不可修改；需先 `Stop()` 再以新参数重新 `Start()`。
</div>

---

## LLBC_ServiceDriveMode：驱动模式

| 枚举值 | 说明 |
|--------|------|
| `SelfDrive`（默认） | Service 自行创建内部线程，循环调用 `OnSvc()`，无需业务侧介入。 |
| `ExternalDrive` | Service 不创建内部线程；业务侧主循环须显式调用 `svc->OnSvc()`，适合需要把 Service 嵌入现有主循环的场景。 |

```cpp
// ExternalDrive 模式：必须在 Start() 之前设置
svc->SetDriveMode(LLBC_ServiceDriveMode::ExternalDrive);
svc->Start();

// 主循环中手动驱动
while (running)
{
    svc->OnSvc(true);   // fullFrame=true：执行完整帧处理后休眠至帧结束
}
```

<div class="callout warning" markdown="1">
`SetDriveMode()` 必须在 `Start()` 之前调用，Service 运行期间不可更改。
</div>

---

## Service FPS 控制

| 项目 | 值 |
|------|-----|
| 默认 FPS | `LLBC_CFG_COMM_DFT_SERVICE_FPS` = **200** |
| 最小 FPS | `LLBC_CFG_COMM_MIN_SERVICE_FPS` = **1** |
| 最大 FPS | `LLBC_CFG_COMM_MAX_SERVICE_FPS` = **1000** |
| 无限制（不限帧） | `LLBC_INFINITE` |

```cpp
// 设置 FPS（可在运行期间动态调整）
svc->SetFPS(60);

// 不限帧（CPU 全速跑，适合性能压测）
svc->SetFPS(LLBC_INFINITE);

// 查询当前帧间隔（毫秒），LLBC_INFINITE 时返回 0
int frameMs = svc->GetFrameInterval();
```

帧间隔 = `1000 / fps`（毫秒，整数截断）；`LLBC_INFINITE` 时帧间隔为 `0`，表示不主动休眠。

---

## Service 负载采样：GetRecentLoadInfo

开启 `loadSampleTime > 0` 后，可在任意线程查询最近一段时间的帧利用率：

```cpp
LLBC_ServiceRecentLoadInfo info;
if (svc->GetRecentLoadInfo(LLBC_TimeSpan::FromSeconds(10), info) == LLBC_OK)
{
    // info.recentTime   — 实际覆盖的采样时长
    // info.workingTime  — 区间内累计工作时长
    // info.updateTimes  — OnUpdate() 调用次数
    // info.overloadTimes — 超帧次数
}
```

`GetRecentLoadInfo()` 的查询窗口不得超过 `startArgs.loadSampleTime`；否则返回 `LLBC_FAILED`。

---

## LLBC_App FPS 控制

`LLBC_App` 有独立的帧率控制，作用于 `OnUpdate()` 主循环节奏，与挂载的 Service FPS 互相独立。

| 项目 | 值 |
|------|-----|
| 默认 FPS | `LLBC_CFG_APP_DFT_FPS` = **200** |
| 最小 FPS | `LLBC_CFG_APP_MIN_FPS` = **1** |
| 最大 FPS | `LLBC_CFG_APP_MAX_FPS` = **1000** |
| 无限制 | `LLBC_INFINITE` |

```cpp
class MyApp final : public LLBC_App
{
    int OnStart(int argc, char *argv[], bool &startFinished) override
    {
        SetFPS(30);   // App 主循环 30 帧/秒
        // ...
        return LLBC_OK;
    }
    void OnStop(bool &stopFinished) override { stopFinished = true; }
};
```

`GetFrameInterval()` 同样在 `LLBC_INFINITE` 时返回 `0`（不限帧）。

---

## LLBC_App 配置文件自动发现

App 启动时依次在 **当前工作目录**、**可执行文件所在目录**（以及两者的 Config/Conf/Cfg 子目录与上级目录）中搜索同名配置文件，后缀探测优先级如下：

| 优先级 | 后缀 | 对应 `LLBC_AppConfigType` |
|--------|------|--------------------------|
| 1 | `.ini` | `Ini` |
| 2 | `.xml` | `Xml` |
| 3 | `.cfg` / `.properties` | `Property` |

找到后自动加载，可通过以下 API 读取：

```cpp
// 检查是否找到配置
if (app.HasConfig())
{
    auto cfgType = app.GetConfigType();   // Ini / Xml / Property
    auto cfgPath = app.GetConfigPath();
    LLBC_Variant cfg = app.GetConfig();   // 线程安全快照
}
```

如需指定路径（绕过自动发现），在 `OnEarlyStart()` 中调用：

```cpp
int OnEarlyStart(int argc, char *argv[], bool &finished) override
{
    SetConfigPath("/etc/myapp/config.cfg");
    return LLBC_OK;
}
```

<div class="callout note" markdown="1">
`GetConfig()` 返回 `LLBC_Variant` 快照，线程安全；`GetConfigUnsafe()` 返回内部引用，
只能在 App 主线程（`OnStart`/`OnUpdate`/`OnReload`）中使用。
</div>

---

## LLBC_App 配置热重载

| API | 说明 |
|-----|------|
| `Reload()` | 主动触发配置重载，重新读取配置文件并调用各 Service 组件的 `OnReload()`。 |
| `PreventReload()` | 暂时阻止重载（计数器 +1）。 |
| `CancelPreventReload()` | 解除一次阻止（计数器 -1），计数回零时允许重载。 |

Linux/macOS 上也可通过 `SIGUSR2` 信号触发重载（`LLBC_CFG_APP_RELOAD_SIGNALS`）；
停止信号为 `SIGINT`/`SIGTERM`/`SIGQUIT`（`LLBC_CFG_APP_STOP_SIGNALS`）。

---

## 综合示例：自定义启动参数与 FPS

```cpp
// 创建 Service：2 个 Poller 线程，启用 30 秒负载采样，60 FPS
auto svc = LLBC_Service::Create("GameSvc");
svc->AddComponent<MyGameComp>();
svc->SetFPS(60);

LLBC_ServiceStartArgs args;
args.pollerCount    = 2;
args.loadSampleTime = LLBC_TimeSpan::FromSeconds(30);
svc->Start(args);
```

---

## 参照

- 头文件：`llbc/include/llbc/comm/Service.h`（`LLBC_ServiceStartArgs`、`LLBC_ServiceDriveMode`、`LLBC_Service`）
- 内联实现：`llbc/include/llbc/comm/ServiceInl.h`（`LLBC_ServiceStartArgs` 默认构造函数）
- 头文件：`llbc/include/llbc/app/App.h`（`LLBC_App`、`LLBC_AppConfigType`）
- 框架默认常量：`llbc/include/llbc/common/Config.h`（`LLBC_CFG_COMM_*` / `LLBC_CFG_APP_*` 宏）
- 负载采样结构：`llbc/include/llbc/comm/ServiceLoadSampler.h`（`LLBC_ServiceRecentLoadInfo`）
- 真实用例：`tests/func_test/comm/FuncTest_Comm_SvcStartStop.cpp`（`pollerCount` 修改）
- 真实用例：`tests/func_test/comm/FuncTest_Comm_SvcFps.cpp`（`LLBC_INFINITE` FPS）
- 真实用例：`tests/func_test/comm/FuncTest_Comm_ExternalDriveSvc.cpp`（ExternalDrive 模式）
- 真实用例：`tests/func_test/comm/FuncTest_Comm_RecentLoadInfo.cpp`（负载采样）
- 真实用例：`tests/func_test/app/FuncTest_App_AppCfgTest.cpp`（App 配置与重载）

## 下一步

- [日志配置项参考](log-config.md)
- [错误码参考](error-codes.md)
