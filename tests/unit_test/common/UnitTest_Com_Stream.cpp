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

#include <llbc.h>
using namespace llbc;

#include <gtest/gtest.h>

// =============================================================================
// Test helpers (custom serializable types for reflection tests).
// =============================================================================
namespace
{
    // Type with Serialize/DeSerialize (upper camel case).
    struct UCCTypeA
    {
        int a = 0;
        int b = 0;

        void Serialize(LLBC_Stream &s) const
        {
            s << a << b;
        }

        bool DeSerialize(LLBC_Stream &s)
        {
            return s.Read(a) && s.Read(b);
        }
    };

    // Type with Ser/DeSer (upper camel case, short form).
    struct UCCShortTypeB
    {
        int a = 0;
        int b = 0;

        void Ser(LLBC_Stream &s) const
        {
            s << a << b;
        }

        bool DeSer(LLBC_Stream &s)
        {
            return s.Read(a) && s.Read(b);
        }
    };

    // Type with serialize/deserialize (lower camel case).
    struct LCCTypeC
    {
        int a = 0;
        int b = 0;

        void serialize(LLBC_Stream &s) const
        {
            s << a << b;
        }

        bool deserialize(LLBC_Stream &s)
        {
            return s.Read(a) && s.Read(b);
        }
    };

    // Type with ser/deser (lower camel case, short form).
    struct LCCShortTypeD
    {
        int a = 0;
        int b = 0;

        void ser(LLBC_Stream &s) const
        {
            s << a << b;
        }

        bool deser(LLBC_Stream &s)
        {
            return s.Read(a) && s.Read(b);
        }
    };

    // Trivial POD, no serialization method - fallback to memcpy.
    struct TrivialPOD
    {
        int a;
        int b;
        double c;

        bool operator==(const TrivialPOD &o) const
        {
            return a == o.a && b == o.b && c == o.c;
        }
    };

    // Type with DeSerialize returning false - always fails.
    struct FailDeser
    {
        int a = 0;

        void Serialize(LLBC_Stream &s) const
        {
            s << a;
        }

        bool DeSerialize(LLBC_Stream &)
        {
            return false;
        }
    };

    // Plain enum.
    enum PlainEnum : int
    {
        PE_None = 0,
        PE_One = 1,
        PE_Big = 0x11223344,
    };

    // Scoped enum with underlying int16.
    enum class ScopedEnum : sint16
    {
        None = 0,
        Foo = 100,
        Bar = 0x0102,
    };
} // anonymous namespace


// =============================================================================
// [Global] Stream global constants.
// =============================================================================
TEST(StreamTest, NposConstant)
{
    // LLBC_Stream::npos should equal std::numeric_limits<size_t>::max().
    ASSERT_EQ(LLBC_Stream::npos, (std::numeric_limits<size_t>::max)());
}


// =============================================================================
// [Ctor / Dtor] Constructors and destructor.
// =============================================================================
TEST(StreamTest, DefaultCtor)
{
    LLBC_Stream stream;

    ASSERT_EQ(stream.GetBuf(), nullptr);
    ASSERT_EQ(stream.GetCap(), 0lu);
    ASSERT_EQ(stream.GetReadPos(), 0lu);
    ASSERT_EQ(stream.GetWritePos(), 0lu);
    ASSERT_EQ(stream.GetReadableSize(), 0lu);
    ASSERT_EQ(stream.GetWritableSize(), 0lu);
    ASSERT_FALSE(stream.IsAttach());
    ASSERT_EQ(stream.GetEndian(), LLBC_DefaultEndian);
    ASSERT_EQ(stream.GetBufStartWithReadPos(), nullptr);
    ASSERT_EQ(stream.GetBufStartWithWritePos(), nullptr);
}

TEST(StreamTest, CapCtor)
{
    constexpr size_t cap = 64;
    LLBC_Stream stream(cap);

    ASSERT_NE(stream.GetBuf(), nullptr);
    ASSERT_EQ(stream.GetCap(), cap);
    ASSERT_EQ(stream.GetReadPos(), 0lu);
    ASSERT_EQ(stream.GetWritePos(), 0lu);
    ASSERT_EQ(stream.GetReadableSize(), 0lu);
    ASSERT_EQ(stream.GetWritableSize(), cap);
    ASSERT_FALSE(stream.IsAttach());
    ASSERT_EQ(stream.GetEndian(), LLBC_DefaultEndian);
}

TEST(StreamTest, CapCtorZero)
{
    // cap = 0 -> buf remains nullptr.
    LLBC_Stream stream(0);

    ASSERT_EQ(stream.GetBuf(), nullptr);
    ASSERT_EQ(stream.GetCap(), 0lu);
    ASSERT_FALSE(stream.IsAttach());
}

TEST(StreamTest, AttachBufCtor)
{
    char buf[16];
    memset(buf, 0, sizeof(buf));

    LLBC_Stream stream(buf, sizeof(buf), true);
    ASSERT_TRUE(stream.IsAttach());
    ASSERT_EQ(stream.GetBuf(), buf);
    ASSERT_EQ(stream.GetCap(), sizeof(buf));
    ASSERT_EQ(stream.GetWritePos(), sizeof(buf));
    ASSERT_EQ(stream.GetReadPos(), 0lu);
    ASSERT_EQ(stream.GetReadableSize(), sizeof(buf));
    ASSERT_EQ(stream.GetWritableSize(), 0lu);
}

TEST(StreamTest, CopyBufCtor)
{
    // Constructor with attach=false will copy the buffer.
    const char src[] = "hello llbc";
    const size_t srcLen = sizeof(src);

    LLBC_Stream stream(const_cast<char *>(src), srcLen, false);
    ASSERT_FALSE(stream.IsAttach());
    ASSERT_NE(stream.GetBuf(), (void *)src);
    ASSERT_EQ(stream.GetWritePos(), srcLen);
    ASSERT_EQ(stream.GetReadPos(), 0lu);
    ASSERT_EQ(memcmp(stream.GetBuf(), src, srcLen), 0);
}

