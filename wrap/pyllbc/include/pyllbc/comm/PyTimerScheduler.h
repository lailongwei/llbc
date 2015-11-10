/**
 * @file    PyTimerScheduler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COMM_PY_TIMER_SCHEDULER_H__
#define __PYLLBC_COMM_PY_TIMER_SCHEDULER_H__

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

/**
 * \brief The python level timer scheduler encapsulation.
 */
class LLBC_HIDDEN pyllbc_TimerScheduler
{
public:
    pyllbc_TimerScheduler();
    ~pyllbc_TimerScheduler();

public:
    /**
     * Initialize the timer scheduler.
     */
    void Init();

    /**
     * Schedule all python level timers.
     */
    void Schedule();

private:
    LLBC_TimerScheduler *_scheduler;
};

#define pyllbc_s_TimerScheduler LLBC_Singleton<pyllbc_TimerScheduler, LLBC_DummyLock>::Instance()

#endif // !__PYLLBC_COMM_PY_TIMER_SCHEDULER_H__
