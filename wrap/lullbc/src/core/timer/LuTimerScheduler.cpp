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
