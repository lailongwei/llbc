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

#include <chrono>
#include <filesystem>
#include <string>

#if LLBC_TARGET_PLATFORM_NON_WIN32
#include <unistd.h>
#endif

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/transcoder/Transcoder.cpp

namespace
{

class ScopedTranscoderFiles
{
public:
    ScopedTranscoderFiles()
    : _base(std::filesystem::temp_directory_path() /
            ("llbc_unit_transcoder_" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
    {
    }

    ~ScopedTranscoderFiles()
    {
        std::error_code ignored;
        std::filesystem::remove(_base.string() + ".utf8", ignored);
        std::filesystem::remove(_base.string() + ".wide", ignored);
        std::filesystem::remove(_base.string() + ".wide-copy", ignored);
        std::filesystem::remove(_base.string() + ".utf8-copy", ignored);
        std::filesystem::remove(_base.string() + ".utf8-from-wide", ignored);
        std::filesystem::remove(_base.string() + ".multibyte-copy", ignored);
        std::filesystem::remove(_base.string() + ".file-to-file-copy", ignored);
        std::filesystem::remove(_base.string() + ".odd-wide", ignored);
    }

    LLBC_String Path(const char *suffix) const
    {
        return LLBC_String((_base.string() + suffix).c_str());
    }

private:
    std::filesystem::path _base;
};

void WriteBytes(const LLBC_String &path, const void *data, size_t size)
{
    LLBC_File file(path, LLBC_FileMode::BinaryWrite);
    ASSERT_TRUE(file.IsOpened());
    ASSERT_EQ(file.Write(data, size), static_cast<sint64>(size));
    ASSERT_EQ(file.Flush(), LLBC_OK);
}

LLBC_String ReadBytes(const LLBC_String &path)
{
    const LLBC_String content = LLBC_File::ReadToEnd(path);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);
    return content;
}

void ExpectWideUtf8Sample(const LLBC_WString &wide)
{
    ASSERT_EQ(wide.size(), 4lu);
    EXPECT_EQ(static_cast<uint16>(wide[0]), 0x0041u);
    EXPECT_EQ(static_cast<uint16>(wide[1]), 0x4e16u);
    EXPECT_EQ(static_cast<uint16>(wide[2]), 0xd83du);
    EXPECT_EQ(static_cast<uint16>(wide[3]), 0xde00u);
}

#if LLBC_TARGET_PLATFORM_NON_WIN32
class ScopedPipe
{
public:
    bool Begin()
    {
        return pipe(_fds) == 0;
    }

    int ReadFd() const
    {
        return _fds[0];
    }

    int WriteFd() const
    {
        return _fds[1];
    }

    void CloseWrite()
    {
        if (_fds[1] != -1)
        {
            close(_fds[1]);
            _fds[1] = -1;
        }
    }

    ~ScopedPipe()
    {
        if (_fds[0] != -1)
            close(_fds[0]);
        if (_fds[1] != -1)
            close(_fds[1]);
    }

private:
    int _fds[2] = {-1, -1};
};
#endif

} // namespace

// UTF-8 <-> LLBC_WString is the primary bridge used by text/file APIs. Cover
// BMP and surrogate-pair characters, direct multi-byte conversion, and the
// convenience-return overloads.
TEST(TranscoderTest, ConvertsUtf8WideAndMultibyteRepresentations)
{
    const LLBC_String utf8("A\xe4\xb8\x96\xf0\x9f\x98\x80");
    const LLBC_String utf16Le("\x41\x00\x16\x4e\x3d\xd8\x00\xde", 8);

    LLBC_WString wide;
    ASSERT_EQ(LLBC_Transcoder::MultiByteToWideChar("UTF-8", utf8, wide), LLBC_OK);
    ExpectWideUtf8Sample(wide);

    LLBC_String roundTripped;
    ASSERT_EQ(LLBC_Transcoder::WideCharToMultiByte("UTF-8", wide, roundTripped), LLBC_OK);
    EXPECT_EQ(roundTripped, utf8);

    LLBC_String converted;
    ASSERT_EQ(LLBC_Transcoder::MultiByteToMultiByte("UTF-8", utf8, "UTF-16LE", converted),
              LLBC_OK);
    EXPECT_EQ(converted, utf16Le);
    ASSERT_EQ(LLBC_Transcoder::MultiByteToMultiByte("UTF-16LE", converted, "UTF-8", roundTripped),
              LLBC_OK);
    EXPECT_EQ(roundTripped, utf8);

    ASSERT_EQ(LLBC_Transcoder::MultiByteToMultiByte("UTF-8", utf8, "UTF-8", converted),
              LLBC_OK);
    EXPECT_EQ(converted, utf8);

    EXPECT_EQ(LLBC_Transcoder::MultiByteToWideChar("UTF-8", utf8), wide);
    EXPECT_EQ(LLBC_Transcoder::WideCharToMultiByte("UTF-8", wide), utf8);
    EXPECT_EQ(LLBC_Transcoder::MultiByteToMultiByte("UTF-8", utf8, "UTF-8"), utf8);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);
}