TEST(StreamTest, CopyCtor)
{
    LLBC_Stream src(32);
    src.SetEndian(LLBC_Endian::LittleEndian);
    int v1 = 100;
    sint64 v2 = 200;
    src << v1 << v2;
    ASSERT_TRUE(src.SetReadPos(sizeof(int)));

    LLBC_Stream dst(src);
    ASSERT_FALSE(dst.IsAttach());
    ASSERT_NE(dst.GetBuf(), src.GetBuf());
    ASSERT_EQ(dst.GetWritePos(), src.GetWritePos());
    ASSERT_EQ(dst.GetReadPos(), src.GetReadPos());
    ASSERT_EQ(dst.GetEndian(), src.GetEndian());

    // Data should be the same.
    ASSERT_EQ(memcmp(dst.GetBuf(), src.GetBuf(), src.GetWritePos()), 0);
}

TEST(StreamTest, AttachCopyCtor)
{
    LLBC_Stream src(16);
    int val = 12345;
    src << val;

    LLBC_Stream dst(src, /*attach=*/true);
    ASSERT_TRUE(dst.IsAttach());
    ASSERT_EQ(dst.GetBuf(), src.GetBuf());
    ASSERT_EQ(dst.GetCap(), src.GetCap());
    ASSERT_EQ(dst.GetWritePos(), src.GetWritePos());
    ASSERT_EQ(dst.GetReadPos(), 0lu); // Attach resets read pos.
    ASSERT_EQ(dst.GetEndian(), src.GetEndian());
}

TEST(StreamTest, MoveCtor)
{
    LLBC_Stream src(32);
    src.SetEndian(LLBC_Endian::BigEndian);
    int val = 42;
    src << val;
    const void *originalBuf = src.GetBuf();
    const size_t originalWpos = src.GetWritePos();
    const size_t originalCap = src.GetCap();

    LLBC_Stream dst(std::move(src));
    ASSERT_EQ(dst.GetBuf(), originalBuf);
    ASSERT_EQ(dst.GetWritePos(), originalWpos);
    ASSERT_EQ(dst.GetCap(), originalCap);
    ASSERT_EQ(dst.GetEndian(), LLBC_Endian::BigEndian);

    ASSERT_EQ(src.GetBuf(), nullptr);
    ASSERT_EQ(src.GetCap(), 0lu);
    ASSERT_EQ(src.GetWritePos(), 0lu);
    ASSERT_EQ(src.GetReadPos(), 0lu);
    ASSERT_FALSE(src.IsAttach());
}

TEST(StreamTest, DtorReleasesOwnedBuffer)
{
    // Just make sure no leak/crash: create then destroy owned buffer stream.
    {
        LLBC_Stream stream(1024);
        int v = 1;
        stream << v;
    }
    // Attached stream doesn't own buffer.
    char buf[8];
    {
        LLBC_Stream stream(buf, sizeof(buf), true);
        (void)stream;
    }
    // If we reach here without crash, dtor works.
    SUCCEED();
}


// =============================================================================
// [Assign operators]
// =============================================================================
TEST(StreamTest, CopyAssign)
{
    LLBC_Stream src(32);
    src.SetEndian(LLBC_Endian::LittleEndian);
    int v = 999;
    src << v;

    LLBC_Stream dst;
    dst = src;

    ASSERT_FALSE(dst.IsAttach());
    ASSERT_NE(dst.GetBuf(), src.GetBuf());
    ASSERT_EQ(dst.GetWritePos(), src.GetWritePos());
    ASSERT_EQ(dst.GetEndian(), src.GetEndian());
    ASSERT_EQ(memcmp(dst.GetBuf(), src.GetBuf(), src.GetWritePos()), 0);
}

TEST(StreamTest, MoveAssign)
{
    LLBC_Stream src(32);
    int v = 7777;
    src << v;
    const void *originalBuf = src.GetBuf();
    const size_t originalWpos = src.GetWritePos();

    LLBC_Stream dst;
    dst = std::move(src);

    ASSERT_EQ(dst.GetBuf(), originalBuf);
    ASSERT_EQ(dst.GetWritePos(), originalWpos);
    ASSERT_EQ(src.GetBuf(), nullptr);
    ASSERT_EQ(src.GetWritePos(), 0lu);
    ASSERT_EQ(src.GetCap(), 0lu);
}

TEST(StreamTest, SelfAssign)
{
    LLBC_Stream stream(16);
    int v = 555;
    stream << v;

    const size_t wposBefore = stream.GetWritePos();
    const void *bufBefore = stream.GetBuf();

    // Assign through a reference so the intentional self-assignment still exercises the
    // self-assign path at runtime without tripping clang's syntactic -Wself-assign-overloaded.
    LLBC_Stream &streamSelfRef = stream;
    stream = streamSelfRef;

    ASSERT_EQ(stream.GetWritePos(), wposBefore);
    ASSERT_EQ(stream.GetBuf(), bufBefore);

    int readV = 0;
    ASSERT_TRUE(stream.Read(readV));
    ASSERT_EQ(readV, v);
}


// =============================================================================
// [Attach / Detach / Assign / Swap / SetAttach]
// =============================================================================
TEST(StreamTest, AttachStream)
{
    LLBC_Stream src(32);
    src.SetEndian(LLBC_Endian::BigEndian);
    int v = 1;
    src << v;

    LLBC_Stream dst;
    dst.Attach(src);

    ASSERT_TRUE(dst.IsAttach());
    ASSERT_EQ(dst.GetBuf(), src.GetBuf());
    ASSERT_EQ(dst.GetCap(), src.GetCap());
    ASSERT_EQ(dst.GetWritePos(), src.GetWritePos());
    ASSERT_EQ(dst.GetReadPos(), 0lu);
    ASSERT_EQ(dst.GetEndian(), src.GetEndian());
}

