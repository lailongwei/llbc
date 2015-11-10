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

#include "llbc/core/time/Time.h"
#include "llbc/core/file/File.h"
#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogTokenChain.h"
#include "llbc/core/log/LogFileAppender.h"

__LLBC_NS_BEGIN

LLBC_LogFileAppender::LLBC_LogFileAppender()
: _isDailyRolling(true)
, _maxFileSize(LONG_MAX)
, _maxBackupIndex(INT_MAX)
, _fileName()
, _file(NULL)
{
}

LLBC_LogFileAppender::~LLBC_LogFileAppender()
{
    this->Finalize();
}

int LLBC_LogFileAppender::GetType() const
{
    return LLBC_LogAppenderType::File;
}

int LLBC_LogFileAppender::Initialize(const LLBC_LogAppenderInitInfo &initInfo)
{
    if (_Base::Initialize(initInfo) != LLBC_RTN_OK)
        return LLBC_RTN_FAILED;

    if (initInfo.file.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    _isDailyRolling = initInfo.dailyRolling;

    _maxFileSize = MAX(1, initInfo.maxFileSize);
    _maxBackupIndex = MAX(0, initInfo.maxBackupIndex);

    _fileName.clear();
    _fileName.append(initInfo.file);

    _file = new LLBC_File;
    if (this->OpenLogFile(*_file) != LLBC_RTN_OK)
    {
        this->Finalize();
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

void LLBC_LogFileAppender::Finalize()
{
    _isDailyRolling = false;

    _maxFileSize = LONG_MAX;
    _maxBackupIndex = INT_MAX;

    _fileName.clear();

    LLBC_XDelete(_file);

    _Base::Finalize();
}

int LLBC_LogFileAppender::Output(const LLBC_LogData &data)
{
    LLBC_LogTokenChain *chain = NULL;
    if (!(chain = this->GetTokenChain()))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_RTN_FAILED;
    }

    const size_t fileSize = _file->GetSize();
    if (fileSize > _maxFileSize || fileSize < 0)
    {
        _file->Close();
        if (this->OpenLogFile(*_file) != LLBC_RTN_OK)
            return LLBC_RTN_FAILED;
    }

    LLBC_String formattedData;

    chain->Format(data, formattedData);
    const size_t actuallyWrite = 
        _file->Write(formattedData.data(), formattedData.size());
    if (actuallyWrite != LLBC_File::npos)
    {
        _file->Flush();
        if (actuallyWrite != formattedData.size())
        {
            LLBC_SetLastError(LLBC_ERROR_TRUNCATED);
            return LLBC_RTN_FAILED;
        }

        return LLBC_RTN_OK;
    }

    return LLBC_RTN_FAILED;
}

int LLBC_LogFileAppender::OpenLogFile(LLBC_File &file) const
{
    LLBC_String name;

    // Build basic name.
    name.append(_fileName);

    // Append date info, if in daily rolling mode.
    if (_isDailyRolling)
    {
        name.append(".");
        name.append(LLBC_Time::GetCurrentTime().Format("%Y-%m-%d"));
    }

    // Try to open this file.
    // If file not exist, open it and return.
    if (!LLBC_File::Exist(name))
        return file.Open(name, "ab+");

    // Exist, we must try to obtain backup index.
    if (file.Open(name, "ab+") != LLBC_RTN_OK)
        return LLBC_RTN_FAILED;
    if (file.GetSize() <= _maxFileSize)
        return LLBC_RTN_OK;

    file.Close();

    int curBackupIndex = 1;
    while (curBackupIndex <= _maxBackupIndex)
    {
        LLBC_String curFileName = name;
        curFileName += ".";
        curFileName += LLBC_Num2Str(curBackupIndex);

        if (!LLBC_File::Exist(curFileName))
            break;

        curBackupIndex ++;
    }

    if (curBackupIndex > _maxBackupIndex)
    {
        if (_maxBackupIndex == 0)
            return file.Open(name, "wb+");

        curBackupIndex --;
    }

    while (curBackupIndex > 0)
    {
        LLBC_String movedFileName = name;
        movedFileName += ".";
        movedFileName += LLBC_Num2Str(curBackupIndex);

        LLBC_String willMoveFileName = name;
        if (-- curBackupIndex > 0)
        {
            willMoveFileName += ".";
            willMoveFileName += LLBC_Num2Str(curBackupIndex);
        }

        if (LLBC_File::Move(willMoveFileName, movedFileName, true) != LLBC_RTN_OK)
            return LLBC_RTN_FAILED;
    }

    return file.Open(name, "ab+");
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
