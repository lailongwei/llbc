/**
 * @file    File.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/06
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_File.h"
#include "llbc/core/os/OS_Directory.h"
#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/file/File.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4290)
#pragma warning(disable:4996)
#endif

namespace
{
    typedef LLBC_NS LLBC_File This;
}

__LLBC_NS_BEGIN

static int __llbclibSeekFlagToCLibSeekFlag(int flag)
{
    switch (flag)
    {
    case LLBC_FilePos::Begin:
        return SEEK_SET;

    case LLBC_FilePos::Cur:
        return SEEK_CUR;

    case LLBC_FilePos::End:
        return SEEK_END;

    default:
        break;
    }

    return SEEK_SET;
}

LLBC_File::LLBC_File()
{
    _handle = NULL;
}

LLBC_File::LLBC_File(const LLBC_String &path,
                     const LLBC_String &mode)
                     throw (LLBC_IOException)
{
    if (this->Open(path, mode) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

LLBC_File::~LLBC_File()
{
    this->Close();
}

int LLBC_File::Open(const LLBC_String &path,
                    const LLBC_String &mode)
{
    _handle = NULL;
    if (path.empty() || mode.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    if (this->IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_OPENED);
        return LLBC_RTN_FAILED;
    }

    if ((_path = This::ToFullPath(path)).empty())
        return LLBC_RTN_FAILED;

    _handle = fopen(_path.c_str(), mode.c_str());
    if (_handle == NULL)
    {
        _path.clear();
        LLBC_SetLastError(LLBC_ERROR_CLIB);

        return LLBC_RTN_FAILED;
    }

    _mode = mode;

    return LLBC_RTN_OK;
}

LLBC_FileHandle LLBC_File::Handle() const
{
    return _handle;
}

bool LLBC_File::IsOpened() const
{
    return (_handle ? true : false);
}

LLBC_File::operator bool() const
{
    return this->IsOpened();
}

bool LLBC_File::operator !() const
{
    return !this->IsOpened();
}

int LLBC_File::Close()
{
    if (!this->IsOpened())
        return LLBC_RTN_OK;

    fclose(_handle);
    _handle = NULL;

    _path.clear();
    _mode.clear();

    return LLBC_RTN_OK;
}

int LLBC_File::SetBufferMode(int mode, size_t size)
{
    if (mode != LLBC_FileBufferMode::FullBuf &&
       mode != LLBC_FileBufferMode::LineBuf &&
       mode != LLBC_FileBufferMode::NoBuf)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    if (mode == LLBC_FileBufferMode::NoBuf)
    {
        size = 0;
    }
    else if (size < 2 || size > LONG_MAX)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    if (!this->IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_RTN_FAILED;
    }

    if (setvbuf(_handle, NULL, mode, size) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

size_t LLBC_File::Tell() const
{
    if (!this->IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return This::npos;
    }

    LLBC_File *nonConstThis = const_cast<LLBC_File *>(this);
    long pos = ftell(nonConstThis->_handle);
    if (pos == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return This::npos;
    }

    return pos;
}

size_t LLBC_File::GetSize() const
{
    if (!this->IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return This::npos;
    }

    size_t curPos = this->Tell();
    if (UNLIKELY(curPos == This::npos))
        return This::npos;

    LLBC_File *ncThis = const_cast<LLBC_File *>(this);
    size_t endPos = ncThis->Seek(LLBC_FilePos::End, 0);
    if (endPos == This::npos)
        return This::npos;

    if (ncThis->Seek(LLBC_FilePos::Begin, 
                static_cast<long>(curPos)) == This::npos)
        return This::npos;

    return endPos;
}

size_t LLBC_File::Seek(int origin, long offset)
{
    if (!(origin >= LLBC_FilePos::Begin && 
        origin < LLBC_FilePos::Unknown))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return This::npos;
    }

    origin = __llbclibSeekFlagToCLibSeekFlag(origin);
    if (fseek(_handle, offset, origin) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return This::npos;
    }

    return this->Tell();
}

size_t LLBC_File::Read(void *buf, size_t size)
{
    if (!buf || size == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return This::npos;
    }
    else if (!this->IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return This::npos;
    }

    size_t actuallyRead = fread(buf, 1, size, _handle);
    if (actuallyRead != size && ferror(_handle) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return This::npos;
    }

    return actuallyRead;
}

int LLBC_File::ReadString(LLBC_String &str)
{
    char ch = '\0';
    int ret = LLBC_RTN_OK;
    while ((ret = this->ReadObj(ch)) == LLBC_RTN_OK)
    {
        if (ch == '\0')
            break;

        str += ch;
    }

    if (ret != LLBC_RTN_OK && LLBC_GetLastError() != LLBC_ERROR_END)
        return LLBC_RTN_FAILED;

    return LLBC_RTN_OK;
}

int LLBC_File::ReadStream(LLBC_Stream &stream)
{
    uint32 size = 0;
    if (this->ReadObj(size) != LLBC_RTN_OK)
        return LLBC_RTN_FAILED;

    if (UNLIKELY(size == 0))
        return LLBC_RTN_OK;

    stream.Resize(stream.GetPos() + size);
    size_t actuallyRead = this->Read(reinterpret_cast<char *>(
        const_cast<void *>(stream.GetBuf())) + stream.GetPos(), size);
    if (actuallyRead != static_cast<size_t>(size))
    {
        if (actuallyRead != This::npos)
        {
            stream.SetPos(stream.GetPos() + actuallyRead);
            LLBC_SetLastError(LLBC_ERROR_TRUNCATED);
        }

        return LLBC_RTN_FAILED;
    }

    stream.SetPos(stream.GetPos() + size);
    return LLBC_RTN_OK;
}

int LLBC_File::ReadLine(LLBC_String &line)
{
    LLBC_String::size_type oldSize = line.size();

    char ch = '\0';
    size_t actuallyRead = 0;
    while ((actuallyRead = this->Read(&ch, sizeof(char))) == sizeof(char))
    {
        if (ch != LLBC_LF_A)
        {
            line += ch;
            continue;
        }

        if (line.size() != oldSize && line[line.length() - 1] == LLBC_CR_A)
            line.erase(line.length() - 1);

        return LLBC_RTN_OK;
    }

    if (actuallyRead == This::npos)
    {
        return LLBC_RTN_FAILED;
    }
    else if (actuallyRead < sizeof(char))
    {
        if (line.size() != oldSize && line[line.length() - 1] == LLBC_CR_A)
        {
            line.erase(line.length() - 1);
        }

        LLBC_SetLastError(LLBC_ERROR_TRUNCATED);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_FAILED;
}

LLBC_File &LLBC_File::operator >>(sint8 &value) throw (LLBC_IOException)
{
    if (this->ReadObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator >>(uint8 &value) throw (LLBC_IOException)
{
    if (this->ReadObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator >>(sint16 &value) throw (LLBC_IOException)
{
    if (this->ReadObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator >>(uint16 &value) throw (LLBC_IOException)
{
    if (this->ReadObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator >>(sint32 &value) throw (LLBC_IOException)
{
    if (this->ReadObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator >>(uint32 &value) throw (LLBC_IOException)
{
    if (this->ReadObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator >>(sint64 &value) throw (LLBC_IOException)
{
    if (this->ReadObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator >>(uint64 &value) throw (LLBC_IOException)
{
    if (this->ReadObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator >>(float &value) throw (LLBC_IOException)
{
    if (this->ReadObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator >>(double &value) throw (LLBC_IOException)
{
    if (this->ReadObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator >>(LLBC_String &value) throw (LLBC_IOException)
{
    if (this->ReadString(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator >>(LLBC_Stream &value) throw (LLBC_IOException)
{
    if (this->ReadStream(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

size_t LLBC_File::Write(const void *buf, size_t size)
{
    if (!buf || size == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }
    else if (!this->IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_RTN_FAILED;
    }

    size_t actuallyWritten = fwrite(buf, 1, size, _handle);
    if (actuallyWritten != size && ferror(_handle) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_RTN_FAILED;
    }

    return actuallyWritten;
}

int LLBC_File::WriteString(const LLBC_String &str, bool writeNull)
{
    if (!this->IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_RTN_FAILED;
    }

    size_t actuallyWritten = 0;
    if (!str.empty())
        actuallyWritten = this->Write(str.data(), str.size());

    if (actuallyWritten != str.size())
    {
        if (actuallyWritten != This::npos)
            LLBC_SetLastError(LLBC_ERROR_TRUNCATED);

        return LLBC_RTN_FAILED;
    }

    if (!writeNull)
        return LLBC_RTN_OK;

    actuallyWritten = this->Write("\0", sizeof(sint8));
    if (actuallyWritten != sizeof(sint8))
    {
        if (actuallyWritten != This::npos)
            LLBC_SetLastError(LLBC_ERROR_TRUNCATED);

        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

int LLBC_File::WriteStream(const LLBC_Stream &stream)
{
    size_t pos = stream.GetPos();
    uint32 size = static_cast<uint32>(pos);
    size_t actuallyWritten = this->Write(&size, sizeof(uint32));
    if (actuallyWritten != sizeof(uint32))
    {
        if (actuallyWritten != This::npos)
            LLBC_SetLastError(LLBC_ERROR_TRUNCATED);

        return LLBC_RTN_FAILED;
    }

    if (size == 0)
        return LLBC_RTN_OK;

    if ((actuallyWritten = 
            this->Write(stream.GetBuf(), pos)) != pos)
    {
        if (actuallyWritten != This::npos)
            LLBC_SetLastError(LLBC_ERROR_TRUNCATED);

        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

int LLBC_File::WriteLine(const LLBC_String &line)
{
    if (!line.empty())
    {
        size_t ret = this->Write(line.data(), line.size());
        if (ret != line.size())
        {
            if (ret != This::npos)
                LLBC_SetLastError(LLBC_ERROR_TRUNCATED);

            return LLBC_RTN_FAILED;
        }
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    char lf = LLBC_LF_A;
    size_t ret = this->Write(&lf, sizeof(char));
    if (ret != sizeof(char))
    {
        if (ret != This::npos)
            LLBC_SetLastError(LLBC_ERROR_TRUNCATED);

        return LLBC_RTN_FAILED;
    }
#else // LLBC_TARGET_PLATFORM_WIN32
    const char *crlf = LLBC_CRLF_A;
    size_t ret = this->Write(crlf, 2 * sizeof(char));
    if (ret != 2 * sizeof(char))
    {
        if (ret != This::npos)
            LLBC_SetLastError(LLBC_ERROR_TRUNCATED);

        return LLBC_RTN_FAILED;
    }
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    return LLBC_RTN_OK;
}

LLBC_File &LLBC_File::operator <<(const sint8 &value) throw (LLBC_IOException)
{
    if (this->WriteObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator <<(const uint8 &value) throw (LLBC_IOException)
{
    if (this->WriteObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator <<(const sint16 &value) throw (LLBC_IOException)
{
    if (this->WriteObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator <<(const uint16 &value) throw (LLBC_IOException)
{
    if (this->WriteObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator <<(const sint32 &value) throw (LLBC_IOException)
{
    if (this->WriteObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator <<(const uint32 &value) throw (LLBC_IOException)
{
    if (this->WriteObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator <<(const sint64 &value) throw (LLBC_IOException)
{
    if (this->WriteObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator <<(const uint64 &value) throw (LLBC_IOException)
{
    if (this->WriteObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator <<(const float &value) throw (LLBC_IOException)
{
    if (this->WriteObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator <<(const double &value) throw (LLBC_IOException)
{
    if (this->WriteObj(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator <<(const LLBC_String &value) throw (LLBC_IOException)
{
    if (this->WriteString(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

LLBC_File &LLBC_File::operator <<(const LLBC_Stream &value) throw (LLBC_IOException)
{
    if (this->WriteStream(value) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());

    return *this;
}

int LLBC_File::Flush()
{
    if (!this->IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_RTN_FAILED;
    }

    if (fflush(_handle) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

int LLBC_File::DeleteSelf()
{
    if (!this->IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_RTN_FAILED;
    }

    LLBC_String path = _path;
    this->Close();

    return This::Delete(path);
}

int LLBC_File::Delete(const LLBC_String &path)
{
    if (path.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (::remove(path.c_str()) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_RTN_FAILED;
    }
#else
    if (::DeleteFileA(path.c_str()) != TRUE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_RTN_FAILED;
    }
#endif

    return LLBC_RTN_OK;
}

int LLBC_File::MoveSelf(const LLBC_String &path1, 
                        bool overlap,
                        bool reopen, 
                        const LLBC_String &mode1)
{
    if (path1.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    if (!this->IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_RTN_FAILED;
    }

    LLBC_String path0 = _path;
    LLBC_String mode0 = _mode;

    this->Close();

    if (This::Move(path0, path1, overlap) != LLBC_RTN_OK)
        return LLBC_RTN_FAILED;

    if (reopen)
        return this->Open(path1, mode1);

    return LLBC_RTN_OK;
}

int LLBC_File::Move(const LLBC_String &path0, 
                    const LLBC_String &path1,
                    bool overlap)
{
    if (path0.empty() || path1.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    if (!This::Exist(path0))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_RTN_FAILED;
    }

    if (This::Exist(path1))
    {
        if (!overlap)
        {
            LLBC_SetLastError(LLBC_ERROR_EXIST);
            return LLBC_RTN_FAILED;
        }

        if (This::Delete(path1) != LLBC_RTN_OK)
            return LLBC_RTN_FAILED;
    }

    if (rename(path0.c_str(), path1.c_str()) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

int LLBC_File::CopySelf(const LLBC_String &path1,
                        bool overlap,
                        void *copyBuf,
                        size_t copyBufLen)
{
    if (!this->IsOpened())
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);

    LLBC_String path0 = _path;
    LLBC_String mode = _mode;

    size_t curPos = this->Tell();
    if (curPos == This::npos)
        return LLBC_RTN_FAILED;

    this->Close();

    int ret = This::Copy(path0, path1, overlap, copyBuf, copyBufLen);
    if (this->Open(path0, mode) != LLBC_RTN_OK)
        return LLBC_RTN_FAILED;

    if (this->Seek(LLBC_FilePos::Begin, 
            static_cast<long>(curPos)) == This::npos)
        return LLBC_RTN_FAILED;

    return ret;
}

int LLBC_File::Copy(const LLBC_String &path0,
                    const LLBC_String &path1,
                    bool overlap,
                    void *copyBuf,
                    size_t copyBufLen)
{
    // argument check.
    if (path0.empty() || path1.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    if (copyBuf && copyBufLen == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    // check path0 exist.
    if (!This::Exist(path0))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_RTN_FAILED;
    }

    // check path1 exist.
    if (This::Exist(path1))
    {
        if (!overlap)
        {
            LLBC_SetLastError(LLBC_ERROR_EXIST);
            return LLBC_RTN_FAILED;
        }

        if (This::Delete(path1) != LLBC_RTN_OK)
            return LLBC_RTN_FAILED;
    }

    // open path0 with 'rb' mode, open path1 with 'wb' mode.
    LLBC_File src;
    if (src.Open(path0, "rb") != LLBC_RTN_OK)
        return LLBC_RTN_FAILED;

    LLBC_File dest;
    if (dest.Open(path1, "wb") != LLBC_RTN_OK)
        return LLBC_RTN_FAILED;

    // get src file size.
    const size_t fileSize = src.GetSize();
    if (fileSize == This::npos)
        return LLBC_RTN_FAILED;
    else if (fileSize == 0)
        return LLBC_RTN_OK;

    // allocate copy buffer, if caller not provide.
    // default buffer size is 4MB.
    const bool attachBuf = copyBuf ? true : false;
    copyBufLen = (copyBufLen == 0 ? (4 << 22) : copyBufLen);
    if (!copyBuf)
        copyBuf = LLBC_Malloc(void, copyBufLen);

    // starting copy.
    size_t alreadyCopied = 0;
    size_t actuallyRead = 0;
    size_t actuallyWritten = 0;
    do
    {
        actuallyRead = src.Read(copyBuf, copyBufLen);
        if (actuallyRead == This::npos)
        {
            if (!attachBuf)
                LLBC_Free(copyBuf);

            return LLBC_RTN_FAILED;
        }

        actuallyWritten = dest.Write(copyBuf, actuallyRead);
        if (actuallyWritten == This::npos)
        {
            if (!attachBuf)
                LLBC_Free(copyBuf);

            return LLBC_RTN_FAILED;
        }

        alreadyCopied += actuallyRead;
    } while (alreadyCopied < fileSize);

    ASSERT(alreadyCopied == fileSize);

    if (!attachBuf)
        LLBC_Free(copyBuf);
    
    return LLBC_RTN_OK;
}

LLBC_String LLBC_File::GetSelfDirectoryName() const
{
    if (!this->IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_String();
    }

    return LLBC_DirName(_path);
}

LLBC_String LLBC_File::GetDirectoryName(const LLBC_String &path)
{
    LLBC_String fullPath = This::ToFullPath(path);
    if (fullPath.empty())
        return LLBC_String();

    return LLBC_DirName(fullPath);
}

LLBC_String LLBC_File::GetSelfBaseName(bool incExtension) const
{
    if (!this->IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_String();
    }

    return LLBC_BaseName(_path, incExtension);
}

LLBC_String LLBC_File::GetBaseName(const LLBC_String &path, bool incExtension)
{
    LLBC_String fullPath = This::ToFullPath(path);
    if (fullPath.empty())
        return LLBC_String();

    return LLBC_BaseName(fullPath, incExtension);
}

LLBC_String LLBC_File::GetSelfExtension() const
{
    if (!this->IsOpened())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_String();
    }

    return LLBC_ExtensionName(_path);
}

LLBC_String LLBC_File::GetExtension(const LLBC_String &path)
{
    LLBC_String fullPath = This::ToFullPath(path);
    if (fullPath.empty())
        return LLBC_String();

    return LLBC_ExtensionName(fullPath);
}

bool LLBC_File::Exist(const LLBC_String &path)
{
    return LLBC_FileExist(path);
}

int LLBC_File::Touch(const LLBC_String &path)
{
    LLBC_File file;
    if (file.Open(path, "wb") != LLBC_RTN_OK)
        return LLBC_RTN_FAILED;

    return file.Close();
}

LLBC_String LLBC_File::ToFullPath(const LLBC_String &path)
{
    if (path.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_String();
    }

    LLBC_String fullPath;
#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (path[0] != LLBC_SLASH_A)
    {
        if ((fullPath = LLBC_GetCurrentDirectory()).empty())
            return fullPath;

        fullPath += LLBC_SLASH_A;
        fullPath += path;
    }
    else
        fullPath = path;
#else // LLBC_TARGET_PLATFORM_WIN32
    if (path.length() == 1 ||
        (path[1] != ':' && path[0] != LLBC_BACKLASH_A))
    {
        if ((fullPath = LLBC_GetCurrentDirectory()).empty())
            return fullPath;
        
        fullPath += LLBC_BACKLASH_A;
        fullPath += path;
    }
    else
        fullPath = path;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    return fullPath;
}

void LLBC_File::OpenT(const LLBC_String &path, 
                      const LLBC_String &mode) throw (LLBC_IOException)
{
    if (this->Open(path, mode) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_File::CloseT() throw (LLBC_IOException)
{
    if (this->Close() != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_File::SetBufferModeT(int mode, size_t size) throw (LLBC_IOException)
{
    if (this->SetBufferMode(mode, size) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

size_t LLBC_File::TellT() const throw (LLBC_IOException)
{
    size_t ret = this->Tell();
    if (ret == This::npos)
        throw LLBC_IOException(LLBC_FormatLastError());

    return ret;
}

size_t LLBC_File::GetSizeT() const throw (LLBC_IOException)
{
    size_t ret = this->GetSize();
    if (ret == This::npos)
        throw LLBC_IOException(LLBC_FormatLastError());

    return ret;
}

size_t LLBC_File::SeekT(int origin, long offset) throw (LLBC_IOException)
{
    size_t ret = this->Seek(origin, offset);
    if (ret == This::npos)
        throw LLBC_IOException(LLBC_FormatLastError());

    return ret;
}

size_t LLBC_File::ReadT(void *buf, size_t size) throw (LLBC_IOException)
{
    size_t ret = this->Read(buf, size);
    if (ret == This::npos)
        throw LLBC_IOException(LLBC_FormatLastError());

    return ret;
}

void LLBC_File::ReadStringT(LLBC_String &str) throw (LLBC_IOException)
{
    if (this->ReadString(str) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_File::ReadStreamT(LLBC_Stream &stream) throw (LLBC_IOException)
{
    if (this->ReadStream(stream) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_File::ReadLineT(LLBC_String &line) throw (LLBC_IOException)
{
    if (this->ReadLine(line) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

size_t LLBC_File::WriteT(const void *buf, size_t size) throw (LLBC_IOException)
{
    size_t ret = this->Write(buf, size);
    if (ret == This::npos)
        throw LLBC_IOException(LLBC_FormatLastError());

    return ret;
}

void LLBC_File::WriteStringT(const LLBC_String &str, bool writeNull) throw (LLBC_IOException)
{
    if (this->WriteString(str, writeNull) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_File::WriteStreamT(const LLBC_Stream &stream) throw (LLBC_IOException)
{
    if (this->WriteStream(stream) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_File::WriteLineT(const LLBC_String &line) throw (LLBC_IOException)
{
    if (this->WriteLine(line) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_File::FlushT() throw (LLBC_IOException)
{
    if (this->Flush() != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_File::DeleteSelfT() throw (LLBC_IOException)
{
    if (this->DeleteSelf() != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_File::DeleteT(const LLBC_String &path) throw (LLBC_IOException)
{
    if (This::Delete(path) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_File::MoveSelfT(const LLBC_String &path1,
                          bool overlap,
                          bool reopen,
                          const LLBC_String &mode1) throw (LLBC_IOException)
{
    if (this->MoveSelf(path1, overlap, reopen, mode1) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_File::MoveT(const LLBC_String &path0, 
                      const LLBC_String &path1, 
                      bool overlap) throw (LLBC_IOException)
{
    if (This::Move(path0, path1, overlap) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_File::CopySelfT(const LLBC_String &path1, 
                          bool overlap, 
                          void *copyBuf, 
                          size_t copyBufLen) throw (LLBC_IOException)
{
    if (this->CopySelf(path1, overlap, copyBuf, copyBufLen) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_File::CopyT(const LLBC_String &path0,
                      const LLBC_String &path1,
                      bool overlap,
                      void *copyBuf,
                      size_t copyBufLen) throw (LLBC_IOException)
{
    if (This::Copy(path0, path1, overlap, copyBuf, copyBufLen) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

void LLBC_File::TouchT(const LLBC_String &path)
{
    if (This::Touch(path) != LLBC_RTN_OK)
        throw LLBC_IOException(LLBC_FormatLastError());
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4290)
#pragma warning(default:4996)
#endif

#include "llbc/common/AfterIncl.h"