// File variants are used for import/export pipelines. Exercise every public
// file-to-memory, memory-to-file, and file-to-file conversion family using
// deterministic temporary files.
TEST(TranscoderTest, ConvertsBetweenMemoryAndFiles)
{
    ScopedTranscoderFiles files;
    const LLBC_String utf8("A\xe4\xb8\x96\xf0\x9f\x98\x80");
    const LLBC_String utf8Path = files.Path(".utf8");
    const LLBC_String widePath = files.Path(".wide");
    const LLBC_String wideCopyPath = files.Path(".wide-copy");
    const LLBC_String utf8CopyPath = files.Path(".utf8-copy");
    const LLBC_String utf8FromWidePath = files.Path(".utf8-from-wide");
    const LLBC_String multibyteCopyPath = files.Path(".multibyte-copy");
    const LLBC_String fileToFileCopyPath = files.Path(".file-to-file-copy");
    WriteBytes(utf8Path, utf8.data(), utf8.size());

    LLBC_WString wide;
    ASSERT_EQ(LLBC_Transcoder::MultiByteFileToWideChar("UTF-8", utf8Path, wide), LLBC_OK);
    ExpectWideUtf8Sample(wide);

    ASSERT_EQ(LLBC_Transcoder::MultiByteToWideCharFile("UTF-8", utf8, widePath), LLBC_OK);
    LLBC_String utf8FromWide;
    ASSERT_EQ(LLBC_Transcoder::WideCharFileToMultiByte("UTF-8", widePath, utf8FromWide), LLBC_OK);
    EXPECT_EQ(utf8FromWide, utf8);

    ASSERT_EQ(LLBC_Transcoder::WideCharToMultiByteFile("UTF-8", wide, utf8FromWidePath), LLBC_OK);
    EXPECT_EQ(ReadBytes(utf8FromWidePath), utf8);

    ASSERT_EQ(LLBC_Transcoder::MultiByteFileToWideCharFile("UTF-8", utf8Path, wideCopyPath),
              LLBC_OK);
    ASSERT_EQ(LLBC_Transcoder::WideCharFileToMultiByteFile("UTF-8", wideCopyPath, utf8CopyPath),
              LLBC_OK);
    EXPECT_EQ(ReadBytes(utf8CopyPath), utf8);

    LLBC_String utf16Le;
    ASSERT_EQ(LLBC_Transcoder::MultiByteFileToMultiByte("UTF-8", utf8Path, "UTF-16LE", utf16Le),
              LLBC_OK);
    ASSERT_EQ(LLBC_Transcoder::MultiByteToMultiByte("UTF-16LE", utf16Le, "UTF-8", utf8FromWide),
              LLBC_OK);
    EXPECT_EQ(utf8FromWide, utf8);

    ASSERT_EQ(LLBC_Transcoder::MultiByteToMultiByteFile(
                  "UTF-8", utf8, "UTF-8", multibyteCopyPath),
              LLBC_OK);
    EXPECT_EQ(ReadBytes(multibyteCopyPath), utf8);

    ASSERT_EQ(LLBC_Transcoder::MultiByteFileToMultiByteFile(
                  "UTF-8", utf8Path, "UTF-8", fileToFileCopyPath),
              LLBC_OK);
    EXPECT_EQ(ReadBytes(fileToFileCopyPath), utf8);
}