TEST(StreamTest, AttachBuf)
{
    char buf[16];
    LLBC_Stream stream;
    stream.Attach(buf, sizeof(buf));

    ASSERT_TRUE(stream.IsAttach());
    ASSERT_EQ(stream.GetBuf(), buf);
    ASSERT_EQ(stream.GetCap(), sizeof(buf));
    ASSERT_EQ(stream.GetWritePos(), sizeof(buf));
    ASSERT_EQ(stream.GetReadPos(), 0lu);
}

TEST(StreamTest, AttachEmpty)
{
    LLBC_Stream stream;
    stream.Attach(nullptr, 0);

    ASSERT_TRUE(stream.IsAttach());
    ASSERT_EQ(stream.GetBuf(), nullptr);
    ASSERT_EQ(stream.GetCap(), 0lu);
    ASSERT_EQ(stream.GetWritePos(), 0lu);

    // Attach(anyBuf, 0) also should be empty.
    char buf[4];
    LLBC_Stream stream2;
    stream2.Attach(buf, 0);
    ASSERT_TRUE(stream2.IsAttach());
    ASSERT_EQ(stream2.GetBuf(), nullptr);
    ASSERT_EQ(stream2.GetCap(), 0lu);
    ASSERT_EQ(stream2.GetWritePos(), 0lu);
}

TEST(StreamTest, DetachNormal)
{
    char buf[8];
    LLBC_Stream stream(buf, sizeof(buf), true);
    void *detached = stream.Detach();

    ASSERT_EQ(detached, buf);
    ASSERT_EQ(stream.GetBuf(), nullptr);
    ASSERT_EQ(stream.GetCap(), 0lu);
    ASSERT_EQ(stream.GetReadPos(), 0lu);
    ASSERT_EQ(stream.GetWritePos(), 0lu);
    ASSERT_FALSE(stream.IsAttach());
}

TEST(StreamTest, DetachEmpty)
{
    LLBC_Stream stream;
    void *detached = stream.Detach();
    ASSERT_EQ(detached, nullptr);
}

TEST(StreamTest, AssignStream)
{
    LLBC_Stream src(32);
    src.SetEndian(LLBC_Endian::BigEndian);
    int v = 20250716;
    src << v;

    LLBC_Stream dst;
    dst.Assign(src);

    ASSERT_FALSE(dst.IsAttach());
    ASSERT_NE(dst.GetBuf(), src.GetBuf());
    ASSERT_EQ(dst.GetWritePos(), src.GetWritePos());
    ASSERT_EQ(dst.GetEndian(), src.GetEndian());
    ASSERT_EQ(memcmp(dst.GetBuf(), src.GetBuf(), src.GetWritePos()), 0);
}

TEST(StreamTest, AssignBuf)
{
    const char src[] = "hello";
    LLBC_Stream stream;
    stream.Assign(const_cast<char *>(src), sizeof(src));

    ASSERT_FALSE(stream.IsAttach());
    ASSERT_NE(stream.GetBuf(), (void *)src);
    ASSERT_EQ(stream.GetWritePos(), sizeof(src));
    ASSERT_EQ(memcmp(stream.GetBuf(), src, sizeof(src)), 0);
}

TEST(StreamTest, Swap)
{
    LLBC_Stream a(16);
    a.SetEndian(LLBC_Endian::BigEndian);
    int va = 1;
    a << va;

    LLBC_Stream b(32);
    b.SetEndian(LLBC_Endian::LittleEndian);
    int vb = 2;
    b << vb;

    void *bufA = a.GetBuf();
    size_t capA = a.GetCap();
    size_t wposA = a.GetWritePos();
    int endianA = a.GetEndian();

    void *bufB = b.GetBuf();
    size_t capB = b.GetCap();
    size_t wposB = b.GetWritePos();
    int endianB = b.GetEndian();

    a.Swap(b);

    ASSERT_EQ(a.GetBuf(), bufB);
    ASSERT_EQ(a.GetCap(), capB);
    ASSERT_EQ(a.GetWritePos(), wposB);
    ASSERT_EQ(a.GetEndian(), endianB);

    ASSERT_EQ(b.GetBuf(), bufA);
    ASSERT_EQ(b.GetCap(), capA);
    ASSERT_EQ(b.GetWritePos(), wposA);
    ASSERT_EQ(b.GetEndian(), endianA);
}

TEST(StreamTest, SetAttach)
{
    LLBC_Stream stream(16);
    ASSERT_FALSE(stream.IsAttach());

    stream.SetAttach(true);
    ASSERT_TRUE(stream.IsAttach());

    // NOTE: to avoid leak in this test, restore attach=false so buffer is freed.
    stream.SetAttach(false);
    ASSERT_FALSE(stream.IsAttach());
}


// =============================================================================
// [Endian]
// =============================================================================
TEST(StreamTest, DefaultEndian)
{
    LLBC_Stream stream;
    ASSERT_EQ(stream.GetEndian(), LLBC_DefaultEndian);
}

TEST(StreamTest, SetEndianValid)
{
    LLBC_Stream stream;

    stream.SetEndian(LLBC_Endian::BigEndian);
    ASSERT_EQ(stream.GetEndian(), LLBC_Endian::BigEndian);

    stream.SetEndian(LLBC_Endian::LittleEndian);
    ASSERT_EQ(stream.GetEndian(), LLBC_Endian::LittleEndian);
}

TEST(StreamTest, SetEndianInvalid)
{
    LLBC_Stream stream;
    stream.SetEndian(LLBC_Endian::LittleEndian);

    // Invalid endian value should be rejected.
    stream.SetEndian(LLBC_Endian::UnknownEndian);
    ASSERT_EQ(stream.GetEndian(), LLBC_Endian::LittleEndian);

    stream.SetEndian(9999);
    ASSERT_EQ(stream.GetEndian(), LLBC_Endian::LittleEndian);
}

