/**
 * @file    TimerData.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/12/01
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_TIMER_TIMER_DATA_H__
#define __LLBC_CORE_TIMER_TIMER_DATA_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

class LLBC_Timer;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The timer data structure encapsulation.
 */
struct LLBC_HIDDEN LLBC_TimerData
{
    // Timer handle, use to build timer heap.
    uint64 handle;

    // Timer Id.
    LLBC_TimerId timerId;

    // Due time.
    uint64 dueTime; 
    // Period.
    uint64 period;

    // Repeat times.
    uint64 repeatTimes;

    // timer object.
    LLBC_Timer *timer;

    // Validate flag.
    bool validate;
    // Cancelling flag.
    bool cancelling;
    // Timeouting flag.
    bool timeouting;

    // ref count.
    uint8 refCount;
};

__LLBC_NS_END

namespace std
{

/**
 * \brief The std::less<T> template class specialization: std::less<LLBC_TimerData *>.
 */
template <>
struct less<LLBC_NS LLBC_TimerData *> : public binary_function<LLBC_NS LLBC_TimerData *, LLBC_NS LLBC_TimerData *, bool>
{
    bool operator()(LLBC_NS LLBC_TimerData * const &t1, LLBC_NS LLBC_TimerData * const &t2)
    {
        return t1->handle < t2->handle;
    }
};

/**
 * \brief The std::greater<T> template class specialization: std::greater<LLBC_TimerData *>.
 */
template <>
struct greater<LLBC_NS LLBC_TimerData *> : public binary_function<LLBC_NS LLBC_TimerData *, LLBC_NS LLBC_TimerData *, bool>
{
    bool operator()(LLBC_NS LLBC_TimerData * const &t1, LLBC_NS LLBC_TimerData * const &t2)
    {
        return t1->handle > t2->handle;
    }
};

}

#endif // !__LLBC_CORE_TIMER_TIMER_DATA_H__
