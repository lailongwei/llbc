/**
 * @file    BaseTimer.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/12/1
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_TIMER_BASETIMER_H__
#define __LLBC_CORE_TIMER_BASETIMER_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

class LLBC_TimerScheduler;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The base timer object encapsulation.
 */
class LLBC_EXPORT LLBC_BaseTimer
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
    explicit LLBC_BaseTimer(Scheduler *scheduler = NULL);
    virtual ~LLBC_BaseTimer();

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
     * @return bool - return true means readd, otherwise means stop this timer.
     */
     virtual bool OnTimeout() = 0;

    /**
     * Timer cancel event handler.
     */
     virtual void OnCancel() = 0;

public:
    /**
     * Schedule timer.
     * @param[in] dueTime - due time, in milli-seconds.
     * @param[in] period  - period value, in milli-seconds.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Schedule(uint64 dueTime, uint64 period);

    /**
     * Cancel this timer.
     * @return int - return 0 if successed, otherwise return -1.
     */
    virtual int Cancel();

    /**
     * Check timer is the scheduling or not(this function not set last error).
     * @return bool - return true if is the scheduling, otherwise return false.
     */
    bool IsScheduling() const;

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

    /**
     * Set scheduling fla, call by timer scheduler.
     * @param[in] scheduling - scheduling flag.
     */
    void SetScheduling(bool scheduling);

    /**
     * Set timer Id.
     * @param[in] timerId - timer Id.
     */
    void SetTimerId(LLBC_TimerId timerId);

    LLBC_DISABLE_ASSIGNMENT(LLBC_BaseTimer);

private:
    LLBC_TimerId _timerId;

    uint64 _dueTime;
    uint64 _period;

    bool _scheduling;
    Scheduler *_scheduler;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_TIMER_BASETIMER_H__
