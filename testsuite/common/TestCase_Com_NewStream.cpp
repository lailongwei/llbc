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


#include "common/TestCase_Com_NewStream.h"

int TestCase_Com_NewStream::Run(int argc, char *argv[])
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

    return LLBC_OK;
}

int TestCase_Com_NewStream::CtorTest()
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

int TestCase_Com_NewStream::AttachTest()
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

int TestCase_Com_NewStream::SwapTest()
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

int TestCase_Com_NewStream::EndianTest()
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

int TestCase_Com_NewStream::RWPosTest()
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

int TestCase_Com_NewStream::SkipRWTest()
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

int TestCase_Com_NewStream::ReadableSizeTest()
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

int TestCase_Com_NewStream::WritableSizeTest()
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
