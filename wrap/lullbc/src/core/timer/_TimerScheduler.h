/**
 * @file    _TimerScheduler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-11-14
 * @version 1.0
 *
 * @brief
 */

// API: TimerScheduler_Update
LULLBC_LUA_METH int _lullbc_TimerScheduler_Update(lua_State *l)
{
    LLBC_TimerScheduler *timerScheduler = 
        lullbc_TimerScheduler::GetLLBCTimerScheduler(l);
    timerScheduler->Update();
    return 0;
}

// API: TimerScheduler_IsEnabled
LULLBC_LUA_METH int _lullbc_TimerScheduler_IsEnabled(lua_State *l)
{
    LLBC_TimerScheduler *timerScheduler = 
        lullbc_TimerScheduler::GetLLBCTimerScheduler(l);
    lua_pushboolean(l, static_cast<int>(timerScheduler->IsEnabled()));

    return 1;
}

// API: TimerScheduler_SetEnabled
LULLBC_LUA_METH int _lullbc_TimerScheduler_SetEnabled(lua_State *l)
{
    LLBC_TimerScheduler *timerScheduler = 
        lullbc_TimerScheduler::GetLLBCTimerScheduler(l);

    int enabledArgType = lua_type(l, 1);
    if (enabledArgType != LUA_TBOOLEAN && enabledArgType != LUA_TNUMBER)
        lullbc_SetError(l, 
            "Set timer scheduler enable/disable failed, argument #1 except boolean/number type, got %s", 
            lua_typename(l, enabledArgType));

    int enabled = 0;
    if (enabledArgType == LUA_TBOOLEAN)
        enabled = lua_toboolean(l, 1);
    else
        enabled = static_cast<int>(lua_tointeger(l, 1));

    timerScheduler->SetEnabled(enabled ? true : false);

    return 0;
}

// API: TimerScheduler_GetTimerCount
LULLBC_LUA_METH int _lullbc_TimerScheduler_GetTimerCount(lua_State *l)
{
    LLBC_TimerScheduler *timerScheduler = 
        lullbc_TimerScheduler::GetLLBCTimerScheduler(l);

    lua_pushinteger(l, static_cast<lua_Integer>(timerScheduler->GetTimerCount()));

    return 1;
}

// API: TimerScheduler_SetTimerErrorHandler
LULLBC_LUA_METH int _lullbc_TimerScheduler_SetTimerErrorHandler(lua_State *l)
{
    lullbc_ArgCheck(l, lua_isfunction(l, 1), 1, "Timer error handler must be function");
    if (lua_gettop(l) > 1)
        lullbc_SetError(l, "SetTimerErrorHandler() function not accept more than 1 arguments");

    lullbc_LuaHolder::Hold(l, 1, LULLBC_HOLD_KEY__TIMER_ERROR_HANDLER);

    return 0;
}
