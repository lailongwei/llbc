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
const static LLBC_NS LLBC_String __ClearOpenFlag = "wb";
const static LLBC_NS LLBC_String __AppendOpenFlag = "ab";

const static int __LogFileCheckInterval = 500;
__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_LogFileAppender::LLBC_LogFileAppender()
: _baseName()

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

    const sint64 now = LLBC_GetMilliSeconds();
    const LLBC_String fileName = BuildLogFileName(now);
    if (ReOpenFile(fileName, false) != LLBC_OK)
        return LLBC_FAILED;

    if (_fileSize >= _maxFileSize)
    {
        BackupFiles();
        if (ReOpenFile(fileName, true) != LLBC_OK)
            return LLBC_FAILED;
    }

    _nonFlushLogCount = 0;
    _logfileLastCheckTime = now;

    return LLBC_OK;
}

void LLBC_LogFileAppender::Finalize()
{
    _baseName.clear();

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
    else if (::memcmp(newFileName.data(), _fileName.data(), _fileName.size()) != 0)
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

    int availableIndex = 0;
    while (availableIndex < _maxBackupIndex)
    {
        availableIndex += 1;
        LLBC_String backupFileName;
        backupFileName.format("%s.%d", _fileName.c_str(), availableIndex);
        if (!LLBC_File::Exists(backupFileName))
            break;
    }

    for (int willMoveIndex = availableIndex - 1; willMoveIndex >= 0; willMoveIndex--)
    {
        LLBC_String willMove;
        if (willMoveIndex > 0)
            willMove.format("%s.%d", _fileName.c_str(), willMoveIndex);
        else
            willMove = _fileName;

        LLBC_String moveTo;
        moveTo.format("%s.%d", _fileName.c_str(), willMoveIndex + 1);

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

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
