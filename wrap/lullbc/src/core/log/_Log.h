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

static LLBC_Logger *__rootLogger = nullptr;
static LLBC_LoggerMgr *__loggerManager = nullptr;

// API: InitLog
LULLBC_LUA_METH int _lullbc_InitLog(lua_State *l)
{
    const char *configFile = lua_tostring(l, 1);
    if (LLBC_LoggerMgrSingleton->Initialize(configFile) != LLBC_OK)
        lullbc_TransferLLBCError(l, __FILE__, __LINE__, "When initialize logger manager");

    __loggerManager = LLBC_LoggerMgrSingleton;
    __rootLogger = __loggerManager->GetRootLogger();
    return 0;
}

// API: UnInitLog
LULLBC_LUA_METH int _lullbc_UnInitLog(lua_State *l)
{
    if (!__loggerManager)
        return 0;

    __rootLogger = nullptr;
    __loggerManager = nullptr;
    LLBC_LoggerMgrSingleton->Finalize();

    return 0;
}

// API: IsLogInit
LULLBC_LUA_METH int _lullbc_IsLogInit(lua_State *l)
{
    lua_pushboolean(l, LLBC_LoggerMgrSingleton->IsInited() ? 1 : 0);
    return 1;
}

// API: LogMsg
LULLBC_LUA_METH int _lullbc_LogMsg(lua_State *l)
{
    // Params: level, logger, tag, file, line, func_name, ...<msg>(optional)
    int paramsCount = lua_gettop(l);
    if (paramsCount < 6)
        lullbc_SetError(l, "failed to log message, parameters count must be equal or greater than 5");

    // Get logger format buf.
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    char * const fmtBufBeg = libTls->coreTls.loggerFmtBuf;
    size_t availableFmtBufSize = sizeof(libTls->coreTls.loggerFmtBuf) - 1;

    // Concat messages to output.
    size_t partMsgSize;
    const char *partMsg;
    char *fmtBuf = fmtBufBeg;
    for (int i = 7; i <= paramsCount; ++i)
    {
        partMsg = luaL_tolstring(l, i, &partMsgSize);
        if (UNLIKELY(partMsg == nullptr))
            luaL_error(l, "'tostring' must return a string to '_lullbc.LogMsg'");

        const size_t copySize = MIN(partMsgSize, availableFmtBufSize);
        memcpy(fmtBuf, partMsg, copySize);
        fmtBuf += copySize;
        if (UNLIKELY((availableFmtBufSize -= copySize) == 0))
        {
            lua_pop(l, 1);
            break;
        }

        if (i < paramsCount &&
            LIKELY(availableFmtBufSize > 0))
        {
            *fmtBuf++ = ' ';
            if (UNLIKELY(--availableFmtBufSize == 0))
            {
                lua_pop(l, 1);
                break;
            }
        }

        lua_pop(l, 1);
    }

    // Append traceback.
    int level = lua_toint32(l, 1);
    if (level >= LLBC_LogLevel::Error &&
        LIKELY(availableFmtBufSize > 1))
    {
        *fmtBuf++ = '\n';
        --availableFmtBufSize;

        luaL_traceback(l, l, nullptr, 3);
        partMsg = lua_tolstring(l, paramsCount + 1, &partMsgSize);

        const size_t copySize = MIN(partMsgSize, availableFmtBufSize);
        memcpy(fmtBuf, partMsg, copySize);
        fmtBuf += copySize;
        availableFmtBufSize -= copySize;

        lua_pop(l, 1);
    }

    // If Log not init, direct output to console.
    if (UNLIKELY(!__loggerManager))
    {
        *fmtBuf = '\0';
        fprintf(stdout, "[Log] %s\n", fmtBufBeg);
        fflush(stdout);

        return 0;
    }

    // Finish log msg.
    *fmtBuf = '\0';
    const size_t msgSize = fmtBuf - fmtBufBeg;

    // Parse logger.
    LLBC_Logger *logger;
    const char *loggerName = lua_tostring(l, 2);
    if (UNLIKELY(loggerName == nullptr))
        logger = __rootLogger;
    else
        logger = __loggerManager->GetLogger(loggerName);

    if (UNLIKELY(logger == nullptr))
    {
        lullbc_SetError(l, "failed to log message, logger[%s] not found", loggerName);
        return 0;
    }

    // Parse tag, file, line, func, traceback
    const char *tag = lua_tostring(l, 3);
    const char *file = lua_tostring(l, 4);
    int line = lua_toint32(l, 5);
    const char *func = lua_tostring(l, 6);

    // Output message.
    if (UNLIKELY(logger->NonFormatOutput(level,
                                         tag,
                                         file,
                                         line,
                                         func,
                                         0,
                                         fmtBufBeg,
                                         msgSize) != LLBC_OK))
        lullbc_TransferLLBCError(l,
                                 __FILE__,
                                 __LINE__,
                                 "failed to log message, native Output() method call failed");

    return 0;
}

// API: GetLogLevel
LULLBC_LUA_METH int _lullbc_GetLogLevel(lua_State *l)
{
    LLBC_Logger *logger;
    const char *loggerName = lua_tostring(l, 1);
    if (UNLIKELY(loggerName == nullptr))
        logger = __rootLogger;
    else
        logger = __loggerManager->GetLogger(loggerName);

    if (UNLIKELY(logger == nullptr))
        lullbc_SetError(l, "failed to log message, logger[%s] not found", loggerName);

    lua_pushinteger(l, logger->GetLogLevel());
    return 1;
}
