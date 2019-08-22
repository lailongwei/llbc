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

static LLBC_Logger *__rootLogger = NULL;
static LLBC_LoggerManager *__loggerManager = NULL;

// API: InitLog
LULLBC_LUA_METH int _lullbc_InitLog(lua_State *l)
{
    const char *configFile = lua_tostring(l, 1);
    if (LLBC_LoggerManagerSingleton->Initialize(configFile) != LLBC_OK)
        lullbc_TransferLLBCError(l, __FILE__, __LINE__, "When initialize logger manager");

    __loggerManager = LLBC_LoggerManagerSingleton;
    __rootLogger = __loggerManager->GetRootLogger();
    return 0;
}

// API: IsLogInit
LULLBC_LUA_METH int _lullbc_IsLogInit(lua_State *l)
{
    lua_pushboolean(l, LLBC_LoggerManagerSingleton->IsInited() ? 1 : 0);
    return 1;
}

// API: LogMsg
LULLBC_LUA_METH int _lullbc_LogMsg(lua_State *l)
{
    // Params: level, logger, tag, file, line, ...<msg>(optional)
    int paramsCount = lua_gettop(l);
    if (paramsCount < 5)
        lullbc_SetError(l, "failed to log message, parameters count must be equal or greater than 5");

    // Concat messages to output.
    size_t msgSize;
    LLBC_String msg;
    const char *partMsg;
    for (int i = 6; i <= paramsCount; ++i)
    {
        partMsg = lua_tolstring(l, i, &msgSize);
        if (UNLIKELY(partMsg == NULL))
            luaL_error(l, "'tostring' must return a string to '_lullbc.LogMsg'");

        msg.append(partMsg, msgSize);
        if (i < paramsCount)
            msg.append(1, ' ');
    }

    // Append traceback.
    int level = lua_toint32(l, 1);
    if (level >= LLBC_LogLevel::Error)
    {
        luaL_traceback(l, l, NULL, 4);
        partMsg = lua_tolstring(l, paramsCount+1, &msgSize);
        msg.append(1, ' ');
        msg.append(partMsg, msgSize);
        lua_pop(l, 1);
    }

    // If Log not init, direct output to console.
    if (UNLIKELY(!__loggerManager))
    {
        fprintf(stdout, "[Log] %s\n", msg.c_str());
        fflush(stdout);
        return 0;
    }

    // Parse logger.
    LLBC_Logger *logger;
    const char *loggerName = lua_tostring(l, 2);
    if (UNLIKELY(loggerName == NULL))
        logger = __rootLogger;
    else
        logger = __loggerManager->GetLogger(loggerName);

    if (UNLIKELY(logger == NULL))
        lullbc_SetError(l, "failed to log message, logger[%s] not found", loggerName);

    // Parse tag, file, line, traceback
    const char *tag = lua_tostring(l, 3);
    const char *file = lua_tostring(l, 4);
    int line = lua_toint32(l, 5);

    // Output message.
    if (UNLIKELY(logger->OutputNonFormat(level, tag, file, line, msg.data(), msg.size()) != LLBC_OK))
        lullbc_TransferLLBCError(l, __FILE__, __LINE__, "failed to log message, Output call failed");

    return 0;
}

// API: GetLogLevel
LULLBC_LUA_METH int _lullbc_GetLogLevel(lua_State *l)
{
    LLBC_Logger *logger;
    const char *loggerName = lua_tostring(l, 1);
    if (UNLIKELY(loggerName == NULL))
        logger = __rootLogger;
    else
        logger = __loggerManager->GetLogger(loggerName);

    if (UNLIKELY(logger == NULL))
        lullbc_SetError(l, "failed to log message, logger[%s] not found", loggerName);

    lua_pushinteger(l, logger->GetLogLevel());
    return 1;
}
