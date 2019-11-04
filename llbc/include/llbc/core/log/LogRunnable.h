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
    /**
     * Flush appenders.
     * @param[in] force - force flush or not, default is false.
     */
    void FlushAppenders(bool force = false);

private:
    volatile bool _stoped;
    LLBC_ILogAppender *_head;

    sint64 _lastFlushTime;
    sint64 _flushInterval;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOG_RUNNABLE_H__
