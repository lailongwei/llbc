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

#include "llbc/core/timer/BinaryHeap.h"

__LLBC_NS_BEGIN

class LLBC_Timer;
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

public:
    LLBC_TimerScheduler();
    virtual ~LLBC_TimerScheduler();

public:
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
     * Set friend class: LLBC_Timer.
     */
    friend class LLBC_Timer;

    /**
     * Get scheduler destroyed flag.
      *@return bool - destroyed flag.
     */
    bool IsDstroyed() const;

    /**
     * Schedule timer.
     * @param[in] timer   - timer object.
     * @param[in] dueTime - due time, in milli-seconds.
     * @param[in] period  - period time, in milli-seconds.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Schedule(LLBC_Timer *timer, sint64 dueTime, sint64 period);

    /**
     * Cancel timer.
     * @param[in] - timer object.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Cancel(LLBC_Timer *timer);

private:
    LLBC_DISABLE_ASSIGNMENT(LLBC_TimerScheduler);

private:
    LLBC_TimerId _maxTimerId;
    bool _enabled;
    bool _destroyed;

    _Heap _heap;
};

__LLBC_NS_END
