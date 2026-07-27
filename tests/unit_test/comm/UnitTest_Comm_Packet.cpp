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

#include <array>
#include <cstring>
#include <deque>
#include <list>
#include <map>
#include <sstream>
#include <set>
#include <string>
#include <vector>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/comm/Packet.cpp
// @coverage-target: llbc/include/llbc/comm/PacketInl.h
// @coverage-target: llbc/src/comm/PacketHeaderAssembler.cpp
// @coverage-target: llbc/include/llbc/comm/SessionOptsInl.h
// @coverage-target: llbc/src/comm/SessionOpts.cpp
// @coverage-target: llbc/src/comm/PollerType.cpp

namespace
{

class TestCoder final : public LLBC_Coder
{
public:
    TestCoder(int *calls, bool result)
    : _calls(calls)
    , _result(result)
    {
    }

    bool Encode(LLBC_Packet &packet) override
    {
        ++*_calls;
        return _result && packet.Write(static_cast<uint32>(0x1a2b3c4du)) == LLBC_OK;
    }

    bool Decode(LLBC_Packet &packet) override
    {
        ++*_calls;
        uint32 marker = 0;
        return _result && packet.Read(marker) == LLBC_OK && marker == 0x1a2b3c4du;
    }

private:
    int *_calls;
    bool _result;
};

class CleanupRecorder
{
public:
    void DeleteInt(void *value)
    {
        ++intCleanupCalls;
        delete static_cast<int *>(value);
    }

    void DeletePayload(LLBC_MessageBlock *block)
    {
        ++payloadCleanupCalls;
        delete block;
    }

    int intCleanupCalls = 0;
    int payloadCleanupCalls = 0;
};

void AssignAddress(LLBC_SockAddr_IN &target, const LLBC_SockAddr_IN &source)
{
    target = source;
}

template <typename T>
void PutHeaderValue(std::array<char, 20> &header, size_t offset, T value)
{
#if LLBC_CFG_COMM_ORDER_IS_NET_ORDER
    value = LLBC_Host2Net(value);
#endif
    std::memcpy(header.data() + offset, &value, sizeof(value));
}

} // namespace

// Packets carry transport metadata independently from their payload. The payload
// uses the configured network byte order and supports stream-compatible values.
TEST(PacketTest, PreservesHeaderMetadataAndPayloadValues)
{
    LLBC_Packet packet;
    EXPECT_EQ(packet.GetLength(), 0u);
    EXPECT_EQ(packet.GetPayload(), nullptr);
    EXPECT_EQ(packet.GetPayloadLength(), 0u);
    std::ostringstream emptyOutput;
    emptyOutput << packet;
    EXPECT_NE(emptyOutput.str().find("read_pos:0"), std::string::npos);

    packet.SetLength(512);
    packet.SetAcceptSessionId(7);
    packet.SetHeader(42, 1001, -3, 0x03u);
    packet.AddFlags(0x0cu);
    packet.RemoveFlags(0x02u);
    packet.SetExtData1(-1);
    packet.SetExtData2(2);
    packet.SetExtData3(3);

    const LLBC_SockAddr_IN local("127.0.0.1", 31001);
    const LLBC_SockAddr_IN peer("127.0.0.1", 31002);
    packet.SetLocalAddr(local);
    packet.SetPeerAddr(peer);
    EXPECT_EQ(packet.GetLength(), 512u);
    EXPECT_EQ(packet.GetSessionId(), 42);
    EXPECT_EQ(packet.GetAcceptSessionId(), 7);
    EXPECT_EQ(packet.GetOpcode(), 1001);
    EXPECT_EQ(packet.GetStatus(), -3);
    EXPECT_TRUE(packet.HasFlags(0x0du));
    EXPECT_FALSE(packet.HasFlags(0x02u));
    EXPECT_EQ(packet.GetExtData1(), -1);
    EXPECT_EQ(packet.GetExtData2(), 2);
    EXPECT_EQ(packet.GetExtData3(), 3);
    EXPECT_EQ(packet.GetLocalAddr(), local);
    EXPECT_EQ(packet.GetPeerAddr(), peer);

    const uint32 number = 0x01020304u;
    const std::string text = "packet-text";
    const std::vector<sint16> values {-4, 0, 9};
    ASSERT_EQ(packet.Write(number), LLBC_OK);
    ASSERT_EQ(packet.Write(text), LLBC_OK);
    ASSERT_EQ(packet.Write(values), LLBC_OK);
    ASSERT_GT(packet.GetPayloadLength(), 0u);

    uint32 readNumber = 0;
    std::string readText;
    std::vector<sint16> readValues;
    EXPECT_EQ(packet.Read(readNumber), LLBC_OK);
    EXPECT_EQ(packet.Read(readText), LLBC_OK);
    EXPECT_EQ(packet.Read(readValues), LLBC_OK);
    EXPECT_EQ(readNumber, number);
    EXPECT_EQ(readText, text);
    EXPECT_EQ(readValues, values);

    std::ostringstream output;
    output << packet;
    EXPECT_NE(output.str().find("sid:42"), std::string::npos);
    EXPECT_NE(output.str().find("opcode:1001"), std::string::npos);

    packet.Clear();
    EXPECT_EQ(packet.GetLength(), 0u);
    EXPECT_EQ(packet.GetSessionId(), 0);
    EXPECT_EQ(packet.GetAcceptSessionId(), 0);
    EXPECT_EQ(packet.GetOpcode(), 0);
    EXPECT_EQ(packet.GetStatus(), 0);
    EXPECT_EQ(packet.GetFlags(), 0u);
    EXPECT_EQ(packet.GetPayloadLength(), 0u);
}

