/**
 * @file    _Log.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-07-16
 * @version 1.0
 *
 * @brief
 */

#include "lullbc/common/LibHeader.h"
#include "lullbc/common/Export.h"
#include "lullbc/common/Macro.h"
#include "lullbc/common/Errors.h"

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
    // Params: level, msg, logger(optional), tag(optional), file(optional), line(optional)
    int paramsCount = lua_gettop(l);
    if (lua_gettop(l) < 2)
        lullbc_SetError(l, "failed to log message, parameters count must be equal or greater than 2");

    int level = lua_toint32(l, 1);
    const char *msg = lua_tostring(l, 2);

    LLBC_Logger *logger = __rootLogger;
    if (paramsCount >= 3)
    {
        const char *loggerName = lua_tostring(l, 3);
        if (UNLIKELY(loggerName == NULL))
        {
            logger = __rootLogger;
        }
        else if (UNLIKELY((logger = LLBC_LoggerManagerSingleton->GetLogger(loggerName)) == NULL))
        {
            LLBC_String errMsg;
            errMsg.format("failed to log message, logger[%s] not found", loggerName);
            lullbc_SetError(l, errMsg);
        }
    }

    const char *tag = paramsCount >= 4 ? lua_tostring(l, 4) : NULL;

    const char *file = paramsCount >= 5 ? lua_tostring(l, 5) : NULL;
    int line = paramsCount >= 6 ? lua_toint32(l, 6) : 0;
    if (UNLIKELY(logger->Output(level, tag, file, line, "%s", msg) != LLBC_OK))
        lullbc_TransferLLBCError(l, "failed to log message, Output call failed");

    return 0;
}
