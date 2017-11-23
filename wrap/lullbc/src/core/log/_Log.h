/**
 * @file    _Log.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-07-16
 * @version 1.0
 *
 * @brief
 */

static LLBC_Logger *__rootLogger = NULL;

// API: InitLog
LULLBC_LUA_METH int _lullbc_InitLog(lua_State *l)
{
    const char *configFile = lua_tostring(l, 1);
    if (LLBC_LoggerManagerSingleton->Initialize(configFile) != LLBC_OK)
        lullbc_TransferLLBCError(l, __FILE__, __LINE__, "When initialize logger manager");

    __rootLogger = LLBC_LoggerManagerSingleton->GetRootLogger();
    return 0;
}

// API: LogMsg
LULLBC_LUA_METH int _lullbc_LogMsg(lua_State *l)
{
    // Params: level, logger, tag, file, line, ...<msg>(optional)
    int paramsCount = lua_gettop(l);
    if (lua_gettop(l) < 5)
        lullbc_SetError(l, "failed to log message, parameters count must be equal or greater than 5");

    // Parse level, logger.
    int level = lua_toint32(l, 1);
    LLBC_Logger *logger = __rootLogger;
    if (paramsCount >= 2)
    {
        const char *loggerName = lua_tostring(l, 2);
        if (UNLIKELY(loggerName == NULL))
            logger = __rootLogger;
        else if (UNLIKELY((logger = LLBC_LoggerManagerSingleton->GetLogger(loggerName)) == NULL))
            lullbc_SetError(l, "failed to log message, logger[%s] not found", loggerName);
    }

    // Parse tag, file, line
    const char *tag = paramsCount >= 3 ? lua_tostring(l, 3) : NULL;
    const char *file = paramsCount >= 4 ? lua_tostring(l, 4) : NULL;
    int line = paramsCount >= 5 ? lua_toint32(l, 5) : 0;

    // Concat messages to output.
    size_t msgSize;
    LLBC_String msg;
    const char *partMsg;
    for (register int i = 6; i <= paramsCount; i++)
    {
        partMsg = luaL_tolstring(l, i, &msgSize);
        if (UNLIKELY(partMsg == NULL))
            luaL_error(l, "'tostring' must return a string to '_lullbc.LogMsg'");

        msg.append(partMsg, msgSize);
        if (i < paramsCount)
            msg.append(1, ' ');

        lua_pop(l, 1);
    }

    if (UNLIKELY(logger->OutputNonFormat(level, tag, file, line, msg.data(), msg.size()) != LLBC_OK))
        lullbc_TransferLLBCError(l, __FILE__, __LINE__, "failed to log message, Output call failed");

    return 0;
}
