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
