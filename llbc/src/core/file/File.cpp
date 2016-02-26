/**
 * @file    File.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/04
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Time.h"

#include "llbc/core/file/File.h"
#include "llbc/core/file/Directory.h"

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
, _path()

, _handle(LLBC_INVALID_FILE_HANDLE)
{
}

LLBC_File::LLBC_File(const LLBC_String &path, int mode)
: _mode(LLBC_FileMode::Read)
, _path()

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
    if (modeRepr == NULL)
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
    _path.append(path.c_str(), path.length());

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

    char *buffer = NULL;
    if (bufferMode == LLBC_FileBufferMode::NoBuf)
    {
        size = 0;
    }
    else if (size < 2 || size > INT_MAX)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if (size > 0)
        buffer = LLBC_Malloc(char, size);
    if (setvbuf(_handle, buffer, bufferMode, size) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        LLBC_Free(buffer);

        return LLBC_FAILED;
    }

    return LLBC_OK;
}

long LLBC_File::GetFileSize() const
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return -1;
    }

    LLBC_FileHandle handle = 
        const_cast<LLBC_FileHandle>(_handle);
    long oldPos = ftell(handle);
    if (fseek(handle, 0, LLBC_FileSeekOrigin::End) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return -1;
    }

    int ret = LLBC_OK;
    long fileSize = ftell(handle);
    if (fileSize == -1L)
    {
        ret = LLBC_FAILED;
        LLBC_SetLastError(LLBC_ERROR_CLIB);
    }

    if (fseek(handle, oldPos, LLBC_FileSeekOrigin::Begin) != 0)
    {
        if (ret == LLBC_OK)
        {
            ret = LLBC_FAILED;
            LLBC_SetLastError(LLBC_ERROR_CLIB);
        }
    }

    return fileSize;
}

int LLBC_File::Seek(int seekOrigin, long offset)
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_FAILED;
    }

    if (fseek(_handle, offset, seekOrigin) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

long LLBC_File::GetFilePosition() const
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return -1;
    }

    long pos = ftell(
        const_cast<LLBC_FileHandle>(_handle));
    if (pos == -1)
        LLBC_SetLastError(LLBC_ERROR_CLIB);

    return pos;
}

int LLBC_File::SetFilePosition(long position)
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return -1;
    }

    if (fseek(_handle, position, LLBC_FileSeekOrigin::Begin) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

long LLBC_File::OffsetFilePosition(long offset)
{
    long curPos = GetFilePosition();
    if (curPos == -1)
        return LLBC_FAILED;

    return SetFilePosition(curPos + offset);
}

long LLBC_File::GetReadableSize() const
{
    long size = GetFileSize();
    if (size == -1)
        return -1;

    long pos = GetFilePosition();
    if (pos == -1)
        return -1;

    return size - pos;
}

LLBC_String LLBC_File::ReadToEnd()
{
    LLBC_String str;
    const long readableSize = GetReadableSize();
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
    long actuallyRead = Read(
        const_cast<char *>(str.data()), static_cast<size_t>(readableSize));
    if (actuallyRead == -1)
    {
        str.resize(0);
        return str;
    }
    else if (actuallyRead < readableSize)
    {
        str.resize(static_cast<size_t>(actuallyRead));
    }

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

long LLBC_File::Read(void *buf, size_t size)
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return -1;
    }

    size_t actuallyRead = fread(buf, 1, size, _handle);
    if (actuallyRead != size && ferror(_handle) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return -1;
    }

    return static_cast<long>(actuallyRead);
}

long LLBC_File::Write(const void *buf, size_t size)
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return -1;
    }

    size_t actuallyWrote = fwrite(buf, 1, size, _handle);
    if (actuallyWrote != size && ferror(_handle) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return -1;
    }

    return static_cast<long>(actuallyWrote);
}

int LLBC_File::Flush()
{
    if (!IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return -1;
    }

    if (fflush(_handle) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

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

    memcpy(&attrs.lastAccessTime, &cStat.st_atim, sizeof(timespec));
    memcpy(&attrs.lastModifyTime, &cStat.st_mtim, sizeof(timespec));
    memcpy(&attrs.lastChangeStatusTime, &cStat.st_ctim, sizeof(timespec));

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

        if ((!updateLastAccessTime || lastAccessTime == NULL) &&
            (!updateLastModifyTime || lastModifyTime == NULL))
            return LLBC_OK;
    }

    if (!updateLastAccessTime && !updateLastModifyTime)
        return LLBC_OK;

    timespec tsNowTime;
    bool gotNowTime = false;
    if (updateLastAccessTime)
    {
        if (lastAccessTime == NULL)
        {
            const sint64 nowTime = LLBC_GetMicroSeconds();
            tsNowTime.tv_sec = static_cast<time_t>(nowTime / 1000000);
            tsNowTime.tv_nsec = static_cast<long>(nowTime % 1000000) * 1000;

            gotNowTime = true;

            lastAccessTime = &tsNowTime;
        }
    }
    else if (lastAccessTime != NULL)
    {
        lastAccessTime = NULL;
    }

    if (updateLastModifyTime)
    {
        if (lastModifyTime == NULL)
        {
            if (!gotNowTime)
            {
                const sint64 nowTime = LLBC_GetMicroSeconds();
                tsNowTime.tv_sec = static_cast<time_t>(nowTime / 1000000);
                tsNowTime.tv_nsec = static_cast<long>(nowTime % 1000000) * 1000;
            }

            lastModifyTime = &tsNowTime;
        }
    }
    else if (lastModifyTime != NULL)
    {
        lastModifyTime = NULL;
    }

#if LLBC_TARGET_PLATFORM_WIN32
    HANDLE handle = ::CreateFileA(filePath.c_str(), 
                                  GENERIC_READ | FILE_WRITE_ATTRIBUTES,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  NULL,
                                  OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
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
                       NULL,
                       lastAccessTime ? &fileTimes[0] : NULL,
                       lastModifyTime ? &fileTimes[1] : NULL))
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        ::CloseHandle(handle);

        return LLBC_FAILED;
    }

    ::CloseHandle(handle);
    return LLBC_OK;
#else // Non-WIN32
    int fd = open(filePath.c_str(), O_NOATIME, O_RDONLY);
    if (fd == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

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

    // Open source file with BinayRead mode.
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

    long srcFileSize = srcFile.GetFileSize();
    if (srcFileSize == 0)
        return LLBC_OK;

    long copyBufSize = MIN(LLBC_CFG_CORE_FILE_COPY_BUF_SIZE, srcFileSize);
    sint8 *copyBuf = LLBC_Malloc(sint8, copyBufSize);

    long copiedSize = 0;
    while (copiedSize != srcFileSize)
    {
        long copiableSize = MIN(srcFileSize - copiedSize, copyBufSize);
        long actuallyCopy = srcFile.Read(copyBuf, copiableSize);
        if (UNLIKELY(actuallyCopy == -1))
        {
            LLBC_Free(copyBuf);
            return LLBC_FAILED;
        }
        else if (UNLIKELY(actuallyCopy != copiableSize))
        {
            LLBC_Free(copyBuf);
            LLBC_SetLastError(LLBC_ERROR_TRUNCATED);

            return LLBC_FAILED;
        }

        copiedSize += actuallyCopy;
    }

    LLBC_Free(copyBuf);
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
    int nonFormatMode = mode &= (~LLBC_FileMode::Text | ~LLBC_FileMode::Binary);
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
        return NULL;
    }
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif

#include "llbc/common/AfterIncl.h"
