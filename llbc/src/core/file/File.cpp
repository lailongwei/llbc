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


#include "llbc/common/Export.h"

#if LLBC_TARGET_PLATFORM_NON_WIN32
 #include <fcntl.h>
#endif // Non-Win32

#include "llbc/core/os/OS_Time.h"

#include "llbc/core/file/File.h"
#if LLBC_TARGET_PLATFORM_NON_WIN32
 #include "llbc/core/file/Directory.h"
#endif // Non-WIN32

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

__LLBC_NS_BEGIN

LLBC_String LLBC_FileMode::GetFileModeDesc(int fileMode)
{
    switch (fileMode)
    {
    case LLBC_FileMode::TextRead:
        return "TextRead";
    case LLBC_FileMode::BinaryRead:
        return "BinaryRead";

    case LLBC_FileMode::TextWrite:
        return "TextWrite";
    case LLBC_FileMode::BinaryWrite:
        return "BinaryWrite";

    case LLBC_FileMode::TextReadWrite:
        return "TextReadWrite";
    case LLBC_FileMode::BinaryReadWrite:
        return "BinaryReadWrite";

    case LLBC_FileMode::TextAppendWrite:
        return "TextAppendWrite";
    case LLBC_FileMode::BinaryAppendWrite:
        return "BinaryAppendWrite";

    case LLBC_FileMode::TextAppendReadWrite:
        return "TextAppendReadWrite";
    case LLBC_FileMode::BinaryAppendReadWrite:
        return "BinaryAppendReadWrite";

    case LLBC_FileMode::LastestMode:
        return "LastestMode";

    default:
        break;
    }

    return "UnknownFileMode";
}

LLBC_File::LLBC_File()
: _mode(LLBC_FileMode::Read)
, _handle(LLBC_INVALID_FILE_HANDLE)
{
}

LLBC_File::LLBC_File(const LLBC_String &path, int mode)
: _mode(LLBC_FileMode::Read)
, _handle(LLBC_INVALID_FILE_HANDLE)
{
    if (Open(path, mode) == LLBC_OK)
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);
}

LLBC_File::~LLBC_File()
{
    Close();
}

int LLBC_File::Open(const LLBC_String &path, int mode)
{
    if (IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_OPENED);
        return LLBC_FAILED;
    }

    const char *modeRepr = ParseFileMode(mode);
    if (modeRepr == nullptr)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }
    else if ((_handle = fopen(
        path.c_str(), modeRepr)) == LLBC_INVALID_FILE_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    _mode = mode;
    _path.assign(path.c_str(), path.length());

    return LLBC_OK;
}

int LLBC_File::ReOpen(int mode)
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_FAILED;
    }

    LLBC_String path = _path;
    if (mode == static_cast<int>(LLBC_FileMode::LastestMode))
        mode = _mode;

    Close();

    return Open(path, mode);
}

bool LLBC_File::IsOpened() const
{
    return _handle != LLBC_INVALID_FILE_HANDLE;
}

void LLBC_File::Close()
{
    if (!IsOpened())
        return;

    fclose(_handle);
    _handle = LLBC_INVALID_FILE_HANDLE;

    _path.clear();
    _mode = LLBC_FileMode::Read;
}

const LLBC_String &LLBC_File::GetFilePath() const
{
    return _path;
}

int LLBC_File::GetFileNo() const
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return -1;
    }

    return GetFileNo(const_cast<LLBC_FileHandle>(_handle));
}

int LLBC_File::GetFileNo(LLBC_FileHandle handle)
{
#if LLBC_TARGET_PLATFORM_WIN32
    int fileNo = _fileno(handle);
#else
    int fileNo = fileno(handle);
#endif

    if (UNLIKELY(fileNo == -1))
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return -1;
    }

    return fileNo;
}

LLBC_FileHandle LLBC_File::GetFileHandle() const
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_INVALID_FILE_HANDLE;
    }

    return const_cast<LLBC_FileHandle>(_handle);
}

int LLBC_File::GetFileMode() const
{
    return _mode;
}

