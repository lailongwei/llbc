/**
 * @file    LogRunnable.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/11
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_LOG_RUNNABLE_H__
#define __LLBC_CORE_LOG_LOG_RUNNABLE_H__

#include "llbc/common/Common.h"

#include "llbc/core/thread/Task.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
struct LLBC_LogData;
class LLBC_ILogAppender;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief Log Runnable class encapsulation.
 */
class LLBC_LogRunnable : public LLBC_BaseTask
{
public:
    LLBC_LogRunnable();
    virtual ~LLBC_LogRunnable();

public:
    /**
     * Cleanup method, when task terminated, will call this method.
     */
    virtual void Cleanup();

    /**
     * Task service routine.
     */
    virtual void Svc();

public:
    /**
     * Set runnable flush interval.
     */
    void SetFlushInterval(sint64 flushInterval);

public:
    /**
     * Add log appender.
     * @param[in] appender - log appender.
     */
    void AddAppender(LLBC_ILogAppender *appender);

    /**
     * Output log data.
     * @param[in] data - log data.
     */
    int Output(LLBC_LogData *data);

    /**
     * Stop log runnable, it just send stop signal to task, must call Wait() to real stop runnable.
     */
    void Stop();

public:
    /**
     * Free log data.
     * @param[in] data - the log data.
     */
    static void FreeLogData(LLBC_LogData *data);

private:
    volatile bool _stoped;
    LLBC_ILogAppender *_head;

    sint64 _lastFlushTime;
    sint64 _flushInterval;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOG_RUNNABLE_H__