TEST(StreamTest, EndianReverseRoundtrip)
{
    // Same endian (write & read use same endian): value should be preserved.
    for (int endian : {LLBC_Endian::BigEndian, LLBC_Endian::LittleEndian})
    {
        LLBC_Stream stream(16);
        stream.SetEndian(endian);
        const uint32 orig = 0x12345678u;
        stream << orig;

        uint32 got = 0;
        ASSERT_TRUE(stream.Read(got));
        ASSERT_EQ(got, orig);
    }
}

TEST(StreamTest, EndianCrossRead)
{
    // Write with BigEndian, read with LittleEndian -> bytes reversed.
    LLBC_Stream writer(16);
    writer.SetEndian(LLBC_Endian::BigEndian);
    const uint32 orig = 0x12345678u;
    writer << orig;

    LLBC_Stream reader;
    reader.Attach(writer.GetBuf(), writer.GetWritePos());
    reader.SetEndian(LLBC_Endian::LittleEndian);

    uint32 got = 0;
    ASSERT_TRUE(reader.Read(got));
    ASSERT_EQ(got, 0x78563412u);
}


// =============================================================================
// [Pos / Cap / Skip / Clear / Recap / GetBufStart]
// =============================================================================
TEST(StreamTest, SetReadPos)
{
    LLBC_Stream stream(16);
    int a = 1, b = 2;
    stream << a << b;

    ASSERT_TRUE(stream.SetReadPos(0));
    ASSERT_EQ(stream.GetReadPos(), 0lu);

    ASSERT_TRUE(stream.SetReadPos(stream.GetWritePos()));
    ASSERT_EQ(stream.GetReadPos(), stream.GetWritePos());

    ASSERT_FALSE(stream.SetReadPos(stream.GetWritePos() + 1));
    ASSERT_EQ(stream.GetReadPos(), stream.GetWritePos());
}

TEST(StreamTest, SetWritePos)
{
    LLBC_Stream stream(16);
    ASSERT_TRUE(stream.SetWritePos(8));
    ASSERT_EQ(stream.GetWritePos(), 8lu);

    ASSERT_TRUE(stream.SetWritePos(16));
    ASSERT_EQ(stream.GetWritePos(), 16lu);

    ASSERT_FALSE(stream.SetWritePos(17));
    ASSERT_EQ(stream.GetWritePos(), 16lu);
}

TEST(StreamTest, SetWritePosAdjustsReadPos)
{
    LLBC_Stream stream(16);
    stream.SetWritePos(10);
    stream.SetReadPos(8);

    // Now shrink write pos below read pos, read pos should follow.
    ASSERT_TRUE(stream.SetWritePos(4));
    ASSERT_EQ(stream.GetWritePos(), 4lu);
    ASSERT_EQ(stream.GetReadPos(), 4lu);
}

TEST(StreamTest, SkipRead)
{
    LLBC_Stream stream(16);
    int a = 1, b = 2;
    stream << a << b;

    ASSERT_TRUE(stream.SkipRead(4));
    ASSERT_EQ(stream.GetReadPos(), 4lu);

    ASSERT_TRUE(stream.SkipRead(-4));
    ASSERT_EQ(stream.GetReadPos(), 0lu);

    ASSERT_FALSE(stream.SkipRead(-1));
    ASSERT_EQ(stream.GetReadPos(), 0lu);

    ASSERT_FALSE(stream.SkipRead(static_cast<sint64>(stream.GetWritePos() + 1)));
    ASSERT_EQ(stream.GetReadPos(), 0lu);
}

TEST(StreamTest, SkipWrite)
{
    LLBC_Stream stream(16);
    ASSERT_TRUE(stream.SkipWrite(8));
    ASSERT_EQ(stream.GetWritePos(), 8lu);

    ASSERT_TRUE(stream.SkipWrite(-4));
    ASSERT_EQ(stream.GetWritePos(), 4lu);

    ASSERT_FALSE(stream.SkipWrite(-100));
    ASSERT_EQ(stream.GetWritePos(), 4lu);

    ASSERT_FALSE(stream.SkipWrite(static_cast<sint64>(stream.GetCap())));
    ASSERT_EQ(stream.GetWritePos(), 4lu);
}

TEST(StreamTest, ReadableWritableSize)
{
    LLBC_Stream stream(16);
    ASSERT_EQ(stream.GetReadableSize(), 0lu);
    ASSERT_EQ(stream.GetWritableSize(), 16lu);

    int v = 42;
    stream << v;
    ASSERT_EQ(stream.GetReadableSize(), sizeof(int));
    ASSERT_EQ(stream.GetWritableSize(), 16lu - sizeof(int));

    stream.SetReadPos(sizeof(int));
    ASSERT_EQ(stream.GetReadableSize(), 0lu);
}

TEST(StreamTest, RecapExpand)
{
    LLBC_Stream stream(16);
    ASSERT_TRUE(stream.Recap(32));
    ASSERT_GE(stream.GetCap(), 32lu);

    // Recap with equal cap should succeed as well.
    const size_t curCap = stream.GetCap();
    ASSERT_TRUE(stream.Recap(curCap));
    ASSERT_EQ(stream.GetCap(), curCap);
}

TEST(StreamTest, RecapAttachFail)
{
    char buf[16];
    LLBC_Stream stream(buf, sizeof(buf), true);
    ASSERT_FALSE(stream.Recap(32));
    ASSERT_EQ(stream.GetCap(), sizeof(buf));
}

TEST(StreamTest, RecapShrinkFail)
{
    LLBC_Stream stream(32);
    ASSERT_FALSE(stream.Recap(16));
    ASSERT_EQ(stream.GetCap(), 32lu);
}

