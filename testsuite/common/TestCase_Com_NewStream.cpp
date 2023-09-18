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
    LLBC_LogAndReturnIf(CtorTest() != LLBC_OK, Error, LLBC_FAILED);

    return LLBC_OK;
}

int TestCase_Com_NewStream::CtorTest()
{
    LLBC_PrintLn("Constructor test:");

    // Test default constructor.
    LLBC_Stream stream;
    LLBC_PrintLn("Construct by default ctor:\n- %s", stream.ToString().c_str());
    LLBC_LogAndReturnIfNot(stream.GetBuf() == nullptr &&
                           stream.GetCap() == 0 &&
                           stream.GetWritePos() == 0 &&
                           stream.GetReadableSize() == 0 &&
                           stream.GetWritableSize() == 0 &&
                           stream.GetEndian() == LLBC_DefaultEndian &&
                           !stream.IsAttach(),
                           Error,
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
        LLBC_LogAndReturnIfNot(stream2.IsAttach() == attach &&
                               (attach ?
                                    stream1.GetBuf() == stream2.GetBuf() :
                                        stream2.GetBuf() != stream1.GetBuf()) &&
                               stream2.GetReadPos() == stream1.GetReadPos() &&
                               stream2.GetWritePos() == stream1.GetWritePos() &&
                               stream2.GetCap() == stream1.GetCap() &&
                               memcmp(stream2.GetBuf(),
                                      stream1.GetBuf(),
                                      stream1.GetWritePos()) == 0,
                               Error,
                               LLBC_FAILED);

        int intVal;
        bool boolVal;
        LLBC_String strVal;
        stream2 >> boolVal >> intVal >> strVal;
        LLBC_LogAndReturnIfNot(boolVal == false &&
                               intVal == 10086 &&
                               strVal == "hello world",
                               Error,
                               LLBC_FAILED);

        return LLBC_OK;
    };

    // Test copy construct.
    LLBC_ReturnIfNot(testCopyConstruct(true) == LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(testCopyConstruct(false) == LLBC_OK, LLBC_FAILED);

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

        LLBC_LogAndReturnIfNot(stream1.GetBuf() == nullptr &&
                               stream1.GetCap() == 0 &&
                               stream1.GetReadPos() == 0 &&
                               stream1.GetWritePos() == 0,
                               Error,
                               LLBC_FAILED);
        LLBC_LogAndReturnIfNot(stream2.GetBuf() == stream1Copy.GetBuf() &&
                               stream2.GetCap() == stream1Copy.GetCap() &&
                               stream2.GetWritePos() == stream1Copy.GetWritePos(),
                               Error,
                               LLBC_FAILED);

        int intVal;
        bool boolVal;
        double dblVal;
        stream2 >> intVal >> boolVal >> dblVal;
        LLBC_LogAndReturnIfNot(
            intVal == 3 && boolVal == false && dblVal == 6.3,
            Error,
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
        LLBC_LogAndReturnIf(!(stream.IsAttach() == attach &&
                              stream.GetEndian() == LLBC_DefaultEndian &&
                              (attach ? stream.GetBuf() == buf : stream.GetBuf() != buf) &&
                              stream.GetReadPos() == 0 &&
                              stream.GetWritePos() == size &&
                              stream.GetCap() == size), Error, LLBC_FAILED);

        LLBC_LogAndReturnIf(memcmp(stream.GetBuf(), "hello", sizeof("hello")) != 0,
                            Error,
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

        LLBC_LogAndReturnIfNot(
            (cap == 0 ? stream.GetBuf() == nullptr : stream.GetBuf() != nullptr) &&
            stream.GetCap() == cap &&
            stream.GetReadPos() == 0 &&
            stream.GetWritePos() == 0 &&
            stream.GetEndian() == LLBC_DefaultEndian,
            Error,
            LLBC_FAILED);

        if (cap > 0)
        {
            LLBC_LogAndReturnIfNot(stream.SetWritePos(cap - 1), Error, LLBC_FAILED);
            LLBC_LogAndReturnIfNot(stream.SetWritePos(cap), Error, LLBC_FAILED);
            LLBC_LogAndReturnIfNot(!stream.SetWritePos(cap + 1), Error, LLBC_FAILED);

            LLBC_LogAndReturnIfNot(stream.SetReadPos(stream.GetWritePos() - 1), Error, LLBC_FAILED);
            LLBC_LogAndReturnIfNot(stream.SetReadPos(stream.GetWritePos()), Error, LLBC_FAILED);
            LLBC_LogAndReturnIfNot(!stream.SetReadPos(stream.GetWritePos() + 1), Error, LLBC_FAILED);
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
        LLBC_LogAndReturnIfNot(stream.IsAttach() == false, Error, LLBC_FAILED);

        stream.SetAttach(true);
        LLBC_PrintLn("  - Set stream attach attr to true:%s", stream.ToString().c_str());
        LLBC_LogAndReturnIfNot(stream.IsAttach() == true, Error, LLBC_FAILED);
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
        LLBC_LogAndReturnIfNot(stream2->IsAttach() &&
                               stream2->GetBuf() == stream1.GetBuf() &&
                               stream2->GetCap() == stream1.GetCap() &&
                               stream2->GetReadPos() == 0 &&
                               stream2->GetWritePos() == stream1.GetWritePos() &&
                               stream2->GetEndian() != stream1.GetEndian(),
                               Error,
                               LLBC_FAILED);

        delete stream2;
        LLBC_PrintLn("  - Delete attach stream, original stream:%s", stream1.ToString().c_str());

        void *buf = stream1.Detach();
        LLBC_Defer(free(buf));
        LLBC_PrintLn("After detach original stream, stream:%s, detach buf:%p",
                     stream1.ToString().c_str(), buf);
        LLBC_LogAndReturnIfNot(buf != nullptr &&
                               stream1.GetBuf() == nullptr &&
                               stream1.GetCap() == 0 &&
                               stream1.GetReadPos() == 0 &&
                               stream1.GetWritePos() == 0 &&
                               stream1.GetEndian() != LLBC_DefaultEndian,
                               Error,
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
        LLBC_LogAndReturnIfNot(stream.GetBuf() == buf &&
                               stream.GetCap() == sizeof(buf) &&
                               stream.GetReadPos() == 0 &&
                               stream.GetWritePos() == sizeof(buf),
                               Error,
                               LLBC_FAILED);

        LLBC_LogAndReturnIfNot(stream.GetWritableSize() == 0, Error, LLBC_FAILED);
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
        LLBC_LogAndReturnIfNot(attStream1.IsAttach() &&
                               attStream2.IsAttach() &&
                               attStream2.GetBuf() == attStream1.GetBuf() &&
                               attStream2.GetCap() == attStream1.GetCap() &&
                               attStream2.GetReadPos() == 0 &&
                               attStream2.GetWritePos() == attStream1.GetWritePos(),
                               Error,
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
    const size_t stream1WritePos = stream1.GetWritePos();
    const size_t stream2WritePos = stream2.GetWritePos();
    const size_t stream1ReadPos = stream1WritePos / 2;
    const size_t stream2ReadPos = stream2WritePos / 2;

    stream1.SetReadPos(stream1ReadPos);
    stream2.SetReadPos(stream2ReadPos);

    LLBC_PrintLn("- stream1: %s", stream1.ToString().c_str());
    LLBC_PrintLn("- stream2: %s", stream2.ToString().c_str());

    // TODO:l

    return LLBC_OK;
}
