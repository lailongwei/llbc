/**
 * @file    LuTimerScheduler.cpp
 * @author  Longwei lai<lailongwei@126.com>
 * @date    2017-11-20
 * @version 1.0
 *
 * @brief
 */

#include "lullbc/common/Export.h"

#include "lullbc/core/timer/LuTimerScheduler.h"

namespace
{
    static inline int __Del_TimerScheduler(lua_State *l)
    {
        LLBC_TimerScheduler *timerScheduler = 
            reinterpret_cast<LLBC_TimerScheduler *>(lua_touserdata(l, -1));
        timerScheduler->~LLBC_TimerScheduler();

        return 0;
    }
};

LLBC_TimerScheduler *lullbc_TimerScheduler::GetLLBCTimerScheduler(lua_State *l)
{
    LLBC_TimerScheduler *timerScheduler;
    lullbc_LuaHolder::Get(l, LULLBC_HOLD_KEY__TIMER_SCHEDULER);
    if (UNLIKELY(!lua_isuserdata(l, -1)))
    {
        lua_remove(l, -1);

        timerScheduler = reinterpret_cast<
            LLBC_TimerScheduler *>(lua_newuserdata(l, sizeof(LLBC_TimerScheduler)));
        new (timerScheduler) LLBC_TimerScheduler();
        if (UNLIKELY(luaL_newmetatable(l, "lullbc_TimerScheduler.__Del_TimerScheduler") == 1))
        {
            lua_pushcfunction(l, __Del_TimerScheduler);
            lua_setfield(l, -2, "__gc");
        }

        lua_setmetatable(l, -2);

        lullbc_LuaHolder::Hold(l, -1, LULLBC_HOLD_KEY__TIMER_SCHEDULER, true);
    }
    else
    {
        timerScheduler = reinterpret_cast<
            LLBC_TimerScheduler *>(lua_touserdata(l, -1));
        lua_remove(l, -1);
    }

    return timerScheduler;
}