int LLBC_File::SetBufferMode(int bufferMode, size_t size)
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_FAILED;
    }

    if (bufferMode == LLBC_FileBufferMode::NoBuf)
    {
        size = 0;
    }
    else if (size < 2 || size > INT_MAX)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if (setvbuf(_handle, nullptr, bufferMode, size) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_File::DiscardPageCache() const
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_FAILED;
    }

    const int fd = GetFileNo();
    if (UNLIKELY(fd == -1))
        return LLBC_FAILED;

    int ret = LLBC_OK;

#if LLBC_TARGET_PLATFORM_LINUX
    // This allows the kernel to free the page cache associated with this file,
    // making memory available for other processes. It is a performance optimization,
    // especially useful when dealing with large files that are read only once
    if (UNLIKELY(posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED) != 0))
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        ret = LLBC_FAILED;
    }
#elif LLBC_TARGET_PLATFORM_MAC
    // Flush all dirty data (modified but not written to disk cache) to storage device (synchronous).
    // Discard all cache data (both clean and dirty) associated with this file from memory.
    if (fcntl(fd, F_PURGEFSYNC) == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        ret = LLBC_FAILED;
    }
#else
    LLBC_SetLastError(LLBC_ERROR_NOT_SUPPORT);
    ret = LLBC_FAILED;
#endif

    return ret;
}


sint64 LLBC_File::GetFileSize() const
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return -1;
    }

#if LLBC_TARGET_PLATFORM_WIN32
    const sint64 oldPos = _ftelli64(_handle);
    if (_fseeki64(_handle, 0, LLBC_FileSeekOrigin::End) != 0)
#else
    const sint64 oldPos = ftell(_handle);
    if (fseek(_handle, 0, LLBC_FileSeekOrigin::End) != 0)
#endif
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return -1;
    }

#if LLBC_TARGET_PLATFORM_WIN32
    sint64 fileSize = _ftelli64(_handle);
#else
    sint64 fileSize = ftell(_handle);
#endif
    if (fileSize == -1)
        LLBC_SetLastError(LLBC_ERROR_CLIB);

#if LLBC_TARGET_PLATFORM_WIN32
    if (_fseeki64(_handle, oldPos, LLBC_FileSeekOrigin::Begin) != 0)
#else
    if (fseek(_handle, oldPos, LLBC_FileSeekOrigin::Begin) != 0)
#endif
    {
        if (fileSize >= 0)
            fileSize = -1;

        LLBC_SetLastError(LLBC_ERROR_CLIB);
    }

    return fileSize;
}

int LLBC_File::Seek(int seekOrigin, sint64 offset)
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_WIN32
    if (_fseeki64(_handle, offset, seekOrigin) != 0)
#else
    if (fseek(_handle, offset, seekOrigin) != 0)
#endif
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

sint64 LLBC_File::GetFilePosition() const
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return -1;
    }

#if LLBC_TARGET_PLATFORM_WIN32
    const sint64 pos = _ftelli64(_handle);
#else
    const sint64 pos = ftell(_handle);
#endif
    if (pos == -1)
        LLBC_SetLastError(LLBC_ERROR_CLIB);

    return pos;
}

int LLBC_File::SetFilePosition(sint64 position)
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return -1;
    }

#if LLBC_TARGET_PLATFORM_WIN32
    if (_fseeki64(_handle, position, LLBC_FileSeekOrigin::Begin) != 0)
#else
    if (fseek(_handle, position, LLBC_FileSeekOrigin::Begin) != 0)
#endif
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

sint64 LLBC_File::OffsetFilePosition(sint64 offset)
{
    const sint64 curPos = GetFilePosition();
    if (UNLIKELY(curPos == -1))
        return LLBC_FAILED;

    return SetFilePosition(curPos + offset);
}

sint64 LLBC_File::GetReadableSize() const
{
    const sint64 size = GetFileSize();
    if (UNLIKELY(size == -1))
        return -1;

    const sint64 pos = GetFilePosition();
    if (UNLIKELY(pos == -1))
        return -1;

    return size - pos;
}

