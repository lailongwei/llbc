---
layout: default
title: Packet 与 Coder 编解码
---

# Packet 与 Coder 编解码

`LLBC_Packet` 是 llbc 网络层的消息载体，封装了包头元数据（sessionId、opcode、status、flags）
以及二进制 payload；`LLBC_Coder` / `LLBC_CoderFactory` 提供业务数据对象的序列化/反序列化接口，
让业务层无需手写 `packet <<` / `packet >>` 的分散代码；`LLBC_PacketHeaderAssembler`
是协议层内部用于处理粘包/拆包的包头缓冲组装器，自定义协议层实现时会用到。

## 包头字段与 SetHeader

每个 `LLBC_Packet` 携带以下元数据：

| 字段 | 类型 | 说明 |
|------|------|------|
| `sessionId` | `int` | 所属会话 ID |
| `opcode` | `int` | 消息操作码 |
| `status` | `int` | 状态码（默认 0） |
| `flags` | `uint32` | 标志位（默认 0） |
| `length` | `size_t` | 含包头的完整包长 |
| `extData1/2/3` | `sint64` | 扩展数据槽 |

`SetHeader` 是最常用的便捷方法，一次性设置发包时最常需要的四个字段：

```cpp
// SetHeader(int sessionId, int opcode, int status = 0, uint32 flags = 0u)
LLBC_Packet *pkt = svc->GetThreadSafeObjPool().Acquire<LLBC_Packet>();
pkt->SetHeader(sessionId, /*opcode=*/1, /*status=*/0);
```

## Payload 读写

payload 是 `LLBC_MessageBlock`；对外暴露 `<<` / `>>` 运算符，与 `LLBC_Stream` 语义相同：

```cpp
// 发送方：写入 payload
LLBC_Packet *pkt = svc->GetThreadSafeObjPool().Acquire<LLBC_Packet>();
pkt->SetHeader(sessionId, opcode);
*pkt << playerId << playerName << level;   // operator<<
svc->Send(pkt);

// 接收方：在消息处理函数中读出
void OnMsg(LLBC_Packet &pkt)
{
    uint64      playerId;
    LLBC_String playerName;
    int         level;
    pkt >> playerId >> playerName >> level; // operator>>
}
```

`Read(T &val)` / `Write(T val)` 是等价的显式调用形式，返回 `LLBC_OK`(0) 或 `LLBC_FAILED`(-1)。

<div class="callout note" markdown="1">
`LLBC_Packet` 的 payload 只有一个读游标，按顺序消费；无法像 `LLBC_Stream` 那样随机定位读游标。
写入后不要重置 payload 再读，否则会读到脏数据。
</div>

## LLBC_Coder —— 业务数据编解码接口

`LLBC_Coder` 将一个业务数据对象绑定到 packet，由框架在发送/接收时自动调用 `Encode` / `Decode`，
避免在多处手工序列化。继承时同时重写两个纯虚函数：

```cpp
struct TestData final : public LLBC_Coder
{
    int         iVal;
    LLBC_String strVal;

    bool Encode(LLBC_Packet &packet) override
    {
        packet << iVal << strVal;
        return true;   // 返回 false 表示编码失败，框架会丢弃该包
    }

    bool Decode(LLBC_Packet &packet) override
    {
        packet >> iVal >> strVal;
        return true;
    }
};
```

<div class="callout note" markdown="1">
`LLBC_Coder` 继承自 `LLBC_PoolObj`，支持对象池复用。框架在收包时由
`LLBC_CoderFactory::Create()` 分配实例，并在处理完毕后释放，业务代码无需手动 delete。
</div>

## LLBC_CoderFactory —— 工厂与注册

框架在收到对应 opcode 的包时，通过 `CoderFactory` 创建 `LLBC_Coder` 实例并赋给 packet。
每个 opcode 注册一个工厂：

```cpp
class TestDataFactory final : public LLBC_CoderFactory
{
public:
    LLBC_Coder *Create() const override { return new TestData; }
};

// 方式一：传入工厂实例指针（所有权转移给 Service）
svc->AddCoderFactory(/*opcode=*/1, new TestDataFactory);

// 方式二：模板形式（框架内部 new，等价）
svc->AddCoderFactory<TestDataFactory>(/*opcode=*/1);
```

注册后，`Subscribe` 绑定的处理函数可以直接通过 `GetDecoder<T>()` 取到已解码的对象：

```cpp
void OnRecvData(LLBC_Packet &packet)
{
    TestData *data = packet.GetDecoder<TestData>();
    LLBC_PrintLn("iVal=%d strVal=%s", data->iVal, data->strVal.c_str());
}
```

## 发包时手动挂载 Encoder

不需要 `CoderFactory` 时，也可以在构造发包逻辑时直接把 Coder 对象设为 encoder：

```cpp
TestData *encoder = new TestData;
encoder->iVal   = 42;
encoder->strVal = "Hello, llbc";

LLBC_Packet *pkt = svc->GetThreadSafeObjPool().Acquire<LLBC_Packet>();
pkt->SetHeader(sessionId, /*opcode=*/1, 0);
pkt->SetEncoder(encoder);  // 框架 Send() 时调用 encoder->Encode(*pkt)
svc->Send(pkt);
```

框架发包完成后自动 delete encoder；若需要提前取回所有权，调用 `GiveUpEncoder()`。

## LLBC_PacketHeaderAssembler —— 包头缓冲组装

`LLBC_PacketHeaderAssembler` 是 `PacketProtocol` 内部使用的包头流式缓冲器，
负责将 TCP 流中的字节逐步填入固定大小的包头缓冲区，并在头完整后将字段写入 `LLBC_Packet`。
**自定义协议层**时才需要直接使用它；常规业务开发不涉及此类。

llbc 默认包头布局（共 20 字节）：

```
[uint32 length][sint32 opcode][sint16 status][uint16 flags][sint64 extData1]
```

典型用法（自定义 Protocol 内部）：

```cpp
// 构造时指定包头字节数（默认协议头为 20 字节）
LLBC_PacketHeaderAssembler _headerAssembler{20};

// 每次收到数据时调用
size_t used = 0;
bool complete = _headerAssembler.Assemble(data, dataLen, used);
// complete == true 时包头已接收完整
if (complete)
{
    LLBC_Packet *pkt = /* 分配 packet */;
    _headerAssembler.SetToPacket(*pkt);  // 将包头字段写入 packet
    _headerAssembler.Reset();            // 重置以接收下一个包头
}
```

<div class="callout important" markdown="1">
`Assemble` 返回 `true` 且 `used` 等于 `headerLen` 时才表示包头完整，此时才可以调用
`SetToPacket`。`Reset()` 必须在每个包头处理完毕后调用，否则下个包的头会追加到旧缓冲区。
</div>

## 参照

- 头文件：`llbc/include/llbc/comm/Packet.h` / `PacketInl.h`
- 头文件：`llbc/include/llbc/comm/Coder.h`
- 头文件：`llbc/include/llbc/comm/PacketHeaderAssembler.h`
- 功能测试（Coder 完整示例）：`tests/func_test/comm/FuncTest_Comm_SvcBase.cpp`
- 功能测试（Packet 读写）：`tests/func_test/comm/FuncTest_Comm_PacketOp.cpp`
- 快速上手示例（可跑）：`tests/example/comm/Example_Comm_PacketCoder.cpp`

## 下一步

- 序列化基础：[序列化 Stream](stream.md)
- 服务与组件：[Service 与 Component](../concepts/service-component.md)（若已存在）
