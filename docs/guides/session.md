---
layout: default
title: Session 与连接管理
---

# Session 与连接管理

**Session** 是 llbc 网络层的基本连接单元。每个 TCP 连接（无论监听端口接入的还是主动发起的）
在框架内都对应一个整数 **session Id**。业务代码通过该 Id 发包、关闭连接，
不直接持有 socket 句柄。

- `Listen` / `Connect` 立即返回 Id；
- `AsyncConn` 立即返回一个*预分配* Id（此时连接尚未完成），结果通过 `OnEvent` 异步通知。
- 会话建立、销毁、异步连接结果均通过 `LLBC_Component::OnEvent` 投递事件，
  组件无需轮询，也不需要额外订阅。

## 监听端口

在 `OnStart` 中调用 `Listen`；成功返回非零 session Id（监听 session），
失败返回 0，用 `LLBC_FormatLastError()` 取原因。

```cpp
int OnStart(bool &startFinished) override
{
    LLBC_Service *svc = GetService();
    int listenSid = svc->Listen("0.0.0.0", 9527);
    if (listenSid == 0)
    {
        LLBC_PrintLn("Listen failed: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Listening on 0.0.0.0:9527, sid=%d", listenSid);
    return LLBC_OK;
}
```

## 同步连接

`Connect` 阻塞直至连接成功或失败，返回 session Id（0 = 失败）。
适合服务启动阶段的初始连接。

```cpp
int sid = svc->Connect("127.0.0.1", 9527);
if (sid == 0)
    LLBC_PrintLn("Connect failed: %s", LLBC_FormatLastError());
else
    LLBC_PrintLn("Connected, sid=%d", sid);
```

## 异步连接

`AsyncConn` 立即返回，框架在后台完成 TCP 握手后通过
`LLBC_ComponentEventType::AsyncConnResult` 事件通知结果。

```cpp
// 发起异步连接，保存预分配 Id 可用于提前取消（或仅忽略）
int pendingSid = svc->AsyncConn("127.0.0.1", 9527);
if (pendingSid == 0)
    LLBC_PrintLn("AsyncConn submit failed: %s", LLBC_FormatLastError());
```

## 会话选项 LLBC_SessionOpts

`LLBC_SessionOpts` 控制 TCP 选项和缓冲区大小，三个 `Listen`/`Connect`/`AsyncConn` 均接受此参数，默认使用 `LLBC_DftSessionOpts`（noDelay=true，缓冲区取配置默认值）。

```cpp
LLBC_SessionOpts opts;
opts.SetNoDelay(true);           // TCP_NODELAY，默认已开启
opts.SetSockSendBufSize(256*1024); // OS 层 socket 发送缓冲（0=OS 默认）
opts.SetSockRecvBufSize(256*1024); // OS 层 socket 接收缓冲
opts.SetSessionSendBufSize(512*1024); // llbc 层应用发送队列大小
opts.SetSessionRecvBufSize(128*1024); // llbc 层应用接收缓冲初始大小
opts.SetMaxPacketSize(4*1024*1024);   // 单包最大字节数（超限断连）

int sid = svc->Listen("0.0.0.0", 9527, nullptr, opts);
```

<div class="callout note" markdown="1">
`sockSendBufSize` / `sockRecvBufSize` 设为 0 表示沿用操作系统默认值。
Linux 2.4+ 和 Windows 7+ 支持动态调整，实际生效大小由内核决定。
</div>

## 主动断开

调用 `RemoveSession` 关闭指定会话；`reason` 可空，会原样出现在
`LLBC_SessionDestroyInfo::GetReason()` 中，便于日志定位。

```cpp
// 断开并附带原因字符串
svc->RemoveSession(sid, "kicked by server");

// 检查 session 是否仍有效
if (svc->IsSessionValidate(sid))
    svc->RemoveSession(sid);
```

## 在 OnEvent 中处理会话事件