TEST(StreamTest, ClearNonAttach)
{
    LLBC_Stream stream(32);
    stream.SetEndian(LLBC_Endian::BigEndian);
    int v = 1;
    stream << v;
    stream.SetReadPos(sizeof(int));

    stream.Clear();
    ASSERT_EQ(stream.GetReadPos(), 0lu);
    ASSERT_EQ(stream.GetWritePos(), 0lu);
    ASSERT_EQ(stream.GetEndian(), LLBC_DefaultEndian);
    ASSERT_FALSE(stream.IsAttach());
}

TEST(StreamTest, ClearAttach)
{
    char buf[16];
    LLBC_Stream stream(buf, sizeof(buf), true);
    stream.SetEndian(LLBC_Endian::BigEndian);

    stream.Clear();
    ASSERT_EQ(stream.GetBuf(), nullptr);
    ASSERT_EQ(stream.GetCap(), 0lu);
    ASSERT_EQ(stream.GetReadPos(), 0lu);
    ASSERT_EQ(stream.GetWritePos(), 0lu);
    ASSERT_FALSE(stream.IsAttach());
    ASSERT_EQ(stream.GetEndian(), LLBC_DefaultEndian);
}

TEST(StreamTest, GetBufStartPointers)
{
    LLBC_Stream stream(16);
    int v = 1;
    stream << v;

    const auto *bufStart = stream.GetBuf<char>();
    ASSERT_EQ(stream.GetBufStartWithReadPos<char>(), bufStart + stream.GetReadPos());
    ASSERT_EQ(stream.GetBufStartWithWritePos<char>(), bufStart + stream.GetWritePos());

    // Const version.
    const LLBC_Stream &cs = stream;
    ASSERT_EQ(cs.GetBufStartWithReadPos<char>(), bufStart + cs.GetReadPos());
    ASSERT_EQ(cs.GetBufStartWithWritePos<char>(), bufStart + cs.GetWritePos());
}


// =============================================================================
// [Raw buffer Read/Write]
// =============================================================================
TEST(StreamTest, WriteReadBuffer)
{
    LLBC_Stream stream(16);
    const char src[] = "abcdefg";
    stream.Write(src, sizeof(src));
    ASSERT_EQ(stream.GetWritePos(), sizeof(src));

    char dst[sizeof(src)] = {0};
    ASSERT_TRUE(stream.Read(dst, sizeof(dst)));
    ASSERT_EQ(memcmp(dst, src, sizeof(src)), 0);
    ASSERT_EQ(stream.GetReadPos(), sizeof(src));
}

TEST(StreamTest, WriteInvalidArgs)
{
    LLBC_Stream stream(16);
    // Write nullptr: no-op.
    stream.Write(nullptr, 4);
    ASSERT_EQ(stream.GetWritePos(), 0lu);

    // Write size=0: no-op.
    char buf[4] = {1, 2, 3, 4};
    stream.Write(buf, 0);
    ASSERT_EQ(stream.GetWritePos(), 0lu);
}

TEST(StreamTest, ReadInvalidArgs)
{
    LLBC_Stream stream(16);
    int v = 1;
    stream << v;

    // Read nullptr: false.
    ASSERT_FALSE(stream.Read(nullptr, sizeof(int)));

    // Read size=0: false.
    char buf[4];
    ASSERT_FALSE(stream.Read(buf, 0));

    // Read beyond writePos: false, readPos unchanged.
    char big[128];
    const size_t oldRPos = stream.GetReadPos();
    ASSERT_FALSE(stream.Read(big, 128));
    ASSERT_EQ(stream.GetReadPos(), oldRPos);
}


// =============================================================================
// [Arithmetic / Enum / Operators / Peek / Read<T>()]
// =============================================================================
TEST(StreamTest, ArithmeticRoundtrip)
{
    LLBC_Stream stream(128);
    const bool bV = true;
    const char cV = 'X';
    const sint8 s8V = -8;
    const uint8 u8V = 8;
    const sint16 s16V = -1600;
    const uint16 u16V = 1600;
    const sint32 s32V = -320000;
    const uint32 u32V = 320000;
    const sint64 s64V = -6400000000LL;
    const uint64 u64V = 6400000000ULL;
    const float fV = 3.14f;
    const double dV = 2.71828;

    stream << bV << cV << s8V << u8V << s16V << u16V
           << s32V << u32V << s64V << u64V << fV << dV;

    bool bR = false; char cR = 0; sint8 s8R = 0; uint8 u8R = 0;
    sint16 s16R = 0; uint16 u16R = 0;
    sint32 s32R = 0; uint32 u32R = 0;
    sint64 s64R = 0; uint64 u64R = 0;
    float fR = 0.0f; double dR = 0.0;

    stream >> bR >> cR >> s8R >> u8R >> s16R >> u16R
           >> s32R >> u32R >> s64R >> u64R >> fR >> dR;

    ASSERT_EQ(bR, bV);
    ASSERT_EQ(cR, cV);
    ASSERT_EQ(s8R, s8V);
    ASSERT_EQ(u8R, u8V);
    ASSERT_EQ(s16R, s16V);
    ASSERT_EQ(u16R, u16V);
    ASSERT_EQ(s32R, s32V);
    ASSERT_EQ(u32R, u32V);
    ASSERT_EQ(s64R, s64V);
    ASSERT_EQ(u64R, u64V);
    ASSERT_FLOAT_EQ(fR, fV);
    ASSERT_DOUBLE_EQ(dR, dV);
}

TEST(StreamTest, StreamOperators)
{
    LLBC_Stream a(16), b(16);
    int i = 100;
    double d = 3.14;

    a << i << d;
    b.Write(i);
    b.Write(d);

    ASSERT_EQ(a.GetWritePos(), b.GetWritePos());
    ASSERT_EQ(memcmp(a.GetBuf(), b.GetBuf(), a.GetWritePos()), 0);

    int i2 = 0; double d2 = 0.0;
    a >> i2 >> d2;
    ASSERT_EQ(i2, i);
    ASSERT_DOUBLE_EQ(d2, d);
}

