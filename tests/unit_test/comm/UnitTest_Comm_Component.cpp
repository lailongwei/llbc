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

#include <sstream>
#include <string>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/comm/Component.cpp
// @coverage-target: llbc/include/llbc/comm/ComponentInl.h

namespace
{

LLBC_SessionInfo *MakeSessionInfo()
{
    auto *info = new LLBC_SessionInfo;
    info->SetIsListenSession(true);
    info->SetSessionId(101);
    info->SetAcceptSessionId(99);
    info->SetLocalAddr(LLBC_SockAddr_IN("127.0.0.1", 41001));
    info->SetPeerAddr(LLBC_SockAddr_IN("127.0.0.1", 41002));
    info->SetSocket(123);
    return info;
}

class MethodComponent final : public LLBC_Component
{
public:
    int Increment(const LLBC_Variant &arg, LLBC_Variant &ret)
    {
        ret = arg.As<int>() + 1;
        return LLBC_OK;
    }
};

} // namespace

// Session metadata owns no transport resource itself. Its values and string
// representation must remain stable when wrapped by a destroy notification.
TEST(ComponentTest, PreservesSessionAndDestroyInformation)
{
    LLBC_SessionInfo info;
    EXPECT_FALSE(info.IsListenSession());
    EXPECT_EQ(info.GetSessionId(), 0);
    EXPECT_EQ(info.GetAcceptSessionId(), 0);
    EXPECT_EQ(info.GetSocket(), LLBC_INVALID_SOCKET_HANDLE);

    info.SetIsListenSession(true);
    info.SetSessionId(101);
    info.SetAcceptSessionId(99);
    info.SetLocalAddr(LLBC_SockAddr_IN("127.0.0.1", 41001));
    info.SetPeerAddr(LLBC_SockAddr_IN("127.0.0.1", 41002));
    info.SetSocket(123);
    EXPECT_TRUE(info.IsListenSession());
    EXPECT_EQ(info.GetSessionId(), 101);
    EXPECT_EQ(info.GetAcceptSessionId(), 99);
    EXPECT_EQ(info.GetLocalAddr(), LLBC_SockAddr_IN("127.0.0.1", 41001));
    EXPECT_EQ(info.GetPeerAddr(), LLBC_SockAddr_IN("127.0.0.1", 41002));
    EXPECT_EQ(info.GetSocket(), 123);
    EXPECT_NE(std::string(info.ToString().c_str()).find("sessionId:101"), std::string::npos);

    std::ostringstream infoOutput;
    infoOutput << info;
    EXPECT_NE(infoOutput.str().find("acceptSessionId:99"), std::string::npos);

    char serviceReason[] = "component-test-close";
    LLBC_SessionDestroyInfo serviceDestroy(
        MakeSessionInfo(), new LLBC_SessionCloseInfo(serviceReason));
    EXPECT_TRUE(serviceDestroy.IsListenSession());
    EXPECT_EQ(serviceDestroy.GetSessionId(), 101);
    EXPECT_EQ(serviceDestroy.GetAcceptSessionId(), 99);
    EXPECT_EQ(serviceDestroy.GetSocket(), 123);
    EXPECT_EQ(serviceDestroy.GetLocalAddr(), LLBC_SockAddr_IN("127.0.0.1", 41001));
    EXPECT_EQ(serviceDestroy.GetPeerAddr(), LLBC_SockAddr_IN("127.0.0.1", 41002));
    EXPECT_TRUE(serviceDestroy.IsDestroyedFromService());
    EXPECT_EQ(serviceDestroy.GetErrno(), LLBC_ERROR_SUCCESS);
    EXPECT_EQ(serviceDestroy.GetSubErrno(), LLBC_ERROR_SUCCESS);
    EXPECT_EQ(serviceDestroy.GetReason(), serviceReason);
    EXPECT_NE(std::string(serviceDestroy.ToString().c_str()).find(serviceReason), std::string::npos);

    std::ostringstream destroyOutput;
    destroyOutput << serviceDestroy;
    EXPECT_NE(destroyOutput.str().find("fromService:true"), std::string::npos);

    LLBC_SessionDestroyInfo networkDestroy(
        MakeSessionInfo(), new LLBC_SessionCloseInfo(LLBC_ERROR_ARG, 7));
    EXPECT_FALSE(networkDestroy.IsDestroyedFromService());
    EXPECT_EQ(networkDestroy.GetErrno(), LLBC_ERROR_ARG);
    EXPECT_EQ(networkDestroy.GetSubErrno(), LLBC_ERROR_SUCCESS);
    EXPECT_FALSE(networkDestroy.GetReason().empty());
}

