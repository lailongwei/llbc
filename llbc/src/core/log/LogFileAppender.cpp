/**
 * @file    LogFileAppender.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/11
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Time.h"
#include "llbc/core/os/OS_Console.h"

#include "llbc/core/file/File.h"
#include "llbc/core/file/Directory.h"
#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/utils/Util_Math.h"
#include "llbc/core/utils/Util_Debug.h"

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogTokenChain.h"
#include "llbc/core/log/LogFileAppender.h"

__LLBC_INTERNAL_NS_BEGIN
const static int __LogFileCheckInterval = 500;
__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_LogFileAppender::LLBC_LogFileAppender()
: _basePath()
, _baseName()
, _fileSuffix()

, _fileBufferSize(0)
, _isDailyRolling(true)

, _maxFileSize(LONG_MAX)
, _maxBackupIndex(INT_MAX)

, _fileName()

, _file(NULL)
, _fileSize(0)

, _nonFlushLogCount(0)
, _logfileLastCheckTime(0)
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
    if (_Base::Initialize(initInfo) != LLBC_OK)
        return LLBC_FAILED;

    if (initInfo.file.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    _baseName = initInfo.file;
    _fileSuffix = initInfo.fileSuffix;
    LLBC_String logDir = LLBC_Directory::DirName(_baseName);

    if (initInfo.forceAppLogPath)
    {
        _basePath = LLBC_Directory::ModuleFileDir();
        logDir = LLBC_Directory::Join(_basePath, logDir);
    }

    if (!logDir.empty() && !LLBC_Directory::Exists(logDir))
    {
        if (LLBC_Directory::Create(logDir) != LLBC_OK)
            return LLBC_FAILED;
    }

    _fileBufferSize = MAX(0, initInfo.fileBufferSize);
    _isDailyRolling = initInfo.dailyRolling;

    _maxFileSize = initInfo.maxFileSize > 0 ? initInfo.maxFileSize : LONG_MAX;
    _maxBackupIndex = MAX(0, initInfo.maxBackupIndex);

    sint64 now = LLBC_GetMilliSeconds();

    _file = LLBC_New(LLBC_File);
    _fileName = BuildLogFileName(now);

    LLBC_FileAttributes fileAttrs;
    fileAttrs.fileSize = 0;
    bool fileExists = LLBC_File::Exists(_fileName);
    if (fileExists)
    {
        if (LLBC_File::GetFileAttributes(_fileName, fileAttrs) != LLBC_OK)
            return LLBC_FAILED;
    }

    bool reOpenClear = false;
    int backupFilesCount = GetBackupFilesCount(_fileName);
    if (fileExists &&
        (fileAttrs.fileSize >= _maxFileSize ||
            backupFilesCount < _maxBackupIndex))
    {
        BackupFiles();
        reOpenClear = true;
    }

    if (ReOpenFile(_fileName, reOpenClear) != LLBC_OK)
        return LLBC_FAILED;

    _nonFlushLogCount = 0;
    _logfileLastCheckTime = now;

    return LLBC_OK;
}

void LLBC_LogFileAppender::Finalize()
{
    _basePath.clear();
    _baseName.clear();
    _fileSuffix.clear();

    _fileBufferSize = 0;
    _isDailyRolling = false;

    _maxFileSize = LONG_MAX;
    _maxBackupIndex = INT_MAX;

    _fileName.clear();

    LLBC_XDelete(_file);
    _fileSize = 0;

    _nonFlushLogCount = 0;
    _logfileLastCheckTime = 0;

    _Base::Finalize();
}

int LLBC_LogFileAppender::Output(const LLBC_LogData &data)
{
    LLBC_LogTokenChain *chain = NULL;
    if (UNLIKELY(!(chain = GetTokenChain())))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    if (data.level < GetLogLevel())
        return LLBC_OK;

    CheckAndUpdateLogFile(data.logTime);

    LLBC_String formattedData;
    chain->Format(data, formattedData);

    const long actuallyWrote = 
        _file->Write(formattedData.data(), formattedData.size());
    if (actuallyWrote != -1)
    {
        _fileSize += actuallyWrote;
        if (_fileBufferSize > 0) // If file buffered, process flush logic
        {
            _nonFlushLogCount += 1;
            if (data.level >= LLBC_LogLevel::Warn)
                Flush();
        }

        if (actuallyWrote != static_cast<long>(formattedData.size()))
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

    if (LIKELY(_file))
    {
        _file->Flush();
        _nonFlushLogCount = 0;
    }
}

void LLBC_LogFileAppender::CheckAndUpdateLogFile(sint64 now)
{
    if (_fileSize < _maxFileSize && 
        LLBC_Abs(_logfileLastCheckTime - now) < LLBC_INL_NS __LogFileCheckInterval)
        return;

    bool clear = false, backup = false;
    const LLBC_String newFileName = BuildLogFileName(now);
    if (!IsNeedReOpenFile(now, newFileName, clear, backup))
        return;

    if (backup)
        BackupFiles();

    ReOpenFile(newFileName, clear);
    _logfileLastCheckTime = now;
}

LLBC_String LLBC_LogFileAppender::BuildLogFileName(sint64 now) const
{
    LLBC_String logFile(_basePath.empty() ? _baseName : LLBC_Directory::Join(_basePath, _baseName));
    if (_isDailyRolling)
    {
        struct tm timeStruct;
        time_t nowInSecond = static_cast<time_t>(now / 1000);
#if LLBC_TARGET_PLATFORM_WIN32
        localtime_s(&timeStruct, &nowInSecond);
#else
        localtime_r(&nowInSecond, &timeStruct);
#endif

        char timeFmtBuf[9];
        timeFmtBuf[sizeof(timeFmtBuf) - 1] = '\0';
        strftime(timeFmtBuf, 9, "%y-%m-%d", &timeStruct);

        logFile.append_format(".%s", timeFmtBuf);
    }

    if (!_fileSuffix.empty())
        logFile.append(_fileSuffix);

    return logFile;
}

bool LLBC_LogFileAppender::IsNeedReOpenFile(sint64 now,
                                            const LLBC_String &newFileName,
                                            bool &clear,
                                            bool &backup) const
{
    if (_fileSize >= _maxFileSize)
    {
        clear = true;
        backup = true;

        return true;
    }
    else if (newFileName.size() != _fileName.size() ||
            ::memcmp(newFileName.data(), _fileName.data(), _fileName.size()) != 0)
    {
        clear = false;
        backup = false;

        return true;
    }
    else if (!LLBC_File::Exists(newFileName))
    {
        clear = true;
        backup = false;

        return true;
    }

    return false;
}

int LLBC_LogFileAppender::ReOpenFile(const LLBC_String &newFileName, bool clear)
{
    // Close old file.
    if (_file)
        _file->Close();
    else
        _file = LLBC_New(LLBC_File);

    // Reset non-reflush log count variables.
    _nonFlushLogCount = 0;
    // Do reopen file.
    if (UNLIKELY(_file->Open(newFileName, clear ? 
        LLBC_FileMode::BinaryWrite : LLBC_FileMode::BinaryAppendWrite) != LLBC_OK))
    {
#ifdef LLBC_DEBUG
        traceline("LLBC_LogFileAppender::ReOpenFile(): Open file failed, name:%s, clear:%s, reason:%s",
            __FILE__, __LINE__, newFileName.c_str(), clear, LLBC_FormatLastError());
#endif
        return LLBC_FAILED;
    }

    // Update file name/size, buffer info.
    _fileName = newFileName;
    _fileSize = _file->GetFileSize();
    UpdateFileBufferInfo();

    return LLBC_OK;
}

void LLBC_LogFileAppender::BackupFiles() const
{
    if (_maxBackupIndex == 0)
        return;
    else if (!LLBC_File::Exists(_fileName))
        return;

    if (_file->IsOpened())
        _file->Close();

    const LLBC_String fileNameNonSuffix = 
        _fileName.substr(0, _fileName.size() - _fileSuffix.size());

    int availableIndex = 0;
    while (availableIndex < _maxBackupIndex)
    {
        availableIndex += 1;
        LLBC_String backupFileName;
        backupFileName.format("%s.%d%s", fileNameNonSuffix.c_str(), availableIndex, _fileSuffix.c_str());
        if (!LLBC_File::Exists(backupFileName))
            break;
    }

    for (int willMoveIndex = availableIndex - 1; willMoveIndex >= 0; willMoveIndex--)
    {
        LLBC_String willMove;
        if (willMoveIndex > 0)
            willMove.format("%s.%d%s", fileNameNonSuffix.c_str(), willMoveIndex, _fileSuffix.c_str());
        else
            willMove = _fileName;

        LLBC_String moveTo;
        moveTo.format("%s.%d%s", fileNameNonSuffix.c_str(), willMoveIndex + 1, _fileSuffix.c_str());

#ifdef LLBC_RELEASE
        LLBC_File::MoveFile(willMove, moveTo, true);
#else
        if (LLBC_File::MoveFile(willMove, moveTo, true) != LLBC_OK)
        {
            traceline("LLBC_LogFileAppender::BackupFiles(): Backup failed, reason: %s", LLBC_FormatLastError());
        }
#endif
    }
}

void LLBC_LogFileAppender::UpdateFileBufferInfo()
{
    if (_fileBufferSize == 0)
        _file->SetBufferMode(LLBC_FileBufferMode::NoBuf, 0);
    else
        _file->SetBufferMode(LLBC_FileBufferMode::FullBuf, _fileBufferSize);
}

int LLBC_LogFileAppender::GetBackupFilesCount(const LLBC_String &logFileName) const
{
    int backupFilesCount = 0;
    for (int i = 1; i <= _maxBackupIndex; i++)
    {
        const LLBC_String bkLogFileName = LLBC_String().format("%s.%d", logFileName.c_str(), i);
        if (!LLBC_File::Exists(bkLogFileName))
            break;

        backupFilesCount += 1;
    }

    return backupFilesCount;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
