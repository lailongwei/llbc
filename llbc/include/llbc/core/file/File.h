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

#pragma once

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The file mode enumerations
 */
class LLBC_EXPORT LLBC_FileMode
{
public:
    enum
    {
        Read = 0x01,                            // Read file, the file must exist.
        Write = 0x02,                           // Write file, if file not exist, will create, if exist, will truncate file.
        ReadWrite = Read | Write,               // Read and write file, if file not exist, will create, if exist, will truncate file.
        AppendWrite = 0x04,                     // Append write file, if file not exist, will create file, otherwise will truncate file.
        AppendReadWrite = AppendWrite | Read,   // Append read and write file, if file not exist, will create file, otherwise will truncate file.

        Text = 0x08,                            // Open file as text format.
        Binary = 0x00,                          // Open file as binary format.

        TextRead = Read | Text,                 // Read file as text format.
        BinaryRead = Read | Binary,             // Read file as binary format.
        TextWrite = Write | Text,               // Write file as text format.
        BinaryWrite = Write | Binary,           // Write file as binary format.
        TextReadWrite = ReadWrite | Text,       // Read and write file as text format.
        BinaryReadWrite = ReadWrite | Binary,   // Read and write file as binary format.
        TextAppendWrite = AppendWrite | Text,   // Append write file as text format.
        BinaryAppendWrite = AppendWrite |Binary,// Append write file as binary format.
        TextAppendReadWrite = AppendReadWrite | Text, // Append read and write file as text format.
        BinaryAppendReadWrite = AppendReadWrite | Binary, // Append read and write file as binary format.

        LastestMode = 0x70000000,               // Lastest open mode, use ReOpen method.
    };

public:
    /**
     * Get the file mode string representation.
     * @param[in] fileMode - the file mode.
     * @return LLBC_String - The file mode string representation.
     */
    static LLBC_String GetFileModeDesc(int fileMode);
};

/**
 * \brief The file buffer mode enumeration.
 */
class LLBC_EXPORT LLBC_FileBufferMode
{
public:
    enum
    {
        NoBuf = _IONBF,   // No buffer is used, all file read/write operation
                          // will direct synchronous to store device.
        LineBuf = _IOLBF, // For some systems, this provides line buffering. However,
                          // for Win32, the behavior is the same as NoBuf - Full Buffering.
        FullBuf = _IOFBF, // Full buffering; that is, buffer is used as the buffer and
                          // size is used as the size of the buffer. If buffer is nullptr,
                          // an automatically allocated buffer size bytes long is used.
    };
};

/**
 * \brief The file seek origin enumeration.
 */
class LLBC_FileSeekOrigin
{
public:
    enum
    {
        Begin = SEEK_SET,
        Current = SEEK_CUR,
        End = SEEK_END
    };
};

/**
 * \brief The file new line format type enumeration.
 */
class LLBC_FileNewLineFormat
{
public:
    enum
    {
        LineFeed,                 // LF, character is: \n, Linux/Unix OS used.
        CarriageReturn,           // CR, character is: \r, Mac OS used.
        CarriageReturn_LineFeed,  // CRLF, characters are: \r\n, Windows OS used.

        UnixStyle = LineFeed,     // Alias for LineFeed enumeration.
        MacStyle = CarriageReturn,// Alias for CarriageReturn.
        WindowsStyle = CarriageReturn_LineFeed, // Alias for CarriageReturn-LineFeed.

        AutoMatch, // Auto match, it means if you use llbc library in Windows, will use CRLF, and so on...
    };
};

/**
 * \brief The file attributes structure encapsulation.
 */
LLBC_BEGIN_C_DECL
struct LLBC_FileAttributes
{
    bool readable; /* file readable or not */
    bool writable; /* file writable or not */
    bool execable; /* file execable or not */
    bool hidden; /* file is hidden or not  */

    bool isDirectory; /* file is directory or not */
    
#if LLBC_TARGET_PLATFORM_WIN32
    timespec createTime; /* file create time(WIN32 specific) */
#else
    timespec lastChangeStatusTime; /* file last change status time(Non-WIN32 specific) */
#endif

    timespec lastModifyTime; /* file last modify time */
    timespec lastAccessTime; /* file last access time */

    sint64 fileSize; /* file size, in bytes */
};
LLBC_END_C_DECL

/**
 * \brief The File class encapsulation.
 *        This file class provides static methods for the creation, copying, deleting, moving, 
 *        and opening of single file, and access FileSystem object.
 */