TEST(StreamTest, ReadTemplateReturn)
{
    LLBC_Stream stream(16);
    int v = 42;
    stream << v;

    ASSERT_EQ(stream.Read<int>(), v);

    // Read from empty stream => default value.
    ASSERT_EQ(stream.Read<int>(), 0);
}

TEST(StreamTest, EnumRoundtrip)
{
    LLBC_Stream stream(32);
    const PlainEnum pe = PE_Big;
    const ScopedEnum se = ScopedEnum::Bar;

    stream << pe << se;

    PlainEnum peR = PE_None;
    ScopedEnum seR = ScopedEnum::None;
    stream >> peR >> seR;

    ASSERT_EQ(peR, pe);
    ASSERT_EQ(seR, se);
}

TEST(StreamTest, ReadInsufficient)
{
    LLBC_Stream stream(4);
    // Only write 1 byte, try to read int (4 bytes).
    uint8 v = 1;
    stream << v;

    int intV = 0;
    const size_t oldRPos = stream.GetReadPos();
    ASSERT_FALSE(stream.Read(intV));
    ASSERT_EQ(stream.GetReadPos(), oldRPos);
}

TEST(StreamTest, PeekOk)
{
    LLBC_Stream stream(16);
    const int v = 12345;
    stream << v;

    int peeked = 0;
    ASSERT_TRUE(stream.Peek(peeked));
    ASSERT_EQ(peeked, v);
    ASSERT_EQ(stream.GetReadPos(), 0lu);

    int readAgain = 0;
    ASSERT_TRUE(stream.Read(readAgain));
    ASSERT_EQ(readAgain, v);
}

TEST(StreamTest, PeekInsufficient)
{
    LLBC_Stream stream(2);
    uint8 v = 1;
    stream << v;

    int peeked = 0;
    const size_t oldRPos = stream.GetReadPos();
    ASSERT_FALSE(stream.Peek(peeked));
    ASSERT_EQ(stream.GetReadPos(), oldRPos);
}


// =============================================================================
// [String / char array]
// =============================================================================
TEST(StreamTest, StdStringRoundtrip)
{
    LLBC_Stream stream(64);
    const std::string s = "hello world";
    stream << s;

    std::string sR;
    ASSERT_TRUE(stream.Read(sR));
    ASSERT_EQ(sR, s);
}

TEST(StreamTest, LLBCStringRoundtrip)
{
    LLBC_Stream stream(64);
    const LLBC_String s = "hello llbc";
    stream << s;

    LLBC_String sR;
    ASSERT_TRUE(stream.Read(sR));
    ASSERT_EQ(sR, s);
}

TEST(StreamTest, EmptyStringRoundtrip)
{
    LLBC_Stream stream(16);
    const std::string s;
    stream << s;
    // Length prefix (uint32) is written, no payload.
    ASSERT_EQ(stream.GetWritePos(), sizeof(uint32));

    std::string sR = "non-empty";
    ASSERT_TRUE(stream.Read(sR));
    ASSERT_TRUE(sR.empty());
}

TEST(StreamTest, CharArrayRoundtrip)
{
    LLBC_Stream stream(64);
    const char src[] = "abcdef"; // sizeof=7 with '\0', written length = 6
    stream.Write(src);

    char dst[16] = {0};
    ASSERT_TRUE(stream.Read(dst));
    ASSERT_STREQ(dst, "abcdef");
}

TEST(StreamTest, CharArrayTargetTooSmall)
{
    LLBC_Stream stream(64);
    const char src[] = "longer_string_data";
    stream.Write(src);

    char smallDst[4] = {0};
    const size_t oldRPos = stream.GetReadPos();
    ASSERT_FALSE(stream.Read(smallDst));
    // Failure should rollback readPos.
    ASSERT_EQ(stream.GetReadPos(), oldRPos);
}

TEST(StreamTest, WriteCStringLiteral)
{
    LLBC_Stream stream(64);
    const char *literal = "hello";
    stream.Write(literal);

    std::string sR;
    ASSERT_TRUE(stream.Read(sR));
    ASSERT_EQ(sR, "hello");
}

TEST(StreamTest, WriteNullCStringPointer)
{
    LLBC_Stream stream(16);
    const char *ptr = nullptr;
    stream.Write(ptr);
    ASSERT_EQ(stream.GetWritePos(), sizeof(uint32));

    std::string sR = "not-empty";
    ASSERT_TRUE(stream.Read(sR));
    ASSERT_TRUE(sR.empty());
}


// =============================================================================
// [STL containers]
// =============================================================================
TEST(StreamTest, VectorRoundtrip)
{
    LLBC_Stream stream(128);
    const std::vector<int> v = {1, 2, 3, 4, 5};
    stream << v;

    std::vector<int> vR;
    ASSERT_TRUE(stream.Read(vR));
    ASSERT_EQ(vR, v);
}

TEST(StreamTest, ListRoundtrip)
{
    LLBC_Stream stream(128);
    const std::list<int> l = {10, 20, 30};
    stream << l;

    std::list<int> lR;
    ASSERT_TRUE(stream.Read(lR));
    ASSERT_EQ(lR, l);
}

TEST(StreamTest, DequeRoundtrip)
{
    LLBC_Stream stream(128);
    const std::deque<int> d = {100, 200, 300, 400};
    stream << d;

    std::deque<int> dR;
    ASSERT_TRUE(stream.Read(dR));
    ASSERT_EQ(dR, d);
}

TEST(StreamTest, SetRoundtrip)
{
    LLBC_Stream stream(128);
    const std::set<int> s = {5, 3, 1, 4, 2};
    stream << s;

    std::set<int> sR;
    ASSERT_TRUE(stream.Read(sR));
    ASSERT_EQ(sR, s);
}

