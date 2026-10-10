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

#include <llbc/core/os/OS_Console.h>

#include <cstdlib>
#include <string>

#if LLBC_TARGET_PLATFORM_NON_WIN32
#include <signal.h>
#include <unistd.h>
#endif

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/os/OS_Console.cpp

namespace
{

std::string ReadFile(FILE *file)
{
    fflush(file);
    fseek(file, 0, SEEK_SET);

    std::string content;
    char buffer[256];
    size_t read = 0;
    while ((read = fread(buffer, 1, sizeof(buffer), file)) != 0)
        content.append(buffer, read);

    clearerr(file);
    return content;
}

#if LLBC_TARGET_PLATFORM_NON_WIN32
class ScopedStdStreamRedirect
{
public:
    ScopedStdStreamRedirect(FILE *stream, int descriptor)
    : _stream(stream)
    , _descriptor(descriptor)
    {
    }

    bool Begin()
    {
        _capture = tmpfile();
        if (!_capture)
            return false;

        fflush(_stream);
        _savedDescriptor = dup(_descriptor);
        if (_savedDescriptor == -1)
            return false;

        if (dup2(fileno(_capture), _descriptor) == -1)
            return false;

        _active = true;
        return true;
    }

    std::string ReadCaptured() const
    {
        return _capture ? ReadFile(_capture) : std::string();
    }

    ~ScopedStdStreamRedirect()
    {
        if (_active)
        {
            fflush(_stream);
            dup2(_savedDescriptor, _descriptor);
        }
        if (_savedDescriptor != -1)
            close(_savedDescriptor);
        if (_capture)
            fclose(_capture);
    }

private:
    FILE *_stream = nullptr;
    int _descriptor = -1;
    FILE *_capture = nullptr;
    int _savedDescriptor = -1;
    bool _active = false;
};

class ScopedSignalIgnore
{
public:
    bool Begin(int signalNumber)
    {
        struct sigaction action = {};
        action.sa_handler = SIG_IGN;
        sigemptyset(&action.sa_mask);

        if (sigaction(signalNumber, &action, &_previous) != 0)
            return false;

        _signalNumber = signalNumber;
        _active = true;
        return true;
    }

    ~ScopedSignalIgnore()
    {
        if (_active)
            sigaction(_signalNumber, &_previous, nullptr);
    }

private:
    struct sigaction _previous = {};
    int _signalNumber = 0;
    bool _active = false;
};

class ScopedBrokenPipe
{
public:
    bool Begin(int bufferingMode)
    {
        int pipeFds[2] = {-1, -1};
        if (pipe(pipeFds) != 0)
            return false;

        close(pipeFds[0]);
        _file = fdopen(pipeFds[1], "w");
        if (!_file)
        {
            close(pipeFds[1]);
            return false;
        }

        if (setvbuf(_file,
                    bufferingMode == _IOFBF ? _buffer : nullptr,
                    bufferingMode,
                    bufferingMode == _IOFBF ? sizeof(_buffer) : 0) != 0)
        {
            fclose(_file);
            _file = nullptr;
            return false;
        }

        return true;
    }

    FILE *Get() const
    {
        return _file;
    }

    ~ScopedBrokenPipe()
    {
        if (_file)
            fclose(_file);
    }

private:
    FILE *_file = nullptr;
    char _buffer[BUFSIZ] = {};
};
#endif

} // namespace