// Invalid encodings and malformed sources must fail without retaining partial
// output. Argument validation is consistent across the three conversion APIs,
// while wide-character files reject odd byte counts.
TEST(TranscoderTest, ValidatesCodesContentAndFileShapes)
{
    ScopedTranscoderFiles files;
    const LLBC_String utf8("text");
    LLBC_WString wide;
    LLBC_String multibyte;

    EXPECT_EQ(LLBC_Transcoder::MultiByteToWideChar("", utf8, wide), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(LLBC_Transcoder::WideCharToMultiByte("", LLBC_WString(), multibyte), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(LLBC_Transcoder::MultiByteToMultiByte("", utf8, "UTF-8", multibyte), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(LLBC_Transcoder::MultiByteToMultiByte("UTF-8", utf8, "", multibyte), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(LLBC_Transcoder::MultiByteToMultiByte("", utf8, "", multibyte), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    wide = LLBC_WString(1, static_cast<wchar>('x'));
    EXPECT_EQ(LLBC_Transcoder::MultiByteToWideChar("not-a-real-encoding", utf8, wide), LLBC_FAILED);
    EXPECT_TRUE(wide.empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    const LLBC_String malformedUtf8("\xff", 1);
    EXPECT_EQ(LLBC_Transcoder::MultiByteToWideChar("UTF-8", malformedUtf8, wide), LLBC_FAILED);
    EXPECT_TRUE(wide.empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    const LLBC_String missingPath = files.Path(".missing");
    EXPECT_EQ(LLBC_Transcoder::MultiByteFileToWideChar("UTF-8", missingPath, wide), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(LLBC_Transcoder::WideCharFileToMultiByte("UTF-8", missingPath, multibyte),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    const LLBC_String oddWidePath = files.Path(".odd-wide");
    const char oddByte = '\0';
    WriteBytes(oddWidePath, &oddByte, sizeof(oddByte));
    EXPECT_EQ(LLBC_Transcoder::WideCharFileToMultiByte("UTF-8", oddWidePath, multibyte),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_FORMAT);
}

// Conversion failures propagate through every file wrapper. Empty source
// values are successful no-ops, while invalid encodings, malformed UTF-16, bad
// destinations, missing files, and non-seekable wide streams keep errors
// explicit and leave output containers empty.
TEST(TranscoderTest, PropagatesFailuresAcrossConversionWrappers)
{
    ScopedTranscoderFiles files;
    const LLBC_String utf8("A\xe4\xb8\x96\xf0\x9f\x98\x80");
    const LLBC_String utf8Path = files.Path(".utf8");
    const LLBC_String oddWidePath = files.Path(".odd-wide");
    const LLBC_String missingPath = files.Path(".missing");
    const LLBC_String badDestination("/definitely/not/a/real/llbc-transcoder-output");
    WriteBytes(utf8Path, utf8.data(), utf8.size());

    LLBC_WString wide(1, static_cast<wchar>('x'));
    EXPECT_EQ(LLBC_Transcoder::MultiByteToWideChar("UTF-8", "", wide), LLBC_OK);
    EXPECT_TRUE(wide.empty());

    LLBC_String multibyte("old");
    EXPECT_EQ(LLBC_Transcoder::WideCharToMultiByte("UTF-8", LLBC_WString(), multibyte), LLBC_OK);
    EXPECT_TRUE(multibyte.empty());
    EXPECT_EQ(LLBC_Transcoder::WideCharToMultiByte("not-a-real-encoding",
                                                    LLBC_WString(1, static_cast<wchar>('x')),
                                                    multibyte),
              LLBC_FAILED);
    EXPECT_TRUE(multibyte.empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    const LLBC_WString invalidUtf16(1, static_cast<wchar>(0xd800));
    EXPECT_EQ(LLBC_Transcoder::WideCharToMultiByte("UTF-8", invalidUtf16, multibyte), LLBC_FAILED);
    EXPECT_TRUE(multibyte.empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    EXPECT_EQ(LLBC_Transcoder::MultiByteToWideCharFile("not-a-real-encoding",
                                                        utf8,
                                                        files.Path(".wide")),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(LLBC_Transcoder::MultiByteFileToWideCharFile(
                  "UTF-8", missingPath, files.Path(".wide-copy")),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    ASSERT_EQ(LLBC_Transcoder::MultiByteToWideChar("UTF-8", utf8, wide), LLBC_OK);
    EXPECT_EQ(LLBC_Transcoder::WideCharToMultiByteFile(
                  "not-a-real-encoding", wide, files.Path(".utf8-copy")),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(LLBC_Transcoder::WideCharFileToMultiByteFile(
                  "UTF-8", oddWidePath, files.Path(".utf8-copy")),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    EXPECT_EQ(LLBC_Transcoder::MultiByteToMultiByte("not-a-real-encoding",
                                                     utf8,
                                                     "UTF-8",
                                                     multibyte),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(LLBC_Transcoder::MultiByteFileToMultiByte(
                  "UTF-8", missingPath, "UTF-8", multibyte),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(LLBC_Transcoder::MultiByteToMultiByteFile(
                  "not-a-real-encoding", utf8, "UTF-8", files.Path(".multibyte-copy")),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(LLBC_Transcoder::MultiByteFileToMultiByteFile(
                  "UTF-8", missingPath, "UTF-8", files.Path(".file-to-file-copy")),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    EXPECT_EQ(LLBC_Transcoder::MultiByteToWideCharFile("UTF-8", utf8, badDestination),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(LLBC_Transcoder::WideCharToMultiByteFile("UTF-8", wide, badDestination),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

#if LLBC_TARGET_PLATFORM_NON_WIN32
    ScopedPipe pipe;
    ASSERT_TRUE(pipe.Begin());
    const char wideCharBytes[] = {'A', '\0'};
    ASSERT_EQ(write(pipe.WriteFd(), wideCharBytes, sizeof(wideCharBytes)),
              static_cast<ssize_t>(sizeof(wideCharBytes)));
    pipe.CloseWrite();
    const LLBC_String pipePath = LLBC_String().format("/dev/fd/%d", pipe.ReadFd());
    EXPECT_EQ(LLBC_Transcoder::WideCharFileToMultiByte("UTF-8", pipePath, multibyte),
              LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
#endif
}