TEST(StreamTest, UnorderedSetRoundtrip)
{
    LLBC_Stream stream(128);
    const std::unordered_set<int> s = {5, 3, 1, 4, 2};
    stream << s;

    std::unordered_set<int> sR;
    ASSERT_TRUE(stream.Read(sR));
    ASSERT_EQ(sR, s);
}

TEST(StreamTest, MapRoundtrip)
{
    LLBC_Stream stream(128);
    const std::map<int, std::string> m = {
        {1, "one"}, {2, "two"}, {3, "three"}
    };
    stream << m;

    std::map<int, std::string> mR;
    ASSERT_TRUE(stream.Read(mR));
    ASSERT_EQ(mR, m);
}

TEST(StreamTest, UnorderedMapRoundtrip)
{
    LLBC_Stream stream(128);
    const std::unordered_map<int, int> m = {
        {1, 10}, {2, 20}, {3, 30}
    };
    stream << m;

    std::unordered_map<int, int> mR;
    ASSERT_TRUE(stream.Read(mR));
    ASSERT_EQ(mR, m);
}

TEST(StreamTest, PairRoundtrip)
{
    LLBC_Stream stream(64);
    const std::pair<int, std::string> p = {42, "answer"};
    stream << p;

    std::pair<int, std::string> pR;
    ASSERT_TRUE(stream.Read(pR));
    ASSERT_EQ(pR, p);
}

TEST(StreamTest, TupleRoundtrip)
{
    LLBC_Stream stream(64);
    const std::tuple<int, double, std::string> t = {1, 2.5, "hi"};
    stream << t;

    std::tuple<int, double, std::string> tR;
    ASSERT_TRUE(stream.Read(tR));
    ASSERT_EQ(tR, t);
}

TEST(StreamTest, StdArrayRoundtrip)
{
    LLBC_Stream stream(64);
    const std::array<int, 4> a = {1, 2, 3, 4};
    stream << a;

    std::array<int, 4> aR{};
    ASSERT_TRUE(stream.Read(aR));
    ASSERT_EQ(aR, a);
}

TEST(StreamTest, EmptyContainerRoundtrip)
{
    LLBC_Stream stream(64);
    const std::vector<int> emptyVec;
    const std::map<int, int> emptyMap;
    stream << emptyVec << emptyMap;

    std::vector<int> vecR = {1, 2, 3};
    std::map<int, int> mapR = {{1, 1}};
    ASSERT_TRUE(stream.Read(vecR));
    ASSERT_TRUE(stream.Read(mapR));
    ASSERT_TRUE(vecR.empty());
    ASSERT_TRUE(mapR.empty());
}


// =============================================================================
// [Insert / Erase / Replace]
// =============================================================================
TEST(StreamTest, InsertMiddle)
{
    LLBC_Stream stream(64);
    const char base[] = "ABCDEF";
    stream.Write(base, 6);

    const char ins[] = "XYZ";
    stream.Insert(3, ins, 3);

    // Result: "ABCXYZDEF"
    ASSERT_EQ(stream.GetWritePos(), 9lu);
    ASSERT_EQ(memcmp(stream.GetBuf(), "ABCXYZDEF", 9), 0);
}

TEST(StreamTest, InsertBeyondWritePos)
{
    LLBC_Stream stream(64);
    const char base[] = "ABC";
    stream.Write(base, 3);

    const char ins[] = "XY";
    stream.Insert(100, ins, 2); // pos > writePos -> auto adjusted to writePos.

    // Result: "ABCXY"
    ASSERT_EQ(stream.GetWritePos(), 5lu);
    ASSERT_EQ(memcmp(stream.GetBuf(), "ABCXY", 5), 0);
}

TEST(StreamTest, EraseRange)
{
    LLBC_Stream stream(64);
    const char base[] = "ABCDEFGH";
    stream.Write(base, 8);

    stream.Erase(2, 5); // Remove "CDE" -> "ABFGH"
    ASSERT_EQ(stream.GetWritePos(), 5lu);
    ASSERT_EQ(memcmp(stream.GetBuf(), "ABFGH", 5), 0);
}

TEST(StreamTest, EraseToNpos)
{
    LLBC_Stream stream(64);
    const char base[] = "ABCDEFGH";
    stream.Write(base, 8);

    stream.Erase(3, LLBC_Stream::npos); // Remove "DEFGH" -> "ABC"
    ASSERT_EQ(stream.GetWritePos(), 3lu);
    ASSERT_EQ(memcmp(stream.GetBuf(), "ABC", 3), 0);
}

TEST(StreamTest, EraseSwappedArgs)
{
    LLBC_Stream stream(64);
    const char base[] = "ABCDEFGH";
    stream.Write(base, 8);

    stream.Erase(5, 2); // n0 > n1 -> auto swap -> equivalent to Erase(2, 5).
    ASSERT_EQ(stream.GetWritePos(), 5lu);
    ASSERT_EQ(memcmp(stream.GetBuf(), "ABFGH", 5), 0);
}

TEST(StreamTest, ReplaceShorter)
{
    LLBC_Stream stream(64);
    const char base[] = "ABCDEFGH";
    stream.Write(base, 8);

    const char rep[] = "X";
    stream.Replace(2, 5, rep, 1); // Replace "CDE" with "X" -> "ABXFGH"
    ASSERT_EQ(stream.GetWritePos(), 6lu);
    ASSERT_EQ(memcmp(stream.GetBuf(), "ABXFGH", 6), 0);
}

TEST(StreamTest, ReplaceLonger)
{
    LLBC_Stream stream(64);
    const char base[] = "ABCDEF";
    stream.Write(base, 6);

    const char rep[] = "XYZW";
    stream.Replace(2, 4, rep, 4); // Replace "CD" with "XYZW" -> "ABXYZWEF"
    ASSERT_EQ(stream.GetWritePos(), 8lu);
    ASSERT_EQ(memcmp(stream.GetBuf(), "ABXYZWEF", 8), 0);
}