// Coder and callback ownership is part of the packet lifecycle. Replacing or
// clearing a packet must release its previous owned values exactly once.
TEST(PacketTest, OwnsCodersPayloadAndPreHandleResults)
{
    LLBC_Packet packet;
    int encodeCalls = 0;
    int decodeCalls = 0;
    packet.SetEncoder(new TestCoder(&encodeCalls, true));
    ASSERT_TRUE(packet.Encode());
    EXPECT_EQ(encodeCalls, 1);
    EXPECT_EQ(packet.GetEncoder(), nullptr);

    packet.SetDecoder(new TestCoder(&decodeCalls, true));
    ASSERT_TRUE(packet.Decode());
    EXPECT_EQ(decodeCalls, 1);
    EXPECT_NE(packet.GetDecoder(), nullptr);

    CleanupRecorder recorder;
    packet.SetPreHandleResult(new int(10), &recorder, &CleanupRecorder::DeleteInt);
    ASSERT_NE(packet.GetPreHandleResult<int>(), nullptr);
    EXPECT_EQ(*packet.GetPreHandleResult<int>(), 10);
    EXPECT_EQ(packet.GetPreHandleResult(), static_cast<void *>(packet.GetPreHandleResult<int>()));
    packet.SetPreHandleResult(new int(20), &recorder, &CleanupRecorder::DeleteInt);
    EXPECT_EQ(recorder.intCleanupCalls, 1);

    packet.SetPayloadDeleteDeleg(
        LLBC_Delegate<void(LLBC_MessageBlock *)>(&recorder, &CleanupRecorder::DeletePayload));
    packet.SetPayload(new LLBC_MessageBlock);
    ASSERT_NE(packet.GetMutablePayload(), nullptr);
    ASSERT_EQ(packet.Write("x", 1), LLBC_OK);
    packet.Clear();
    EXPECT_EQ(recorder.intCleanupCalls, 2);
    EXPECT_EQ(recorder.payloadCleanupCalls, 1);
    EXPECT_EQ(packet.GetDecoder(), nullptr);

    packet.SetPayload(new LLBC_MessageBlock);
    LLBC_MessageBlock *detached = packet.DetachPayload();
    ASSERT_NE(detached, nullptr);
    EXPECT_EQ(packet.GetPayload(), nullptr);
    delete detached;
}

