/**
 * @file    File.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/06
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_FILE_FILE_H__
#define __LLBC_CORE_FILE_FILE_H__

#include "llbc/common/Common.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4290)
#endif

__LLBC_NS_BEGIN

/**
 * \brief The file position seek star flag enumeration.
 */
class LLBC_FilePos
{
public:
    enum
    {
        Begin   = 0,
        Cur     = 1,
        End     = 2,
        
        Unknown
    };
};

/**
 * \brief The file buffer mode enumeration.
 */
class LLBC_FileBufferMode
{
public:
    enum
    {
        FullBuf        = _IOFBF,
        LineBuf        = _IOLBF,
        NoBuf          = _IONBF
    };
};

/**
 * \brief file class encapsulation.
 */
class LLBC_EXPORT LLBC_File
{
public:
    static const size_t npos = -1;

public:
    /**
     * Default constructor.
     */
    LLBC_File();
    /**
     * Parameters constructor.
     * @param[in] path - file path.
     * @param[in] mode - type of access permitted, see fopen(), like: 'r', 'w', 'rb', 'wb', 'a', ...
     */
    explicit LLBC_File(const LLBC_String &path, 
                       const LLBC_String &mode = "rb")
                       throw (LLBC_IOException);

    ~LLBC_File();

public:
    /**
     * Open file.
     * @param[in] path - file path.
     * @param[in] mode - type of access permitted, see fopen(), like: 'r', 'w', 'rb', 'wb', 'a', ...
     * @return int - if success return 0, otherwise return -1.
     */
    int Open(const LLBC_String &path, 
             const LLBC_String &mode = "rb");

    /**
     * Get file handle, unsefe method.
     * @return LLBC_FileHandle - the file handle.
     */
    LLBC_FileHandle Handle() const;

    /**
     * Check file opened or not.
     * @return bool - return true if opened, otherwise return false.
     */
    bool IsOpened() const;

    /**
     * bool operation.
     */
    operator bool() const;

    /**
     * ! operation.
     */
    bool operator !() const;

    /**
     * Close file.
     * @return int - return success return 0, otherwise return -1.
     */
    int Close();

public:
    /**
     * Set file buffer mode, must call when file open and not undergone any I/O operations.
     * @param[in] mode - file buffer mode.
     * @param[in] size - buffer size, allowable range 2 <= size <= INT_MAX, size will adjust to size/2*2.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int SetBufferMode(int mode, size_t size);

public:
    /**
     * Get current position of the file.
     * @return size_t - file position, if return npos, it means error occurred.
     */
    size_t Tell() const;

    /**
     * Get file size.
     * @return size_t - file size, if return npos, it means error occurred.
     */
    size_t GetSize() const;

    /**
     * Moves the file pointers to a specified location.
     * @param origin - start move flag, see This class's enumeration.
     * @param offset - number of bytes from origin.
     * @return size_t - new position, if return npos, it means error occurred.
     */
    size_t Seek(int origin, long offset);

public:
    /**
     * Read data from the file.
     * @param[out] buf  - storage location for data.
     * @param[in]  size - require read size, in bytes.
     * @return size_t - successfully to read bytes size, if return npos, it means error occurred.
     */
    size_t Read(void *buf, size_t size);

    /**
     * Read string data from the file.
     * @param[out] str - storage location for string.
     * @return int - if success return 0, otherwise return -1.
     */
    int ReadString(LLBC_String &str);

    /**
     * Read stream data from the file.
     * @param[out] stream - storage location for stream.
     * @return int - if success return 0, otherwise return -1.
     */
    int ReadStream(LLBC_Stream &stream);

    /**
     * Read one line data from file.
     * @param[out] line - store location for line data.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ReadLine(LLBC_String &line);

    /**
     * Read data from the file to object.
     * @param[out] obj - object reference.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _Ty>
    int ReadObj(_Ty &obj)
    {
        size_t readRet = this->Read(&obj, sizeof(_Ty));
        if (readRet != sizeof(_Ty))
        {
            if (readRet != npos)
                LLBC_SetLastError(LLBC_ERROR_END);

            return LLBC_RTN_FAILED;
        }

        return LLBC_RTN_OK;
    }

    /**
     * Stream input operations.
     */
    LLBC_File &operator >>(sint8 &value) throw (LLBC_IOException);
    LLBC_File &operator >>(uint8 &value) throw (LLBC_IOException);
    LLBC_File &operator >>(sint16 &value) throw (LLBC_IOException);
    LLBC_File &operator >>(uint16 &value) throw (LLBC_IOException);
    LLBC_File &operator >>(sint32 &value) throw (LLBC_IOException);
    LLBC_File &operator >>(uint32 &value) throw (LLBC_IOException);
    LLBC_File &operator >>(sint64 &value) throw (LLBC_IOException);
    LLBC_File &operator >>(uint64 &value) throw (LLBC_IOException);
    LLBC_File &operator >>(float &value) throw (LLBC_IOException);
    LLBC_File &operator >>(double &value) throw (LLBC_IOException);
    LLBC_File &operator >>(LLBC_String &value) throw (LLBC_IOException);
    LLBC_File &operator >>(LLBC_Stream &value) throw (LLBC_IOException);

public:
    /**
     * Write data to the file.
     * @param[in] buf  - pointer to data to be written.
     * @param[in] size - bytes to be written.
     * @return size_t - real written size, if return npos, it means error occurred.
     */
    size_t Write(const void *buf, size_t size);

