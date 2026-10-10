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

#if LLBC_TARGET_PLATFORM_NON_WIN32
#include <unistd.h>
#endif

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/os/OS_Select.cpp

namespace
{

#if LLBC_TARGET_PLATFORM_NON_WIN32
class ScopedPipe
{
public:
    bool Create()
    {
        return ::pipe(_fds) == 0;
    }

    ~ScopedPipe()
    {
        if (_fds[0] != -1)
            ::close(_fds[0]);
        if (_fds[1] != -1)
            ::close(_fds[1]);
    }

    int ReadFd() const
    {
        return _fds[0];
    }

    int WriteFd() const
    {
        return _fds[1];
    }

private:
    int _fds[2] = {-1, -1};
};
#endif

} // namespace

// Select is a thin readiness wrapper used by socket/event loops. Validate the
// timeout error mapping first, then use a local pipe to exercise readiness and
// the POSIX invalid-argument path without opening network connections.
TEST(SelectOsTest, ReportsTimeoutReadinessAndPosixErrors)
{
    EXPECT_EQ(LLBC_Select(0, nullptr, nullptr, nullptr, 0), 0);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_TIMEOUTED);

#if LLBC_TARGET_PLATFORM_NON_WIN32
    ScopedPipe pipe;
    ASSERT_TRUE(pipe.Create());
    ASSERT_EQ(::write(pipe.WriteFd(), "x", 1), 1);

    LLBC_FdSet readFds;
    LLBC_ZeroFdSet(&readFds);
    LLBC_SetFd(pipe.ReadFd(), &readFds);
    EXPECT_EQ(LLBC_Select(pipe.ReadFd() + 1, &readFds, nullptr, nullptr, 100), 1);
    EXPECT_TRUE(LLBC_FdIsSet(pipe.ReadFd(), &readFds));

    char byte = '\0';
    ASSERT_EQ(::read(pipe.ReadFd(), &byte, 1), 1);
    EXPECT_EQ(byte, 'x');

    EXPECT_EQ(LLBC_Select(-1, nullptr, nullptr, nullptr, 0), -1);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
#endif
}