llbc 将会话生命周期事件统一投递到 `LLBC_Component::OnEvent`，
事件类型来自 `LLBC_ComponentEventType`。

```cpp
void OnEvent(int eventType, const LLBC_Variant &eventParams) override
{
    switch (eventType)
    {
        case LLBC_ComponentEventType::SessionCreate:
        {
            auto &info = *eventParams.As<LLBC_SessionInfo *>();
            if (!info.IsListenSession())
                LLBC_PrintLn("New conn sid=%d peer=%s",
                    info.GetSessionId(), info.GetPeerAddr().ToString().c_str());
            break;
        }
        case LLBC_ComponentEventType::SessionDestroy:
        {
            auto &d = *eventParams.As<LLBC_SessionDestroyInfo *>();
            LLBC_PrintLn("Session %d closed, reason: %s",
                d.GetSessionId(), d.GetReason().c_str());
            break;
        }
        case LLBC_ComponentEventType::AsyncConnResult:
        {
            auto &r = *eventParams.As<LLBC_AsyncConnResult *>();
            if (r.IsConnected())
                LLBC_PrintLn("AsyncConn ok, sid=%d", r.GetSessionId());
            else
                LLBC_PrintLn("AsyncConn fail: %s", r.GetReason().c_str());
            break;
        }
        default:
            break;
    }
}
```

### LLBC_SessionInfo 字段速查

| 方法 | 说明 |
|------|------|
| `IsListenSession()` | 是否是监听 session（被动端口，非数据连接）|
| `GetSessionId()` | 当前 session Id |
| `GetAcceptSessionId()` | 接受该连接的监听 session Id（主动连接侧为 0）|
| `GetLocalAddr()` | 本端地址 |
| `GetPeerAddr()` | 对端地址 |

### LLBC_SessionDestroyInfo 字段速查

| 方法 | 说明 |
|------|------|
| `GetSessionInfo()` | 销毁 session 的 `LLBC_SessionInfo` 引用 |
| `IsDestroyedFromService()` | `true` = 业务主动断开；`false` = 网络错误 |
| `GetErrno()` / `GetSubErrno()` | 网络层错误码（仅 `IsDestroyedFromService()==false` 时有效）|
| `GetReason()` | 原因字符串（主动断开时为传入的 `reason`，网络错误时为系统错误描述）|

<div class="callout warning" markdown="1">
监听 session（`IsListenSession()==true`）销毁意味着端口已关闭，
所有从该端口接入的子 session **不会**被自动关闭，需业务自行追踪并调用 `RemoveSession`。
</div>

<div class="callout note" markdown="1">
**线程安全**：`Listen` / `Connect` / `AsyncConn` / `RemoveSession` 均可在服务线程外调用；
框架内部通过线程安全队列将操作派发到服务线程。服务启动前调用会被记录为 pending 操作，
服务启动后自动执行，不会报错，但也不会立即返回错误。
</div>

## 参照

- 头文件：
  - `llbc/include/llbc/comm/Session.h` / `SessionInl.h`
  - `llbc/include/llbc/comm/SessionOpts.h` / `SessionOptsInl.h`
  - `llbc/include/llbc/comm/Component.h`（`LLBC_SessionInfo`、`LLBC_SessionDestroyInfo`、`LLBC_AsyncConnResult`、`LLBC_ComponentEventType`）
  - `llbc/include/llbc/comm/Service.h`（`Listen` / `Connect` / `AsyncConn` / `RemoveSession`）
- 真实用例：
  - `tests/func_test/comm/FuncTest_Comm_Echo.cpp`
  - `tests/func_test/comm/FuncTest_Comm_SvcBase.cpp`

## 下一步

- [Component 生命周期](../concepts/lifecycle-event.md) — `OnEvent` 与其他钩子的完整执行顺序
- [协议栈与 Packet](protocol.md) — 在 session 上收发结构化数据
- [Service 概览](../concepts/service-component.md) — 多 service 拓扑与 session 归属
