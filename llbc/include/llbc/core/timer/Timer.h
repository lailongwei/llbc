/**
 * @file    Timer.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/12/1
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_TIMER_TIMER_H__
#define __LLBC_CORE_TIMER_TIMER_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

struct LLBC_TimerData;
class LLBC_TimerScheduler;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The base timer object encapsulation.
 */
class LLBC_EXPORT LLBC_Timer
{
    typedef LLBC_TimerScheduler Scheduler;

public:
    /**
     * Constructor.
     * @param[in] scheduler - timer scheduler, if set to NULL, it means use default scheduler, 
     *                        otherwise use you specific timer scheduler.
     * Note:
     *          The default scheduler is means:
     *              In entry thread, use LLBC library Startup() API create's timer scheduler.
     *              In llbc service logic thread, use Service's timer scheduler.
     *              In other non-llbc library style thread, scheduler is NULL.
     */
    explicit LLBC_Timer(Scheduler *scheduler = NULL);
    virtual ~LLBC_Timer();

public:
    /**
     * Get due time.
     * @return uint64 - due time.
     */
    uint64 GetDueTime() const;

    /**
     * Get timer period.
     * @return uint64 - timer period.
     */
    uint64 GetPeriod() const;

    /**
     * Get timer Id.
     * @return LLBC_TimerId - timer Id.
     */
    LLBC_TimerId GetTimerId() const;

public:
    /**
     * Timeout event handler.
     */
     virtual void OnTimeout() = 0;

    /**
     * Timer cancel event handler.
     */
     virtual void OnCancel();

public:
    /**
     * Schedule timer.
     * @param[in] dueTime - due time, in milli-seconds.
     * @param[in] period  - period value, in milli-seconds, if is zero, will use dueTime to schedule.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Schedule(uint64 dueTime, uint64 period = 0);

    /**
     * Cancel this timer.
     * @return int - return 0 if succeed, otherwise return -1.
     */
    virtual int Cancel();

    /**
     * Check timer is the scheduling or not(this function not set last error).
     * @return bool - return true if is scheduling, otherwise return false.
     */
    bool IsScheduling() const;

    /**
     * Check timer is timeouting or not(this function not set last error).
     * @return bool - return true if is timeouting, otherwise return false.
     */
    bool IsTimeouting() const;

    /**
     * Check timer is cancelling or not(this function not set last error).
     * @return bool - return true if is canceling, otherwise return false.
     */
    bool IsCancelling() const;

public:
    /**
     * Get timer string representation.
     * @return LLBC_String - the timer string representation.
     */
    LLBC_String ToString() const;

private:
    /**
     * Friend class: LLBC_TimerScheduler.
     */
    friend class LLBC_TimerScheduler;

private:
    LLBC_DISABLE_ASSIGNMENT(LLBC_Timer);

private:
    Scheduler *_scheduler;
    LLBC_TimerData *_timerData;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_TIMER_TIMER_H__