    /**
     * Write string value, will auto append the '\0' character.
     * @param[in] str       - string value.
     * @param[in] writeNull - write NULL flag, default is true.
     * @return int - return 0 if success, otherwise return -1.
     */
    int WriteString(const LLBC_String &str, bool writeNull = true);

    /**
     * Write stream value to file.
     * @param[in] stream - stream value.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int WriteStream(const LLBC_Stream &stream);

    /**
     * Write line data, will auto append the CRLF or LF character(s).
     * @param[in] line - line data.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int WriteLine(const LLBC_String &line);

    /**
     * Write data to the file from object.
     * @param[in] obj - will write object.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _Ty>
    int WriteObj(const _Ty &obj)
    {
        size_t writeRet = this->Write(&obj, sizeof(_Ty));
        if (writeRet != sizeof(_Ty))
        {
            if (writeRet != npos)
                LLBC_SetLastError(LLBC_ERROR_TRUNCATED);

            return LLBC_RTN_FAILED;
        }

        return LLBC_RTN_OK;
    }

    /**
     * Stream output operation.
     */
    LLBC_File &operator <<(const sint8 &value) throw (LLBC_IOException);
    LLBC_File &operator <<(const uint8 &value) throw (LLBC_IOException);
    LLBC_File &operator <<(const sint16 &value) throw (LLBC_IOException);
    LLBC_File &operator <<(const uint16 &value) throw (LLBC_IOException);
    LLBC_File &operator <<(const sint32 &value) throw (LLBC_IOException);
    LLBC_File &operator <<(const uint32 &value) throw (LLBC_IOException);
    LLBC_File &operator <<(const sint64 &value) throw (LLBC_IOException);
    LLBC_File &operator <<(const uint64 &value) throw (LLBC_IOException);
    LLBC_File &operator <<(const float &value) throw (LLBC_IOException);
    LLBC_File &operator <<(const double &value) throw (LLBC_IOException);
    LLBC_File &operator <<(const LLBC_String &value) throw (LLBC_IOException);
    LLBC_File &operator <<(const LLBC_Stream &value) throw (LLBC_IOException);

    /**
     * Flush file data to device.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Flush();

public:
    /**
     * Delete current file.
     * @return int - return 0 if success, otherwise return -1.
     */
    int DeleteSelf();

    /**
     * Delete specified path file.
     * @param[in] path - file path.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int Delete(const LLBC_String &path);

public:
    /**
     * Move current file.
     * @param[in] path1   - new file path.
     * @param[in] overlap - overlapped flag.
     * @param[in] reopen  - when move completed, reopen the file or not.
     * @param[in] mode1   - type of access permitted.
     * @return int - return 0 if success, otherwise return -1.
     */
    int MoveSelf(const LLBC_String &path1, 
                 bool overlap = false,
                 bool reopen = false, 
                 const LLBC_String &mode1 = "rb");

    /**
     * Move file.
     * @param[in] path0   - current file path.
     * @param[in] path1   - new file path.
     * @param[in] overlap - overlapped flag.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int Move(const LLBC_String &path0, 
                    const LLBC_String &path1,
                    bool overlap = false);

public:
    /**
     * Copy current file.
     * @param[in] path1      - new file path.
     * @param[in] overlap    - overlapped flag.
     * @param[in] copyBuf    - copy buffer, if NULL, will allocate in internal.
     * @param[in] copyBufLen - copy buffer length.
     * @return int - return 0 if success, otherwise return -1.
     */
    int CopySelf(const LLBC_String &path1,
                 bool overlap = false,
                 void *copyBuf = NULL,
                 size_t copyBufLen = 0);

    /**
     * Copy file.
     * @param[in] path0      - current file path.
     * @param[in] path1      - new file path.
     * @param[in] overlap    - overlapped flag.
     * @param[in] copyBuf    - copy buffer, if NULL, will allocate in internal.
     * @param[in] copyBufLen - copy buffer length.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int Copy(const LLBC_String &path0,
                    const LLBC_String &path1,
                    bool overlap = false,
                    void *copyBuf = NULL,
                    size_t copyBufLen = 0);

public:
    /**
     * Get current file directory name.
     * @return LLBC_String - directory name.
     */
    LLBC_String GetSelfDirectoryName() const;

