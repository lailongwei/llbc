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

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/ILogAppender.h"
#include "llbc/core/log/LogAppenderBuilder.h"
#include "llbc/core/log/LogRunnable.h"

__LLBC_NS_BEGIN

LLBC_LogRunnable::LLBC_LogRunnable()
: _stoped(false)
, _head(NULL)

, _lastFlushTime(0)
, _flushInterval(LLBC_CFG_LOG_DEFAULT_LOG_FLUSH_INTERVAL)
{
}

LLBC_LogRunnable::~LLBC_LogRunnable()
{
}

void LLBC_LogRunnable::Cleanup()
{
    // Flush all not process's message blocks.
    LLBC_LogData *logData = NULL;
    LLBC_MessageBlock *block = NULL;

    while (TryPop(block) == LLBC_OK)
    {
        block->Read(&logData, sizeof(LLBC_LogData *));

        Output(logData);
        FreeLogData(logData);

        LLBC_Delete(block);
    }

    // Delete all appender.
    while (_head)
    {
        LLBC_ILogAppender *appender = _head;
        _head = _head->GetAppenderNext();

        LLBC_Delete(appender);
    }
}

void LLBC_LogRunnable::Svc()
{
    LLBC_LogData *logData = NULL;
    LLBC_MessageBlock *block = NULL;
    while (LIKELY(!_stoped))
    {
        // Flush all appenders(cycle config: flushInterval)
        sint64 now = LLBC_GetMilliSeconds();
        sint64 diff = now - _lastFlushTime;
        if (diff < 0 || diff >= _flushInterval)
        {
            LLBC_ILogAppender *appender = _head;
            while (appender)
            {
                appender->Flush();
                appender = appender->GetAppenderNext();
            }

            _lastFlushTime = now;
        }

        // Try pop log message to output.
        if (TimedPop(block, 50) != LLBC_OK)
            continue;

        block->Read(&logData, sizeof(LLBC_LogData *));

        Output(logData);
        FreeLogData(logData);

        LLBC_Delete(block);

    }
}

void LLBC_LogRunnable::SetFlushInterval(sint64 flushInterval)
{
    _flushInterval = flushInterval;
}

void LLBC_LogRunnable::AddAppender(LLBC_ILogAppender *appender)
{
    appender->SetAppenderNext(NULL);
    if (!_head)
    {
        _head = appender;
        return;
    }

    LLBC_ILogAppender *tmpAppender = _head;
    while (tmpAppender->GetAppenderNext())
    {
        tmpAppender = tmpAppender->GetAppenderNext();
    }

    tmpAppender->SetAppenderNext(appender);
}

int LLBC_LogRunnable::Output(LLBC_LogData *data)
{
    LLBC_ILogAppender *appender = _head;
    if (!appender)
    {
        return LLBC_OK;
    }
    
    while (appender)
    {
        if (appender->Output(*data) != LLBC_OK)
        {
            return LLBC_FAILED;
        }

        appender = appender->GetAppenderNext();
    }

    return LLBC_OK;
}

void LLBC_LogRunnable::Stop()
{
    _stoped = true;
}

void LLBC_LogRunnable::FreeLogData(LLBC_LogData *data)
{
    LLBC_XFree(data->msg);
    LLBC_XFree(data->others);

    LLBC_Delete(data);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