// Every scalar and standard-container overload shares the packet wire format.
// Exercise the public round-trip contract, including the stream and operator
// conveniences that callers use to construct application payloads.
TEST(PacketTest, RoundTripsScalarContainerAndStreamOverloads)
{
    LLBC_Packet packet;
    const bool boolValue = true;
    const sint8 sint8Value = -8;
    const uint8 uint8Value = 8;
    const sint16 sint16Value = -16;
    const uint16 uint16Value = 16;
    const sint32 sint32Value = -32;
    const uint32 uint32Value = 32;
    const long longValue = -64;
    const ulong ulongValue = 64;
    const sint64 sint64Value = -128;
    const uint64 uint64Value = 128;
    const float floatValue = 1.25f;
    const double doubleValue = 2.5;
    const std::list<uint16> listValue {1, 2, 3};
    const std::deque<sint32> dequeValue {-1, 0, 1};
    const std::set<uint32> setValue {4, 5, 6};
    const std::map<uint16, std::string> mapValue {{7, "seven"}, {8, "eight"}};
    const char cString[] = "packet-c-string";
    LLBC_Stream stream;
    stream.Write("stream-data", sizeof("stream-data"));

    ASSERT_EQ(packet.Write(boolValue), LLBC_OK);
    ASSERT_EQ(packet.Write(sint8Value), LLBC_OK);
    ASSERT_EQ(packet.Write(uint8Value), LLBC_OK);
    ASSERT_EQ(packet.Write(sint16Value), LLBC_OK);
    ASSERT_EQ(packet.Write(uint16Value), LLBC_OK);
    ASSERT_EQ(packet.Write(sint32Value), LLBC_OK);
    ASSERT_EQ(packet.Write(uint32Value), LLBC_OK);
    ASSERT_EQ(packet.Write(longValue), LLBC_OK);
    ASSERT_EQ(packet.Write(ulongValue), LLBC_OK);
    ASSERT_EQ(packet.Write(sint64Value), LLBC_OK);
    ASSERT_EQ(packet.Write(uint64Value), LLBC_OK);
    ASSERT_EQ(packet.Write(floatValue), LLBC_OK);
    ASSERT_EQ(packet.Write(doubleValue), LLBC_OK);
    ASSERT_EQ(packet.Write(cString), LLBC_OK);
    ASSERT_EQ(packet.Write(static_cast<const char *>(nullptr)), LLBC_OK);
    ASSERT_EQ(packet.Write(stream), LLBC_OK);
    ASSERT_EQ(packet.Write(listValue), LLBC_OK);
    ASSERT_EQ(packet.Write(dequeValue), LLBC_OK);
    ASSERT_EQ(packet.Write(setValue), LLBC_OK);
    ASSERT_EQ(packet.Write(mapValue), LLBC_OK);

    bool readBool = false;
    sint8 readSint8 = 0;
    uint8 readUint8 = 0;
    sint16 readSint16 = 0;
    uint16 readUint16 = 0;
    sint32 readSint32 = 0;
    uint32 readUint32 = 0;
    long readLong = 0;
    ulong readUlong = 0;
    sint64 readSint64 = 0;
    uint64 readUint64 = 0;
    float readFloat = 0;
    double readDouble = 0;
    char readCString[sizeof(cString)] {};
    char readStream[sizeof("stream-data")] {};
    std::list<uint16> readList;
    std::deque<sint32> readDeque;
    std::set<uint32> readSet;
    std::map<uint16, std::string> readMap;
    EXPECT_EQ(packet.Read(readBool), LLBC_OK);
    EXPECT_EQ(packet.Read(readSint8), LLBC_OK);
    EXPECT_EQ(packet.Read(readUint8), LLBC_OK);
    EXPECT_EQ(packet.Read(readSint16), LLBC_OK);
    EXPECT_EQ(packet.Read(readUint16), LLBC_OK);
    EXPECT_EQ(packet.Read(readSint32), LLBC_OK);
    EXPECT_EQ(packet.Read(readUint32), LLBC_OK);
    EXPECT_EQ(packet.Read(readLong), LLBC_OK);
    EXPECT_EQ(packet.Read(readUlong), LLBC_OK);
    EXPECT_EQ(packet.Read(readSint64), LLBC_OK);
    EXPECT_EQ(packet.Read(readUint64), LLBC_OK);
    EXPECT_EQ(packet.Read(readFloat), LLBC_OK);
    EXPECT_EQ(packet.Read(readDouble), LLBC_OK);
    EXPECT_EQ(packet.Read(readCString, sizeof(readCString)), LLBC_OK);
    EXPECT_EQ(packet.Read(readStream, sizeof(readStream)), LLBC_OK);
    EXPECT_EQ(packet.Read(readList), LLBC_OK);
    EXPECT_EQ(packet.Read(readDeque), LLBC_OK);
    EXPECT_EQ(packet.Read(readSet), LLBC_OK);
    EXPECT_EQ(packet.Read(readMap), LLBC_OK);
    EXPECT_EQ(readBool, boolValue);
    EXPECT_EQ(readSint8, sint8Value);
    EXPECT_EQ(readUint8, uint8Value);
    EXPECT_EQ(readSint16, sint16Value);
    EXPECT_EQ(readUint16, uint16Value);
    EXPECT_EQ(readSint32, sint32Value);
    EXPECT_EQ(readUint32, uint32Value);
    EXPECT_EQ(readLong, longValue);
    EXPECT_EQ(readUlong, ulongValue);
    EXPECT_EQ(readSint64, sint64Value);
    EXPECT_EQ(readUint64, uint64Value);
    EXPECT_FLOAT_EQ(readFloat, floatValue);
    EXPECT_DOUBLE_EQ(readDouble, doubleValue);
    EXPECT_STREQ(readCString, cString);
    EXPECT_STREQ(readStream, "stream-data");
    EXPECT_EQ(readList, listValue);
    EXPECT_EQ(readDeque, dequeValue);
    EXPECT_EQ(readSet, setValue);
    EXPECT_EQ(readMap, mapValue);

    LLBC_Packet streamOperators;
    streamOperators << static_cast<sint32>(-99) << std::string("operator-value");
    sint32 operatorNumber = 0;
    std::string operatorText;
    streamOperators >> operatorNumber >> operatorText;
    EXPECT_EQ(operatorNumber, -99);
    EXPECT_EQ(operatorText, "operator-value");
}

