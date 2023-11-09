// The MIT License (MIT)

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


#include "common/TestCase_Com_StreamFunc.h"

int TestCase_Com_StreamFunc::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Stream test:");
    LLBC_ErrorAndReturnIfNot(CtorTest() == LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(AttachTest() == LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(SwapTest() == LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(EndianTest() == LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(RWPosTest() == LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(SkipRWTest() == LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(ReadableSizeTest() == LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(WritableSizeTest() == LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(GetBufTest() == LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(BufOpTest() == LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(NumericSerTest() == LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(StrSerTest() == LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(StlContainerSerTest() == LLBC_OK, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(SerializableObjSerTest() == LLBC_OK, LLBC_FAILED);

    return LLBC_OK;
}

int TestCase_Com_StreamFunc::CtorTest()
{
    LLBC_PrintLn("Constructor test:");

    // Test default constructor.
    LLBC_Stream stream;
    LLBC_PrintLn("Construct by default ctor:\n- %s", stream.ToString().c_str());
    LLBC_ErrorAndReturnIfNot(stream.GetBuf() == nullptr &&
                             stream.GetCap() == 0 &&
                             stream.GetWritePos() == 0 &&
                             stream.GetReadableSize() == 0 &&
                             stream.GetWritableSize() == 0 &&
                             stream.GetEndian() == LLBC_DefaultEndian &&
                             !stream.IsAttach(),
                             LLBC_FAILED);

    stream.SetEndian(
        stream.GetEndian() == LLBC_Endian::BigEndian ?
            LLBC_Endian::LittleEndian : LLBC_Endian::BigEndian);
    stream << 3 << "hello";

    // Define copy construct test lambda.
    auto testCopyConstruct = [](bool attach)
    {
        LLBC_Stream stream1;
        stream1 << false << 10086 << "hello world";

        LLBC_Stream stream2(stream1, attach);
        LLBC_PrintLn("Copy construct by another stream(attach = %s)",
                     attach ? "true" : "false");
        LLBC_PrintLn("stream1:%s", stream1.ToString().c_str());
        LLBC_PrintLn("stream2:%s", stream2.ToString().c_str());
        LLBC_ErrorAndReturnIfNot(stream2.IsAttach() == attach &&
                                 (attach ?
                                      stream1.GetBuf() == stream2.GetBuf() :
                                          stream2.GetBuf() != stream1.GetBuf()) &&
                                 stream2.GetReadPos() == stream1.GetReadPos() &&
                                 stream2.GetWritePos() == stream1.GetWritePos() &&
                                 (attach ?
                                      stream2.GetCap() == stream1.GetCap() : true) &&
                                  memcmp(stream2.GetBuf(),
                                         stream1.GetBuf(),
                                         stream1.GetWritePos()) == 0,
                                 LLBC_FAILED);

        int intVal;
        bool boolVal;
        LLBC_String strVal;
        stream2 >> boolVal >> intVal >> strVal;
        LLBC_ErrorAndReturnIfNot(boolVal == false &&
                                 intVal == 10086 &&
                                 strVal == "hello world",
                                 LLBC_FAILED);

        return LLBC_OK;
    };

    // Test copy construct.
    LLBC_ReturnIfNot(testCopyConstruct(true) == LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIfNot(testCopyConstruct(false) == LLBC_OK, LLBC_FAILED);

    // Test move construct.
    {
        LLBC_Stream stream1;
        stream1 << 3 << false << 6.3;

        LLBC_PrintLn("Move construct:");
        LLBC_PrintLn("- stream1:%s", stream1.ToString().c_str());

        LLBC_Stream stream1Copy(stream1, true);
        LLBC_Stream stream2(std::move(stream1));
        LLBC_PrintLn("- move to stream2:");
        LLBC_PrintLn("  - stream1:%s", stream1.ToString().c_str());
        LLBC_PrintLn("  - stream2:%s", stream2.ToString().c_str());

        LLBC_ErrorAndReturnIfNot(stream1.GetBuf() == nullptr &&
                                 stream1.GetCap() == 0 &&
                                 stream1.GetReadPos() == 0 &&
                                 stream1.GetWritePos() == 0,
                                 LLBC_FAILED);
        LLBC_ErrorAndReturnIfNot(stream2.GetBuf() == stream1Copy.GetBuf() &&
                                 stream2.GetCap() == stream1Copy.GetCap() &&
                                 stream2.GetWritePos() == stream1Copy.GetWritePos(),
                                 LLBC_FAILED);

        int intVal;
        bool boolVal;
        double dblVal;
        stream2 >> intVal >> boolVal >> dblVal;
        LLBC_ErrorAndReturnIfNot(intVal == 3 && boolVal == false && dblVal == 6.3,
                                 LLBC_FAILED);
    }

    // Define construct by buf and size test lambda.
    auto testConstructByBufAndSize = [](bool attach) {
        char buf[128] = "hello";
        const size_t size = sizeof(buf);
        LLBC_Stream stream(buf, size, attach);
        LLBC_PrintLn("Construct by buf and size(attach = %s):", attach ? "true" : "false");
        LLBC_PrintLn("- buf:%p, size:%lu", buf, size);
        LLBC_PrintLn("- stream:%s", stream.ToString().c_str());
        LLBC_ErrorAndReturnIfNot(stream.IsAttach() == attach &&
                                 stream.GetEndian() == LLBC_DefaultEndian &&
                                 (attach ? stream.GetBuf() == buf : stream.GetBuf() != buf) &&
                                 stream.GetReadPos() == 0 &&
                                 stream.GetWritePos() == size &&
                                 stream.GetCap() == size,
                                 LLBC_FAILED);

        LLBC_ErrorAndReturnIfNot(memcmp(stream.GetBuf(), "hello", sizeof("hello")) == 0,
                                 LLBC_FAILED);

        return LLBC_OK;
    };

    // Test construct by buf and size.
    LLBC_ReturnIfNot(testConstructByBufAndSize(true) == LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIfNot(testConstructByBufAndSize(false) == LLBC_OK, LLBC_FAILED);

    // Define construct by cap test lambda.
    auto testConstructByCap = [](size_t cap) {
        LLBC_Stream stream(cap);
        LLBC_PrintLn("Construct by cap:");
        LLBC_PrintLn("- cap:%lu", cap);
        LLBC_PrintLn("- cap lower 8 bit: %02lx", stream.GetCap() & 0xf);
        LLBC_PrintLn("- stream:%s", stream.ToString().c_str());

        LLBC_ErrorAndReturnIfNot((cap == 0 ?
                                      stream.GetBuf() == nullptr : stream.GetBuf() != nullptr) &&
                                      stream.GetCap() == cap &&
                                      stream.GetReadPos() == 0 &&
                                      stream.GetWritePos() == 0 &&
                                      stream.GetEndian() == LLBC_DefaultEndian,
                                      LLBC_FAILED);

        if (cap > 0)
        {
            LLBC_ErrorAndReturnIfNot(stream.SetWritePos(cap - 1), LLBC_FAILED);
            LLBC_ErrorAndReturnIfNot(stream.SetWritePos(cap), LLBC_FAILED);
            LLBC_ErrorAndReturnIfNot(!stream.SetWritePos(cap + 1), LLBC_FAILED);

            LLBC_ErrorAndReturnIfNot(stream.SetReadPos(stream.GetWritePos() - 1), LLBC_FAILED);
            LLBC_ErrorAndReturnIfNot(stream.SetReadPos(stream.GetWritePos()), LLBC_FAILED);
            LLBC_ErrorAndReturnIfNot(!stream.SetReadPos(stream.GetWritePos() + 1), LLBC_FAILED);
        }

        return LLBC_OK;
    };

    // Test construct by cap.
    LLBC_ReturnIfNot(testConstructByCap(0) == LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIfNot(testConstructByCap(3) == LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIfNot(testConstructByCap(9) == LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIfNot(testConstructByCap(16) == LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIfNot(testConstructByCap(18) == LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIfNot(testConstructByCap(128) == LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIfNot(testConstructByCap(299) == LLBC_OK, LLBC_FAILED);

    return LLBC_OK;
}

int TestCase_Com_StreamFunc::AttachTest()
{
    LLBC_PrintLn("Attach test:");

    // Simple test.
    {
        LLBC_PrintLn("- Simple test:");

        LLBC_Stream stream;
        LLBC_PrintLn("  - Default stream:%s", stream.ToString().c_str());
        LLBC_ErrorAndReturnIfNot(stream.IsAttach() == false, LLBC_FAILED);

        stream.SetAttach(true);
        LLBC_PrintLn("  - Set stream attach attr to true:%s", stream.ToString().c_str());
        LLBC_ErrorAndReturnIfNot(stream.IsAttach() == true, LLBC_FAILED);
    }

    // Test attach from another stream.
    {
        LLBC_PrintLn("- Test attach from another stream:");

        LLBC_Stream stream1;
        stream1 << 100 << false << 3.14 << "Hello World";
        stream1.SkipRead(sizeof(sint32));
        stream1.SetEndian(LLBC_DefaultEndian == LLBC_Endian::BigEndian ?
                              LLBC_Endian::LittleEndian : LLBC_Endian::BigEndian);
        LLBC_PrintLn("  - Original stream:%s", stream1.ToString().c_str());

        LLBC_Stream *stream2 = new LLBC_Stream;
        stream2->Attach(stream1);
        LLBC_PrintLn("  - Attach construct from origin stream:%s", stream2->ToString().c_str());
        LLBC_ErrorAndReturnIfNot(stream2->IsAttach() &&
                                 stream2->GetBuf() == stream1.GetBuf() &&
                                 stream2->GetCap() == stream1.GetCap() &&
                                 stream2->GetReadPos() == 0 &&
                                 stream2->GetWritePos() == stream1.GetWritePos() &&
                                 stream2->GetEndian() == stream1.GetEndian(),
                                 LLBC_FAILED);

        delete stream2;
        LLBC_PrintLn("  - Delete attach stream, original stream:%s", stream1.ToString().c_str());

        void *buf = stream1.Detach();
        LLBC_Defer(free(buf));
        LLBC_PrintLn("After detach original stream, stream:%s, detach buf:%p",
                     stream1.ToString().c_str(), buf);
        LLBC_ErrorAndReturnIfNot(buf != nullptr &&
                                 stream1.GetBuf() == nullptr &&
                                 stream1.GetCap() == 0 &&
                                 stream1.GetReadPos() == 0 &&
                                 stream1.GetWritePos() == 0 &&
                                 stream1.GetEndian() != LLBC_DefaultEndian,
                                 LLBC_FAILED);
    }

    // Test attach from buf.
    {
        LLBC_PrintLn("- Test attach from buf:");

        char buf[128];
        LLBC_Stream stream;
        stream.Attach(buf, sizeof(buf));
        LLBC_PrintLn("  - Attach from buf, buf:%p, bufSize:%lu, stream:%s",
                     buf, sizeof(buf), stream.ToString().c_str());
        LLBC_ErrorAndReturnIfNot(stream.GetBuf() == buf &&
                                 stream.GetCap() == sizeof(buf) &&
                                 stream.GetReadPos() == 0 &&
                                 stream.GetWritePos() == sizeof(buf),
                                 LLBC_FAILED);

        LLBC_ErrorAndReturnIfNot(stream.GetWritableSize() == 0, LLBC_FAILED);
    }

    // Test reattach.
    {
        LLBC_PrintLn("- Test reattach:");

        LLBC_Stream origStream;
        origStream << "Hello World";
        LLBC_PrintLn("  - Origin stream:%s", origStream.ToString().c_str());

        LLBC_Stream attStream1;
        attStream1.Attach(origStream);
        LLBC_PrintLn("  - attStream1(attach from origStream):%s",
                     attStream1.ToString().c_str());

        LLBC_Stream attStream2;
        attStream2.Attach(attStream1);
        LLBC_PrintLn("  - attStream2(attach from origStream):%s",
                     attStream2.ToString().c_str());
        LLBC_ErrorAndReturnIfNot(attStream1.IsAttach() &&
                                 attStream2.IsAttach() &&
                                 attStream2.GetBuf() == attStream1.GetBuf() &&
                                 attStream2.GetCap() == attStream1.GetCap() &&
                                 attStream2.GetReadPos() == 0 &&
                                 attStream2.GetWritePos() == attStream1.GetWritePos(),
                                 LLBC_FAILED);
    }

    return LLBC_OK;
}

int TestCase_Com_StreamFunc::SwapTest()
{
    LLBC_PrintLn("Swap test:");

    LLBC_Stream stream1;
    stream1.SetEndian(LLBC_Endian::BigEndian);
    stream1 << "Hello World" << false;

    LLBC_Stream stream2;
    stream2.SetEndian(LLBC_Endian::LittleEndian);
    stream2 << 3 << 4.5 << true << "Hey, Judy";

    const void *stream1Buf = stream1.GetBuf();
    const void *stream2Buf = stream2.GetBuf();
    const size_t stream1Cap = stream1.GetCap();
    const size_t stream2Cap = stream2.GetCap();
    const int stream1Endian = stream1.GetEndian();
    const int stream2Endian = stream2.GetEndian();
    const size_t stream1WritePos = stream1.GetWritePos();
    const size_t stream2WritePos = stream2.GetWritePos();
    const size_t stream1ReadPos = stream1WritePos / 2;
    const size_t stream2ReadPos = stream2WritePos / 2;

    stream1.SetReadPos(stream1ReadPos);
    stream2.SetReadPos(stream2ReadPos);

    LLBC_PrintLn("- Before swap:");
    LLBC_PrintLn("  - stream1: %s", stream1.ToString().c_str());
    LLBC_PrintLn("  - stream2: %s", stream2.ToString().c_str());

    stream1.Swap(stream2);

    LLBC_PrintLn("- After swap:");
    LLBC_PrintLn("  - stream1: %s", stream1.ToString().c_str());
    LLBC_PrintLn("  - stream2: %s", stream2.ToString().c_str());

    LLBC_ErrorAndReturnIfNot(stream1.GetBuf() == stream2Buf &&
                             stream2.GetBuf() == stream1Buf,
                             LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(stream1.GetCap() == stream2Cap &&
                             stream2.GetCap() == stream1Cap,
                             LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(stream1.GetEndian() == stream2Endian &&
                             stream2.GetEndian() == stream1Endian,
                             LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(stream1.GetReadPos() == stream2ReadPos &&
                             stream2.GetReadPos() == stream1ReadPos,
                             LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(stream1.GetWritePos() == stream2WritePos &&
                             stream2.GetWritePos() == stream1WritePos,
                             LLBC_FAILED);

    return LLBC_OK;
}

int TestCase_Com_StreamFunc::EndianTest()
{
    LLBC_PrintLn("Endian test:");

    // Test default stream endian.
    LLBC_Stream stream;
    LLBC_PrintLn("- Default stream endian is:%s", LLBC_Endian::Type2Str(stream.GetEndian()));
    LLBC_ErrorAndReturnIfNot(stream.GetEndian() == LLBC_DefaultEndian, LLBC_FAILED);

    // Test big endian.
    LLBC_Stream beStream;
    beStream.SetEndian(LLBC_Endian::BigEndian);
    beStream << 1;
    LLBC_PrintLn("- Big endian stream:%s", beStream.ToString().c_str());
    LLBC_ErrorAndReturnIfNot(beStream.GetBuf<uint8>()[0] == 0 &&
                             beStream.GetBuf<uint8>()[sizeof(int) - 1] == 1,
                             LLBC_FAILED);

    // Test little endian.
    LLBC_Stream leStream;
    leStream.SetEndian(LLBC_Endian::LittleEndian);
    leStream << 1;
    LLBC_PrintLn("- Little endian stream:%s", leStream.ToString().c_str());
    LLBC_ErrorAndReturnIfNot(leStream.GetBuf<uint8>()[0] == 1 &&
                             leStream.GetBuf<uint8>()[sizeof(int) - 1] == 0,
                             LLBC_FAILED);

    return LLBC_OK;
}

int TestCase_Com_StreamFunc::RWPosTest()
{
    LLBC_PrintLn("RWPos test:");

    // Test default stream cap/rpos/wpos.
    LLBC_Stream stream;
    LLBC_PrintLn("- Default stream:%s", stream.ToString().c_str());
    LLBC_ErrorAndReturnIfNot(stream.GetCap() == 0 &&
                             stream.GetReadPos() == 0 &&
                             stream.GetWritePos() == 0,
                             LLBC_FAILED);

    // Test Recap().
    LLBC_PrintLn("- Test recap");
    for (int i = 0; i < 10; ++i)
    {
        const size_t oldCap = stream.GetCap();
        const size_t recapSize = LLBC_Rand(2048);

        const bool recapRet = stream.Recap(recapSize);
        LLBC_PrintLn("  - Recap %lu -> %lu, ret:%s",
                     oldCap, recapSize, recapRet ? "true" : "false");
        LLBC_ErrorAndReturnIfNot(recapSize >= oldCap ? recapRet : !recapRet, LLBC_FAILED);
    }

    // Test SetWritePos().
    LLBC_PrintLn("- Test set wpos:");
    for (int i = 0; i < 11; ++i)
    {
        const size_t oldWPos = stream.GetWritePos();
        const size_t newWPos = i < 10 ? LLBC_Rand(stream.GetCap() * 2) : stream.GetCap();

        const bool setWPosRet = stream.SetWritePos(newWPos);
        LLBC_PrintLn("  - Set wpos %lu -> %lu, cap:%lu, ret:%s",
                     oldWPos, newWPos, stream.GetCap(), setWPosRet ? "true" : "false");
        LLBC_ErrorAndReturnIfNot(newWPos <= stream.GetCap() ? setWPosRet : !setWPosRet,
                                 LLBC_FAILED);
    }

    // Test SetReadPos().
    LLBC_PrintLn("- Test set rpos:");
    stream.SetWritePos(stream.GetCap() / 2);
    for (int i = 0; i < 11; ++i)
    {
        const size_t oldRPos = stream.GetReadPos();
        const size_t newRPos = i < 10 ? LLBC_Rand(stream.GetCap() + 1) : stream.GetWritePos();

        const bool setRPosRet = stream.SetReadPos(newRPos);
        LLBC_PrintLn("  - Set rpos %lu -> %lu, rpos:%lu, ret:%s",
                     oldRPos, newRPos, stream.GetWritePos(), setRPosRet ? "true" : "false");;
        LLBC_ErrorAndReturnIfNot(newRPos <= stream.GetWritePos() ? setRPosRet : !setRPosRet,
                                 LLBC_FAILED);
    }

    // Test auto normalize rpos.
    LLBC_PrintLn("- Test auto normalize rpos:");
    stream.Recap(4096);
    stream.SetWritePos(stream.GetCap() / 2);
    stream.SetReadPos(stream.GetWritePos() / 2);
    stream.SetWritePos(stream.GetReadPos() - 1);
    LLBC_PrintLn("  - Auto normalized rpos, stream:%s", stream.ToString().c_str());
    LLBC_ErrorAndReturnIfNot(stream.GetReadPos() == stream.GetWritePos(),
                             LLBC_FAILED);

    return LLBC_OK;
}

int TestCase_Com_StreamFunc::SkipRWTest()
{
    LLBC_PrintLn("Skip rpos/wpos test:");

    // Test skip default stream(cap:0, r/w pos:0).
    LLBC_Stream stream;
    auto skipReadRet = stream.SkipRead(20);
    auto skipWriteRet = stream.SkipWrite(40);
    LLBC_PrintLn("- Skip default stream read:%s", skipReadRet ? "true" : "false");
    LLBC_PrintLn("- Skip default stream write:%s", skipWriteRet ? "true" : "false");

    // Recap stream to 8192.
    stream.Recap(8192);

    // Test skip write..
    for (int i = 0; i < 10; ++i)
    {
        int skip = LLBC_Rand(-static_cast<int>(stream.GetCap()),
                         static_cast<int>(stream.GetCap() + 1));

        const auto newWPos = static_cast<sint64>(stream.GetWritePos()) + skip;
        const auto skipWRet = stream.SkipWrite(skip);
        LLBC_PrintLn("  - Skip write, skip:%d(to wpos:%lld), ret:%s, stream:%s",
                     skip, newWPos, skipWRet ? "true" : "false", stream.ToString().c_str());

        LLBC_ErrorAndReturnIfNot(newWPos < 0 ||
                                   newWPos > static_cast<sint64>(stream.GetCap()) ?
                                   !skipWRet : skipWRet,
                                 LLBC_FAILED);
    }

    // Set stream wpos to cap/2.
    stream.SetWritePos(stream.GetCap() / 2);

    // Test skip read.
    for (int i = 0; i < 10; ++i)
    {
        int skip = LLBC_Rand(-static_cast<int>(stream.GetWritePos()),
                         static_cast<int>(stream.GetWritePos() + 1));

        const auto newRPos = static_cast<sint64>(stream.GetReadPos()) + skip;
        const auto skipRRet = stream.SkipRead(skip);
        LLBC_PrintLn("  - Skip read, skip:%d(to rpos:%lld), ret:%s, stream:%s",
                     skip, newRPos, skipRRet ? "true" : "false", stream.ToString().c_str());

        LLBC_ErrorAndReturnIfNot(newRPos < 0 ||
                                     newRPos > static_cast<sint64>(stream.GetWritePos()) ?
                                     !skipRRet : skipRRet,
                                 LLBC_FAILED);
    }

    return LLBC_OK;
}

int TestCase_Com_StreamFunc::ReadableSizeTest()
{
    LLBC_PrintLn("Readable size test:");

    // Test default stream readable size.
    LLBC_Stream stream;
    LLBC_PrintLn("- Default stream readable size:%lu", stream.GetReadableSize());
    LLBC_ErrorAndReturnIfNot(stream.GetReadableSize() == 0, LLBC_FAILED);

    // Set cap&write pos to 4096.
    stream.Recap(4096);
    stream.SetWritePos(stream.GetCap());
    LLBC_PrintLn("  - Set stream cap&wpos to %lu for test", stream.GetWritePos());

    // Test.
    for (int i = 0; i < 10; ++i)
    {
        const size_t readPos = LLBC_Rand(0, static_cast<int>(stream.GetWritePos()) + 1);

        stream.SetReadPos(readPos);
        LLBC_PrintLn("  - Set read pos, readPos:%lu, readableSize:%lu",
                     readPos, stream.GetReadableSize());

        LLBC_ErrorAndReturnIfNot(stream.GetReadableSize() == stream.GetWritePos() - readPos,
                                 LLBC_FAILED);
    }

    return LLBC_OK;
}

int TestCase_Com_StreamFunc::WritableSizeTest()
{
    LLBC_PrintLn("Writable size test:");

    // Test default stream writable size.
    LLBC_Stream stream;
    LLBC_PrintLn("- Default stream writable size:%lu", stream.GetWritableSize());

    // Set cap pos to 4096.
    stream.Recap(4096);
    LLBC_PrintLn("  - Set stream cap to %lu for test", stream.GetCap());

    // Test.
    for (int i = 0; i < 10; ++i)
    {
        const size_t writePos = LLBC_Rand(0, static_cast<int>(stream.GetCap()) + 1);

        stream.SetWritePos(writePos);
        LLBC_PrintLn("  - Set write pos, writePos:%lu, writableSize:%lu",
                     writePos, stream.GetWritableSize());

        LLBC_ErrorAndReturnIfNot(stream.GetWritableSize() == stream.GetCap() - writePos,
                                 LLBC_FAILED);
    }

    return LLBC_OK;
}

int TestCase_Com_StreamFunc::GetBufTest()
{
    LLBC_PrintLn("GetBuf test:");

    // Test get default stream GetBuf().
    LLBC_Stream stream;
    LLBC_ErrorAndReturnIfNot(!stream.GetBuf(), LLBC_FAILED);
    LLBC_PrintLn("- Default stream GetBuf<void>() ret:%p", stream.GetBuf());
    LLBC_ErrorAndReturnIfNot(!stream.GetBuf<int>(), LLBC_FAILED);
    LLBC_PrintLn("- Default stream Getbuf<int>() ret:%p", stream.GetBuf<int>());

    // Normal test:
    stream.SetEndian(LLBC_Endian::LittleEndian);
    stream << 3 << "hello world";
    LLBC_ErrorAndReturnIfNot(stream.GetBuf(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(stream.GetBuf() == stream.GetBuf<int>(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(stream.GetBuf() == stream.GetBufStartWithReadPos(),
                             LLBC_FAILED);

    LLBC_ErrorAndReturnIfNot(stream.GetBufStartWithReadPos() != stream.GetBufStartWithWritePos(),
                             LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(stream.GetBufStartWithWritePos<char>() - stream.GetBuf<char>() == sizeof(int) + 15,
                             LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(*stream.GetBuf<int>() == 3, LLBC_FAILED);
    LLBC_PrintLn("- After write some bytes to stream, stream:%s", stream.ToString().c_str());

    int intVal;
    LLBC_String strVal;
    stream >> intVal >> strVal;
    LLBC_ErrorAndReturnIfNot(intVal == 3, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(strVal == "hello world", LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(stream.GetBufStartWithReadPos() == stream.GetBufStartWithWritePos(),
                             LLBC_FAILED);
    LLBC_PrintLn("- After read from stream, stream:%s, intVal:%d, strVal:%s",
                 stream.ToString().c_str(), intVal, strVal.c_str());

    // Read/Write begin buf test:
    stream.SetWritePos(3);
    stream.SetReadPos(1);
    LLBC_ErrorAndReturnIfNot(stream.GetBufStartWithWritePos<char>() - stream.GetBufStartWithReadPos<char>() == 2,
                             LLBC_FAILED);
    LLBC_PrintLn("- After set wpos=3/rpos=1, buf diff:%ld",
                 stream.GetBufStartWithWritePos<char>() - stream.GetBufStartWithReadPos<char>());

    return LLBC_OK;
}

int TestCase_Com_StreamFunc::BufOpTest()
{
    LLBC_PrintLn("Buf op test:");

    // Define dump stream lbda(as string).
    auto dumpStream = [](const LLBC_Stream& stream, const char *prompt) {
        LLBC_PrintLn("- %s", prompt);
        LLBC_PrintLn("  - stream:%s", stream.ToString().c_str());
        LLBC_PrintLn("  - str:%s", stream.GetBuf<char>());
    };

    // Insert "hello"
    LLBC_Stream stream;
    stream.Insert(0, "hello", sizeof("hello"));
    LLBC_ErrorAndReturnIfNot(stream.GetWritePos() == sizeof("hello"), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(LLBC_String("hello") == stream.GetBuf<char>(), LLBC_FAILED);
    dumpStream(stream, "After write \"hello\" str:");

    // Insert "xxx" after 'e' character.
    stream.SetReadPos(4);
    stream.Insert(2, "xxx", 3);
    LLBC_ErrorAndReturnIfNot(stream.GetWritePos() == 9, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(stream.GetReadPos() == 7, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(LLBC_String("xxxllo") == stream.GetBuf<char>() + 2, LLBC_FAILED);
    dumpStream(stream, "After insert \"xxx\" after 'e':");

    // Insert "0" at 0 pos.
    stream.Insert(0, "0", 1);
    LLBC_ErrorAndReturnIfNot(stream.GetWritePos() == 10, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(stream.GetBuf<char>()[0] == '0', LLBC_FAILED);
    dumpStream(stream, "After insert '0' at 0 pos:");

    // Insert "x" at buffer end.
    stream.Insert(LLBC_Stream::npos, "x", 1);
    LLBC_ErrorAndReturnIfNot(stream.GetWritePos() == 11, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(*(stream.GetBuf<char>() + 10) == 'x', LLBC_FAILED);
    dumpStream(stream, "After insert 'x' ad end pos:");

    // Erase string "hello" end \0.
    stream.Replace(9, 10, nullptr, 0);
    // Insert '\0' at the end of stream.
    stream.Insert(LLBC_Stream::npos, "", 1);
    LLBC_ErrorAndReturnIfNot(stream.GetWritePos() == 11, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(*(stream.GetBuf<char>() + 9) == 'x', LLBC_FAILED);
    dumpStream(stream, "After adjust '\0' pos:");

    // Erase "xxx".
    stream.SetReadPos(4);
    stream.Erase(3, 6);
    LLBC_ErrorAndReturnIfNot(stream.GetWritePos() == 8, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(stream.GetReadPos() == 3, LLBC_FAILED);
    dumpStream(stream, "After erase \"xxx\":");

    return LLBC_OK;
}

int TestCase_Com_StreamFunc::NumericSerTest()
{
    LLBC_PrintLn("Numeric type serialize test:");

    bool bVal1 = LLBC_BoolJudge(), bVal2 = false;
    sint8 s8Val1 = static_cast<sint8>(LLBC_Rand(std::numeric_limits<sint8>::min(), 
                                            std::numeric_limits<sint8>::max() + 1)), s8Val2 = 0;
    uint8 u8Val1 = static_cast<uint8>(LLBC_Rand(0,
                                            std::numeric_limits<uint8>::max() + 1)), u8Val2 = 0;
    sint16 s16Val1 = static_cast<sint16>(LLBC_Rand(std::numeric_limits<sint16>::min(), 
                                               std::numeric_limits<sint16>::max() + 1)), s16Val2 = 0;
    uint16 u16Val1 = static_cast<uint16>(LLBC_Rand(0,
                                               std::numeric_limits<uint16>::max() + 1)), u16Val2 = 0;
    sint32 s32Val1 = LLBC_Rand(std::numeric_limits<sint32>::min(), std::numeric_limits<sint32>::max()), s32Val2 = 0;
    uint32 u32Val1 = LLBC_Rand(0, std::numeric_limits<sint32>::max()), u32Val2 = 0;
    long lVal1 = LLBC_Rand(), lVal2 = 0;
    ulong ulVal1 = static_cast<ulong>(LLBC_Rand()), ulVal2 = 0;
    sint64 s64Val1 = (static_cast<sint64>(LLBC_Rand()) << 32) | (static_cast<sint64>(LLBC_Rand())), s64Val2 = 0;
    uint64 u64Val1 = (static_cast<uint64>(LLBC_Rand()) << 32) | (static_cast<uint64>(LLBC_Rand())), u64Val2 = 0;
    float fVal1 = static_cast<float>(LLBC_RandReal()), fVal2 = 0.0f;
    double dVal1 = LLBC_RandReal(), dVal2 = 0.0;

    LLBC_Stream s;
    s << bVal1 >> bVal2
        << s8Val1 >> s8Val2
        << u8Val1 >> u8Val2
        << s16Val1 >> s16Val2
        << u16Val1 >> u16Val2
        << s32Val1 >> s32Val2
        << u32Val1 >> u32Val2
        << lVal1 >> lVal2
        << ulVal1 >> ulVal2
        << s64Val1 >> s64Val2
        << u64Val1 >> u64Val2
        << fVal1 >> fVal2
        << dVal1 >> dVal2;

    LLBC_ErrorAndReturnIfNot(bVal1 == bVal2 &&
                             s8Val1 == s8Val2 &&
                             u8Val1 == u8Val2 &&
                             s16Val1 == s16Val2 &&
                             u16Val1 == u16Val2 &&
                             s32Val1 == s32Val2 &&
                             u32Val1 == u32Val2 &&
                             lVal1 == lVal2 &&
                             ulVal1 == ulVal2 &&
                             s64Val1 == s64Val2 &&
                             u64Val1 == u64Val2 &&
                             std::fabs(fVal1 - fVal2) < std::numeric_limits<float>::epsilon() &&
                             std::fabs(dVal1- dVal2) < std::numeric_limits<double>::epsilon(),
                             LLBC_FAILED);

    LLBC_ErrorAndReturnIfNot(s.GetWritePos() ==
                             sizeof(bool) +
                             sizeof(sint8) + sizeof(uint8) +
                             sizeof(sint16) + sizeof(uint16) +
                             sizeof(sint32) + sizeof(uint32) +
                             sizeof(long) + sizeof(ulong) +
                             sizeof(sint64) + sizeof(uint64) +
                             sizeof(float) + sizeof(double),
                             LLBC_FAILED);

    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);

    LLBC_PrintLn("- sint8: %d, %d", s8Val1, s8Val2);
    LLBC_PrintLn("- uint8: %u, %u", u8Val1, u8Val2);
    LLBC_PrintLn("- sint16: %d, %d", s16Val1, s16Val2);
    LLBC_PrintLn("- uint16: %u, %u", u16Val1, u16Val2);
    LLBC_PrintLn("- sint32: %d, %d", s32Val1, s32Val2);
    LLBC_PrintLn("- uint32: %u, %u", u32Val1, u32Val2);
    LLBC_PrintLn("- long: %ld, %ld", lVal1, lVal2);
    LLBC_PrintLn("- long: %lu, %lu", ulVal1, ulVal2);
    LLBC_PrintLn("- long: %ld, %ld", lVal1, lVal2);
    LLBC_PrintLn("- long: %lu, %lu", ulVal1, ulVal2);

    return LLBC_OK;
}

int TestCase_Com_StreamFunc::StrSerTest()
{
    LLBC_PrintLn("String serialize test:");

    // Test std::string ser/deser.
    LLBC_Stream s;
    std::string stdStr1("Hello world"), stdStr2;
    s << stdStr1 >> stdStr2;
    LLBC_ErrorAndReturnIfNot(stdStr2 == stdStr1, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == stdStr1.size() + sizeof(uint32), LLBC_FAILED);
    LLBC_PrintLn("- std::string ser/deser:");
    LLBC_PrintLn("  - stdStr1: %s", stdStr1.c_str());
    LLBC_PrintLn("  - stdStr2: %s", stdStr2.c_str());

    // Test LLBC_String ser/deser.
    s.Clear();
    LLBC_String llbcStr1("Hey Judy"), llbcStr2;
    s << llbcStr1 >> llbcStr2;
    LLBC_ErrorAndReturnIfNot(llbcStr1 == llbcStr2, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == llbcStr1.size() + sizeof(uint32), LLBC_FAILED);
    LLBC_PrintLn("- LLBC_String ser/deser:");
    LLBC_PrintLn("  - llbcStr1: %s", llbcStr1.c_str());
    LLBC_PrintLn("  - llbcStr2: %s", llbcStr2.c_str());

    // Test literal string ser/deser.
    s.Clear();
    char literalStr1[] = "I am literal string";
    char literalStr2[sizeof(literalStr1)];
    s << literalStr1 >> literalStr2;
    LLBC_ErrorAndReturnIfNot(memcmp(literalStr1, literalStr2, strlen(literalStr1)) == 0,
                             LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == sizeof(literalStr1) - 1 + sizeof(uint32), LLBC_FAILED);
    LLBC_PrintLn("- literal string ser/deser:");
    LLBC_PrintLn("  - literalStr1: %s", literalStr1);
    LLBC_PrintLn("  - literalStr2: %s", literalStr2);

    // Test std::string -> LLBC_String.
    s.Clear();
    LLBC_String llbcStr3;
    s << stdStr1 >> llbcStr3;
    LLBC_ErrorAndReturnIfNot(llbcStr3 == stdStr1, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == stdStr1.size() + sizeof(uint32), LLBC_FAILED);
    LLBC_PrintLn("- std::string -> LLBC_String:");
    LLBC_PrintLn("  - stdStr1: %s", stdStr1.c_str());
    LLBC_PrintLn("  - llbcStr3: %s", llbcStr3.c_str());

    // Test std::string -> literal string.
    s.Clear();
    char literalStr3[sizeof(literalStr1)];
    s << stdStr1 >> literalStr3;
    LLBC_ErrorAndReturnIfNot(stdStr1 == literalStr3, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == stdStr1.size() + sizeof(uint32), LLBC_FAILED);
    LLBC_PrintLn("- std::string -> literal string:");
    LLBC_PrintLn("  - stdStr1: %s", stdStr1.c_str());
    LLBC_PrintLn("  - literalStr3: %s", literalStr3);

    // Test LLBC_String -> std::string.
    s.Clear();
    std::string stdStr3;
    s << llbcStr1 >> stdStr3;
    LLBC_ErrorAndReturnIfNot(stdStr3 == llbcStr1, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == llbcStr1.size() + sizeof(uint32), LLBC_FAILED);
    LLBC_PrintLn("- LLBC_String -> std::string:");
    LLBC_PrintLn("  - llbcStr1: %s", llbcStr1.c_str());
    LLBC_PrintLn("  - stdStr3: %s", stdStr3.c_str());

    // Test LLBC_String -> literal string.
    s.Clear();
    char literalStr4[32];
    s << llbcStr1 >> literalStr4;
    LLBC_ErrorAndReturnIfNot(llbcStr1 == literalStr4, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == llbcStr1.size() + sizeof(uint32), LLBC_FAILED);
    LLBC_PrintLn("- LLBC_String -> literal string:");
    LLBC_PrintLn("  - llbcStr1: %s", llbcStr1.c_str());
    LLBC_PrintLn("  - literalStr4: %s", literalStr4);

    // Test literal string -> std::string.
    s.Clear();
    std::string stdStr4;
    s << literalStr1 >> stdStr4;
    LLBC_ErrorAndReturnIfNot(stdStr4 == literalStr1, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == stdStr4.size() + sizeof(uint32), LLBC_FAILED);
    LLBC_PrintLn("- literal string -> std::string:");
    LLBC_PrintLn("  - literalStr1: %s", literalStr1);
    LLBC_PrintLn("  - stdStr4: %s", stdStr4.c_str());

    // Test literal string -> LLBC_String.
    s.Clear();
    LLBC_String llbcStr4;
    s << literalStr1 >> llbcStr4;
    LLBC_ErrorAndReturnIfNot(llbcStr4 == literalStr1, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == llbcStr4.size() + sizeof(uint32), LLBC_FAILED);
    LLBC_PrintLn("- literal string -> LLBC_String:");
    LLBC_PrintLn("  - literalStr1: %s", literalStr1);
    LLBC_PrintLn("  - llbcStr4: %s", llbcStr4.c_str());

    // literal string overflow read test.
    s.Clear();
    s << "hello world";
    char literalStr5[3];
    bool readRet = s.Read(literalStr5);
    LLBC_ErrorAndReturnIfNot(!readRet, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == 0, LLBC_FAILED);
    LLBC_PrintLn("- literal string overflow read test:");
    LLBC_PrintLn("  - string in stream: hello world");
    LLBC_PrintLn("  - Read() return: %s", readRet ? "true" : "false");

    return LLBC_OK;
}

template <typename Container>
typename std::enable_if<LLBC_IsTemplSpec<Container, std::vector>::value ||
                        LLBC_IsTemplSpec<Container, std::list>::value ||
                        LLBC_IsTemplSpec<Container, std::deque>::value, int>::type
_StlContainerSerTest()
{
    LLBC_PrintLn("- %s ser/deser test:", LLBC_GetTypeName(Container));

    Container c1, c2;
    const int contSize = LLBC_Rand(10, 20);
    for (int i = 0; i < contSize; ++i)
        c1.emplace_back(LLBC_Rand(10000));

    LLBC_Stream s;
    s << c1 >> c2;
    LLBC_ErrorAndReturnIfNot(c1 == c2, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetWritePos() ==
                                 sizeof(uint32) + sizeof(int) * contSize,
                             LLBC_FAILED);
    LLBC_PrintLn("  - c1: %s", LLBC_Variant(c1).ValueToString().c_str());
    LLBC_PrintLn("  - c2: %s", LLBC_Variant(c2).ValueToString().c_str());

    return LLBC_OK;
}

template <typename Container>
typename std::enable_if<LLBC_IsTemplSpec<Container, std::set>::value ||
                        LLBC_IsTemplSpec<Container, std::unordered_set>::value, int>::type
_StlContainerSerTest()
{
    LLBC_PrintLn("- %s ser/deser test:", LLBC_GetTypeName(Container));

    Container c1, c2;
    const int contSize = LLBC_Rand(10, 20);
    for (int i = 0; i < contSize; ++i)
        c1.insert(LLBC_Rand(10000));

    LLBC_Stream s;
    s << c1 >> c2;
    LLBC_ErrorAndReturnIfNot(c1 == c2, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetWritePos() ==
                                 sizeof(uint32) + sizeof(int) * contSize,
                             LLBC_FAILED);
    LLBC_PrintLn("  - c1: %s", LLBC_Variant(c1).ValueToString().c_str());
    LLBC_PrintLn("  - c2: %s", LLBC_Variant(c2).ValueToString().c_str());

    return LLBC_OK;
}

template <typename Container>
typename std::enable_if<LLBC_IsTemplSpec<Container, std::map>::value ||
                        LLBC_IsTemplSpec<Container, std::unordered_map>::value, int>::type
_StlContainerSerTest()
{
    LLBC_PrintLn("- %s ser/deser test:", LLBC_GetTypeName(Container));

    Container c1, c2;
    const int contSize = LLBC_Rand(10, 20);
    for (int i = 0; i < contSize; ++i)
        c1.emplace(LLBC_Rand(10000), LLBC_Rand(20000));

    LLBC_Stream s;
    s << c1 >> c2;
    LLBC_ErrorAndReturnIfNot(c1 == c2, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetWritePos() ==
                                 sizeof(uint32) + (sizeof(typename Container::key_type) +
                                    sizeof(typename Container::mapped_type)) * contSize,
                             LLBC_FAILED);
    LLBC_PrintLn("  - c1: %s", LLBC_Variant(c1).ValueToString().c_str());
    LLBC_PrintLn("  - c2: %s", LLBC_Variant(c2).ValueToString().c_str());

    return LLBC_OK;
}

template <typename Container>
typename std::enable_if<LLBC_IsSTLArraySpec<Container, std::array>::value, int>::type
_StlContainerSerTest()
{
    LLBC_PrintLn("- %s ser/deser test:", LLBC_GetTypeName(Container));

    Container c1, c2;
    for (int i = 0; i < std::tuple_size<Container>::value; ++i)
        c1.at(i) = LLBC_Rand(10000);

    LLBC_Stream s;
    s << c1 >> c2;
    LLBC_ErrorAndReturnIfNot(c1 == c2, LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);
    LLBC_ErrorAndReturnIfNot(s.GetWritePos() ==
                                 sizeof(uint32) + sizeof(typename Container::value_type) *
                                    std::tuple_size<Container>::value,
                             LLBC_FAILED);

    return LLBC_OK;
}

int TestCase_Com_StreamFunc::StlContainerSerTest()
{
    LLBC_PrintLn("STL Container serialize test:");

    // Simple stl container test.
    {
        LLBC_ReturnIf(_StlContainerSerTest<std::vector<int> >() != LLBC_OK, LLBC_FAILED);
        LLBC_ReturnIf(_StlContainerSerTest<std::list<int> >() != LLBC_OK, LLBC_FAILED);
        LLBC_ReturnIf(_StlContainerSerTest<std::deque<int> >() != LLBC_OK, LLBC_FAILED);
        LLBC_ReturnIf(_StlContainerSerTest<std::set<int> >() != LLBC_OK, LLBC_FAILED);
        LLBC_ReturnIf(_StlContainerSerTest<std::unordered_set<int> >() != LLBC_OK, LLBC_FAILED);
        if (_StlContainerSerTest<std::map<int, int> >() != LLBC_OK)
            return LLBC_FAILED;
        if (_StlContainerSerTest<std::unordered_map<int, int> >() != LLBC_OK)
            return LLBC_FAILED;
        if (_StlContainerSerTest<std::array<int, 20> >() != LLBC_OK)
            return LLBC_FAILED;

        LLBC_PrintLn("- std::tuple ser/deser test:");
        LLBC_Stream s;
        std::tuple<int, uint32, int> tup1(3, 4, 5), tup2;
        s << tup1 >> tup2;
        LLBC_ErrorAndReturnIfNot(tup1 == tup2, LLBC_FAILED);
        LLBC_ErrorAndReturnIfNot(s.GetWritePos() == sizeof(uint32) + sizeof(int) * 3, LLBC_FAILED);
    }

    // Nested stl container test.
    {
        LLBC_PrintLn("- Nested stl container ser/deser test:");
        std::map<int, std::vector<std::list<int> > > nestedCont1, nestedCont2;
        const size_t nest1Size = LLBC_Rand(5, 10);
        const size_t nest2Size = LLBC_Rand(5, 10);
        const size_t nest3Size = LLBC_Rand(10, 20);
        for (size_t i = 0; i < nest1Size; ++i)
        {
            auto &nest1 = nestedCont1.emplace(
                LLBC_Rand(100000), std::vector<std::list<int> >()).first->second;
            for (size_t j = 0; j < nest2Size; ++j)
            {
                nest1.push_back(std::list<int>());
                auto& nest2 = *--nest1.end();
                for (size_t k = 0; k < nest3Size; ++k)
                    nest2.push_back(LLBC_Rand(200000));
            }
        }

        LLBC_Stream s;
        s << nestedCont1 >> nestedCont2;
        LLBC_ErrorAndReturnIfNot(nestedCont1 == nestedCont2, LLBC_FAILED);
        LLBC_ErrorAndReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED);
        LLBC_PrintLn("- nestedCont1: %s", LLBC_Variant(nestedCont1).ToString().c_str());
        LLBC_PrintLn("- nestedCont2: %s", LLBC_Variant(nestedCont2).ToString().c_str());
    }

    return LLBC_OK;
}

int TestCase_Com_StreamFunc::SerializableObjSerTest()
{
    LLBC_PrintLn("Serializable obj ser/deser test:");

    // Define base seriazable class.
    struct _BaseSerCls
    {
        int intVal = 0;
        double dblVal = 0;
        LLBC_String strVal = 0;

        mutable bool serCalled = false;
        bool deserCalled = false;

    protected:
        void _Ser(LLBC_Stream &s) const
        {
            serCalled = true;
            s << intVal << dblVal << strVal;
        }

        bool _Deser(LLBC_Stream &s)
        {
            deserCalled = true;
            LLBC_ReturnIfNot(s.Read(intVal), false);
            LLBC_ReturnIfNot(s.Read(dblVal), false);
            LLBC_ReturnIfNot(s.Read(strVal), false);

            return true;
        }
    };

    // Define test code gen macro.
    #define __SERIALIZABLE_TEST_CODE_GEN(clsName, serMeth, deserMeth) { \
            struct clsName : public _BaseSerCls { \
                clsName(int intVal, double dblVal, const LLBC_String &strVal) { \
                    this->intVal = intVal; \
                    this->dblVal = dblVal; \
                    this->strVal = strVal; \
                } \
                 \
                void serMeth(LLBC_Stream &s) const { \
                    LLBC_PrintLn(" - " #clsName "::%s Called", __FUNCTION__); \
                    _Ser(s); \
                } \
                 \
                bool deserMeth(LLBC_Stream &s) { \
                    LLBC_PrintLn(" - " #clsName "::%s Called", __FUNCTION__); \
                    return _Deser(s); \
                } \
                 \
                bool operator==(const clsName &another) const { \
                    return intVal == another.intVal && \
                        std::fabs(dblVal - another.dblVal) < std::numeric_limits<double>::epsilon() && \
                        strVal == another.strVal; \
                } \
            }; \
             \
            LLBC_PrintLn("- %s/%s call test:", #serMeth, #deserMeth); \
             \
            LLBC_Stream s; \
            clsName obj1(LLBC_Rand(10, 20), LLBC_RandReal(), "hello world"), obj2(0, 0.0, ""); \
            s << obj1 >> obj2; \
            LLBC_ReturnIfNot(obj1 == obj2, LLBC_FAILED); \
            LLBC_ReturnIfNot(obj1.serCalled && obj2.deserCalled, LLBC_FAILED); \
            LLBC_ReturnIfNot(s.GetReadPos() == s.GetWritePos(), LLBC_FAILED); \
            LLBC_PrintLn("  - obj1: %d, %f, %s", obj1.intVal, obj1.dblVal, obj1.strVal.c_str()); \
            LLBC_PrintLn("  - obj2: %d, %f, %s", obj2.intVal, obj2.dblVal, obj2.strVal.c_str()); \
        } \

    // Ser/Deser.
    __SERIALIZABLE_TEST_CODE_GEN(SerCls, Ser, Deser);
    // Ser/DeSer.
    __SERIALIZABLE_TEST_CODE_GEN(SerCls, Ser, DeSer);
    // ser/deser.
    __SERIALIZABLE_TEST_CODE_GEN(SerCls, ser, deser);
    // Serialize/Deserialize.
    __SERIALIZABLE_TEST_CODE_GEN(SerCls, Serialize, Deserialize);
    // Serialize/DeSerialize.
    __SERIALIZABLE_TEST_CODE_GEN(SerCls, Serialize, DeSerialize);
    // serialize/deserialize.
    __SERIALIZABLE_TEST_CODE_GEN(SerCls, serialize, deserialize);

    // Undef code gen macro.
    #undef __SERIALIZABLE_TEST_CODE_GEN

    return LLBC_OK;
}
