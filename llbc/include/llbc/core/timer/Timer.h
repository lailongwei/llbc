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

#pragma once

#include "llbc/core/time/Time.h"
#include "llbc/core/utils/Util_Delegate.h"

__LLBC_NS_BEGIN

struct LLBC_TimerData;
class LLBC_TimerScheduler;

class LLBC_Variant;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The timer encapsulation.
 */
class LLBC_EXPORT LLBC_Timer
{
    typedef LLBC_TimerScheduler Scheduler;

public:
    /**
     * Default constructor.
     */
    LLBC_Timer(): LLBC_Timer(nullptr, nullptr, nullptr) {  }

    /**
     * Construct from delegate(s) and scheduler.
     * @param[in] timeoutDeleg - the timeout handler delegate.
     * @param[in] cancelDeleg  - the cancel handler delegate(optional).
     * @param[in] scheduler    - timer scheduler, if set to nullptr, it means use default scheduler, 
     *                           otherwise use you specific timer scheduler.
     * Note:
     *          The default scheduler is means:
     *              In entry thread, use LLBC library Startup() API create's timer scheduler.
     *              In llbc service logic thread, use Service's timer scheduler.
     *              In other non-llbc library style thread, scheduler is nullptr.
     */
    explicit LLBC_Timer(const LLBC_Delegate<void(LLBC_Timer *)> &timeoutDeleg,
                        const LLBC_Delegate<void(LLBC_Timer *)> &cancelDeleg = nullptr,
                        Scheduler *scheduler = nullptr);
    virtual ~LLBC_Timer();

public:
    /**
     * Get due time.
     * @return LLBC_TimeSpan - due time.
     */
    LLBC_TimeSpan GetDueTime() const;

    /**
     * Get timer period.
     * @return LLBC_TimeSpan - timer period.
     */
    LLBC_TimeSpan GetPeriod() const;

    /**
     * Get timer Id.
     * @return LLBC_TimerId - timer Id.
     */
    LLBC_TimerId GetTimerId() const;

    /**
     * Get timeout times.
     * @return sint64 - timeout times.
     */
    sint64 GetTimeoutTimes() const;

public:
    /**
     * Get timeout handler.
     * @return LLBC_Delegate<void(LLBC_Timer *)> & - timeout handler.
     */
    const LLBC_Delegate<void(LLBC_Timer *)> &GetTimeoutHandler() const;

    /**
     * Set timeout handler.
     * @param[in] timeoutDeleg - the timeout delegate.
     */
    void SetTimeoutHandler(const LLBC_Delegate<void(LLBC_Timer *)> &timeoutDeleg);

    /**
     * @brief Set timeout handler(obj method).
     * @param obj    - the object.
     * @param method - the object method.
     */
    template <typename ObjType>
    void SetTimeoutHandler(ObjType *obj, void (ObjType::*method)(LLBC_Timer *));

    /**
     * Get cancel handler.
     * @return LLBC_Delegate<void(LLBC_Timer *)> & - cancel handler.
     */
     const LLBC_Delegate<void(LLBC_Timer *)> &GetCancelHandler() const;

    /**
     * Set cancel handler.
     * @param[in] cancelDeleg - the cancel delegate.
     */
    void SetCancelHandler(const LLBC_Delegate<void(LLBC_Timer *)> &cancelDeleg);

    /**
     * @brief Set cancel handler(obj method).
     * @param obj    - the object.
     * @param method - the object method.
     */
    template <typename ObjType>
    void SetCancelHandler(ObjType *obj, void (ObjType::*method)(LLBC_Timer *));

public:
    /**
     * Get timer data.
     * @return [const] LLBC_Variant & - the timer data.
     */
    LLBC_Variant &GetTimerData();
    const LLBC_Variant &GetTimerData() const;

public:
    /**
     * Timeout event handler.
     */
     virtual void OnTimeout();

    /**
     * Timer cancel event handler.
     */
     virtual void OnCancel();

public:
    /**
     * Schedule timer.
     * @param[in] firstTime - first timeout time, must be >= LLBC_Time::Now().
     * @param[in] period    - period time, if is zero, will use firstTime - LLBC_Time::Now().
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Schedule(const LLBC_Time &firstTime, const LLBC_TimeSpan &period = LLBC_TimeSpan::zero);

    /**
     * Schedule timer.
     * @param[in] firstPeriod - first period time.
     * @param[in] period      - period time, if is zero, wll use firstPeriod.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Schedule(const LLBC_TimeSpan &firstPeriod, const LLBC_TimeSpan &period = LLBC_TimeSpan::zero);

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
    virtual LLBC_String ToString() const;

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

    LLBC_Variant *_data;
    LLBC_Delegate<void(LLBC_Timer *)> _timeoutDeleg;
    LLBC_Delegate<void(LLBC_Timer *)> _cancelDeleg;
};

__LLBC_NS_END

#include "llbc/core/timer/TimerInl.h"


