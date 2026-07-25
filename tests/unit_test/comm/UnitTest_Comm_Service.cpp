// The MIT License (MIT)
//
// Copyright (c) 2013 lailongwei<lailongwei@126.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <llbc.h>
using namespace llbc;

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/comm/Service.cpp

namespace
{

constexpr int RequestOpcode = 7001;
constexpr int ResponseOpcode = 7002;

struct LoopbackState
{
    std::mutex lock;
    std::condition_variable received;
    int serverReceiveCount = 0;
    int clientReceiveCount = 0;
    int serverSendResult = LLBC_FAILED;
    int clientConnectedSessionId = 0;
    int clientDestroyedSessionId = 0;
    int asyncResultSessionId = 0;
    bool clientDestroyFromService = false;
    bool asyncConnected = false;
    std::string serverPayload;
    std::string clientPayload;
    std::string clientDestroyReason;
    std::string asyncReason;
    std::string asyncPeerIp;
    uint16 asyncPeerPort = 0;
};

class PassThroughCoder final : public LLBC_Coder
{
public:
    bool Encode(LLBC_Packet &packet) override
    {
        return true;
    }

    bool Decode(LLBC_Packet &packet) override
    {
        return true;
    }
};

class PassThroughCoderFactory final : public LLBC_CoderFactory
{
public:
    LLBC_Coder *Create() const override
    {
        return new PassThroughCoder;
    }
};

class EchoServerComponent final : public LLBC_Component
{
public:
    explicit EchoServerComponent(LoopbackState &state)
    : _state(state)
    {
    }

    int OnInit(bool &initFinished) override
    {
        LLBC_Service *service = GetService();
        auto *coderFactory = new PassThroughCoderFactory;
        if (service->AddCoderFactory(RequestOpcode, coderFactory) != LLBC_OK)
        {
            delete coderFactory;
            return LLBC_FAILED;
        }

        return service->Subscribe(RequestOpcode, this, &EchoServerComponent::OnRequest);
    }

private:
    void OnRequest(LLBC_Packet &packet)
    {
        const auto *payload = static_cast<const char *>(packet.GetPayload());
        const size_t payloadLength = packet.GetPayloadLength();
        const int sendResult = GetService()->Send(
            packet.GetSessionId(), ResponseOpcode, payload, payloadLength);

        {
            std::lock_guard<std::mutex> guard(_state.lock);
            ++_state.serverReceiveCount;
            _state.serverSendResult = sendResult;
            _state.serverPayload.assign(payload, payloadLength);
        }
        _state.received.notify_all();
    }

    LoopbackState &_state;
};

class EchoClientComponent final : public LLBC_Component
{
public:
    explicit EchoClientComponent(LoopbackState &state)
    : _state(state)
    {
    }

    int OnInit(bool &initFinished) override
    {
        LLBC_Service *service = GetService();
        auto *coderFactory = new PassThroughCoderFactory;
        if (service->AddCoderFactory(ResponseOpcode, coderFactory) != LLBC_OK)
        {
            delete coderFactory;
            return LLBC_FAILED;
        }

        return service->Subscribe(ResponseOpcode, this, &EchoClientComponent::OnResponse);
    }

    void OnEvent(int eventType, const LLBC_Variant &eventParams) override
    {
        if (eventType == LLBC_ComponentEventType::AsyncConnResult)
        {
            const LLBC_AsyncConnResult &result = *eventParams.As<LLBC_AsyncConnResult *>();
            {
                std::lock_guard<std::mutex> guard(_state.lock);
                _state.asyncResultSessionId = result.GetSessionId();
                _state.asyncConnected = result.IsConnected();
                _state.asyncReason = result.GetReason().c_str();
                _state.asyncPeerIp = result.GetPeerAddr().GetIpAsString().c_str();
                _state.asyncPeerPort = result.GetPeerAddr().GetPort();
            }
            _state.received.notify_all();
            return;
        }

        if (eventType == LLBC_ComponentEventType::SessionDestroy)
        {
            const LLBC_SessionDestroyInfo &destroyInfo =
                *eventParams.As<LLBC_SessionDestroyInfo *>();
            if (destroyInfo.IsListenSession())
                return;

            {
                std::lock_guard<std::mutex> guard(_state.lock);
                _state.clientDestroyedSessionId = destroyInfo.GetSessionId();
                _state.clientDestroyFromService = destroyInfo.IsDestroyedFromService();
                _state.clientDestroyReason = destroyInfo.GetReason().c_str();
            }
            _state.received.notify_all();
            return;
        }

        if (eventType != LLBC_ComponentEventType::SessionCreate)
            return;

        const LLBC_SessionInfo &sessionInfo = *eventParams.As<LLBC_SessionInfo *>();
        if (sessionInfo.IsListenSession())
            return;

        {
            std::lock_guard<std::mutex> guard(_state.lock);
            _state.clientConnectedSessionId = sessionInfo.GetSessionId();
        }
        _state.received.notify_all();
    }

private:
    void OnResponse(LLBC_Packet &packet)
    {
        const auto *payload = static_cast<const char *>(packet.GetPayload());
        const size_t payloadLength = packet.GetPayloadLength();

        {
            std::lock_guard<std::mutex> guard(_state.lock);
            ++_state.clientReceiveCount;
            _state.clientPayload.assign(payload, payloadLength);
        }
        _state.received.notify_all();
    }