LLBC_String LLBC_File::ReadLn()
{
    // Read line bytes.
    char ch;
    LLBC_String line;
    bool hasBeenRead = false;
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    while (Read(ch) != LLBC_FAILED)
    {
        hasBeenRead = true;
        if (ch != '\r' && ch != '\n')
        {
            line.append(1, ch);
            continue;
        }

        if (ch == '\r')
        {
            // Read linefeed.
            if (ReadRawObj<char>(ch) == LLBC_OK)
            {
                if (ch != '\n')
                    OffsetFilePosition(-1);
            }
        }

        break;
    }

    // Read first byte failed, return empty line(LLBC_GetLastError() return non LLBC_ERROR_SUCCESS).
    if (!hasBeenRead)
        return line;

    // If read to end, set last error to SUCCESS.
    if (LLBC_GetLastError() == LLBC_ERROR_TRUNCATED)
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    // Return line.
    return line;
}

LLBC_Strings LLBC_File::ReadLns()
{
    // File opened check.
    if (UNLIKELY(!IsOpened()))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_Strings();
    }

    // File position check.
    if (UNLIKELY(GetFilePosition() == GetFileSize()))
    {
        LLBC_SetLastError(LLBC_ERROR_TRUNCATED);
        return LLBC_Strings();
    }

    // Read file content.
    const LLBC_String fileCnt = ReadToEnd();
    if (LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
        return LLBC_Strings();

    // Set error to SUCCESS.
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    // Split(don't strip empty line).
    LLBC_Strings lines = fileCnt.split("\n", -1, false);
    // Strip CR character(\r).
    for (auto &line : lines)
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
    }

    return lines;
}

LLBC_String LLBC_File::ReadToEnd()
{
    LLBC_String str;
    const sint64 readableSize = GetReadableSize();
    if (UNLIKELY(readableSize < 0))
    {
        return str;
    }
    else if (readableSize == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);
        return str;
    }

    str.resize(static_cast<size_t>(readableSize));
    const sint64 actuallyRead = Read(
        const_cast<char *>(str.data()), static_cast<size_t>(readableSize));
    if (actuallyRead == -1)
    {
        str.resize(0);
        return str;
    }

    if (actuallyRead < readableSize)
        str.resize(static_cast<size_t>(actuallyRead));

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return str;
}

 LLBC_String LLBC_File::ReadToEnd(const LLBC_String &filePath)
 {
     LLBC_File file(filePath, LLBC_FileMode::BinaryRead);
     if (!file.IsOpened())
         return "";

     return file.ReadToEnd();
 }

sint64 LLBC_File::Read(void *buf, size_t size)
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return -1;
    }

    const size_t actuallyRead = fread(buf, 1, size, _handle);
    if (actuallyRead != size && ferror(_handle) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return -1;
    }

    LLBC_SetLastError(actuallyRead != size ? LLBC_ERROR_TRUNCATED : LLBC_ERROR_SUCCESS);
    return static_cast<sint64>(actuallyRead);
}

int LLBC_File::WriteLn(const LLBC_String &line, int newLineFormat)
{
    // Write line content.
    const sint64 contentRet = Write(line.data(), line.size());
    if (contentRet != static_cast<sint64>(line.size()))
        return LLBC_FAILED;

    // If use auto-match new line format and file opened as Text mode, newLineFormat force changed to LF.
    if (newLineFormat == LLBC_FileNewLineFormat::AutoMatch &&
        (_mode & LLBC_FileMode::Text))
        newLineFormat = LLBC_FileNewLineFormat::LineFeed;

    // Determine new line format, if is AutoMatch.
    if (newLineFormat == LLBC_FileNewLineFormat::AutoMatch)
    {
#if LLBC_TARGET_PLATFORM_WIN32
        newLineFormat = LLBC_FileNewLineFormat::WindowsStyle;
#elif LLBC_TARGET_PLATFORM_MAC || LLBC_TARGET_PLATFORM_IPHONE
        newLineFormat = LLBC_FileNewLineFormat::MacStyle;
#else // Linux, Android and others.
        newLineFormat = LLBC_FileNewLineFormat::UnixStyle;
#endif // Win32
    }

    // Write line end.
    sint64 requireRet;
    sint64 lineEndingRet;
    if (newLineFormat == LLBC_FileNewLineFormat::WindowsStyle)
    {
        requireRet = 2;
        lineEndingRet = Write("\r\n", 2);
    }
    else
    {
        requireRet = 1;
        lineEndingRet = Write('\n') == LLBC_OK ? 1 : 0;
    }

    return lineEndingRet != requireRet ? LLBC_FAILED : LLBC_OK;
}

