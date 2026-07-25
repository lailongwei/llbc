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

#include <chrono>
#include <cstring>
#include <filesystem>

#if LLBC_TARGET_PLATFORM_NON_WIN32
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/file/File.cpp
// @coverage-target: llbc/include/llbc/core/file/FileInl.h

namespace
{

class ScopedTempFiles
{
public:
    ScopedTempFiles()
    : _base(std::filesystem::temp_directory_path() /
            ("llbc_unit_file_" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
    {
    }

    ~ScopedTempFiles()
    {
        std::error_code ignored;
        static const char *suffixes[] = {
            "",
            ".copy",
            ".moved",
            ".touch",
            ".text",
            ".reopen",
            ".raw",
            ".crlf",
            ".empty",
            ".format",
            ".append",
            ".instance-copy",
            ".instance-moved",
            ".empty-copy",
            ".move-source",
            ".move-destination",
            ".auto",
            ".auto-binary",
            ".readonly",
            ".writeonly",
            ".append-binary",
            ".directory-copy",
            ".restricted-copy-source",
            ".restricted-copy-destination",
            ".hard-link-source",
            ".hard-link-destination",
        };

        for (const char *suffix : suffixes)
            std::filesystem::remove(_base.string() + suffix, ignored);

        std::filesystem::remove(_base.parent_path() /
                                    ("." + _base.filename().string() + ".hidden"),
                                ignored);
    }

    LLBC_String Path(const char *suffix = "") const
    {
        return LLBC_String((_base.string() + suffix).c_str());
    }

    LLBC_String HiddenPath() const
    {
        const auto path = _base.parent_path() /
                          ("." + _base.filename().string() + ".hidden");
        return LLBC_String(path.string().c_str());
    }

private:
    std::filesystem::path _base;
};

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

    void CloseWrite()
    {
        if (_fds[1] != -1)
        {
            ::close(_fds[1]);
            _fds[1] = -1;
        }
    }

private:
    int _fds[2] = {-1, -1};
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

class ScopedFifo
{
public:
    bool Begin(const LLBC_String &path)
    {
        if (mkfifo(path.c_str(), S_IRUSR | S_IWUSR) != 0)
            return false;

        _path = path.c_str();
        _readFd = open(_path.c_str(), O_RDONLY | O_NONBLOCK);
        if (_readFd != -1)
            return true;

        unlink(_path.c_str());
        _path.clear();
        return false;
    }

    void CloseRead()
    {
        if (_readFd != -1)
        {
            close(_readFd);
            _readFd = -1;
        }
    }

    bool OpenWrite()
    {
        _writeFd = open(_path.c_str(), O_WRONLY | O_NONBLOCK);
        return _writeFd != -1;
    }

    ~ScopedFifo()
    {
        CloseRead();
        if (_writeFd != -1)
            close(_writeFd);
        if (!_path.empty())
            unlink(_path.c_str());
    }

private:
    std::string _path;
    int _readFd = -1;
    int _writeFd = -1;
};

class ScopedFilePermissions
{
public:
    explicit ScopedFilePermissions(const std::filesystem::path &path)
    : _path(path)
    {
        std::error_code error;
        _original = std::filesystem::status(_path, error).permissions();
        _valid = !error;
    }

    bool Set(std::filesystem::perms permissions, std::error_code &error)
    {
        if (!_valid)
        {
            error = std::make_error_code(std::errc::invalid_argument);
            return false;
        }

        std::filesystem::permissions(
            _path, permissions, std::filesystem::perm_options::replace, error);
        _restore = !error;
        return !error;
    }

    ~ScopedFilePermissions()
    {
        if (!_restore)
            return;

        std::error_code ignored;
        std::filesystem::permissions(
            _path, _original, std::filesystem::perm_options::replace, ignored);
    }

private:
    std::filesystem::path _path;
    std::filesystem::perms _original {};
    bool _valid = false;
    bool _restore = false;
};
#endif

} // namespace

// File modes and unopened-handle errors define the basic API contract before a
// file is opened.
TEST(FileTest, DescribesModesAndRejectsUnopenedOrInvalidOperations)
{
    EXPECT_EQ(LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextRead), "TextRead");
    EXPECT_EQ(LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryRead), "BinaryRead");
    EXPECT_EQ(LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextWrite), "TextWrite");
    EXPECT_EQ(LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryWrite), "BinaryWrite");
    EXPECT_EQ(LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextReadWrite), "TextReadWrite");
    EXPECT_EQ(LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryReadWrite), "BinaryReadWrite");
    EXPECT_EQ(LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextAppendWrite), "TextAppendWrite");
    EXPECT_EQ(LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryAppendWrite), "BinaryAppendWrite");
    EXPECT_EQ(LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::TextAppendReadWrite),
              "TextAppendReadWrite");
    EXPECT_EQ(LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::BinaryAppendReadWrite),
              "BinaryAppendReadWrite");
    EXPECT_EQ(LLBC_FileMode::GetFileModeDesc(LLBC_FileMode::LastestMode), "LastestMode");
    EXPECT_EQ(LLBC_FileMode::GetFileModeDesc(-1), "UnknownFileMode");

    LLBC_File file;
    EXPECT_FALSE(file.IsOpened());
    EXPECT_TRUE(file.GetFilePath().empty());
    EXPECT_EQ(file.GetFileMode(), LLBC_FileMode::Read);
    EXPECT_EQ(file.GetFileNo(), -1);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.GetFileHandle(), LLBC_INVALID_FILE_HANDLE);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.Flush(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.ReOpen(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.SetBufferMode(LLBC_FileBufferMode::FullBuf, 64), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.DiscardPageCache(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.CopyFile("unused"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.MoveFile("unused"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.DeleteFile(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.GetFileSize(), -1);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.Seek(LLBC_FileSeekOrigin::Begin, 0), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.GetFilePosition(), -1);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.SetFilePosition(0), -1);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.OffsetFilePosition(1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.GetReadableSize(), -1);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    char byte = '\0';
    EXPECT_EQ(file.Read(&byte, 1), -1);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.Write(&byte, 1), -1);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.ReadLn(), "");
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.ReadToEnd(), "");
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.FormatWrite("%s", "unused"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_EQ(file.Truncate(0), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);

    ScopedTempFiles paths;
    EXPECT_EQ(file.Open(paths.Path().c_str(), 0), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
}

// Opening establishes the path/handle/mode contract. ReOpen() owns the old
// handle, preserves the path, and validates the requested buffering policy.
TEST(FileTest, OpensReopensConfiguresBuffersAndExposesHandleMetadata)
{
    ScopedTempFiles paths;
    const auto path = paths.Path(".reopen");
    LLBC_File file(path, LLBC_FileMode::TextWrite);
    ASSERT_TRUE(file.IsOpened());

    EXPECT_EQ(file.GetFilePath(), path);
    EXPECT_EQ(file.GetFileMode(), LLBC_FileMode::TextWrite);
    EXPECT_NE(file.GetFileHandle(), LLBC_INVALID_FILE_HANDLE);
    EXPECT_GE(file.GetFileNo(), 0);
    EXPECT_EQ(file.Open(path, LLBC_FileMode::TextWrite), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_OPENED);

    EXPECT_EQ(file.SetBufferMode(LLBC_FileBufferMode::FullBuf, 1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(file.SetBufferMode(12345, 64), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(file.SetBufferMode(LLBC_FileBufferMode::LineBuf, 64), LLBC_OK);
    EXPECT_EQ(file.SetBufferMode(LLBC_FileBufferMode::NoBuf, 0), LLBC_OK);
    EXPECT_EQ(file.Write("first"), LLBC_OK);

    ASSERT_EQ(file.ReOpen(), LLBC_OK);
    EXPECT_EQ(file.GetFilePath(), path);
    EXPECT_EQ(file.GetFileMode(), LLBC_FileMode::TextWrite);
    EXPECT_EQ(file.GetFileSize(), 0);
    ASSERT_EQ(file.ReOpen(LLBC_FileMode::TextRead), LLBC_OK);
    EXPECT_EQ(file.GetFileMode(), LLBC_FileMode::TextRead);
    EXPECT_EQ(file.Truncate(0), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_ALLOW);

#if LLBC_TARGET_PLATFORM_LINUX
    EXPECT_EQ(file.DiscardPageCache(), LLBC_OK);
#else
    EXPECT_EQ(file.DiscardPageCache(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_SUPPORT);
#endif

    file.Close();
    EXPECT_FALSE(file.IsOpened());
    EXPECT_TRUE(file.GetFilePath().empty());
    EXPECT_EQ(file.GetFileMode(), LLBC_FileMode::Read);
}

// Binary reads/writes must preserve scalar representations, embedded NUL bytes,
// seek positions, readable size, and truncate behavior.
TEST(FileTest, ReadsWritesSeeksAndTruncatesBinaryData)
{
    ScopedTempFiles paths;
    LLBC_File file(paths.Path(), LLBC_FileMode::BinaryReadWrite);
    ASSERT_TRUE(file.IsOpened());

    const bool boolValue = true;
    const sint32 intValue = -123456;
    const double doubleValue = 3.25;
    const char payload[] = {'A', '\0', 'B', '\x7f'};
    const sint64 expectedSize = sizeof(boolValue) + sizeof(intValue) + sizeof(doubleValue) + sizeof(payload);

    EXPECT_EQ(file.Write(boolValue), LLBC_OK);
    EXPECT_EQ(file.Write(intValue), LLBC_OK);
    EXPECT_EQ(file.Write(doubleValue), LLBC_OK);
    EXPECT_EQ(file.Write(payload, sizeof(payload)), static_cast<sint64>(sizeof(payload)));
    EXPECT_EQ(file.Flush(), LLBC_OK);
    EXPECT_EQ(file.GetFilePosition(), expectedSize);
    EXPECT_EQ(file.GetFileSize(), expectedSize);
    EXPECT_EQ(file.GetReadableSize(), 0);

    ASSERT_EQ(file.SetFilePosition(0), LLBC_OK);
    bool readBool = false;
    sint32 readInt = 0;
    double readDouble = 0.0;
    char readPayload[sizeof(payload)] {};
    EXPECT_EQ(file.Read(readBool), LLBC_OK);
    EXPECT_EQ(file.Read(readInt), LLBC_OK);
    EXPECT_EQ(file.Read(readDouble), LLBC_OK);
    EXPECT_EQ(file.Read(readPayload, sizeof(readPayload)), static_cast<sint64>(sizeof(readPayload)));
    EXPECT_EQ(readBool, boolValue);
    EXPECT_EQ(readInt, intValue);
    EXPECT_DOUBLE_EQ(readDouble, doubleValue);
    EXPECT_EQ(::memcmp(readPayload, payload, sizeof(payload)), 0);

    ASSERT_EQ(file.Seek(LLBC_FileSeekOrigin::End, -static_cast<sint64>(sizeof(payload))), LLBC_OK);
    EXPECT_EQ(file.GetFilePosition(), expectedSize - static_cast<sint64>(sizeof(payload)));
    EXPECT_EQ(file.OffsetFilePosition(2), expectedSize - 2);
    EXPECT_EQ(file.Truncate(static_cast<size_t>(expectedSize - 1)), LLBC_OK);
    EXPECT_EQ(file.GetFileSize(), expectedSize - 1);
    EXPECT_EQ(LLBC_File::ReadToEnd(paths.Path()).size(), static_cast<size_t>(expectedSize - 1));
}

// Scalar overloads are intentionally thin binary helpers. Exercise every
// supported primitive type plus the string overload so serialization callers
// receive a stable round-trip contract.
TEST(FileTest, RoundTripsAllScalarAndStringWriteReadOverloads)
{
    ScopedTempFiles paths;
    LLBC_File file(paths.Path(), LLBC_FileMode::BinaryReadWrite);
    ASSERT_TRUE(file.IsOpened());

    const bool boolValue = true;
    const sint8 sint8Value = -8;
    const uint8 uint8Value = 8;
    const sint16 sint16Value = -1600;
    const uint16 uint16Value = 1600;
    const sint32 sint32Value = -320000;
    const uint32 uint32Value = 320000;
    const long longValue = -640000L;
    const ulong ulongValue = 640000UL;
    const sint64 sint64Value = -6400000000LL;
    const uint64 uint64Value = 6400000000ULL;
    const float floatValue = 1.25f;
    const double doubleValue = -2.5;
    const ldouble longDoubleValue = 3.75L;
    const LLBC_String stringValue("binary-string");

    EXPECT_EQ(file.Write(boolValue), LLBC_OK);
    EXPECT_EQ(file.Write(sint8Value), LLBC_OK);
    EXPECT_EQ(file.Write(uint8Value), LLBC_OK);
    EXPECT_EQ(file.Write(sint16Value), LLBC_OK);
    EXPECT_EQ(file.Write(uint16Value), LLBC_OK);
    EXPECT_EQ(file.Write(sint32Value), LLBC_OK);
    EXPECT_EQ(file.Write(uint32Value), LLBC_OK);
    EXPECT_EQ(file.Write(longValue), LLBC_OK);
    EXPECT_EQ(file.Write(ulongValue), LLBC_OK);
    EXPECT_EQ(file.Write(sint64Value), LLBC_OK);
    EXPECT_EQ(file.Write(uint64Value), LLBC_OK);
    EXPECT_EQ(file.Write(floatValue), LLBC_OK);
    EXPECT_EQ(file.Write(doubleValue), LLBC_OK);
    EXPECT_EQ(file.Write(longDoubleValue), LLBC_OK);
    EXPECT_EQ(file.Write(stringValue), LLBC_OK);

    ASSERT_EQ(file.SetFilePosition(0), LLBC_OK);
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
    ldouble readLongDouble = 0;
    char readString[32] {};

    EXPECT_EQ(file.Read(readBool), LLBC_OK);
    EXPECT_EQ(file.Read(readSint8), LLBC_OK);
    EXPECT_EQ(file.Read(readUint8), LLBC_OK);
    EXPECT_EQ(file.Read(readSint16), LLBC_OK);
    EXPECT_EQ(file.Read(readUint16), LLBC_OK);
    EXPECT_EQ(file.Read(readSint32), LLBC_OK);
    EXPECT_EQ(file.Read(readUint32), LLBC_OK);
    EXPECT_EQ(file.Read(readLong), LLBC_OK);
    EXPECT_EQ(file.Read(readUlong), LLBC_OK);
    EXPECT_EQ(file.Read(readSint64), LLBC_OK);
    EXPECT_EQ(file.Read(readUint64), LLBC_OK);
    EXPECT_EQ(file.Read(readFloat), LLBC_OK);
    EXPECT_EQ(file.Read(readDouble), LLBC_OK);
    EXPECT_EQ(file.Read(readLongDouble), LLBC_OK);
    EXPECT_EQ(file.Read(readString, stringValue.size()), static_cast<sint64>(stringValue.size()));

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
    EXPECT_EQ(readLongDouble, longDoubleValue);
    EXPECT_EQ(LLBC_String(readString, stringValue.size()), stringValue);
}

// Line-oriented readers accept LF, CRLF, and a lone CR without losing the
// following byte. They also distinguish an empty final read from a valid empty
// line and report truncation for short binary reads.
TEST(FileTest, HandlesLineEndingAndEndOfFileEdgeCases)
{
    ScopedTempFiles paths;
    const auto rawPath = paths.Path(".raw");
    const auto crlfPath = paths.Path(".crlf");
    const auto emptyPath = paths.Path(".empty");

    {
        LLBC_File rawFile(rawPath, LLBC_FileMode::BinaryWrite);
        ASSERT_TRUE(rawFile.IsOpened());
        static const char contents[] = "alpha\rbeta\ncharlie\r\ndelta";
        ASSERT_EQ(rawFile.Write(contents, sizeof(contents) - 1),
                  static_cast<sint64>(sizeof(contents) - 1));
    }

    {
        LLBC_File rawFile(rawPath, LLBC_FileMode::BinaryRead);
        ASSERT_TRUE(rawFile.IsOpened());
        EXPECT_EQ(rawFile.ReadLn(), "alpha");
        EXPECT_EQ(rawFile.ReadLn(), "beta");
        EXPECT_EQ(rawFile.ReadLn(), "charlie");
        EXPECT_EQ(rawFile.ReadLn(), "delta");
        EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);
        EXPECT_EQ(rawFile.ReadLn(), "");
        EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_TRUNCATED);

        ASSERT_EQ(rawFile.SetFilePosition(rawFile.GetFileSize() - 1), LLBC_OK);
        sint64 shortRead = 0;
        EXPECT_EQ(rawFile.Read(shortRead), LLBC_FAILED);
        EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_TRUNCATED);
    }

    {
        LLBC_File crlfFile(crlfPath, LLBC_FileMode::BinaryWrite);
        ASSERT_TRUE(crlfFile.IsOpened());
        EXPECT_EQ(crlfFile.Write("one\r\ntwo\r\n"), LLBC_OK);
    }
    {
        LLBC_File crlfFile(crlfPath, LLBC_FileMode::BinaryRead);
        ASSERT_TRUE(crlfFile.IsOpened());
        const auto lines = crlfFile.ReadLns();
        ASSERT_EQ(lines.size(), 3lu);
        EXPECT_EQ(lines[0], "one");
        EXPECT_EQ(lines[1], "two");
        EXPECT_EQ(lines[2], "");
    }

    LLBC_File emptyFile(emptyPath, LLBC_FileMode::BinaryWrite);
    ASSERT_TRUE(emptyFile.IsOpened());
    emptyFile.Close();
    ASSERT_EQ(emptyFile.Open(emptyPath, LLBC_FileMode::BinaryRead), LLBC_OK);
    EXPECT_EQ(emptyFile.ReadToEnd(), "");
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);
    EXPECT_TRUE(emptyFile.ReadLns().empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_TRUNCATED);

    LLBC_File unopened;
    EXPECT_TRUE(unopened.ReadLns().empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_OPEN);
    EXPECT_TRUE(LLBC_File::ReadToEnd(paths.Path(".missing")).empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
}

// Text line helpers normalize line endings while static file operations provide
// copy, move, attributes, touch, and deletion workflows.
TEST(FileTest, HandlesTextLinesAndStaticFileOperations)
{
    ScopedTempFiles paths;
    const auto sourcePath = paths.Path();
    const auto textPath = paths.Path(".text");
    const auto copyPath = paths.Path(".copy");
    const auto movedPath = paths.Path(".moved");
    const auto touchPath = paths.Path(".touch");

    {
        LLBC_File textFile(textPath, LLBC_FileMode::TextWrite);
        ASSERT_TRUE(textFile.IsOpened());
        EXPECT_EQ(textFile.WriteLn("first", LLBC_FileNewLineFormat::UnixStyle), LLBC_OK);
        EXPECT_EQ(textFile.WriteLn("second", LLBC_FileNewLineFormat::WindowsStyle), LLBC_OK);
        EXPECT_EQ(textFile.WriteLns(LLBC_Strings {"third", "fourth"}, LLBC_FileNewLineFormat::UnixStyle),
                  LLBC_OK);
    }

    {
        LLBC_File textFile(textPath, LLBC_FileMode::TextRead);
        ASSERT_TRUE(textFile.IsOpened());
        EXPECT_EQ(textFile.ReadLn(), "first");
        EXPECT_EQ(textFile.ReadLn(), "second");
        const auto remaining = textFile.ReadLns();
        ASSERT_EQ(remaining.size(), 3lu);
        EXPECT_EQ(remaining[0], "third");
        EXPECT_EQ(remaining[1], "fourth");
        EXPECT_EQ(remaining[2], "");
    }

    {
        LLBC_File source(sourcePath, LLBC_FileMode::BinaryWrite);
        ASSERT_TRUE(source.IsOpened());
        EXPECT_EQ(source.FormatWrite("%s:%d", "llbc", 42), LLBC_OK);
    }
    EXPECT_TRUE(LLBC_File::Exists(sourcePath));

    LLBC_FileAttributes attrs {};
    ASSERT_EQ(LLBC_File::GetFileAttributes(sourcePath, attrs), LLBC_OK);
    EXPECT_FALSE(attrs.isDirectory);
    EXPECT_EQ(attrs.fileSize, 7);

    EXPECT_EQ(LLBC_File::CopyFile(sourcePath, copyPath), LLBC_OK);
    EXPECT_EQ(LLBC_File::CopyFile(sourcePath, copyPath), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_EXIST);
    EXPECT_EQ(LLBC_File::CopyFile(sourcePath, copyPath, true), LLBC_OK);
    EXPECT_EQ(LLBC_File::MoveFile(copyPath, movedPath), LLBC_OK);
    EXPECT_FALSE(LLBC_File::Exists(copyPath));
    EXPECT_TRUE(LLBC_File::Exists(movedPath));

    EXPECT_EQ(LLBC_File::TouchFile(touchPath), LLBC_OK);
    EXPECT_TRUE(LLBC_File::Exists(touchPath));

    LLBC_File openedMoved(movedPath, LLBC_FileMode::BinaryRead);
    ASSERT_TRUE(openedMoved.IsOpened());
    EXPECT_EQ(openedMoved.DeleteFile(), LLBC_OK);
    EXPECT_FALSE(LLBC_File::Exists(movedPath));
    EXPECT_EQ(LLBC_File::DeleteFile(sourcePath), LLBC_OK);
    EXPECT_EQ(LLBC_File::DeleteFile(touchPath), LLBC_OK);
}

// Auto newline selection follows the file mode/platform policy, while an empty
// line list is a valid no-op for generated text output.
TEST(FileTest, UsesAutoNewlinePoliciesForTextAndBinaryFiles)
{
    ScopedTempFiles paths;
    const auto textPath = paths.Path(".auto");
    const auto binaryPath = paths.Path(".auto-binary");

    {
        LLBC_File textFile(textPath, LLBC_FileMode::TextWrite);
        ASSERT_TRUE(textFile.IsOpened());
        EXPECT_EQ(textFile.WriteLn("text", LLBC_FileNewLineFormat::AutoMatch), LLBC_OK);
        EXPECT_EQ(textFile.WriteLns(LLBC_Strings(), LLBC_FileNewLineFormat::AutoMatch), LLBC_OK);
    }
    EXPECT_EQ(LLBC_File::ReadToEnd(textPath), "text\n");

    {
        LLBC_File binaryFile(binaryPath, LLBC_FileMode::BinaryWrite);
        ASSERT_TRUE(binaryFile.IsOpened());
        EXPECT_EQ(binaryFile.WriteLn("binary", LLBC_FileNewLineFormat::AutoMatch), LLBC_OK);
    }
#if LLBC_TARGET_PLATFORM_WIN32
    EXPECT_EQ(LLBC_File::ReadToEnd(binaryPath), "binary\r\n");
#else
    EXPECT_EQ(LLBC_File::ReadToEnd(binaryPath), "binary\n");
#endif
}

// C-string and printf-style writers support no-op, stack-buffer, and heap-buffer
// formatting paths. Append mode and the instance copy/move helpers preserve the
// expected ownership and file contents.
TEST(FileTest, WritesFormatsAppendsAndUsesInstanceCopyMoveOperations)
{
    ScopedTempFiles paths;
    const auto formatPath = paths.Path(".format");
    const auto copyPath = paths.Path(".instance-copy");
    const auto movedPath = paths.Path(".instance-moved");
    const std::string largePayload(9000, 'x');
    const std::string expectedPayload = "prefix:7|" + largePayload + "|tail";

    {
        LLBC_File writer(formatPath, LLBC_FileMode::BinaryWrite);
        ASSERT_TRUE(writer.IsOpened());
        EXPECT_EQ(writer.Write(static_cast<const char *>(nullptr)), LLBC_OK);
        EXPECT_EQ(writer.Write(""), LLBC_OK);
        EXPECT_EQ(writer.Write("prefix"), LLBC_OK);
        EXPECT_EQ(writer.FormatWrite("%s", ""), LLBC_OK);
        EXPECT_EQ(writer.FormatWrite(":%d|", 7), LLBC_OK);
        EXPECT_EQ(writer.FormatWrite("%s|", largePayload.c_str()), LLBC_OK);
    }

    {
        LLBC_File appendFile(formatPath, LLBC_FileMode::BinaryAppendReadWrite);
        ASSERT_TRUE(appendFile.IsOpened());
        EXPECT_EQ(appendFile.GetFileMode(), LLBC_FileMode::BinaryAppendReadWrite);
        EXPECT_EQ(appendFile.Write("tail"), LLBC_OK);
        EXPECT_EQ(appendFile.Flush(), LLBC_OK);
        ASSERT_EQ(appendFile.SetFilePosition(0), LLBC_OK);
        const auto content = appendFile.ReadToEnd();
        EXPECT_EQ(std::string(content.c_str(), content.size()), expectedPayload);
    }

    {
        LLBC_File source(formatPath, LLBC_FileMode::BinaryAppendReadWrite);
        ASSERT_TRUE(source.IsOpened());
        ASSERT_EQ(source.CopyFile(copyPath), LLBC_OK);
        EXPECT_TRUE(LLBC_File::Exists(copyPath));
        ASSERT_EQ(source.MoveFile(movedPath), LLBC_OK);
        EXPECT_FALSE(source.IsOpened());
        EXPECT_TRUE(source.GetFilePath().empty());
    }
    EXPECT_FALSE(LLBC_File::Exists(formatPath));
    const auto copiedContent = LLBC_File::ReadToEnd(copyPath);
    const auto movedContent = LLBC_File::ReadToEnd(movedPath);
    EXPECT_EQ(std::string(copiedContent.c_str(), copiedContent.size()), expectedPayload);
    EXPECT_EQ(std::string(movedContent.c_str(), movedContent.size()), expectedPayload);

    const auto emptyPath = paths.Path(".empty");
    const auto emptyCopyPath = paths.Path(".empty-copy");
    {
        LLBC_File emptyFile(emptyPath, LLBC_FileMode::BinaryWrite);
        ASSERT_TRUE(emptyFile.IsOpened());
    }
    EXPECT_EQ(LLBC_File::CopyFile(emptyPath, emptyCopyPath), LLBC_OK);
    EXPECT_TRUE(LLBC_File::ReadToEnd(emptyCopyPath).empty());
    EXPECT_EQ(LLBC_File::CopyFile(paths.Path(".missing"), paths.Path(".copy")), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    const auto moveSourcePath = paths.Path(".move-source");
    const auto moveDestPath = paths.Path(".move-destination");
    {
        LLBC_File source(moveSourcePath, LLBC_FileMode::BinaryWrite);
        LLBC_File destination(moveDestPath, LLBC_FileMode::BinaryWrite);
        ASSERT_TRUE(source.IsOpened());
        ASSERT_TRUE(destination.IsOpened());
        ASSERT_EQ(source.Write("from"), LLBC_OK);
        ASSERT_EQ(destination.Write("to"), LLBC_OK);
    }
    EXPECT_EQ(LLBC_File::MoveFile(moveSourcePath, moveDestPath), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_EXIST);
    EXPECT_EQ(LLBC_File::MoveFile(moveSourcePath, moveDestPath, true), LLBC_OK);
    EXPECT_EQ(LLBC_File::ReadToEnd(moveDestPath), "from");
}

// Attribute and touch helpers are used by configuration, cache, and deployment
// code to reason about ordinary files, hidden files, and directory entries.
TEST(FileTest, ReportsAttributesAndUpdatesExistingFileTimestamps)
{
    ScopedTempFiles paths;
    const auto hiddenPath = paths.HiddenPath();
    const auto touchPath = paths.Path(".touch");

    {
        LLBC_File hiddenFile(hiddenPath, LLBC_FileMode::BinaryWrite);
        ASSERT_TRUE(hiddenFile.IsOpened());
        ASSERT_EQ(hiddenFile.Write("hidden"), LLBC_OK);
        ASSERT_EQ(hiddenFile.Flush(), LLBC_OK);

        LLBC_FileAttributes attrs {};
        ASSERT_EQ(hiddenFile.GetFileAttributes(attrs), LLBC_OK);
        EXPECT_TRUE(attrs.readable);
        EXPECT_TRUE(attrs.writable);
        EXPECT_TRUE(attrs.hidden);
        EXPECT_FALSE(attrs.isDirectory);
        EXPECT_EQ(attrs.fileSize, 6);
    }

    LLBC_FileAttributes directoryAttrs {};
    ASSERT_EQ(LLBC_File::GetFileAttributes(
                  LLBC_String(std::filesystem::temp_directory_path().string().c_str()),
                  directoryAttrs),
              LLBC_OK);
    EXPECT_TRUE(directoryAttrs.isDirectory);
    EXPECT_EQ(directoryAttrs.fileSize, 0);

    LLBC_FileAttributes missingAttrs {};
    EXPECT_EQ(LLBC_File::GetFileAttributes(paths.Path(".missing"), missingAttrs), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    ASSERT_EQ(LLBC_File::TouchFile(touchPath), LLBC_OK);
    ASSERT_TRUE(LLBC_File::Exists(touchPath));
    const timespec knownTime {1700000000, 0};
    ASSERT_EQ(LLBC_File::TouchFile(touchPath,
                                   true,
                                   &knownTime,
                                   true,
                                   &knownTime),
              LLBC_OK);
    ASSERT_EQ(LLBC_File::TouchFile(touchPath,
                                   true,
                                   &knownTime,
                                   true,
                                   nullptr),
              LLBC_OK);
    ASSERT_EQ(LLBC_File::TouchFile(touchPath, false, nullptr, false, nullptr), LLBC_OK);

    LLBC_FileAttributes touchedAttrs {};
    ASSERT_EQ(LLBC_File::GetFileAttributes(touchPath, touchedAttrs), LLBC_OK);
    EXPECT_FALSE(touchedAttrs.isDirectory);
    EXPECT_GE(touchedAttrs.lastModifyTime.tv_sec, 0);
}

// Read-only/write-only streams must surface C-library failures instead of
// silently reporting success. Exercise invalid positioning, platform cache
// advice, append-only modes, and safe static filesystem failure paths.
TEST(FileTest, ReportsDirectionalIoAndStaticFilesystemFailurePaths)
{
    ScopedTempFiles paths;
    const auto sourcePath = paths.Path(".readonly");
    const auto writeOnlyPath = paths.Path(".writeonly");
    const auto appendTextPath = paths.Path(".append");
    const auto appendBinaryPath = paths.Path(".append-binary");

    {
        LLBC_File source(sourcePath, LLBC_FileMode::BinaryWrite);
        ASSERT_TRUE(source.IsOpened());
        ASSERT_EQ(source.Write("payload"), LLBC_OK);
    }

    LLBC_File readOnly(sourcePath, LLBC_FileMode::BinaryRead);
    ASSERT_TRUE(readOnly.IsOpened());
    EXPECT_EQ(readOnly.Write("x"), -1);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(readOnly.WriteLn("x"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(readOnly.WriteLns(LLBC_Strings {"x", "y"}), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    const sint32 typedWriteValue = 7;
    EXPECT_EQ(readOnly.Write(typedWriteValue), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(readOnly.Write(LLBC_String("typed")), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    EXPECT_EQ(LLBC_File::CopyFile(sourcePath, sourcePath, true), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(LLBC_File::ReadToEnd(sourcePath), "payload");

#if LLBC_TARGET_PLATFORM_NON_WIN32
    const LLBC_String tempDirectory(
        std::filesystem::temp_directory_path().string().c_str());
    LLBC_File directoryFile(tempDirectory, LLBC_FileMode::BinaryRead);
    ASSERT_TRUE(directoryFile.IsOpened());
    EXPECT_TRUE(directoryFile.ReadLns().empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    const LLBC_String directoryCopyPath = paths.Path(".directory-copy");
    EXPECT_EQ(LLBC_File::CopyFile(tempDirectory, directoryCopyPath), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_ALLOW);
    EXPECT_FALSE(LLBC_File::Exists(directoryCopyPath));

    if (::geteuid() != 0)
    {
        const LLBC_String restrictedSourcePath = paths.Path(".restricted-copy-source");
        const LLBC_String restrictedDestinationPath =
            paths.Path(".restricted-copy-destination");
        {
            LLBC_File restrictedSource(restrictedSourcePath, LLBC_FileMode::BinaryWrite);
            ASSERT_TRUE(restrictedSource.IsOpened());
            ASSERT_EQ(restrictedSource.Write("locked"), LLBC_OK);
        }

        ScopedFilePermissions permissions(
            std::filesystem::path(restrictedSourcePath.c_str()));
        std::error_code permissionError;
        ASSERT_TRUE(permissions.Set(std::filesystem::perms::none, permissionError))
            << permissionError.message();

        EXPECT_EQ(LLBC_File::CopyFile(restrictedSourcePath, restrictedDestinationPath),
                  LLBC_FAILED);
        EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
        EXPECT_FALSE(LLBC_File::Exists(restrictedDestinationPath));
    }
#endif

    LLBC_File seekFile(sourcePath, LLBC_FileMode::BinaryRead);
    ASSERT_TRUE(seekFile.IsOpened());
    EXPECT_EQ(seekFile.Seek(LLBC_FileSeekOrigin::Begin, -1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(seekFile.SetFilePosition(-1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    ASSERT_EQ(seekFile.SetFilePosition(0), LLBC_OK);
    EXPECT_EQ(seekFile.OffsetFilePosition(-1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    LLBC_File writeOnly(writeOnlyPath, LLBC_FileMode::BinaryWrite);
    ASSERT_TRUE(writeOnly.IsOpened());
    ASSERT_EQ(writeOnly.Write("payload"), LLBC_OK);

#if LLBC_TARGET_PLATFORM_LINUX
    EXPECT_EQ(writeOnly.DiscardPageCache(), LLBC_OK);
#else
    EXPECT_EQ(writeOnly.DiscardPageCache(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_SUPPORT);
#endif

    ASSERT_EQ(writeOnly.SetFilePosition(0), LLBC_OK);
    EXPECT_TRUE(writeOnly.ReadToEnd().empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    ASSERT_EQ(writeOnly.SetFilePosition(0), LLBC_OK);
    EXPECT_TRUE(writeOnly.ReadLns().empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    ASSERT_EQ(writeOnly.SetFilePosition(0), LLBC_OK);
    sint32 typedReadValue = 0;
    EXPECT_EQ(writeOnly.Read(typedReadValue), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    {
        LLBC_File appendText(appendTextPath, LLBC_FileMode::TextAppendWrite);
        ASSERT_TRUE(appendText.IsOpened());
        EXPECT_EQ(appendText.Write("one"), LLBC_OK);
    }
    {
        LLBC_File appendText(appendTextPath, LLBC_FileMode::TextAppendWrite);
        ASSERT_TRUE(appendText.IsOpened());
        EXPECT_EQ(appendText.Write("two"), LLBC_OK);
    }
    EXPECT_EQ(LLBC_File::ReadToEnd(appendTextPath), "onetwo");

    {
        LLBC_File appendBinary(appendBinaryPath, LLBC_FileMode::BinaryAppendWrite);
        ASSERT_TRUE(appendBinary.IsOpened());
        EXPECT_EQ(appendBinary.Write("bin"), LLBC_OK);
    }
    EXPECT_EQ(LLBC_File::ReadToEnd(appendBinaryPath), "bin");

    const LLBC_String missingPath("/definitely/not/a/real/llbc-file");
    EXPECT_EQ(LLBC_File::CopyFile(sourcePath, missingPath), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(LLBC_File::MoveFile(missingPath, paths.Path(".moved"), true), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(LLBC_File::DeleteFile(missingPath), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(LLBC_File::TouchFile(missingPath), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
}

// Distinct paths can still refer to the same file through a hard link. Copying
// over that alias must fail before the destination is opened and truncates data.
TEST(FileTest, RejectsHardLinkedDestinationThatAliasesSource)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    ScopedTempFiles paths;
    const LLBC_String sourcePath = paths.Path(".hard-link-source");
    const LLBC_String hardLinkPath = paths.Path(".hard-link-destination");
    {
        LLBC_File source(sourcePath, LLBC_FileMode::BinaryWrite);
        ASSERT_TRUE(source.IsOpened());
        ASSERT_EQ(source.Write("hard-link-payload"), LLBC_OK);
    }

    std::error_code linkError;
    std::filesystem::create_hard_link(
        std::filesystem::path(sourcePath.c_str()),
        std::filesystem::path(hardLinkPath.c_str()),
        linkError);
    if (linkError)
        GTEST_SKIP() << "unable to create hard link: " << linkError.message();

    EXPECT_EQ(LLBC_File::CopyFile(sourcePath, hardLinkPath, true), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(LLBC_File::ReadToEnd(sourcePath), "hard-link-payload");
    EXPECT_EQ(LLBC_File::ReadToEnd(hardLinkPath), "hard-link-payload");
#else
    GTEST_SKIP() << "hard-link alias detection is POSIX-specific";
#endif
}

// Pipes are readable streams but not seekable files. Position/size helpers and
// ReadLns() must preserve the C-library error instead of confusing two failed
// position queries with a normal end-of-file condition.
TEST(FileTest, ReportsNonSeekableHandleErrorsWithoutLosingReadableData)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    ScopedTempFiles paths;
    ScopedPipe pipe;
    ASSERT_TRUE(pipe.Create());
    ASSERT_EQ(::write(pipe.WriteFd(), "line\n", 5), 5);
    pipe.CloseWrite();

    const LLBC_String pipePath = LLBC_String().format("/dev/fd/%d", pipe.ReadFd());
    LLBC_File file(pipePath, LLBC_FileMode::BinaryRead);
    ASSERT_TRUE(file.IsOpened());

    EXPECT_EQ(file.GetFileSize(), -1);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(file.GetFilePosition(), -1);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(file.GetReadableSize(), -1);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(file.Seek(LLBC_FileSeekOrigin::Begin, 0), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(file.SetFilePosition(0), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_EQ(file.OffsetFilePosition(1), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_TRUE(file.ReadLns().empty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    const LLBC_String copyPath = paths.Path(".copy");
    EXPECT_EQ(LLBC_File::CopyFile(pipePath, copyPath), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_FALSE(LLBC_File::Exists(copyPath));

    char data[6] {};
    EXPECT_EQ(file.Read(data, 5), 5);
    EXPECT_EQ(std::string(data, 5), "line\n");
#else
    GTEST_SKIP() << "/dev/fd pipe semantics are POSIX-specific";
#endif
}

// FIFO-backed streams expose real POSIX write failures without modifying
// filesystem permissions. LLBC_File must surface formatted-output and flush
// failures instead of reporting buffered operations as OK.
TEST(FileTest, ReportsFifoBackedOutputFailures)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    ScopedSignalIgnore ignoreSigpipe;
    ASSERT_TRUE(ignoreSigpipe.Begin(SIGPIPE));

    ScopedTempFiles paths;

    ScopedFifo formatFifo;
    const LLBC_String formatPath = paths.Path(".format-fifo");
    ASSERT_TRUE(formatFifo.Begin(formatPath));
    LLBC_File formatFile(formatPath, LLBC_FileMode::BinaryWrite);
    ASSERT_TRUE(formatFile.IsOpened());
    ASSERT_EQ(formatFile.SetBufferMode(LLBC_FileBufferMode::NoBuf, 0), LLBC_OK);
    formatFifo.CloseRead();
    EXPECT_EQ(formatFile.FormatWrite("format-%d", 7), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    ScopedFifo flushFifo;
    const LLBC_String flushPath = paths.Path(".flush-fifo");
    ASSERT_TRUE(flushFifo.Begin(flushPath));
    LLBC_File flushFile(flushPath, LLBC_FileMode::BinaryWrite);
    ASSERT_TRUE(flushFile.IsOpened());
    ASSERT_EQ(flushFile.SetBufferMode(LLBC_FileBufferMode::FullBuf, 64), LLBC_OK);
    flushFifo.CloseRead();
    ASSERT_EQ(flushFile.Write("queued"), LLBC_OK);
    EXPECT_EQ(flushFile.Flush(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);

    ScopedFifo copySourceFifo;
    const LLBC_String copySourcePath = paths.Path(".copy-source-fifo");
    const LLBC_String copyDestinationPath = paths.Path(".copy-destination");
    ASSERT_TRUE(copySourceFifo.Begin(copySourcePath));
    ASSERT_TRUE(copySourceFifo.OpenWrite());
    EXPECT_EQ(LLBC_File::CopyFile(copySourcePath, copyDestinationPath), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_CLIB);
    EXPECT_FALSE(LLBC_File::Exists(copyDestinationPath));
#else
    GTEST_SKIP() << "pipe-backed failure paths are POSIX-specific";
#endif
}
