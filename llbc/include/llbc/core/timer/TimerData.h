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
    sint64 handle;

    // Timer Id.
    LLBC_TimerId timerId;

    // First period.
    sint64 firstPeriod; 
    // Period.
    sint64 period;

    // Total trigger count.
    size_t totalTriggerCount;
    // Triggered count.
    size_t triggeredCount;

    // timer object.
    LLBC_Timer *timer;

    // timer status.
    union
    {
        uint8 statusVal;
        struct
        {
            // Scheduled status.
            uint8 isScheduled:1;
            // Handling timeout status.
            uint8 isHandlingTimeout:1;
            // Handling cancel status.
            uint8 isHandlingCancel:1;

            // Unused status.
            uint8 unused:5;
        } status;
    } unStatus;

    // ref count.
    uint8 refCount;

    // Unused bytes.
    uint16 unused1;
    uint32 unused2;
};

__LLBC_NS_END

namespace std
{

/**
 * \brief The std::less<T> template class specialization: std::less<LLBC_TimerData *>.
 */
template <>
struct less<LLBC_NS LLBC_TimerData *>
{
    bool operator()(LLBC_NS LLBC_TimerData * const &t1, LLBC_NS LLBC_TimerData * const &t2) const
    {
        return t1->handle < t2->handle;
    }
};

/**
 * \brief The std::greater<T> template class specialization: std::greater<LLBC_TimerData *>.
 */
template <>
struct greater<LLBC_NS LLBC_TimerData *>
{
    bool operator()(LLBC_NS LLBC_TimerData * const &t1, LLBC_NS LLBC_TimerData * const &t2) const
    {
        return t1->handle > t2->handle;
    }
};

}