    LoopbackState &_state;
};

class ServiceGuard
{
public:
    explicit ServiceGuard(LLBC_Service *service)
    : _service(service)
    {
    }

    ~ServiceGuard()
    {
        if (!_service)
            return;

        if (_service->IsStarted())
            _service->Stop(true);
        delete _service;
    }

    LLBC_Service *Get() const
    {
        return _service;
    }

private:
    LLBC_Service *_service;
};

LLBC_Service *StartServerOnAvailablePort(LoopbackState &state,
                                         uint16 &port,
                                         std::string &lastFailure)
{
    const uint16 firstPort = static_cast<uint16>(35000 + LLBC_GetCurrentProcessId() % 1000);
    for (uint16 offset = 0; offset < 64; ++offset)
    {
        const uint16 candidatePort = static_cast<uint16>(firstPort + offset);
        const LLBC_String serviceName = LLBC_String().format(
            "UnitTestCommLoopbackServer-%d-%u", LLBC_GetCurrentProcessId(), offset);
        LLBC_Service *service = LLBC_Service::Create(serviceName);
        if (!service)
            return nullptr;

        if (service->AddComponent(new EchoServerComponent(state)) != LLBC_OK)
        {
            lastFailure = LLBC_FormatLastError();
            delete service;
            continue;
        }

        if (service->Listen("127.0.0.1", candidatePort) == 0)
        {
            lastFailure = LLBC_FormatLastError();
            delete service;
            continue;
        }

        if (service->Start() != LLBC_OK)
        {
            lastFailure = LLBC_FormatLastError();
            delete service;
            continue;
        }

        port = candidatePort;
        return service;
    }

    return nullptr;
}

} // namespace

// A normal-protocol service pair must establish a local session, preserve packet
// headers across the wire, dispatch handlers, and release both service threads.
TEST(ServiceTest, ExchangesPacketOverLoopbackAndStopsCleanly)
{
    LoopbackState state;
    uint16 port = 0;
    std::string serverStartFailure;
    ServiceGuard server(StartServerOnAvailablePort(state, port, serverStartFailure));
    ASSERT_NE(server.Get(), nullptr) << serverStartFailure;
    ASSERT_NE(port, 0u);

    const LLBC_String clientName = LLBC_String().format(
        "UnitTestCommLoopbackClient-%d", LLBC_GetCurrentProcessId());
    ServiceGuard client(LLBC_Service::Create(clientName));
    ASSERT_NE(client.Get(), nullptr);
    ASSERT_EQ(client.Get()->AddComponent(new EchoClientComponent(state)), LLBC_OK);
    ASSERT_EQ(client.Get()->Start(), LLBC_OK);

    const int sessionId = client.Get()->Connect("127.0.0.1", port);
    ASSERT_NE(sessionId, 0);

    std::unique_lock<std::mutex> lock(state.lock);
    ASSERT_TRUE(state.received.wait_for(lock, std::chrono::seconds(3), [&state, sessionId]
    {
        return state.clientConnectedSessionId == sessionId;
    }));
    lock.unlock();

    const std::string request = "loopback-packet";
    ASSERT_EQ(client.Get()->Send(sessionId, RequestOpcode, request.data(), request.size()), LLBC_OK);

    lock.lock();
    const bool receivedResponse = state.received.wait_for(lock, std::chrono::seconds(3), [&state]
    {
        return state.serverReceiveCount == 1 && state.clientReceiveCount == 1;
    });
    ASSERT_TRUE(receivedResponse)
        << "server receives: " << state.serverReceiveCount
        << ", client receives: " << state.clientReceiveCount
        << ", server send result: " << state.serverSendResult;
    EXPECT_EQ(state.serverPayload, request);
    EXPECT_EQ(state.clientPayload, request);
    EXPECT_EQ(state.serverSendResult, LLBC_OK);
    lock.unlock();

    ASSERT_EQ(client.Get()->RemoveSession(sessionId, "unit-test-close"), LLBC_OK);
    EXPECT_FALSE(client.Get()->IsSessionValidate(sessionId));

    lock.lock();
    ASSERT_TRUE(state.received.wait_for(lock, std::chrono::seconds(3), [&state, sessionId]
    {
        return state.clientDestroyedSessionId == sessionId;
    }));
    EXPECT_TRUE(state.clientDestroyFromService);
    EXPECT_EQ(state.clientDestroyReason, "unit-test-close");
    lock.unlock();

    EXPECT_EQ(client.Get()->Stop(true), LLBC_OK);
    EXPECT_EQ(server.Get()->Stop(true), LLBC_OK);
}