class LLBC_EXPORT LLBC_File
{
public:
    /**
     * Default constructor.
     */
    LLBC_File();
    /**
     * Construct file object with path and file mode.
     * Note: If file open success when constructor called, LLBC_GetLastError() return 0, 
     *       otherwise return error code, or you can call IsOpened() method to check file open success or not.
     * @param[in] path - the file path.
     * @param[in] mode - the file access mode.
     */;
    explicit LLBC_File(const LLBC_String &path, int mode = LLBC_FileMode::BinaryRead);

    /**
     * Destructor.
     */
    virtual ~LLBC_File();

public:
    /**
     * Open file.
     * @param[in] path - the file path.
     * @param[in] mode - the file access mode.
     * @return int - return 0 if open operation success, otherwise return -1.
     */
    int Open(const LLBC_String &path, int mode = LLBC_FileMode::BinaryRead);

    /**
     * ReOpen file.
     * @param[in] mode - the reopen mode, default reopen mode is LLBC_FileMode::LastestMode.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ReOpen(int mode = LLBC_FileMode::LastestMode);

    /**
     * Check whether the file object opened file.
     * @return bool - return true if opened file, otherwise return false.
     */
    bool IsOpened() const;

    /**
     * Close file.
     */
    void Close();

    /**
     * Get file path.
     */
    const LLBC_String &GetFilePath() const;

    /**
     * Get file no.
     * @param[in] handle - the file handle(FILE *).
     * @return int - the file no, if failed, return -1.
     */
    int GetFileNo() const;
    static int GetFileNo(LLBC_FileHandle handle);

    /**
     * Get file object wrapped file system level file handle, unsafe.
     * @return LLBC_FileHandle - the file system level file  handle.
     */
    LLBC_FileHandle GetFileHandle() const;

    /**
     * Get the file open mode.
     * @return int - the file open mode.
     */
    int GetFileMode() const;

    /**
     * Set file buffer mode.
     * @param[in] bufferMode - the file buffer mode, see LLBC_FileBufferMode class.
     * @param[in] size       - Buffer size in bytes. Allowable range: 2 <= size <= INT_MAX (2147483647). Internally, the value supplied for size is rounded down to the nearest multiple of 2.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetBufferMode(int bufferMode, size_t size);

public:
    /**
     * Get file size.
     * @return long - the file size, if failed, return -1.
     */
    long GetFileSize() const;

    /**
     * Move the file pointer to a specified location.
     * @param[in] seekOrigin - initial position.
     * @param[in] offset     - number of bytes from origin.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Seek(int seekOrigin, long offset);

    /**
     * Get file position.
     * @return long - the file position, if failed, return -1.
     */
    long GetFilePosition() const;

    /**
     * Set file position.
     * @param[in] position - the new file position.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetFilePosition(long position);

    /**
     * Offset file position.
     * @param[in] offset - the file offset.
     * @return long - the new file position.
     */
    long OffsetFilePosition(long offset);

    /**
     * Get file readable size.
     * @return long - the file readable size.
     */
    long GetReadableSize() const;

public:
    /**
     * RAW data read helper methods.
     */
    int Read(bool &boolVal);
    int Read(sint8 &sint8Val);
    int Read(uint8 &uint8Val);
    int Read(sint16 &sint16Val);
    int Read(uint16 &uint16Val);
    int Read(sint32 &sint32Val);
    int Read(uint32 &uint32Val);
    int Read(long &longVal);
    int Read(LLBC_NS ulong &ulongVal);
    int Read(sint64 &sint64Val);
    int Read(uint64 &uint64Val);
    int Read(float &floatVal);
    int Read(double &doubleVal);
    int Read(ldouble &ldoubleVal);
    
    /**
     * Read oneline data to LLBC_String object.
     * @return LLBC_String - the line data, if failed, LLBC_GetLastError() return value is non-zero.
     */
    LLBC_String ReadLine();

    /**
     * Read data to end.
     * @param[in] filePath - the file path.
     * @return LLBC_String - the all non-read data, if success, LLBC_GetLastError() return LLBC_ERROR_SUCCESS, 
     *                       otherwise return error a special error code.
     */
    LLBC_String ReadToEnd();
    static LLBC_String ReadToEnd(const LLBC_String &filePath);

