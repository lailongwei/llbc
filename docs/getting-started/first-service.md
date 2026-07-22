---
layout: default
title: 第一个 Service
---

# 第一个 Service

[Hello World](hello-world.md) 里我们只做了库的启停。这一步更进一步：创建一个 `LLBC_Service`，
定义一个 `LLBC_Component` 并重写它的生命周期钩子，把 Component 挂到 Service 上，启动、运行几秒、
再停止清理。跑通它，你就掌握了 llbc 的 **Service + Component** 基本工作流。

## 完整代码

```cpp
#include "llbc.h"
using namespace llbc;

// 1. 定义一个 Component，重写几个真实的生命周期钩子
class GreetComp : public LLBC_Component
{
public:
    int OnInit(bool &initFinished) override
    {
        LLBC_PrintLn("GreetComp: OnInit");
        return LLBC_OK;   // 返回 LLBC_FAILED 会中断启动流程
    }

    int OnStart(bool &startFinished) override
    {
        LLBC_PrintLn("GreetComp: OnStart");
        return LLBC_OK;
    }

    void OnUpdate() override
    {
        // 每帧调用一次；这里只是演示，实际逻辑写在这里
        LLBC_PrintLn("GreetComp: OnUpdate, fps=%d", GetService()->GetFPS());
    }

    void OnStop(bool &stopFinished) override
    {
        LLBC_PrintLn("GreetComp: OnStop");
    }
};

int main(int argc, char *argv[])
{
    // 2. 初始化 llbc
    LLBC_Startup();
    LLBC_Defer(LLBC_Cleanup());

    // 3. 创建 Service
    LLBC_Service *svc = LLBC_Service::Create("FirstService");
    LLBC_Defer(delete svc);      // Create 出来的对象需手动释放

    // 4. 挂载 Component 并设置帧率
    svc->AddComponent<GreetComp>();
    svc->SetFPS(1);              // 1 帧/秒，方便观察 OnUpdate 输出

    // 5. 启动（默认 SelfDrive：Service 内部起线程逐帧驱动）
    if (svc->Start() != LLBC_OK)
    {
        LLBC_PrintLn("Start service failed: %s", LLBC_FormatLastError());
        return -1;
    }

    // 6. 主线程运行几秒，让 Service 自行逐帧
    LLBC_ThreadMgr::Sleep(3000);

    // 7. 停止（离开作用域时 LLBC_Defer 会 delete svc，再 LLBC_Cleanup）
    svc->Stop();
    return 0;
}
```

运行输出（大致，`OnUpdate` 每秒一行）：

```
GreetComp: OnInit
GreetComp: OnStart
GreetComp: OnUpdate, fps=1
GreetComp: OnUpdate, fps=1
GreetComp: OnUpdate, fps=1
GreetComp: OnStop
```

## 逐步说明

1. **定义 Component**：继承 `LLBC_Component`，重写需要的钩子。这里用了 `OnInit`（初始化）、
   `OnStart`（启动）、`OnUpdate`（每帧）、`OnStop`（停止）。返回 `int` 的钩子要返回
   `LLBC_OK`；`bool &` 出参用于分帧完成（本例一帧内即完成，无需改动）。完整钩子列表与调用顺序见
   [生命周期与事件](../concepts/lifecycle-event.md)。
2. **`LLBC_Startup()` / `LLBC_Cleanup()`**：与 [Hello World](hello-world.md) 一致，进程边界成对调用。
3. **`LLBC_Service::Create("FirstService")`**：创建一个名为 `FirstService` 的 Service，返回堆对象。
4. **`AddComponent<GreetComp>()`**：模板重载，框架内部 `new` 出实例并托管其生命周期。
5. **`Start()`**：默认 `SelfDrive` 模式，Service 内部起线程按 FPS 逐帧驱动，会依次回调 Component 的
   `OnInit` → `OnStart` → 每帧 `OnUpdate`。返回非 `LLBC_OK` 时用 `LLBC_FormatLastError()` 看原因。
6. **让它跑一会儿**：主线程 `Sleep`，Service 的工作线程在后台逐帧运行。
7. **`Stop()` 与释放**：`Stop()` 停止 Service（会回调 `OnStop`）；`LLBC_Defer(delete svc)`
   保证离开作用域时释放 Service，随后 `LLBC_Cleanup()` 清理库。

<div class="callout note" markdown="1">
本例用 `SelfDrive`（默认）模式。若想把 Service 融进已有主循环，可
`svc->SetDriveMode(LLBC_ServiceDriveMode::ExternalDrive)`，再由你的循环手动调用 `svc->OnSvc()`。
详见 [Service 与 Component](../concepts/service-component.md)。
</div>

## 加一点网络

Service 的价值在于网络。要收发消息，在 `Start()` 前监听端口，并把某个 opcode 订阅到 Component 的
处理方法：

```cpp
GreetComp *comp = new GreetComp;
svc->AddComponent(comp);
svc->Subscribe(1, comp, &GreetComp::OnRecvData);  // opcode=1 → OnRecvData
svc->Start();
svc->Listen("127.0.0.1", 7788);                   // 监听；返回 0 表示失败
```

其中 `OnRecvData` 的签名为 `void(LLBC_Packet &)`。完整的收发 / 编解码示例见下方参照文件。

## 构建与运行

本页代码可作为一个独立 `main` 编译（链接核心库 `libllbc`）。若想直接体验仓库里现成的 Service 示例，
构建交互式的 `func_test`：

```bash
make func_test config=debug64
./output/gmake2/debug64/func_test_debug
```

在菜单中选择通信（comm）相关用例即可看到 Service + Component 的实际运行。

## 参照

- 完整可运行示例：`tests/func_test/comm/FuncTest_Comm_SvcBase.cpp`（Service + Component + 编解码 + 收发）
- 头文件：`llbc/include/llbc/comm/Service.h`、`llbc/include/llbc/comm/Component.h`

## 下一步

- [Service 与 Component](../concepts/service-component.md)：Service 创建、驱动模式、Poller、消息订阅。
- [生命周期与事件](../concepts/lifecycle-event.md)：Component 钩子调用顺序、`OnEvent()`、`OnReload()`。
- [架构总览](../concepts/architecture.md)：模块划分与 App→Service→Component 全景。