// AsyncConn returns a pending id first. A successful result and SessionCreate
// event together establish the point at which callers may send application data.
TEST(ServiceTest, AsyncConnectReportsCompletionBeforePacketExchange)
{
    LoopbackState state;
    uint16 port = 0;
    std::string serverStartFailure;
    ServiceGuard server(StartServerOnAvailablePort(state, port, serverStartFailure));
    ASSERT_NE(server.Get(), nullptr) << serverStartFailure;

    const LLBC_String clientName = LLBC_String().format(
        "UnitTestCommAsyncClient-%d", LLBC_GetCurrentProcessId());
    ServiceGuard client(LLBC_Service::Create(clientName));
    ASSERT_NE(client.Get(), nullptr);
    ASSERT_EQ(client.Get()->AddComponent(new EchoClientComponent(state)), LLBC_OK);
    ASSERT_EQ(client.Get()->Start(), LLBC_OK);

    const int sessionId = client.Get()->AsyncConn("127.0.0.1", port);
    ASSERT_NE(sessionId, 0);

    std::unique_lock<std::mutex> lock(state.lock);
    ASSERT_TRUE(state.received.wait_for(lock, std::chrono::seconds(3), [&state, sessionId]
    {
        return state.asyncResultSessionId == sessionId &&
            state.clientConnectedSessionId == sessionId;
    }));
    EXPECT_TRUE(state.asyncConnected);
    EXPECT_FALSE(state.asyncReason.empty());
    EXPECT_EQ(state.asyncPeerIp, "127.0.0.1");
    EXPECT_EQ(state.asyncPeerPort, port);
    lock.unlock();

    const std::string request = "async-loopback-packet";
    ASSERT_EQ(client.Get()->Send(sessionId, RequestOpcode, request.data(), request.size()), LLBC_OK);

    lock.lock();
    ASSERT_TRUE(state.received.wait_for(lock, std::chrono::seconds(3), [&state]
    {
        return state.serverReceiveCount == 1 && state.clientReceiveCount == 1;
    }));
    EXPECT_EQ(state.serverPayload, request);
    EXPECT_EQ(state.clientPayload, request);
    EXPECT_EQ(state.serverSendResult, LLBC_OK);
    lock.unlock();

    EXPECT_EQ(client.Get()->Stop(true), LLBC_OK);
    EXPECT_EQ(server.Get()->Stop(true), LLBC_OK);
}

// Reserving and then releasing a local ephemeral port gives AsyncConn a bounded
// loopback refusal path without depending on an external service.
TEST(ServiceTest, AsyncConnectReportsFailureForUnlistenedPort)
{
    LLBC_Socket reservedSocket;
    ASSERT_TRUE(reservedSocket);
    ASSERT_EQ(reservedSocket.BindTo("127.0.0.1", 0), LLBC_OK);
    ASSERT_EQ(reservedSocket.UpdateLocalAddress(), LLBC_OK);
    const uint16 reservedPort = reservedSocket.GetLocalAddress().GetPort();
    ASSERT_NE(reservedPort, 0u);
    ASSERT_EQ(reservedSocket.Close(), LLBC_OK);

    LoopbackState state;
    const LLBC_String clientName = LLBC_String().format(
        "UnitTestCommAsyncFailureClient-%d", LLBC_GetCurrentProcessId());
    ServiceGuard client(LLBC_Service::Create(clientName));
    ASSERT_NE(client.Get(), nullptr);
    ASSERT_EQ(client.Get()->AddComponent(new EchoClientComponent(state)), LLBC_OK);
    ASSERT_EQ(client.Get()->Start(), LLBC_OK);

    const int sessionId = client.Get()->AsyncConn("127.0.0.1", reservedPort);
    ASSERT_NE(sessionId, 0);

    std::unique_lock<std::mutex> lock(state.lock);
    ASSERT_TRUE(state.received.wait_for(lock, std::chrono::seconds(3), [&state, sessionId]
    {
        return state.asyncResultSessionId == sessionId;
    }));
    EXPECT_FALSE(state.asyncConnected);
    EXPECT_FALSE(state.asyncReason.empty());
    EXPECT_EQ(state.asyncPeerIp, "127.0.0.1");
    EXPECT_EQ(state.asyncPeerPort, reservedPort);
    lock.unlock();

    EXPECT_FALSE(client.Get()->IsSessionValidate(sessionId));
    EXPECT_EQ(client.Get()->Stop(true), LLBC_OK);
}
