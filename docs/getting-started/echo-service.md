---
layout: default
title: 第一个网络回显服务
---

# 第一个网络回显服务

承接[第一个 Service](first-service.md)，本页写一个最小的 TCP 回显服务：服务端收到消息后原样发回，客户端打印回显内容。通过这个例子你将掌握 llbc 网络开发的核心流程：**Create Service → AddComponent → Listen/Connect → Subscribe → 在处理函数里收发包**。

本例使用 `LLBC_RawProtocolFactory`，让框架以"裸字节"模式收发，所有数据以 opcode `0` 派发到订阅处理函数，无需自定义编解码器。

## 定义服务端 Component

把消息订阅放在 `OnInit`，把端口监听放在 `OnStart`；回显逻辑写在包处理函数里。

```cpp
#include "llbc.h"
using namespace llbc;

class EchoServerComp final : public LLBC_Component
{
public:
    int OnInit(bool &finished) override
    {
        // 裸协议下所有包 opcode = 0，订阅到本组件的处理方法
        GetService()->Subscribe(0, this, &EchoServerComp::OnPkt);
        return LLBC_OK;
    }

    int OnStart(bool &finished) override
    {
        int sId = GetService()->Listen("0.0.0.0", 9527);
        if (sId == 0)
        {
            LLBC_PrintLn("Listen failed: %s", LLBC_FormatLastError());
            return LLBC_FAILED;
        }
        LLBC_PrintLn("Listening on 0.0.0.0:9527 (sessionId=%d)", sId);
        return LLBC_OK;
    }

private:
    void OnPkt(LLBC_Packet &pkt)
    {
        // 取原始 payload 原样回发
        GetService()->Send(pkt.GetSessionId(), 0,
                           pkt.GetPayload(), pkt.GetPayloadLength());
    }
};
```

<div class="callout note" markdown="1">
`Subscribe` 在 `OnInit` 里调用——此时 Service 尚未对外建立连接，保证订阅先于任何包到达。`Listen` 放在 `OnStart` 里，是因为此时 Service 的 Poller 已就绪，可以正确处理新连接。
</div>

## 感知会话建立与断开

重写 `OnEvent` 可以处理非流程类事件，例如客户端连入或断开：

```cpp
void OnEvent(int eventType, const LLBC_Variant &eventParams) override
{
    if (eventType == LLBC_ComponentEventType::SessionCreate)
    {
        auto *info = eventParams.As<LLBC_SessionInfo *>();
        if (!info->IsListenSession())
            LLBC_PrintLn("Client connected: sId=%d, peer=%s",
                         info->GetSessionId(),
                         info->GetPeerAddr().ToString().c_str());
    }
    else if (eventType == LLBC_ComponentEventType::SessionDestroy)
    {
        auto *info = eventParams.As<LLBC_SessionDestroyInfo *>();
        if (!info->IsListenSession())
            LLBC_PrintLn("Client disconnected: sId=%d, reason=%s",
                         info->GetSessionId(), info->GetReason().c_str());
    }
}
```

<div class="callout note" markdown="1">
`OnEvent` 中判断 `IsListenSession()` 可过滤掉监听 Session 本身的建立/销毁通知，只处理真实客户端连接。
</div>

## 定义客户端 Component

客户端在 `OnStart` 里连接服务端，在处理函数中打印回显、再发下一条：

```cpp
class EchoClientComp final : public LLBC_Component
{
public:
    int OnInit(bool &finished) override
    {
        GetService()->Subscribe(0, this, &EchoClientComp::OnPkt);
        return LLBC_OK;
    }

    int OnStart(bool &finished) override
    {
        _sId = GetService()->Connect("127.0.0.1", 9527);
        if (_sId == 0)
        {
            LLBC_PrintLn("Connect failed: %s", LLBC_FormatLastError());
            return LLBC_FAILED;
        }
        // 发送第一条消息
        const char *hello = "Hello llbc!";
        GetService()->Send(_sId, 0, hello, strlen(hello) + 1);
        return LLBC_OK;
    }

private:
    void OnPkt(LLBC_Packet &pkt)
    {
        LLBC_PrintLn("[Echo] %s", reinterpret_cast<const char *>(pkt.GetPayload()));
        GetService()->Stop();  // 收到回显后停止
    }

    int _sId = 0;
};
```

## 创建并启动 Service

`LLBC_RawProtocolFactory` 告知框架使用裸字节协议；`AddComponent` 传入指针，框架接管其生命周期（`delete svc` 时随之销毁）：

```cpp
int main(int argc, char *argv[])
{
    LLBC_Startup();
    LLBC_Defer(LLBC_Cleanup());

    // true = 以服务端模式运行，false = 客户端
    bool asServer = (argc > 1 && LLBC_String(argv[1]) == "server");

    LLBC_Service *svc = LLBC_Service::Create("EchoSvc",
                                             new LLBC_RawProtocolFactory);
    LLBC_Defer(delete svc);

    if (asServer)
        svc->AddComponent(new EchoServerComp);
    else
        svc->AddComponent(new EchoClientComp);

    if (svc->Start() != LLBC_OK)
    {
        LLBC_PrintLn("Start service failed: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    asServer ? getchar() : svc->Wait();  // 服务端按键退出；客户端等 Stop()
    return LLBC_OK;
}
```

<div class="callout important" markdown="1">
`Send` 无论成功还是失败，框架都会负责释放传入的 `LLBC_Packet *`（若使用带 Packet 指针的重载）。使用字节重载 `Send(sessionId, opcode, buf, len)` 时框架内部分配 Packet，调用方无需管理。
</div>

## 构建与运行

使用 premake+make 路径构建并运行 `func_test`，其中已包含上述 echo 用例：

```bash
# 构建（-j4 并行加速）
make func_test config=debug64 -j4

# 先启动服务端（另一个终端）
./output/gmake2/debug64/func_test_debug
# 在菜单中选择 comm / Echo

# 再启动客户端（另一个终端，同样菜单选 comm / Echo，选 N 进入客户端模式）
./output/gmake2/debug64/func_test_debug
```

若直接编译独立 `main.cpp`，链接 `libllbc`（或 `libllbc_shared`）即可：

```bash
g++ -std=c++17 -Illbc/include main.cpp -Loutput/gmake2/debug64 -lllbc_debug -o echo
```

## 参照

- 真实可运行示例：`tests/func_test/comm/FuncTest_Comm_Echo.cpp`
- 头文件：
  - `llbc/include/llbc/comm/Service.h`
  - `llbc/include/llbc/comm/Packet.h`
  - `llbc/include/llbc/comm/Component.h`
  - `llbc/include/llbc/comm/protocol/RawProtocolFactory.h`
- 快速上手示例（可跑）：`tests/example/comm/Example_Comm_EchoService.cpp`

## 下一步

- [Service 与 Component](../concepts/service-component.md)：驱动模式、Poller 数量、消息订阅机制。
- [生命周期与事件](../concepts/lifecycle-event.md)：Component 所有钩子的调用顺序与 `OnEvent` 完整事件列表。
- [序列化 Stream](../guides/stream.md)：用 `LLBC_Stream` / `LLBC_Packet` `<<` / `>>` 取代裸字节收发，实现结构化消息。