int LLBC_File::WriteLns(const LLBC_Strings &lines, int newLineFormat)
{
    for (auto &line : lines)
    {
        if (WriteLn(line, newLineFormat) != LLBC_OK)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}

sint64 LLBC_File::Write(const void *buf, size_t size)
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return -1;
    }

    const size_t actuallyWrote = fwrite(buf, 1, size, _handle);
    if (actuallyWrote != size && ferror(_handle) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return -1;
    }

    LLBC_SetLastError(actuallyWrote != size ? LLBC_ERROR_TRUNCATED : LLBC_ERROR_SUCCESS);
    return static_cast<sint64>(actuallyWrote);
}

int LLBC_File::FormatWrite(const char *fmt, ...)
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_FAILED;
    }

    // Try use tls.fmtBuf to format.
    auto &tlsBuf = __LLBC_GetLibTls()->commonTls.fmtBuf;

    va_list ap;
    va_start(ap, fmt);
    const int len = vsnprintf(tlsBuf, sizeof(tlsBuf), fmt, ap);
    va_end(ap);
    if (len < 0)
    { 
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }
    else if (len == 0)
    {
        return LLBC_OK;
    }
    else if (static_cast<size_t>(len) < sizeof(tlsBuf))
    {
        const auto writeRet = Write(tlsBuf, len);
        return writeRet == static_cast<sint64>(len) ? LLBC_OK : LLBC_FAILED;
    }
	// Use heapBuf to format.
    const auto heapBuf = LLBC_Malloc(char, len + 1);

    va_start(ap, fmt);
    const int len2 = vsnprintf(heapBuf, len + 1, fmt, ap);
    va_end(ap);
    if (UNLIKELY(len2 < 0))
    {
        free(heapBuf);
        LLBC_SetLastError(LLBC_ERROR_CLIB);

        return LLBC_FAILED;
    }
    else if (UNLIKELY(len2 != len))
    {
        free(heapBuf);
        LLBC_SetLastError(LLBC_ERROR_UNKNOWN);

        return LLBC_FAILED;
    }

    const auto writeRet = Write(heapBuf, len);
    free(heapBuf);

    return writeRet == static_cast<sint64>(len) ? LLBC_OK : LLBC_FAILED;
}

int LLBC_File::Flush()
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_FAILED;
    }

    if (fflush(_handle) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_File::Truncate(size_t newSize)
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_FAILED;
    }

    if ((_mode & LLBC_FileMode::Write) != LLBC_FileMode::Write)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    #if LLBC_TARGET_PLATFORM_WIN32
    if (_chsize_s(GetFileNo(), newSize) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }
    #else // Non-Win32
    if (ftruncate(GetFileNo(), newSize) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }
    #endif // Win32

    rewind(_handle);

    return LLBC_OK;
}

bool LLBC_File::Exists(const LLBC_String &path)
{
#if LLBC_TARGET_PLATFORM_WIN32
    if (_access(path.c_str(), 0) != 0)
#else
    if (access(path.c_str(), 0) != 0)
#endif
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return false;
    }

    return true;
}

int LLBC_File::GetFileAttributes(LLBC_FileAttributes &attrs)
{
    return GetFileAttributes(_path, attrs);
}

