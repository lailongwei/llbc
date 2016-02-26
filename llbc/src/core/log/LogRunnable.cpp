/**
 * @file    LogRunnable.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/11
 * @version 1.0
 *
 * @brief
 */

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

    while (this->TryPop(block) == LLBC_OK)
    {
        block->Read(&logData, sizeof(LLBC_LogData *));

        this->Output(logData);
        this->FreeLogData(logData);

        delete block;
    }

    // Delete all appender.
    while (_head)
    {
        LLBC_ILogAppender *appender = _head;
        _head = _head->GetAppenderNext();

        delete appender;
    }
}

void LLBC_LogRunnable::Svc()
{
    LLBC_LogData *logData = NULL;
    LLBC_MessageBlock *block = NULL;
    while (LIKELY(!_stoped))
    {
        if (this->TimedPop(block, 50) != LLBC_OK)
            continue;

        block->Read(&logData, sizeof(LLBC_LogData *));

        this->Output(logData);
        this->FreeLogData(logData);

        delete block;

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

    delete data;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