    /**
     * Get specified file path's directory name.
     * @param[in] path - file path.
     * @return LLBC_String - directory name.
     */
    static LLBC_String  GetDirectoryName(const LLBC_String &path);

    /**
     * Get current file base name.
     * @param[in] incExtension - sure include extension, default is true.
     * @return LLBC_String - file base name.
     */
    LLBC_String GetSelfBaseName(bool incExtension = true) const;

    /**
     * Get specified file path's base name.
     * @param[in] path         - file path.
     * @param[in] incExtension - sure include extension, default is true.
     * @return LLBC_String - file base name.
     */
    static LLBC_String GetBaseName(const LLBC_String &path, bool incExtension = true);

    /**
     * Get current file extension name.
     * @return LLBC_String - file extension name.
     */
    LLBC_String GetSelfExtension() const;

    /**
     * Get specified file path's extension name.
     * @param[in] path - file path.
     * @return LLBC_String - file extension name.
     */
    static LLBC_String GetExtension(const LLBC_String &path);

public:
    /**
     * Check given path is exist or not.
     * @param[in] path - file path.
     * @return bool - return true if exist, otherwise return false.
     */
    static bool Exist(const LLBC_String &path);

    /**
     * Create empty file.
     * @param[in] path - file path.
     * @return int - return 0 if successed, otherwise return -1.
     */
    static int Touch(const LLBC_String &path);

public:
    /**
     * Convert given path to full path.
     * @param[in] path - file path.
     * @return LLBC_String - full path.
     */
    static LLBC_String ToFullPath(const LLBC_String &path);


    /* Exception throw version APIs define. */
public:
    void OpenT(const LLBC_String &path, 
               const LLBC_String &mode = "rb") throw (LLBC_IOException);

    void CloseT() throw (LLBC_IOException);

public:
    void SetBufferModeT(int mode, size_t size) throw (LLBC_IOException);

public:
    size_t TellT() const throw (LLBC_IOException);

    size_t GetSizeT() const throw (LLBC_IOException);

    size_t SeekT(int origin, long offset) throw (LLBC_IOException);

public:
    size_t ReadT(void *buf, size_t size) throw (LLBC_IOException);

    void ReadStringT(LLBC_String &str) throw (LLBC_IOException);

    void ReadStreamT(LLBC_Stream &stream) throw (LLBC_IOException);

    void ReadLineT(LLBC_String &line) throw (LLBC_IOException);

    template <typename _Ty>
    void ReadObjT(_Ty &obj) throw (LLBC_IOException)
    {
        size_t readRet = this->Read(&obj, sizeof(_Ty));
        if (readRet != sizeof(_Ty))
        {
            if (readRet != npos)
                LLBC_SetLastError(LLBC_ERROR_END);

            throw LLBC_IOException(LLBC_FormatLastError());
        }
    }

public:
    size_t WriteT(const void *buf, size_t size) throw (LLBC_IOException);

    void WriteStringT(const LLBC_String &str, bool writeNull = true) throw (LLBC_IOException);

    void WriteStreamT(const LLBC_Stream &stream) throw (LLBC_IOException);

    void WriteLineT(const LLBC_String &line) throw (LLBC_IOException);

    template <typename _Ty>
    void WriteObjT(const _Ty &obj) throw (LLBC_IOException)
    {
        size_t writeRet = this->Write(&obj, sizeof(_Ty));
        if (writeRet != sizeof(_Ty))
        {
            if (writeRet != npos)
                LLBC_SetLastError(LLBC_ERROR_TRUNCATED);

            throw LLBC_IOException(LLBC_FormatLastError());
        }
    }

    void FlushT() throw (LLBC_IOException);

public:
    void DeleteSelfT() throw (LLBC_IOException);

    static void DeleteT(const LLBC_String &path) throw (LLBC_IOException);

public:
    void MoveSelfT(const LLBC_String &path1, 
                   bool overlap = false,
                   bool reopen = false, 
                   const LLBC_String &mode1 = "rb") throw (LLBC_IOException);

    static void MoveT(const LLBC_String &path0, 
                      const LLBC_String &path1,
                      bool overlap = false) throw (LLBC_IOException);

public:
    void CopySelfT(const LLBC_String &path1,
                   bool overlap = false,
                   void *copyBuf = NULL,
                   size_t copyBufLen = 0) throw (LLBC_IOException);

    static void CopyT(const LLBC_String &path0,
                      const LLBC_String &path1,
                      bool overlap = false,
                      void *copyBuf = NULL,
                      size_t copyBufLen = 0) throw (LLBC_IOException);

public:
    static void TouchT(const LLBC_String &path);

private:
    LLBC_String _path;
    LLBC_String _mode;

    LLBC_FileHandle _handle;
};

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4290)
#endif

#endif // __LLBC_CORE_FILE_FILE_H__