// Connection and protocol reports are passed to components by value. Every
// field, including the human-readable representation, must reflect setters.
TEST(ComponentTest, PreservesAsyncProtocolAndReloadReports)
{
    LLBC_AsyncConnResult asyncResult;
    EXPECT_FALSE(asyncResult.IsConnected());
    EXPECT_EQ(asyncResult.GetSessionId(), 0);
    asyncResult.SetIsConnected(true);
    asyncResult.SetSessionId(303);
    asyncResult.SetReason("connected");
    asyncResult.SetPeerAddr(LLBC_SockAddr_IN("127.0.0.1", 42001));
    EXPECT_TRUE(asyncResult.IsConnected());
    EXPECT_EQ(asyncResult.GetSessionId(), 303);
    EXPECT_EQ(asyncResult.GetReason(), "connected");
    EXPECT_EQ(asyncResult.GetPeerAddr(), LLBC_SockAddr_IN("127.0.0.1", 42001));
    EXPECT_NE(std::string(asyncResult.ToString().c_str()).find("connected:true"), std::string::npos);

    std::ostringstream asyncOutput;
    asyncOutput << asyncResult;
    EXPECT_NE(asyncOutput.str().find("peer_addr"), std::string::npos);

    LLBC_ProtoReport report;
    report.SetSessionId(303);
    report.SetOpcode(7001);
    report.SetLayer(LLBC_ProtocolLayer::CodecLayer);
    report.SetLevel(LLBC_ProtoReportLevel::Warn);
    report.SetReport("decode warning");
    EXPECT_EQ(report.GetSessionId(), 303);
    EXPECT_EQ(report.GetOpcode(), 7001);
    EXPECT_EQ(report.GetLayer(), LLBC_ProtocolLayer::CodecLayer);
    EXPECT_EQ(report.GetLevel(), LLBC_ProtoReportLevel::Warn);
    EXPECT_EQ(report.GetReport(), "decode warning");
    EXPECT_NE(std::string(report.ToString().c_str()).find("decode warning"), std::string::npos);

    std::ostringstream reportOutput;
    reportOutput << report;
    EXPECT_NE(reportOutput.str().find("opcode:7001"), std::string::npos);

    LLBC_AppReloadFailedInfo reloadFailure;
    reloadFailure.SetErrNo(LLBC_ERROR_ARG);
    reloadFailure.SetSubErrNo(7);
    EXPECT_EQ(reloadFailure.GetErrNo(), LLBC_ERROR_ARG);
    EXPECT_EQ(reloadFailure.GetSubErrNo(), 7);
    EXPECT_FALSE(reloadFailure.GetErrDesc().empty());
    EXPECT_NE(std::string(reloadFailure.ToString().c_str()).find("subErrNo:7"), std::string::npos);
}

// Component method dispatch must reject invalid registrations, preserve a
// registered delegate, and expose its result through both wrapper classes.
TEST(ComponentTest, RegistersAndDispatchesComponentMethods)
{
    MethodComponent component;
    EXPECT_EQ(component.GetService(), nullptr);
    EXPECT_EQ(component.GetConfigType(), LLBC_AppConfigType::End);
    EXPECT_TRUE(component.GetConfig().Is<void>());
    EXPECT_TRUE(component.GetAllMethods().GetAllMethods().empty());

    LLBC_Variant result;
    EXPECT_EQ(component.CallMethod("missing", 1, result), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_EQ(component.AddMethod("", &MethodComponent::Increment), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    ASSERT_EQ(component.AddMethod("increment", &MethodComponent::Increment), LLBC_OK);
    EXPECT_EQ(component.AddMethod("increment", &MethodComponent::Increment), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_REPEAT);
    ASSERT_EQ(component.CallMethod("increment", 41, result), LLBC_OK);
    EXPECT_EQ(result.As<int>(), 42);
    EXPECT_TRUE(component.GetAllMethods().GetMethod("increment"));
    EXPECT_FALSE(component.GetAllMethods().GetMethod("missing"));
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    component.SetConfig(LLBC_Variant("component-config"));
    EXPECT_EQ(component.GetConfig(), "component-config");
}
