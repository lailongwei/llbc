/**
 * @file    csTimer.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/06/29
 * @version 1.0
 *
 * @brief   The csharp timer class encapsulation.
 */
#ifndef __CSLLBC_COMM_CSTIMER_H__
#define __CSLLBC_COMM_CSTIMER_H__

#include "csllbc/common/Common.h"
#include "csllbc/core/Core.h"

/**
 * \brief The csharp timer class encapsulation.
 */
class CSLLBC_EXPORT csllbc_Timer : public LLBC_Timer
{
    /**
     * typedef some timer about delegate types.
     */
    typedef csllbc_Delegates _D;
    typedef _D::Deleg_Timer_OnTimeout _TimeoutDeleg;
    typedef _D::Deleg_Timer_OnCancel _CancelDeleg;

public:
    csllbc_Timer(_TimeoutDeleg timeoutDeleg, _CancelDeleg cancelDeleg);
    virtual ~csllbc_Timer();

public:
    /**
     * Timeout event handler.
     * @return bool - return true means continuing schedule, otherwise means stop this timer.
     */
    virtual void OnTimeout();

    /**
     * Timer cancel event handler.
     */
    virtual void OnCancel();

private:
    _TimeoutDeleg _timeoutDeleg;
    _CancelDeleg _cancelDeleg;
};

#endif // !__CSLLBC_COMM_CSTIMER_H__