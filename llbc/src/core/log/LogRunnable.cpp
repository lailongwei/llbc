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
#include "llbc/core/thread/MessageBlock.h"
#include "llbc/core/objectpool/PoolObjectReflection.h"

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/Logger.h"
#include "llbc/core/log/ILogAppender.h"
#include "llbc/core/log/LogRunnable.h"

__LLBC_NS_BEGIN

LLBC_LogRunnable::LLBC_LogRunnable()
: _stopping(false)
{
    for (size_t i = 0; i < sizeof(_logDatas) / sizeof(_logDatas[0]); ++i)
        _logDatas[i].reserve(4096);
}

LLBC_LogRunnable::~LLBC_LogRunnable()
{
}

int LLBC_LogRunnable::AddLogger(LLBC_Logger* logger)
{
    if (IsActivated())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    if (std::find(_loggers.begin(), _loggers.end(), logger) != _loggers.end())
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    _loggers.push_back(logger);

    return LLBC_OK;
}

void LLBC_LogRunnable::Stop()
{
    LLBC_ReturnIf(GetTaskState() == LLBC_TaskState::NotActivated, void());

    // Mask stopping and waiting for thread stopped.
    _stopping = true;
    LLBC_ReturnIf(Wait() == LLBC_OK, void());

    // If Wait() call failed, maybe call LogRunnable::Stop() in difference thread(eg: in crash hook),
    // in this case, force waiting for LogRunnable thread stopped.
    while (GetTaskState() != LLBC_TaskState::NotActivated)
        LLBC_Sleep(10);
}

void LLBC_LogRunnable::PushLogData(LLBC_LogData *logData)
{
    _logDataLock.Lock();
    _logDatas[0].push_back(logData);
    _logDataLock.Unlock();
}

void LLBC_LogRunnable::Cleanup()
{
    (void)TryPopAndProcLogDatas();

    FlushLoggers(true, 0);
    _loggers.clear();

    _stopping = false;
}

void LLBC_LogRunnable::Svc()
{
    while (LIKELY(!_stopping))
    {
        if (!TryPopAndProcLogDatas())
            LLBC_Sleep(1);

        FlushLoggers(false, LLBC_GetMilliSeconds());
    }
}

LLBC_FORCE_INLINE bool LLBC_LogRunnable::TryPopAndProcLogDatas()
{
    _logDataLock.Lock();
    std::swap(_logDatas[0], _logDatas[1]);
    _logDataLock.Unlock();

    auto &logDatas = _logDatas[1];
    if (logDatas.empty())
        return false;

    for (auto &logData : logDatas)
    {
        logData->logger->OutputLogData(*logData);
        LLBC_Recycle(logData);
    }

    logDatas.clear();

    return true;
}

LLBC_FORCE_INLINE void LLBC_LogRunnable::FlushLoggers(bool force, sint64 now)
{
    const size_t loggerCnt = _loggers.size();
    for (size_t i = 0; i < loggerCnt; ++i)
        _loggers[i]->Flush(false, now);
}

__LLBC_NS_END
