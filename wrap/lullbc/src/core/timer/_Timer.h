/**
 * @file    _Timer.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-11-14
 * @version 1.0
 *
 * @brief
 */

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
    if (UNLIKELY(timer == NULL))
        lullbc_SetError(l, "argument error, except #1 is a c timer object");

    return timer;
}

// API: Timer_New
LULLBC_LUA_METH int _lullbc_Timer_New(lua_State *l)
{
    // Create userdata(sizeof(lullbc_Timer)), and set metatable: { __gc }
    void *timerBuf = lua_newuserdata(l, sizeof(lullbc_Timer));
    // Explicit call lullbc_Timer constructor
    lullbc_Timer *timer = new (timerBuf) lullbc_Timer(l);

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

    uint64 dueTime = static_cast<uint64>(static_cast<double>(lua_tonumber(l, -2)) * 1000);
    uint64 period = static_cast<uint64>(static_cast<double>(lua_tonumber(l, -1)) * 1000);

    // Schedule timer
    lullbc_Timer *timer = __Get_LuTimer(l);
    int ret = timer->Schedule(dueTime, period);
    if (ret != LLBC_OK)
        lullbc_TransferLLBCError(l, "when schedule timer");

    return 0;
}
