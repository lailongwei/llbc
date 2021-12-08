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

#include "lullbc/core/timer/LuTimer.h"
#include "lullbc/core/timer/LuTimerScheduler.h"

static const int __TimerKey_LuaTimer = 1;
static const int __TimerKey_Timeout= 2;

static int __Timer_PCallErrorHandler(lua_State *l)
{
    size_t errMsgLen;
    const char *errMsg = luaL_tolstring(l, 1, &errMsgLen);

    luaL_traceback(l, l, nullptr, 2);
    size_t tbInfoLen;
    const char *tbInfo = luaL_tolstring(l, -1, &tbInfoLen);

    LLBC_String tbAppendedErrMsg;
    tbAppendedErrMsg.append(errMsg, errMsgLen);
    tbAppendedErrMsg.append("\n");
    tbAppendedErrMsg.append(tbInfo, tbInfoLen);

    lua_remove(l, -1);
    lua_pushlstring(l, tbAppendedErrMsg.data(), tbAppendedErrMsg.size());

    return 1;
}

lullbc_Timer::lullbc_Timer(lua_State *l)
: LLBC_Timer(nullptr, nullptr, lullbc_TimerScheduler::GetLLBCTimerScheduler(l))
, _luaState(l)
, _callableInfo(0)
{
    // Check argument:
    //   # 1[required] - lua timer object
    //   # 2[required] - timeout callable
    if (lua_gettop(l) < 2)
        lullbc_SetError(l, 
            "Failed to create native Timer object, argument count[%d] unmatched, required 2", lua_gettop(l));

    lullbc_ArgCheck(l, lua_istable(l, 1), -2, "invalidate timer object");
    lullbc_ArgCheck(l, lua_isfunction(l, 2), -1, "invalidate timeout callable object");

    // Create timer info table
    lua_newtable(l);
    // Push lua timer object.
    lua_pushinteger(l, __TimerKey_LuaTimer);
    lua_pushvalue(l, 1);
    lua_settable(l, -3);

    // Create metatable: { __mode = "v" }
    lua_newtable(l);
    lua_pushstring(l, "v");
    lua_setfield(l, -2, "__mode");
    // Make callable info table value weakref
    lua_setmetatable(l, -2);

    // Push timeout callable
    lua_pushinteger(l, __TimerKey_Timeout);
    lua_pushvalue(l, 2);
    lua_settable(l, -3);
    
    // Push timer info table to registry-index table
    _callableInfo = luaL_ref(l, LUA_REGISTRYINDEX);
}

lullbc_Timer::~lullbc_Timer()
{
    Cancel();

    luaL_unref(_luaState, LUA_REGISTRYINDEX, _callableInfo);
    _luaState = 0;
    _callableInfo = 0;
}

void lullbc_Timer::OnTimeout()
{
    // Push pcall error handler.
    int errHandlerPos = PushPCallErrorHandler();

    // Push timer object and timeout callable.
    if (UNLIKELY(!PushCallable()))
    {
        Cancel();
        return;
    }

    // Call.
    int callRet = lua_pcall(_luaState, 1, 0, errHandlerPos);
    if (UNLIKELY(callRet != LUA_OK))
    {
        HandleTimerError("timeout");
        if (errHandlerPos != 0) // If has pcall error handler, remove it.
            lua_remove(_luaState, errHandlerPos);

        Cancel();
        return;
    }

    // Reset lua_stack
    if (errHandlerPos != 0)
        lua_remove(_luaState, errHandlerPos);
}

void lullbc_Timer::OnCancel()
{
    // Do nothing.
}

bool lullbc_Timer::PushCallable()
{
    // Get timer info.
    lua_pushinteger(_luaState, _callableInfo);
    lua_gettable(_luaState, LUA_REGISTRYINDEX);
    if (UNLIKELY(!lua_istable(_luaState, -1)))
    {
#if LLBC_DEBUG
        trace("lullbc library internal error: timer info table not found");
#endif
        lua_remove(_luaState, -1);
        return false;
    }

    // Push timer callable.
    lua_pushinteger(_luaState, __TimerKey_Timeout);
    lua_gettable(_luaState, -2);
    if (UNLIKELY(!lua_isfunction(_luaState, -1))) // Check callable destroyed or not, if destroyed, return false.
    {
        // Failed, pop timer info table, invalid callable object.
        lua_settop(_luaState, lua_gettop(_luaState) - 2);
        return false;
    }

    // Push timer object
    lua_pushinteger(_luaState, __TimerKey_LuaTimer);
    lua_gettable(_luaState, -3);
    if (UNLIKELY(!lua_istable(_luaState, -1))) // Check timer object destroyed or not, if destroyed, return false.
    {
        // Failed, pop timer info table, timeout callable, invalid timer object.
        lua_settop(_luaState, lua_gettop(_luaState) - 3);
        return false;
    }

    // Remove timer info from stack.
    lua_remove(_luaState, -3);
    
    return true;
}

int lullbc_Timer::PushPCallErrorHandler()
{
    lua_pushcfunction(_luaState, __Timer_PCallErrorHandler);
    return lua_gettop(_luaState);
}

void lullbc_Timer::HandleTimerError(const char *eventType)
{
    // Get timer error handler.
    lullbc_LuaHolder::Get(_luaState, LULLBC_HOLD_KEY__TIMER_ERROR_HANDLER);
    if (lua_isnil(_luaState, -1))
    {
        lua_settop(_luaState, lua_gettop(_luaState) - 2); // Remove invalid invalid timer error handler, error msg.
        return;
    }

    // Push timer object.
    lua_pushinteger(_luaState, _callableInfo);
    lua_gettable(_luaState, LUA_REGISTRYINDEX);
    lua_pushinteger(_luaState, __TimerKey_LuaTimer);
    lua_gettable(_luaState, -2);
    if (UNLIKELY(!lua_istable(_luaState, -1))) // Check timer object destroyed or not, if destroyed, stop handle.
    {
        lua_settop(_luaState, lua_gettop(_luaState) - 4);
        return;
    }
    lua_remove(_luaState, -2);

    // Push event type.
    lua_pushstring(_luaState, eventType);

    // Swap error message to the stack top.
    lua_pushvalue(_luaState, -4);
    lua_remove(_luaState, -5);

    // Do pcall
    if (lua_pcall(_luaState, 3, 0, 0) != LLBC_OK)
        lua_remove(_luaState, -1);
}
