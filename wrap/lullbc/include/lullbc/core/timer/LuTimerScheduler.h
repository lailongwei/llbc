/**
 * @file    LuTimerScheduler.h
 * @author  Longwei lai<lailongwei@126.com>
 * @date    2017-11-20
 * @version 1.0
 *
 * @brief
 */
#ifndef __LULLBC_CORE_TIMER_LUTIMER_SCHEDULER_H__
#define __LULLBC_CORE_TIMER_LUTIMER_SCHEDULER_H__

#include "lullbc/common/Common.h"

class LULLBC_HIDDEN lullbc_TimerScheduler
{
public:
    static LLBC_TimerScheduler *GetLLBCTimerScheduler(lua_State *l);

private:
    LLBC_DISABLE_ASSIGNMENT(lullbc_TimerScheduler);
};

#endif // !__LULLBC_CORE_TIMER_LUTIMER_SCHEDULER_H__