// Console helpers support arbitrary files for synchronized formatting, while
// colors are valid only for stdout/stderr. Redirect stdout temporarily so ANSI
// formatting can be verified without emitting escape sequences to the terminal.
TEST(ConsoleOsTest, FormatsFlushesAndAppliesConsoleColorsSafely)
{
    FILE *temporary = tmpfile();
    ASSERT_NE(temporary, nullptr);

    EXPECT_EQ(LLBC_GetConsoleColor(temporary), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);
    EXPECT_EQ(LLBC_SetConsoleColor(temporary, LLBC_ConsoleColor::Fg_Red), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);

#if LLBC_TARGET_PLATFORM_NON_WIN32
    char *memoryData = nullptr;
    size_t memorySize = 0;
    FILE *memoryStream = open_memstream(&memoryData, &memorySize);
    ASSERT_NE(memoryStream, nullptr);
    EXPECT_EQ(LLBC_GetConsoleColor(memoryStream), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(LLBC_SetConsoleColor(memoryStream, LLBC_ConsoleColor::Fg_Red), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(__LLBC_FilePrint(false, memoryStream, "%s", "ignored"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    fclose(memoryStream);
    free(memoryData);
#endif

    const std::string largePayload(900, 'x');
    ASSERT_EQ(__LLBC_FilePrint(false, temporary, "%s", "small"), LLBC_OK);
    ASSERT_EQ(__LLBC_FilePrint(true, temporary, "%s", "line"), LLBC_OK);
    ASSERT_EQ(__LLBC_FilePrint(false, temporary, "%s", largePayload.c_str()), LLBC_OK);
    ASSERT_EQ(LLBC_FlushFile(temporary), LLBC_OK);
    EXPECT_EQ(ReadFile(temporary), "smallline\n" + largePayload);
    fclose(temporary);

    EXPECT_EQ(LLBC_FlushFile(nullptr), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);

#if LLBC_TARGET_PLATFORM_NON_WIN32
    ScopedSignalIgnore ignoreSigpipe;
    ASSERT_TRUE(ignoreSigpipe.Begin(SIGPIPE));

    ScopedBrokenPipe unbufferedBrokenPipe;
    ASSERT_TRUE(unbufferedBrokenPipe.Begin(_IONBF));
    EXPECT_EQ(__LLBC_FilePrint(false, unbufferedBrokenPipe.Get(), "%s", "broken"),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    ScopedBrokenPipe bufferedBrokenPipe;
    ASSERT_TRUE(bufferedBrokenPipe.Begin(_IOFBF));
    ASSERT_GE(fputs("queued", bufferedBrokenPipe.Get()), 0);
    EXPECT_EQ(LLBC_FlushFile(bufferedBrokenPipe.Get()), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    const int originalStdoutColor = LLBC_GetConsoleColor(stdout);
    const int originalStderrColor = LLBC_GetConsoleColor(stderr);
    ASSERT_GE(originalStdoutColor, 0);
    ASSERT_GE(originalStderrColor, 0);

    std::string stdoutCaptured;
    {
        ScopedStdStreamRedirect redirect(stdout, STDOUT_FILENO);
        if (!redirect.Begin())
            GTEST_SKIP() << "unable to redirect stdout";

        const int highlightedColor = LLBC_ConsoleColor::Highlight_Fg |
                                     LLBC_ConsoleColor::Fg_Red |
                                     LLBC_ConsoleColor::Bg_Blue;
        const int foregroundColor = LLBC_ConsoleColor::Fg_Green;
        const int backgroundColor = LLBC_ConsoleColor::Bg_Red;
        const int highlightOnlyColor = LLBC_ConsoleColor::Highlight_Fg;
        const int highlightedRet = LLBC_SetConsoleColor(stdout, highlightedColor);
        const int highlightedPrintRet = __LLBC_FilePrint(false, stdout, "%s", "highlighted");
        const int highlightedLinePrintRet =
            __LLBC_FilePrint(true, stdout, "%s", "highlighted-line");
        const int foregroundRet = LLBC_SetConsoleColor(stdout, foregroundColor);
        const int foregroundPrintRet = __LLBC_FilePrint(false, stdout, "%s", "foreground");
        const int backgroundRet = LLBC_SetConsoleColor(stdout, backgroundColor);
        const int backgroundPrintRet = __LLBC_FilePrint(false, stdout, "%s", "background");
        const int highlightOnlyRet = LLBC_SetConsoleColor(stdout, highlightOnlyColor);
        const int highlightOnlyPrintRet = __LLBC_FilePrint(false, stdout, "%s", "highlight-only");
        const int resetRet = LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default);
        const int plainPrintRet = __LLBC_FilePrint(true, stdout, "%s", "plain");
        const int flushRet = LLBC_FlushFile(stdout);
        stdoutCaptured = redirect.ReadCaptured();

        LLBC_SetConsoleColor(stdout, originalStdoutColor);
        EXPECT_EQ(highlightedRet, LLBC_OK);
        EXPECT_EQ(highlightedPrintRet, LLBC_OK);
        EXPECT_EQ(highlightedLinePrintRet, LLBC_OK);
        EXPECT_EQ(foregroundRet, LLBC_OK);
        EXPECT_EQ(foregroundPrintRet, LLBC_OK);
        EXPECT_EQ(backgroundRet, LLBC_OK);
        EXPECT_EQ(backgroundPrintRet, LLBC_OK);
        EXPECT_EQ(highlightOnlyRet, LLBC_OK);
        EXPECT_EQ(highlightOnlyPrintRet, LLBC_OK);
        EXPECT_EQ(resetRet, LLBC_OK);
        EXPECT_EQ(plainPrintRet, LLBC_OK);
        EXPECT_EQ(flushRet, LLBC_OK);
    }

    EXPECT_NE(stdoutCaptured.find("\033[1;31;44mhighlighted\033[0m"), std::string::npos);
    EXPECT_NE(stdoutCaptured.find("\033[1;31;44mhighlighted-line\033[0m\n"),
              std::string::npos);
    EXPECT_NE(stdoutCaptured.find("\033[32mforeground\033[0m"), std::string::npos);
    EXPECT_NE(stdoutCaptured.find("\033[41mbackground\033[0m"), std::string::npos);
    EXPECT_NE(stdoutCaptured.find("\033[1mhighlight-only\033[0m"), std::string::npos);
    EXPECT_NE(stdoutCaptured.find("plain\n"), std::string::npos);

    std::string stderrCaptured;
    {
        ScopedStdStreamRedirect redirect(stderr, STDERR_FILENO);
        if (!redirect.Begin())
            GTEST_SKIP() << "unable to redirect stderr";

        ASSERT_EQ(LLBC_SetConsoleColor(stderr, LLBC_ConsoleColor::Fg_Blue), LLBC_OK);
        ASSERT_EQ(__LLBC_FilePrint(false, stderr, "%s", "stderr"), LLBC_OK);
        ASSERT_EQ(LLBC_FlushFile(stderr), LLBC_OK);
        stderrCaptured = redirect.ReadCaptured();
        ASSERT_EQ(LLBC_SetConsoleColor(stderr, originalStderrColor), LLBC_OK);
    }

    EXPECT_NE(stderrCaptured.find("\033[34mstderr\033[0m"), std::string::npos);
#endif
}