// Empty packets must report bounded reads, and ownership transfer methods must
// leave no dangling payload, coder, result, or codec-error state behind.
TEST(PacketTest, HandlesEmptyReadsAndExplicitOwnershipTransfers)
{
    LLBC_Packet packet;
    uint32 value = 99;
    EXPECT_EQ(packet.Read(value), LLBC_FAILED);
    EXPECT_EQ(value, 0u);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_LIMIT);
    EXPECT_EQ(packet.Read(nullptr, 1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_LIMIT);
    EXPECT_TRUE(packet.Encode());
    EXPECT_TRUE(packet.Decode());
    EXPECT_EQ(packet.GiveUpPayload(), nullptr);
    EXPECT_EQ(packet.GetCodecError(), "");

    int failedEncodeCalls = 0;
    packet.SetEncoder(new TestCoder(&failedEncodeCalls, false));
    EXPECT_FALSE(packet.Encode());
    EXPECT_EQ(failedEncodeCalls, 1);
    delete packet.GiveUpEncoder();

    int failedDecodeCalls = 0;
    packet.SetDecoder(new TestCoder(&failedDecodeCalls, false));
    EXPECT_FALSE(packet.Decode());
    EXPECT_EQ(failedDecodeCalls, 1);
    delete packet.GiveUpDecoder();

    packet.SetCodecError("first-error");
    EXPECT_EQ(packet.GetCodecError(), "first-error");
    packet.SetCodecError("second-error");
    EXPECT_EQ(packet.GetCodecError(), "second-error");

    int encoderCalls = 0;
    auto *encoder = new TestCoder(&encoderCalls, true);
    packet.SetEncoder(encoder);
    packet.SetEncoder(encoder);
    EXPECT_EQ(packet.GiveUpEncoder(), encoder);
    EXPECT_EQ(packet.GetEncoder(), nullptr);
    delete encoder;

    int decoderCalls = 0;
    auto *decoder = new TestCoder(&decoderCalls, true);
    packet.SetDecoder(decoder);
    packet.SetDecoder(decoder);
    EXPECT_EQ(packet.GiveUpDecoder(), decoder);
    EXPECT_EQ(packet.GetDecoder(), nullptr);
    delete decoder;

    packet.SetEncoder(new TestCoder(&encoderCalls, true));
    LLBC_MessageBlock *encoded = packet.GiveUpPayload();
    ASSERT_NE(encoded, nullptr);
    EXPECT_EQ(encoderCalls, 1);
    EXPECT_EQ(packet.GetPayload(), nullptr);
    delete encoded;

    ASSERT_EQ(packet.Write(static_cast<uint32>(123)), LLBC_OK);
    uint32 consumed = 0;
    ASSERT_EQ(packet.Read(consumed), LLBC_OK);
    EXPECT_EQ(consumed, 123u);
    packet.ResetPayload();
    EXPECT_EQ(packet.GetPayloadLength(), 0u);
    packet.Clear();
    EXPECT_EQ(packet.GetCodecError(), "");
}

