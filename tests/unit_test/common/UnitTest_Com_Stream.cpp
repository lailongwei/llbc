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

// Test stream global setting:
TEST(StreamTest, GlobalStreamSetting)
{
    // Stream global setting:
    // - default endian is LLBC_DefaultEndian.
    // - LLBC_Stream::npos is std::numeric_limits<size_t>::max().

    LLBC_Stream stream;
    ASSERT_EQ(stream.GetEndian(), LLBC_DefaultEndian);
    ASSERT_EQ(LLBC_Stream::npos, std::numeric_limits<size_t>::max());
}

// Test default construct:
TEST(StreamTest, DefaultConstruct)
{
    // Default constructed stream:
    // - capacity is 0.
    // - buffer is nullptr.
    // - attach flag is false.

    LLBC_Stream stream;
    ASSERT_EQ(stream.GetCap(), 0lu);

    ASSERT_EQ(stream.IsAttach(), false);

    ASSERT_EQ(stream.GetBuf() == nullptr, true);
    ASSERT_EQ(stream.GetBufStartWithReadPos() == nullptr, true);
    ASSERT_EQ(stream.GetBufStartWithWritePos() == nullptr, true);
    ASSERT_TRUE(stream.GetReadPos() == 0lu && stream.GetReadableSize() == 0lu);
    ASSERT_TRUE(stream.GetWritePos() == 0lu && stream.GetWritableSize() == 0lu);

    int intVal;
    ASSERT_EQ(stream.Read(intVal), false);
}
