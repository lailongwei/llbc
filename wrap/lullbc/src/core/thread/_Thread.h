/**
 * @file    _Thread.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-11-15
 * @version 1.0
 *
 * @brief
 */

// API: Thread_Sleep
LULLBC_LUA_METH int _lullbc_Thread_Sleep(lua_State *l)
{
    lullbc_ArgCheck(l, lua_isnumber(l, 1), 1, NULL);

    int milliSeconds = 
        static_cast<int>(static_cast<double>(lua_tonumber(l, 1)) * 1000);
    LLBC_Sleep(milliSeconds);

    return 0;
}