// A normal packet header may arrive in multiple socket reads. The assembler
// must consume only header bytes, retain partial input, and decode every field
// into the packet using the configured wire byte order.
TEST(PacketHeaderAssemblerTest, AssemblesPartialNetworkOrderedHeader)
{
    constexpr uint32 length = 4096;
    constexpr sint32 opcode = -17;
    constexpr uint16 status = 7;
    constexpr uint16 flags = 0x35u;
    constexpr sint64 extData = -1234567890123LL;
    std::array<char, 20> header {};
    PutHeaderValue(header, 0, length);
    PutHeaderValue(header, 4, opcode);
    PutHeaderValue(header, 8, status);
    PutHeaderValue(header, 10, flags);
    PutHeaderValue(header, 12, extData);

    LLBC_PacketHeaderAssembler assembler(header.size());
    size_t used = 0;
    EXPECT_FALSE(assembler.Assemble(header.data(), 5, used));
    EXPECT_EQ(used, 5u);
    EXPECT_FALSE(assembler.Assemble(header.data() + 5, 9, used));
    EXPECT_EQ(used, 9u);
    EXPECT_TRUE(assembler.Assemble(header.data() + 14, header.size() - 14, used));
    EXPECT_EQ(used, 6u);
    EXPECT_TRUE(assembler.Assemble(header.data(), 1, used));
    EXPECT_EQ(used, 0u);

    LLBC_Packet packet;
    assembler.SetToPacket(packet);
    EXPECT_EQ(packet.GetLength(), length);
    EXPECT_EQ(packet.GetOpcode(), opcode);
    EXPECT_EQ(packet.GetStatus(), status);
    EXPECT_EQ(packet.GetFlags(), flags);
    EXPECT_EQ(packet.GetExtData1(), extData);

    assembler.Reset();
    EXPECT_TRUE(assembler.Assemble(header.data(), header.size(), used));
    EXPECT_EQ(used, header.size());
}

// Session options are immutable defaults plus simple value semantics, while
// poller names are configuration-facing strings and must reject unknown values.
TEST(CommValueTest, ComparesSessionOptionsAndConvertsPollerTypes)
{
    LLBC_SessionOpts options(false, 1, 2, 3, 4, 5);
    EXPECT_FALSE(options.IsNoDelay());
    EXPECT_EQ(options.GetSockSendBufSize(), 1u);
    EXPECT_EQ(options.GetSockRecvBufSize(), 2u);
    EXPECT_EQ(options.GetSessionSendBufSize(), 3u);
    EXPECT_EQ(options.GetSessionRecvBufSize(), 4u);
    EXPECT_EQ(options.GetMaxPacketSize(), 5u);

    LLBC_SessionOpts copy = options;
    EXPECT_TRUE(copy == options);
    copy.SetNoDelay(true);
    copy.SetSockSendBufSize(10);
    copy.SetSockRecvBufSize(20);
    copy.SetSessionSendBufSize(30);
    copy.SetSessionRecvBufSize(40);
    copy.SetMaxPacketSize(50);
    EXPECT_FALSE(copy == options);
    EXPECT_TRUE(copy.IsNoDelay());
    EXPECT_EQ(copy.GetSockSendBufSize(), 10u);
    EXPECT_EQ(copy.GetSockRecvBufSize(), 20u);
    EXPECT_EQ(copy.GetSessionSendBufSize(), 30u);
    EXPECT_EQ(copy.GetSessionRecvBufSize(), 40u);
    EXPECT_EQ(copy.GetMaxPacketSize(), 50u);
    EXPECT_TRUE(LLBC_DftSessionOpts.IsNoDelay());

    EXPECT_TRUE(LLBC_PollerType::IsValid(LLBC_PollerType::SelectPoller));
    EXPECT_FALSE(LLBC_PollerType::IsValid(LLBC_PollerType::End));
    EXPECT_FALSE(LLBC_PollerType::IsValid(-1));
    EXPECT_EQ(LLBC_PollerType::Type2Str(LLBC_PollerType::SelectPoller), "SelectPoller");
    EXPECT_EQ(LLBC_PollerType::Type2Str(-1), "Invalid");
    EXPECT_EQ(LLBC_PollerType::Str2Type("selectpoller"), LLBC_PollerType::SelectPoller);
    EXPECT_EQ(LLBC_PollerType::Str2Type("unknown"), LLBC_PollerType::End);
}