int LLBC_File::GetFileAttributes(const LLBC_String &filePath, LLBC_FileAttributes &attrs)
{
#if LLBC_TARGET_PLATFORM_WIN32
    WIN32_FILE_ATTRIBUTE_DATA sysFileAttrData;
    if (!::GetFileAttributesExA(filePath.c_str(), GetFileExInfoStandard, &sysFileAttrData))
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    static const LLBC_String exeSuffix(".exe");

    attrs.readable = true;
    attrs.writable = (sysFileAttrData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != FILE_ATTRIBUTE_READONLY;
    attrs.isDirectory = (sysFileAttrData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
    if (attrs.isDirectory || filePath.length() < 4)
        attrs.execable = false;
    else
        attrs.execable = filePath.substr(filePath.length() - 4, 4).tolower() == exeSuffix;
    attrs.hidden = (sysFileAttrData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN;

    LLBC_WinFileTime2TimeSpec(sysFileAttrData.ftCreationTime, attrs.createTime);
    LLBC_WinFileTime2TimeSpec(sysFileAttrData.ftLastAccessTime, attrs.lastAccessTime);
    LLBC_WinFileTime2TimeSpec(sysFileAttrData.ftLastWriteTime, attrs.lastModifyTime);

    if (attrs.isDirectory)
        attrs.fileSize = 0;
    else
        attrs.fileSize = (static_cast<sint64>(sysFileAttrData.nFileSizeHigh) << 32) + sysFileAttrData.nFileSizeLow;
#else // Non-WIN32
    struct stat cStat;
    if (stat(filePath.c_str(), &cStat) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    attrs.readable = (cStat.st_mode & S_IRUSR) == S_IRUSR;
    attrs.writable = (cStat.st_mode & S_IWUSR) == S_IWUSR;
    attrs.execable = (cStat.st_mode & S_IXUSR) == S_IXUSR;

    const LLBC_String baseName = LLBC_Directory::BaseName(filePath);
    if (baseName.length() > 0 && baseName[0] == '.')
        attrs.hidden = true;
    else
        attrs.hidden = false;

    attrs.isDirectory = (cStat.st_mode & S_IFDIR) == S_IFDIR;

#if LLBC_TARGET_PLATFORM_MAC || LLBC_TARGET_PLATFORM_IPHONE
    memcpy(&attrs.lastAccessTime, &cStat.st_atimespec, sizeof(timespec));
    memcpy(&attrs.lastModifyTime, &cStat.st_mtimespec, sizeof(timespec));
    memcpy(&attrs.lastChangeStatusTime, &cStat.st_ctimespec, sizeof(timespec));
#else // Linux or Android
    memcpy(&attrs.lastAccessTime, &cStat.st_atim, sizeof(timespec));
    memcpy(&attrs.lastModifyTime, &cStat.st_mtim, sizeof(timespec));
    memcpy(&attrs.lastChangeStatusTime, &cStat.st_ctim, sizeof(timespec));
#endif // LLBC_TARGET_PLATFORM_MAC || LLBC_TARGET_PLATFORM_IPHONE

    if (attrs.isDirectory)
        attrs.fileSize = 0;
    else
        attrs.fileSize = cStat.st_size;
#endif

    return LLBC_OK;
}

int LLBC_File::TouchFile(const LLBC_String &filePath,
                         bool updateLastAccessTime,
                         const timespec *lastAccessTime,
                         bool updateLastModifyTime,
                         const timespec *lastModifyTime)
{
    if (!Exists(filePath))
    {
        LLBC_File newFile(filePath, LLBC_FileMode::BinaryWrite);
        if (!newFile.IsOpened())
            return LLBC_FAILED;

        newFile.Close();

        if ((!updateLastAccessTime || lastAccessTime == nullptr) &&
            (!updateLastModifyTime || lastModifyTime == nullptr))
            return LLBC_OK;
    }

#if LLBC_TARGET_PLATFORM_MAC || LLBC_TARGET_PLATFORM_IPHONE
    // In MAC or iPhone platform, always update access & modify time.
    updateLastAccessTime = true;
    updateLastModifyTime = true;
#endif // LLBC_TARGET_PLATFORM_MAC || LLBC_TARGET_PLATFORM_IPHONE

    if (!updateLastAccessTime && !updateLastModifyTime)
        return LLBC_OK;

    timespec tsNowTime;
    bool gotNowTime = false;
    if (updateLastAccessTime)
    {
        if (lastAccessTime == nullptr)
        {
            const sint64 nowTime = LLBC_GetMicroseconds();
            tsNowTime.tv_sec = static_cast<time_t>(nowTime / 1000000);
            tsNowTime.tv_nsec = static_cast<long>(nowTime % 1000000) * 1000;

            gotNowTime = true;

            lastAccessTime = &tsNowTime;
        }
    }
    else if (lastAccessTime != nullptr)
    {
        lastAccessTime = nullptr;
    }

    if (updateLastModifyTime)
    {
        if (lastModifyTime == nullptr)
        {
            if (!gotNowTime)
            {
                const sint64 nowTime = LLBC_GetMicroseconds();
                tsNowTime.tv_sec = static_cast<time_t>(nowTime / 1000000);
                tsNowTime.tv_nsec = static_cast<long>(nowTime % 1000000) * 1000;
            }

            lastModifyTime = &tsNowTime;
        }
    }
    else if (lastModifyTime != nullptr)
    {
        lastModifyTime = nullptr;
    }

#if LLBC_TARGET_PLATFORM_WIN32
    HANDLE handle = ::CreateFileA(filePath.c_str(), 
                                  GENERIC_READ | FILE_WRITE_ATTRIBUTES,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  nullptr,
                                  OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL,
                                  nullptr);
    if (handle == INVALID_HANDLE_VALUE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    FILETIME fileTimes[2];
    if (lastAccessTime)
        LLBC_WinTimeSpec2FileTime(*lastAccessTime, fileTimes[0]);
    if (lastModifyTime)
        LLBC_WinTimeSpec2FileTime(*lastModifyTime, fileTimes[1]);

    if (!::SetFileTime(handle,
                       nullptr,
                       lastAccessTime ? &fileTimes[0] : nullptr,
                       lastModifyTime ? &fileTimes[1] : nullptr))
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        ::CloseHandle(handle);

        return LLBC_FAILED;
    }

    ::CloseHandle(handle);
    return LLBC_OK;
#else // Non-WIN32
#if LLBC_TARGET_PLATFORM_MAC || LLBC_TARGET_PLATFORM_IPHONE
    const int openFlags = O_RDONLY;
#else // Linux & Android
    const int openFlags = O_RDONLY | O_NOATIME;
#endif
    int fd = open(filePath.c_str(), openFlags);
    if (fd == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_MAC || LLBC_TARGET_PLATFORM_IPHONE
    timeval times[2];
    times[0].tv_sec = lastAccessTime->tv_sec;
    times[0].tv_usec = static_cast<__darwin_suseconds_t>(lastAccessTime->tv_nsec / 1000);
    times[1].tv_sec = lastModifyTime->tv_sec;
    times[1].tv_usec = static_cast<__darwin_suseconds_t>(lastModifyTime->tv_nsec / 1000);
    if (futimes(fd, times) == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        close(fd);

        return LLBC_FAILED;
    }
#else // Linux & Android
    timespec times[2];
    if (lastAccessTime)
        memcpy(&times[0], lastAccessTime, sizeof(timespec));
    else
        times[0].tv_nsec = UTIME_OMIT;

    if (lastModifyTime)
        memcpy(&times[1], lastModifyTime, sizeof(timespec));
    else
        times[1].tv_nsec = UTIME_OMIT;

    if (futimens(fd, times) == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        close(fd);

        return LLBC_FAILED;
    }
#endif // LLBC_TARGET_PLATFORM_MAC || LLBC_TARGET_PLATFORM_IPHONE

    close(fd);
    return LLBC_OK;
#endif // Win32
}

int LLBC_File::CopyFile(const LLBC_String &destFilePath, bool overlapped)
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_FAILED;
    }

    if (Flush() != LLBC_OK)
        return LLBC_FAILED;

    return CopyFile(_path, destFilePath, overlapped);
}

int LLBC_File::CopyFile(const LLBC_String &srcFilePath, const LLBC_String &destFilePath, bool overlapped)
{
#if LLBC_TARGET_PLATFORM_WIN32
    if (!::CopyFileA(srcFilePath.c_str(), destFilePath.c_str(), !overlapped))
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // Non_WIN32
    if (!Exists(srcFilePath))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    // Open source file with BinaryRead mode.
    LLBC_File srcFile(srcFilePath, LLBC_FileMode::BinaryRead);
    if (!srcFile.IsOpened())
        return LLBC_FAILED;

    // Check dest file exist or not.
    if (!overlapped && Exists(destFilePath))
    {
        LLBC_SetLastError(LLBC_ERROR_EXIST);
        return LLBC_FAILED;
    }

    // Open dest file with BinaryWrite mode.
    LLBC_File destFile(destFilePath, LLBC_FileMode::BinaryWrite);
    if (!destFile.IsOpened())
        return LLBC_FAILED;

    sint64 srcFileSize = srcFile.GetFileSize();
    if (srcFileSize == 0)
        return LLBC_OK;

    sint64 copyBufSize = MIN(LLBC_CFG_CORE_FILE_COPY_BUF_SIZE, srcFileSize);
    sint8 *copyBuf = LLBC_Malloc(sint8, copyBufSize);

    sint64 copiedSize = 0;
    while (copiedSize != srcFileSize)
    {
        sint64 copiableSize = MIN(srcFileSize - copiedSize, copyBufSize);
        sint64 actuallyRead = srcFile.Read(copyBuf, copiableSize);
        if (UNLIKELY(actuallyRead == -1)) // encountered error, return failed.
        {
            free(copyBuf);
            return LLBC_FAILED;
        }

        if (LIKELY(actuallyRead != 0)) // actually read some data from source file, copy to dest file.
        {
            sint64 actuallyCopy = destFile.Write(copyBuf, actuallyRead);
            if (UNLIKELY(actuallyCopy == -1)) // encountered error, return failed.
            {
                free(copyBuf);
                return LLBC_FAILED;
            }
            else if (UNLIKELY(actuallyCopy != actuallyRead)) // could not write all data to dest file, return failed.
            {
                free(copyBuf);
                return LLBC_FAILED;
            }
        }

        if (UNLIKELY(actuallyRead != copiableSize))
        {
            free(copyBuf);
            LLBC_SetLastError(LLBC_ERROR_TRUNCATED);

            return LLBC_FAILED;
        }

        copiedSize += actuallyRead;
    }

    free(copyBuf);
    return LLBC_OK;
#endif
}

int LLBC_File::MoveFile(const LLBC_String &toFilePath, bool overlapped)
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_FAILED;
    }

    const LLBC_String filePath(_path);

    Close();
    return MoveFile(filePath, toFilePath, overlapped);
}

int LLBC_File::MoveFile(const LLBC_String &fromFilePath, const LLBC_String &toFilePath, bool overlapped)
{
#if LLBC_TARGET_PLATFORM_WIN32
    DWORD moveFlags = MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH;
    if (overlapped)
        moveFlags |= MOVEFILE_REPLACE_EXISTING;
    if (::MoveFileExA(fromFilePath.c_str(), toFilePath.c_str(), moveFlags) == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // Non-WIN32
    if (!overlapped && Exists(toFilePath))
    {
        LLBC_SetLastError(LLBC_ERROR_EXIST);
        return LLBC_FAILED;
    }

    if (rename(fromFilePath.c_str(), toFilePath.c_str()) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif
}

int LLBC_File::DeleteFile()
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_FAILED;
    }

    Close();
    return DeleteFile(_path);
}

int LLBC_File::DeleteFile(const LLBC_String &filePath)
{
#if LLBC_TARGET_PLATFORM_WIN32
    if (!::DeleteFileA(filePath.c_str()))
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#else // Non-WIN32
    if (remove(filePath.c_str()) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
#endif
}

const char *LLBC_File::ParseFileMode(int mode)
{
    bool openAsText = (mode & LLBC_FileMode::Text) == LLBC_FileMode::Text;
    int nonFormatMode = mode & ~LLBC_FileMode::Text;
    nonFormatMode = nonFormatMode & ~LLBC_FileMode::Binary;
    switch (nonFormatMode)
    {
    case LLBC_FileMode::Read:
        return openAsText ? "r" : "rb";
    case LLBC_FileMode::Write:
        return openAsText ? "w" : "wb";
    case LLBC_FileMode::ReadWrite:
        return openAsText ? "w+" : "wb+";
    case LLBC_FileMode::AppendWrite:
        return openAsText ? "a" : "ab";
    case LLBC_FileMode::AppendReadWrite:
        return openAsText ? "a+" : "ab+";
    default:
        return nullptr;
    }
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif
