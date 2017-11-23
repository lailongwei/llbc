/**
 * @file    TimerScheduler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/12/01
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_TIMER_TIMER_SCHEDULER_H__
#define __LLBC_CORE_TIMER_TIMER_SCHEDULER_H__

#include "llbc/common/Common.h"

#include "llbc/core/timer/BinaryHeap.h"

__LLBC_NS_BEGIN

class LLBC_BaseTimer;
struct LLBC_TimerData;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The timer scheduler class encapsulation.
 */
class LLBC_EXPORT LLBC_TimerScheduler
{
    typedef LLBC_TimerScheduler _This;

    typedef LLBC_BinaryHeap<LLBC_TimerData *> _Heap;
    typedef std::map<LLBC_TimerId, LLBC_TimerData *> _IdxMap;

public:
    LLBC_TimerScheduler();
    virtual ~LLBC_TimerScheduler();

public:
    /**
     * Create the entry thread(Call LLBC_Startup()/LLBC_Cleanup() methods' thread) scheduler.
     * Call by LLBC framework, do not try to call it.
     */
    static void CreateEntryThreadScheduler();

    /**
     * Destroy entry thread(Call LLBC_Startup()/LLBC_Cleanup() methods' thread) scheduler.
     * Call by LLBC framework, do not try to call it.
     */
    static void DestroyEntryThreadScheduler();

public:
    /**
     * Get entry thread(Call LLBC_Startup()/LLBC_Cleanup() methods' thread) scheduler.
     * @return _This * - scheduler pointer.
     * Note:
     *      If you use entry thread to schedule timer, you must call this method to fetch
     *      the entry thread scheduler and call Update() method to drive all timers.
     */
    static _This *GetEntryThreadScheduler();

    /**
     * Get current thread timer scheduler, thread must be LLBC style thread.
     * @return _This * - timer scheduler pointer.
     */
    static _This *GetCurrentThreadScheduler();

public:
    /**
     * Timer manager update drive function.
     */
    void Update();

    /**
     * Check timer scheduler is enabled or not.
     * @return bool - enable flag.
     */
    bool IsEnabled() const;

    /**
     * Set timer scheduler enabled flag.
     * @param[in] enabled - the enabled flag.
     */
    void SetEnabled(bool enabled);

    /**
     * Get timer count in this scheduler.
     * @return size_t - the timer count.
     */
    size_t GetTimerCount() const;

public:
    /**
     * Cancel all timers.
     */
    virtual void CancelAll();

private:
    /**
     * Set friend class: LLBC_BaseTimer.
     */
    friend class LLBC_BaseTimer;

    /**
     * Get scheduler destroyed flag.
      *@return bool - destroyed flag.
     */
    bool IsDstroyed() const;

    /**
     * Schedule timer.
     * @param[in] timer - timer object.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Schedule(LLBC_BaseTimer *timer);

    /**
     * Cancel timer.
     * @param[in] - timer object.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Cancel(LLBC_BaseTimer *timer);

private:
    LLBC_DISABLE_ASSIGNMENT(LLBC_TimerScheduler);

private:
    LLBC_TimerId _maxTimerId;
    bool _enabled;
    bool _destroyed;

    _Heap _heap;
    _IdxMap _idxMap;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_TIMER_TIMER_SCHEDULER_H__
