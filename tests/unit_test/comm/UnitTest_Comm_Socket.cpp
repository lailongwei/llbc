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

#include <memory>
#include <string>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/comm/Socket.cpp

namespace
{

class SocketPair
{
public:
    bool Connect()
    {
        if (listener.EnableAddressReusable() != LLBC_OK ||
            listener.BindTo("127.0.0.1", 0) != LLBC_OK ||
            listener.UpdateLocalAddress() != LLBC_OK ||
            listener.Listen(1) != LLBC_OK)
        {
            return false;
        }

        if (client.Connect(listener.GetLocalAddress()) != LLBC_OK)
            return false;

        accepted.reset(listener.Accept());
        return accepted != nullptr;
    }

    LLBC_Socket listener;
    LLBC_Socket client;
    std::unique_ptr<LLBC_Socket> accepted;
};

uint16 ReserveAndReleaseLoopbackPort()
{
    LLBC_Socket reserved;
    if (!reserved || reserved.BindTo("127.0.0.1", 0) != LLBC_OK ||
        reserved.UpdateLocalAddress() != LLBC_OK)
    {
        return 0;
    }

    const uint16 port = reserved.GetLocalPort();
    return reserved.Close() == LLBC_OK ? port : 0;
}

} // namespace

// Socket state and options are public contracts independent of a running service
// or poller. Buffer sizes are OS-managed, so only their observable lower bounds
// and successful option transitions are asserted.
TEST(SocketTest, CreatesConfiguresQueuesAndCloses)
{
    LLBC_Socket socket;
    ASSERT_TRUE(socket);
    EXPECT_FALSE(socket.IsClosed());
    EXPECT_FALSE(!socket);
    EXPECT_NE(socket.Handle(), LLBC_INVALID_SOCKET_HANDLE);
    EXPECT_EQ(socket.GetPollerType(), LLBC_PollerType::End);

    socket.SetPollerType(LLBC_PollerType::SelectPoller);
    EXPECT_EQ(socket.GetPollerType(), LLBC_PollerType::SelectPoller);
    ASSERT_EQ(socket.EnableAddressReusable(), LLBC_OK);
    ASSERT_EQ(socket.DisableAddressReusable(), LLBC_OK);

    ASSERT_EQ(socket.SetNoDelay(true), LLBC_OK);
    EXPECT_TRUE(socket.IsNoDelay());
    ASSERT_EQ(socket.SetNoDelay(false), LLBC_OK);
    EXPECT_FALSE(socket.IsNoDelay());
    ASSERT_EQ(socket.SetNonBlocking(), LLBC_OK);
    EXPECT_TRUE(socket.IsNonBlocking());

    ASSERT_EQ(socket.SetSendBufSize(8192), LLBC_OK);
    ASSERT_EQ(socket.SetRecvBufSize(8192), LLBC_OK);
    EXPECT_GE(socket.GetSendBufSize(), 8192u);
    EXPECT_GE(socket.GetRecvBufSize(), 8192u);

    int keepAlive = 1;
    ASSERT_EQ(socket.SetOption(SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(keepAlive)), LLBC_OK);
    keepAlive = 0;
    LLBC_SocketLen optionLength = sizeof(keepAlive);
    ASSERT_EQ(socket.GetOption(SOL_SOCKET, SO_KEEPALIVE, &keepAlive, &optionLength), LLBC_OK);
    EXPECT_NE(keepAlive, 0);
    EXPECT_EQ(optionLength, sizeof(keepAlive));

    ASSERT_EQ(socket.SetMaxPacketSize(4096), LLBC_OK);
    EXPECT_EQ(socket.GetMaxPacketSize(), 4096u);
    ASSERT_EQ(socket.SetMaxPacketSize(0), LLBC_OK);
    EXPECT_EQ(socket.GetMaxPacketSize(), static_cast<size_t>(LLBC_INFINITE));

    ASSERT_EQ(socket.AsyncSend("queued", 6), LLBC_OK);
    EXPECT_TRUE(socket.IsExistNoSendData());
    EXPECT_EQ(socket.GetWillSendBuffer().GetSize(), 6u);

    ASSERT_EQ(socket.Close(), LLBC_OK);
    EXPECT_TRUE(socket.IsClosed());
    EXPECT_FALSE(socket);
    EXPECT_EQ(socket.Close(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
}

// A listener and client must establish address caches on both endpoints, accept
// a peer, exchange bytes in both directions, expose no pending error, and support
// an orderly half-close without depending on service-owned session callbacks.
TEST(SocketTest, ConnectsAcceptsExchangesAndShutsDownOverLoopback)
{
    SocketPair pair;
    ASSERT_TRUE(pair.Connect()) << LLBC_FormatLastError();
    ASSERT_NE(pair.listener.GetLocalPort(), 0u);
    ASSERT_TRUE(pair.listener.IsListen());
    ASSERT_NE(pair.accepted, nullptr);

    ASSERT_EQ(pair.accepted->UpdateLocalAddress(), LLBC_OK);
    ASSERT_EQ(pair.accepted->UpdatePeerAddress(), LLBC_OK);
    EXPECT_EQ(pair.client.GetLocalHostname(), "127.0.0.1");
    EXPECT_EQ(pair.client.GetPeerHostname(), "127.0.0.1");
    EXPECT_EQ(pair.client.GetPeerPort(), pair.listener.GetLocalPort());
    EXPECT_EQ(pair.accepted->GetLocalPort(), pair.listener.GetLocalPort());
    EXPECT_EQ(pair.accepted->GetPeerPort(), pair.client.GetLocalPort());

    int pendingError = -1;
    ASSERT_EQ(pair.client.GetPendingError(pendingError), LLBC_OK);
    EXPECT_EQ(pendingError, 0);

    const std::string request = "socket-request";
    ASSERT_EQ(pair.client.Send(request.data(), static_cast<int>(request.size())),
              static_cast<int>(request.size()));
    std::string received(request.size(), '\0');
    ASSERT_EQ(pair.accepted->Recv(&received[0], static_cast<int>(received.size())),
              static_cast<int>(received.size()));
    EXPECT_EQ(received, request);

    const std::string response = "socket-response";
    ASSERT_EQ(pair.accepted->Send(response.data(), static_cast<int>(response.size())),
              static_cast<int>(response.size()));
    received.assign(response.size(), '\0');
    ASSERT_EQ(pair.client.Recv(&received[0], static_cast<int>(received.size())),
              static_cast<int>(received.size()));
    EXPECT_EQ(received, response);

    ASSERT_EQ(pair.client.ShutdownInput(), LLBC_OK);
    ASSERT_EQ(pair.client.ShutdownOutput(), LLBC_OK);
    char endOfStream = '\0';
    EXPECT_EQ(pair.accepted->Recv(&endOfStream, 1), 0);

    SocketPair shutdownPair;
    ASSERT_TRUE(shutdownPair.Connect()) << LLBC_FormatLastError();
    ASSERT_EQ(shutdownPair.client.ShutdownInputOutput(), LLBC_OK);
    EXPECT_EQ(shutdownPair.accepted->Recv(&endOfStream, 1), 0);
}

// A released local port supplies an observable refusal path without involving an
// external network service. Connect must report the operating-system failure.
TEST(SocketTest, ReportsFailureWhenPeerDoesNotListen)
{
    const uint16 port = ReserveAndReleaseLoopbackPort();
    ASSERT_NE(port, 0u);

    LLBC_Socket client;
    ASSERT_TRUE(client);
    EXPECT_EQ(client.Connect(LLBC_SockAddr_IN("127.0.0.1", port)), LLBC_FAILED);
    EXPECT_NE(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);
}

// A nonblocking listener with no queued peer must return immediately instead
// of waiting for an incoming connection.
TEST(SocketTest, NonBlockingAcceptReportsNoQueuedPeer)
{
    LLBC_Socket listener;
    ASSERT_TRUE(listener);
    ASSERT_EQ(listener.BindTo("127.0.0.1", 0), LLBC_OK);
    ASSERT_EQ(listener.Listen(1), LLBC_OK);
    ASSERT_EQ(listener.SetNonBlocking(), LLBC_OK);

    EXPECT_EQ(listener.Accept(), nullptr);
    EXPECT_NE(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);
}
