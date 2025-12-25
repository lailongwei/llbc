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

#include "llbc/core/file/File.h"
#include "llbc/core/log/BaseLogAppender.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_File;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief File log appender class encapsulation.
 */
class LLBC_HIDDEN LLBC_LogFileAppender : public LLBC_BaseLogAppender
{
    typedef LLBC_BaseLogAppender _Base;

public:
    LLBC_LogFileAppender();
    ~LLBC_LogFileAppender() override;

public:
    /**
     * Get log appender type, see LLBC_LogAppenderType.
     * @return int - log appender type.
     */
    int GetType() const override;

public:
    /**
     * Initialize the log appender.
     * @param[in] initInfo - log appender initialize info structure.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Initialize(const LLBC_LogAppenderInitInfo &initInfo) override;

    /**
     * Finalize the appender.
     */
    void Finalize() override;

    /**
     * Output log data.
     * @param[in] data - log data.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Output(const LLBC_LogData &data) override;

protected:
    /**
     * Flush method.
     */
    void Flush() override;

private:
    /**
     * Check and update log file.
     * @param[in] now - now time, in micro-seconds.
     */
    void CheckAndUpdateLogFile(sint64 now);

    /**
     * Build log file path.
     * @now                - now time, in micro-seconds.
     * @return LLBC_String - the log file path.
     */
    LLBC_String BuildLogFilePath(sint64 now) const;

    /**
     * Check is need reopen file or not.
     * @param[in] newFilePath - the new file path.
     * @param[out] backup     - the backup flag, if true, means must backup log files.
     *                          about backup, see BackupFiles() method.
     * @param[out] clear      - the clear flag, if true, means when reopen file, must clear file content.
     * @return bool - need reopen if true, otherwise return false.
     */
    bool IsNeedReOpenFile(const LLBC_String &newFilePath, bool &backup, bool &clear) const;

    /**
     * ReOpen the log file.
      * @param[in] newFileName - the new log file name.
      * @param[in] clear       - clear flag.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ReOpenFile(const LLBC_String &newFileName, bool clear);

    /**
     * Backup all log files.
     */
    void BackupFiles();

    /**
     * Update log file buffer info(included buffer mode and buffer size).
     */
    void UpdateFileBufferInfo();

    /**
     * Get backup files count, if failed, return -1.
     */
    int GetBackupFilesCount(const LLBC_String &logFileName) const;

    /**
     * Update advise discard file page cache.
     */
    void UpdateAdviseDiscardPageCache();

private:
    LLBC_String _fileDir;
    LLBC_String _fileBasePath;
    LLBC_String _fileSuffix;

    int _fileBufferSize;
    int _fileRollingMode;

    sint64 _maxFileSize;
    int _maxBackupIndex;

    bool _fadviseDiscardEnabled;
    sint64 _fadviseDiscardSize;
    sint64 _fadviseDiscardRetainSize;

private:
    LLBC_File _file;
    sint64 _fileSize;

    sint64 _notFlushLogCount;
    sint64 _logFileLastCheckTime;

    sint64 _lastAdviseEndOffset;
};

__LLBC_NS_END