// Socket addresses are transport values: conversion, copy, padding, and error
// handling must work without opening a socket or changing process-wide state.
TEST(SocketDataTypeTest, ConvertsAddressesAndValidatesPadding)
{
    LLBC_SockAddr_IN address("127.0.0.1", 12345);
    EXPECT_EQ(address.GetAddressFamily(), AF_INET);
    EXPECT_EQ(address.GetIpAsString(), "127.0.0.1");
    EXPECT_EQ(address.GetIpAsNumber(), 0x7f000001);
    EXPECT_EQ(address.GetPort(), 12345);
    EXPECT_EQ(address.ToString(), "127.0.0.1:12345");

    std::ostringstream output;
    output << address;
    EXPECT_EQ(output.str(), "127.0.0.1:12345");
    EXPECT_EQ(address.SetAddressFamily(AF_UNSPEC), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(address.SetAddressFamily(AF_INET), LLBC_OK);

    address.SetIp("10.20.30.40");
    address.SetPort(54321);
    EXPECT_EQ(address.GetIpAsString(), "10.20.30.40");
    EXPECT_EQ(address.GetPort(), 54321);
    address.SetIp(0x7f000001);
    EXPECT_EQ(address.GetIpAsString(), "127.0.0.1");
    address.SetIp(LLBC_String());
    EXPECT_EQ(address.GetIpAsString(), "127.0.0.1");

    std::array<char, 8> padding {{'p', 'a', 'd', 'd', 'i', 'n', 'g', '!'}};
    size_t paddingLength = padding.size() - 1;
    EXPECT_EQ(address.SetPaddingBuf(nullptr, padding.size()), LLBC_FAILED);
    EXPECT_EQ(address.SetPaddingBuf(padding.data(), paddingLength), LLBC_FAILED);
    EXPECT_EQ(address.GetPaddingBuf(nullptr, paddingLength), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(address.SetPaddingBuf(padding.data(), padding.size()), LLBC_OK);
    std::array<char, 8> copiedPadding {};
    paddingLength = copiedPadding.size();
    EXPECT_EQ(address.GetPaddingBuf(copiedPadding.data(), paddingLength), LLBC_OK);
    EXPECT_EQ(paddingLength, copiedPadding.size());
    EXPECT_EQ(copiedPadding, padding);

    const sockaddr_in osAddress = address.ToOSDataType();
    LLBC_SockAddr_IN restored;
    EXPECT_EQ(restored.FromOSDataType(nullptr), LLBC_FAILED);
    EXPECT_EQ(restored.FromOSDataType(reinterpret_cast<const sockaddr *>(&osAddress),
                                      sizeof(osAddress) - 1),
              LLBC_FAILED);
    EXPECT_EQ(restored.FromOSDataType(&osAddress), LLBC_OK);
    EXPECT_EQ(restored, address);
    LLBC_SockAddr_IN assigned;
    assigned = restored;
    AssignAddress(assigned, assigned);
    EXPECT_EQ(assigned, restored);
    assigned.ZeroPaddingBuf();
    EXPECT_FALSE(assigned == restored);
}