    /**
     * File bytes read method.
     * @param[in] buf  - storage location for data.
     * @param[in] size - buffer size in bytes.
     * @return long - actually read size, if -1, read failed, else if 0 < actuallyRead < size, 
     *                it means truncated(LLBC_GetLastError() will return LLBC_ERROR_TRUNCATED),
     *                otherwise success.
     */
    long Read(void *buf, size_t size);

public:
    /**
     * Raw data write helper methods.
     * Note: All write operation maybe truncated.
     * @param[in] val - the will write simple object.
     * @return int - if success return 0, otherwise return -1.
     */
    int Write(const bool &boolVal);
    int Write(const sint8 &sint8Val);
    int Write(const uint8 &uint8Val);
    int Write(const sint16 &sint16Val);
    int Write(const uint16 &uint16Val);
    int Write(const sint32 &sint32Val);
    int Write(const uint32 &uint32Val);
    int Write(const sint64 &sint64Val);
    int Write(const uint64 &uint64Val);
    int Write(const long &longVal);
    int Write(const LLBC_NS ulong &ulongVal);
    int Write(const float &floatVal);
    int Write(const double &doubleVal);
    int Write(const ldouble &ldoubleVal);
    int Write(const char *cstr);
    int Write(const LLBC_String &str);
    int Write(const LLBC_WString &wstr);
    int Write(const std::string &str);
    int Write(const std::wstring &wstr);

    /**
     * Write line data.
     * @param[in] line          - the line content.
     * @param[in] newLineFormat - the new line format, default is AutoMatch. 
     * @return int - actually written size, in bytes, if write failed, return -1,
     *               else if 0 < actuallyWrote < size, it means truncated, otherwise success.
     */
    long WriteLine(const LLBC_String &line, int newLineFormat = LLBC_FileNewLineFormat::AutoMatch);

    /**
     * File bytes write method.
     * @param[in] buf  - pointer to data to be written.
     * @param[in] size - buffer size in bytes.
     * @return int - actually written size, in bytes, if write failed, return -1,
     *               else if 0 < actuallyWrote < size, it means truncated, otherwise success.
     */
    long Write(const void *buf, size_t size);

    /**
     * Flush file.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Flush();

public:
    /**
     * Determine whether the specified file exists.
     * @param[in] path - the file path.
     * @return bool - return true if the file exist, otherwise return false.
     */
    static bool Exists(const LLBC_String &path);

    /**
     * Get file attributes.
     * @param[in] path   - the file path.
     * @param[out] attrs - the file attributes.
     * @return int - return 0 if success, otherwise return -1.
     */
    int GetFileAttributes(LLBC_FileAttributes &attrs);
    static int GetFileAttributes(const LLBC_String &path, LLBC_FileAttributes &attrs);

    /**
     * Touch file.
     * Note:
     *      In iOS & Mac platform, updateLastAccessTime and updateLastModifyTime will always set to true.
     *      In this two platforms, if lastAccessTime and lastModifyTime not set, will update to now.
     * @param[in] filePath - the file path.
     * @param[in] updateLastAccessTime - need update last access time?
     * @param[in] lastAccessTime       - the last access time, if nullptr, will update to now.
     * @param[in] updateLastModifyTime - need update last modify time?
     * @param[in] lastModifyTime       - the last modify time, if nullptr, will update to now.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int TouchFile(const LLBC_String &filePath,
                         bool updateLastAccessTime = true,
                         const timespec *lastAccessTime = nullptr,
                         bool updateLastModifyTime = true,
                         const timespec *lastModifyTime = nullptr);

public:
    /**
     * Copy file.
     * @param[in] srcFilePath  - the source file path.
     * @param[in] destFilePath - the destination file path.
     * @param[in] overlapped   - if the new file already exists, the function overrides
     *                           the existing file and succeeds, otherwise failed.
     *                           default is true.
     * @return int - return 0 if success, otherwise return -1.
     */
    int CopyFile(const LLBC_String &destFilePath, bool overlapped = false);
    static int CopyFile(const LLBC_String &srcFilePath, const LLBC_String &destFilePath, bool overlapped = false);

    /**
     * Move file.
     * @param[in] fromFilePath - the from file path.
     * @param[in] toFilePath   - the to file path.
     * @param[in] overlapped   - if the new file already exists, the function overrides the existing fail and succeeds, otherwise failed.
     *                           default is true.
     * @return int - return 0 if success, otherwise return -1.
     */
    int MoveFile(const LLBC_String &toFilePath, bool overlapped = false);
    static int MoveFile(const LLBC_String &fromFilePath, const LLBC_String &toFilePath, bool overlapped = false);

    /**
     * Delete file.
     * @param[in] filePath - the will delete file path.
     * @return int - return 0 if success, otherwise return -1.
     */
    int DeleteFile();
    static int DeleteFile(const LLBC_String &filePath);

private:
    /**
     * Parse file mode to system file access mode.
     * @return const char * - the system file string access mode.
     */
    static const char *ParseFileMode(int mode);

    template <typename T>
    int ReadRawObj(T &obj);

    template <typename T>
    int WriteRawObj(const T&obj);

private:
    LLBC_DISABLE_ASSIGNMENT(LLBC_File);

private:
    int _mode;
    LLBC_String _path;
    LLBC_FileHandle _handle;
};

__LLBC_NS_END

#include "llbc/core/file/FileInl.h"


