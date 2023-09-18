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

#include "lullbc/core/timer/LuTimer.h"

static int __Del_LuTimer(lua_State *l)
{
    lullbc_Timer *timer = 
        reinterpret_cast<lullbc_Timer *>(lua_touserdata(l, 1));
    timer->~lullbc_Timer();

    return 0;
}

static inline lullbc_Timer *__Get_LuTimer(lua_State *l)
{
    lullbc_Timer *timer = 
        reinterpret_cast<lullbc_Timer *>(lua_touserdata(l, 1));
    if (UNLIKELY(timer == nullptr))
        lullbc_SetError(l, "argument error, except #1 is a c timer object");

    return timer;
}

// API: Timer_New
LULLBC_LUA_METH int _lullbc_Timer_New(lua_State *l)
{
    // Create userdata(sizeof(lullbc_Timer)), and set metatable: { __gc }
    void *timerBuf = lua_newuserdata(l, sizeof(lullbc_Timer));
    // Explicit call lullbc_Timer constructor
    new (timerBuf) lullbc_Timer(l);

    // Set metatable.
    if (UNLIKELY(luaL_newmetatable(l, "lullbc_Timer.__Del_LuTimer") == 1))
    {
        lua_pushcfunction(l, __Del_LuTimer);
        lua_setfield(l, -2, "__gc");
    }

    lua_setmetatable(l, -2);

    return 1;
}

// API: Timer_GetTimerId
LULLBC_LUA_METH int _lullbc_Timer_GetTimerId(lua_State *l)
{
    lullbc_Timer *timer = __Get_LuTimer(l);
    lua_pushinteger(l, static_cast<lua_Integer>(timer->GetTimerId()));

    return 1;
}

// API: Timer_IsScheduling
LULLBC_LUA_METH int _lullbc_Timer_IsScheduling(lua_State *l)
{
    lullbc_Timer *timer = __Get_LuTimer(l);
    lua_pushboolean(l, timer->IsScheduling());

    return 1;
}

// API: Timer_GetDueTime
LULLBC_LUA_METH int _lullbc_Timer_GetDueTime(lua_State *l)
{
    lullbc_Timer *timer = __Get_LuTimer(l);
    lua_pushnumber(l, timer->GetDueTime() / 1000.0);

    return 1;
}

// API: Timer_GetPeriod
LULLBC_LUA_METH int _lullbc_Timer_GetPeriod(lua_State *l)
{
    lullbc_Timer *timer = __Get_LuTimer(l);
    lua_pushnumber(l, timer->GetPeriod() / 1000.0);

    return 1;
}

// API: Timer_Schedule
LULLBC_LUA_METH int _lullbc_Timer_Schedule(lua_State *l)
{
    // Argument check
    // #-1: period, #-2: duetime
    if (UNLIKELY(!lua_isnumber(l, -2)))
        lullbc_SetError(l, "dueTime except number type");
    if (UNLIKELY(!lua_isnumber(l, -1)))
        lullbc_SetError(l, "period except number type");

    const LLBC_TimeSpan dueTime = LLBC_TimeSpan::FromMillis(
        static_cast<sint64>(static_cast<double>(lua_tonumber(l, -2)) * 1000));
    const LLBC_TimeSpan period = LLBC_TimeSpan::FromMillis(
        static_cast<sint64>(static_cast<double>(lua_tonumber(l, -1)) * 1000));

    // Schedule timer
    lullbc_Timer *timer = __Get_LuTimer(l);
    int ret = timer->Schedule(dueTime, period);
    if (ret != LLBC_OK)
        lullbc_TransferLLBCError(l, "when schedule timer");

    return 0;
}
