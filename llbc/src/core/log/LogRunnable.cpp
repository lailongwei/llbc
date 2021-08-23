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
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Time.h"
#include "llbc/core/thread/MessageBlock.h"
#include "llbc/core/objectpool/PoolObjectReflection.h"

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/Logger.h"
#include "llbc/core/log/ILogAppender.h"
#include "llbc/core/log/LogRunnable.h"

__LLBC_NS_BEGIN

LLBC_LogRunnable::LLBC_LogRunnable()
: _stoped(false)
{
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
    _stoped = true;
    Wait();
}

void LLBC_LogRunnable::Cleanup()
{
    while (TryPopAndProcLogData(10));

    FlushLoggers(true, 0);
    _loggers.clear();
}

void LLBC_LogRunnable::Svc()
{
    while (LIKELY(!_stoped))
    {
        if (!TryPopAndProcLogData(100))
            continue;

        FlushLoggers(false, LLBC_GetMilliSeconds());
    }
}

LLBC_FORCE_INLINE bool LLBC_LogRunnable::TryPopAndProcLogData(int maxPopWaitTime)
{
    LLBC_LogData *logData;
    LLBC_MessageBlock *block;
    if (TimedPop(block, maxPopWaitTime) != LLBC_OK)
        return false;

    if (UNLIKELY(block->Read(&logData, sizeof(LLBC_LogData *)) != LLBC_OK))
    {
        LLBC_Recycle(block);
        return false;
    }

    LLBC_Recycle(block);

    logData->logger->OutputLogData(*logData);
    LLBC_Recycle(logData);

    return true;
}

void LLBC_LogRunnable::FlushLoggers(bool force, sint64 now)
{
    size_t loggerCnt = _loggers.size();
    for (size_t i = 0; i < loggerCnt; ++i)
        _loggers[i]->Flush(false, now);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
