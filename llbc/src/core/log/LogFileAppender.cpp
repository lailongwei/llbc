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

#include "llbc/core/os/OS_Time.h"
#include "llbc/core/os/OS_Console.h"

#include "llbc/core/file/File.h"
#include "llbc/core/file/Directory.h"
#include "llbc/core/utils/Util_Debug.h"

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogRollingMode.h"
#include "llbc/core/log/LogTokenChain.h"
#include "llbc/core/log/LogFileAppender.h"

__LLBC_INTERNAL_NS_BEGIN
const static int __LogFileCheckInterval = 500000; // In micro-seconds.
__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_LogFileAppender::LLBC_LogFileAppender()
: _fileBufferSize(0)
, _fileRollingMode(LLBC_LogRollingMode::End)

, _maxFileSize(LONG_MAX)
, _maxBackupIndex(INT_MAX)

, _file(nullptr)
, _fileSize(0)

, _nonFlushLogCount(0)
, _logFileLastCheckTime(0)
{
}

LLBC_LogFileAppender::~LLBC_LogFileAppender()
{
    Finalize();
}

int LLBC_LogFileAppender::GetType() const
{
    return LLBC_LogAppenderType::File;
}

int LLBC_LogFileAppender::Initialize(const LLBC_LogAppenderInitInfo &initInfo)
{
    // Init info check.
    if (initInfo.filePath.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    // Init base appender.
    if (_Base::Initialize(initInfo) != LLBC_OK)
        return LLBC_FAILED;

    // Save file appender config to members.
    _fileBasePath = initInfo.filePath;
    _fileSuffix = initInfo.fileSuffix;
    const LLBC_String fileDir = LLBC_Directory::DirName(_fileBasePath);
    if (!LLBC_Directory::Exists(fileDir))
    {
        if (LLBC_Directory::Create(fileDir) != LLBC_OK)
        {
            _fileSuffix.clear();
            _fileBasePath.clear();

            _Base::Finalize();

            return LLBC_FAILED;
        }
    }

    _fileBufferSize = MAX(0, initInfo.fileBufferSize);
    _fileRollingMode = initInfo.fileRollingMode;

    _maxFileSize = initInfo.maxFileSize > 0 ? initInfo.maxFileSize : LONG_MAX;
    _maxBackupIndex = MAX(0, initInfo.maxBackupIndex);

    // If lazy create log file, return it.
    if (initInfo.lazyCreateLogFile)
        return LLBC_OK;

    // Force check and update file one time.
    CheckAndUpdateLogFile(LLBC_GetMicroSeconds());

    return LLBC_OK;
}

void LLBC_LogFileAppender::Finalize()
{
    _fileBasePath.clear();
    _fileSuffix.clear();

    _fileBufferSize = 0;
    _fileRollingMode = LLBC_LogRollingMode::End;

    _maxFileSize = LONG_MAX;
    _maxBackupIndex = INT_MAX;

    _file.Close();
    _fileSize = 0;

    _nonFlushLogCount = 0;
    _logFileLastCheckTime = 0;

    _Base::Finalize();
}

int LLBC_LogFileAppender::Output(const LLBC_LogData &data)
{
    LLBC_LogTokenChain *chain = GetTokenChain();
    if (UNLIKELY(!chain))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    if (data.level < GetLogLevel())
        return LLBC_OK;

    CheckAndUpdateLogFile(data.logTime);

    auto &logFmtBuf = GetLogFormatBuf();
    logFmtBuf.clear();
    chain->Format(data, logFmtBuf);

    const long actuallyWrote = 
        _file.Write(logFmtBuf.data(), logFmtBuf.size());
    if (actuallyWrote != -1)
    {
        _fileSize += actuallyWrote;
        if (_fileBufferSize > 0) // If file buffered, process flush logic
        {
            _nonFlushLogCount += 1;
            if (data.level >= LLBC_LogLevel::Warn)
                Flush();
        }

        if (UNLIKELY(actuallyWrote != static_cast<long>(logFmtBuf.size())))
        {
            LLBC_SetLastError(LLBC_ERROR_TRUNCATED);
            return LLBC_FAILED;
        }

        return LLBC_OK;
    }

    return LLBC_FAILED;
}

void LLBC_LogFileAppender::Flush()
{
    if (_nonFlushLogCount == 0)
        return;

    _file.Flush();
    _nonFlushLogCount = 0;
}

void LLBC_LogFileAppender::CheckAndUpdateLogFile(sint64 now)
{
    // Is need check?
    const sint64 timeDiff = now - _logFileLastCheckTime;
    if (_fileSize < _maxFileSize && // File size not reach to limit.
        (timeDiff >= 0 && timeDiff < LLBC_INL_NS __LogFileCheckInterval) && // not reach check interval.
        now / 1000000 == _logFileLastCheckTime / 1000000) // no cross seconds.
        return;

    // Update log file last check time.
    _logFileLastCheckTime = now;

    // If file not opened, open file first.
    const LLBC_String filePath = BuildLogFilePath(now);
    if (UNLIKELY(!_file.IsOpened()))
    {
        if (ReOpenFile(filePath, false) != LLBC_OK)
            return;
    }

    // Execute log file(s) backup and reopen, if need.
    bool backup = false, clear = false;
    if (IsNeedReOpenFile(filePath, backup, clear))
    {
        if (backup)
            BackupFiles();
        ReOpenFile(filePath, clear);
    }
}

LLBC_String LLBC_LogFileAppender::BuildLogFilePath(sint64 now) const
{
    LLBC_String filePath = _fileBasePath;
    if (_fileRollingMode != LLBC_LogRollingMode::NoRolling)
    {
        struct tm timeStruct;
        time_t nowInSecs = static_cast<time_t>(now / 1000000);
#if LLBC_TARGET_PLATFORM_WIN32
        localtime_s(&timeStruct, &nowInSecs);
#else
        localtime_r(&nowInSecs, &timeStruct);
#endif

        char timeFmtBuf[16];
        timeFmtBuf[0] = '.';
        const size_t len = strftime(timeFmtBuf + 1,
                                    sizeof(timeFmtBuf) - 1,
                                    _fileRollingMode == 
                                        LLBC_LogRollingMode::HourlyRolling ? "%y%m%d%H" : "%y%m%d",
                                    &timeStruct);
        if (LIKELY(len > 0))
            filePath.append(timeFmtBuf, len + 1);
    }

    if (!_fileSuffix.empty())
        filePath.append(_fileSuffix);

    return filePath;
}

bool LLBC_LogFileAppender::IsNeedReOpenFile(const LLBC_String &newFilePath,
                                            bool &backup,
                                            bool &clear) const
{
    if (_fileSize >= _maxFileSize)
    {
        backup = true;
        clear = true;

        return true;
    }
    else if (newFilePath.size() != _file.GetFilePath().size() ||
             memcmp(newFilePath.data(), _file.GetFilePath().data(), _file.GetFilePath().size()) != 0)
    {
        backup = false;
        clear = false;

        return true;
    }
    else if (!LLBC_File::Exists(newFilePath))
    {
        backup = false;
        clear = true;

        return true;
    }

    return false;
}

int LLBC_LogFileAppender::ReOpenFile(const LLBC_String &newFileName, bool clear)
{
    // Close old file.
    if (LIKELY(_file.IsOpened()))
    {
        _file.Close();
        _fileSize = 0;
    }

    // Reset non-reflush log count variables.
    _nonFlushLogCount = 0;
    // Do reopen file.
    if (UNLIKELY(_file.Open(newFileName, clear ? 
        LLBC_FileMode::BinaryWrite : LLBC_FileMode::BinaryAppendWrite) != LLBC_OK))
    {
#ifdef LLBC_DEBUG
        traceline("LLBC_LogFileAppender::ReOpenFile(): "
                  "Open file failed, name:%s, clear:%s, reason:%s",
                  newFileName.c_str(), clear ? "true" : "false", LLBC_FormatLastError());
#endif
        return LLBC_FAILED;
    }

    // Update file size, buffer info.
    _fileSize = _file.GetFileSize();
    UpdateFileBufferInfo();

    return LLBC_OK;
}

void LLBC_LogFileAppender::BackupFiles()
{
    if (_maxBackupIndex == 0)
        return;

    const LLBC_String filePath = _file.GetFilePath();
    _file.Close();
    _fileSize = 0;

    const LLBC_String filePathNoSuffix = 
        filePath.substr(0, filePath.size() - _fileSuffix.size());

    int availableIndex = 0;
    while (availableIndex < _maxBackupIndex)
    {
        availableIndex += 1;
        LLBC_String backupFileName;
        backupFileName.format("%s.%d%s", filePathNoSuffix.c_str(), availableIndex, _fileSuffix.c_str());
        if (!LLBC_File::Exists(backupFileName))
            break;
    }

    for (int willMoveIndex = availableIndex - 1; willMoveIndex >= 0; --willMoveIndex)
    {
        LLBC_String willMove;
        if (willMoveIndex > 0)
            willMove.format("%s.%d%s", filePathNoSuffix.c_str(), willMoveIndex, _fileSuffix.c_str());
        else
            willMove = filePath;

        LLBC_String moveTo;
        moveTo.format("%s.%d%s", filePathNoSuffix.c_str(), willMoveIndex + 1, _fileSuffix.c_str());

#ifdef LLBC_RELEASE
        LLBC_File::MoveFile(willMove, moveTo, true);
#else
        if (LLBC_File::MoveFile(willMove, moveTo, true) != LLBC_OK)
        {
            traceline("LLBC_LogFileAppender::BackupFiles(): Backup failed, %s -> %s, reason: %s", 
                willMove.c_str(), moveTo.c_str(), LLBC_FormatLastError());
        }
#endif
    }
}

void LLBC_LogFileAppender::UpdateFileBufferInfo()
{
    if (_fileBufferSize == 0)
        _file.SetBufferMode(LLBC_FileBufferMode::NoBuf, 0);
    else
        _file.SetBufferMode(LLBC_FileBufferMode::FullBuf, _fileBufferSize);
}

int LLBC_LogFileAppender::GetBackupFilesCount(const LLBC_String &logFileName) const
{
    int backupFilesCount = 0;
    for (int i = 1; i <= _maxBackupIndex; ++i)
    {
        const LLBC_String bkLogFileName = LLBC_String().format("%s.%d", logFileName.c_str(), i);
        if (!LLBC_File::Exists(bkLogFileName))
            break;

        backupFilesCount += 1;
    }

    return backupFilesCount;
}

__LLBC_NS_END