TEST(StreamTest, ReplaceSameLen)
{
    LLBC_Stream stream(64);
    const char base[] = "ABCDEF";
    stream.Write(base, 6);
    const size_t wposBefore = stream.GetWritePos();

    const char rep[] = "XYZ";
    stream.Replace(1, 4, rep, 3); // Replace "BCD" with "XYZ" -> "AXYZEF"
    ASSERT_EQ(stream.GetWritePos(), wposBefore);
    ASSERT_EQ(memcmp(stream.GetBuf(), "AXYZEF", 6), 0);
}

TEST(StreamTest, ReplaceReadPosAdjustBefore)
{
    // Case 1: readPos <= n0 -> readPos unchanged.
    LLBC_Stream stream(64);
    stream.Write("ABCDEFGH", 8);
    stream.SetReadPos(1);
    const size_t rposBefore = stream.GetReadPos();

    const char rep[] = "XX";
    stream.Replace(3, 5, rep, 2);
    ASSERT_EQ(stream.GetReadPos(), rposBefore);
}

TEST(StreamTest, ReplaceReadPosAdjustInside)
{
    // Case 2: n0 < readPos < n1 -> readPos = n0.
    LLBC_Stream stream(64);
    stream.Write("ABCDEFGH", 8);
    stream.SetReadPos(4);

    const char rep[] = "XX";
    stream.Replace(3, 6, rep, 2);
    ASSERT_EQ(stream.GetReadPos(), 3lu);
}

TEST(StreamTest, ReplaceReadPosAdjustAfter)
{
    // Case 3: readPos >= n1 -> readPos + (size - (n1-n0)).
    LLBC_Stream stream(64);
    stream.Write("ABCDEFGH", 8);
    stream.SetReadPos(7);

    const char rep[] = "XY";
    // n0=2, n1=5, eraseSize=3, size=2 -> newRPos = 7 + 2 - 3 = 6.
    stream.Replace(2, 5, rep, 2);
    ASSERT_EQ(stream.GetReadPos(), 6lu);
}

TEST(StreamTest, ReplaceWithNullBuf)
{
    LLBC_Stream stream(64);
    stream.Write("ABCDEFGH", 8);

    // buf=nullptr, size ignored -> equivalent to Erase.
    stream.Replace(2, 5, nullptr, 100);
    ASSERT_EQ(stream.GetWritePos(), 5lu);
    ASSERT_EQ(memcmp(stream.GetBuf(), "ABFGH", 5), 0);
}


// =============================================================================
// [Custom Serializable reflection]
// =============================================================================
TEST(StreamTest, CustomSerializeDeSerialize)
{
    LLBC_Stream stream(64);
    UCCTypeA v{100, 200};
    stream << v;

    UCCTypeA v2{};
    ASSERT_TRUE(stream.Read(v2));
    ASSERT_EQ(v2.a, 100);
    ASSERT_EQ(v2.b, 200);
}

TEST(StreamTest, CustomSerDeSer)
{
    LLBC_Stream stream(64);
    UCCShortTypeB v{111, 222};
    stream << v;

    UCCShortTypeB v2{};
    ASSERT_TRUE(stream.Read(v2));
    ASSERT_EQ(v2.a, 111);
    ASSERT_EQ(v2.b, 222);
}

TEST(StreamTest, CustomLowercaseSerializeDeserialize)
{
    LLBC_Stream stream(64);
    LCCTypeC v{333, 444};
    stream << v;

    LCCTypeC v2{};
    ASSERT_TRUE(stream.Read(v2));
    ASSERT_EQ(v2.a, 333);
    ASSERT_EQ(v2.b, 444);
}

TEST(StreamTest, CustomLowercaseSerDeser)
{
    LLBC_Stream stream(64);
    LCCShortTypeD v{555, 666};
    stream << v;

    LCCShortTypeD v2{};
    ASSERT_TRUE(stream.Read(v2));
    ASSERT_EQ(v2.a, 555);
    ASSERT_EQ(v2.b, 666);
}

TEST(StreamTest, TrivialFallbackMemcpy)
{
    LLBC_Stream stream(64);
    TrivialPOD v{1, 2, 3.14};
    stream << v;
    // memcpy fallback -> sizeof(TrivialPOD) bytes.
    ASSERT_EQ(stream.GetWritePos(), sizeof(TrivialPOD));

    TrivialPOD v2{};
    ASSERT_TRUE(stream.Read(v2));
    ASSERT_EQ(v2, v);
}

TEST(StreamTest, DeSerializeReturnFalse)
{
    LLBC_Stream stream(64);
    FailDeser v{7};
    stream << v;

    FailDeser v2{};
    ASSERT_FALSE(stream.Read(v2));
}


// =============================================================================
// [ToString / operator<<]
// =============================================================================
TEST(StreamTest, ToStringContainsKeyFields)
{
    LLBC_Stream stream(16);
    int v = 42;
    stream << v;

    LLBC_String repr = stream.ToString();
    ASSERT_NE(repr.find("Stream["), std::string::npos);
    ASSERT_NE(repr.find("rpos:"), std::string::npos);
    ASSERT_NE(repr.find("wpos:"), std::string::npos);
    ASSERT_NE(repr.find("cap:"), std::string::npos);
    ASSERT_NE(repr.find("attached:"), std::string::npos);
    ASSERT_NE(repr.find("endian:"), std::string::npos);
}

TEST(StreamTest, OstreamOperator)
{
    LLBC_Stream stream(16);
    int v = 42;
    stream << v;

    std::ostringstream oss;
    oss << stream;

    ASSERT_EQ(oss.str(), std::string(stream.ToString().c_str()));
}