/**
 * @file    _Config.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017/07/16
 * @version 1.0
 *
 * @brief
 */

#include "lullbc/common/LibHeader.h"
#include "lullbc/common/Export.h"
#include "lullbc/common/Macro.h"
#include "lullbc/common/Errors.h"

// API: SetLibConfigToLua
LULLBC_LUA_METH int _lullbc_SetLibConfigToLua(lua_State *l)
{
    if (lua_gettop(l) < 1)
        lullbc_SetError(l, "failed to set library config to lua layer, table not pass in");
    else if (lua_type(l, 1) != LUA_TTABLE)
        lullbc_SetError(l, "failed to set library config to lua layer, pass library object not table type");

    // Push OS level config.
    // OS
    lua_pushinteger(l, LLBC_CFG_OS_DFT_BACKLOG_SIZE);
    lua_setfield(l, 1, "CFG_OS_DFT_BACKLOG_SIZE");

    // Push core module about config.
    // File
    lua_pushinteger(l, LLBC_CFG_CORE_FILE_COPY_BUF_SIZE);
    lua_setfield(l, 1, "CFG_CORE_FILE_COPY_BUF_SIZE");

    // Utils
    lua_pushboolean(l, LLBC_CFG_CORE_UTILS_IMPL__ITOA);
    lua_setfield(l, 1, "CFG_CORE_UTILS_IMPL__ITOA");
    lua_pushboolean(l, LLBC_CFG_CORE_UTILS_IMPL__I64TOA);
    lua_setfield(l, 1, "CFG_CORE_UTILS_IMPL__I64TOA");
    lua_pushboolean(l, LLBC_CFG_CORE_UTILS_IMPL__UI64TOA);
    lua_setfield(l, 1, "CFG_CORE_UTILS_IMPL__UI64TOA");

    // Thread
    lua_pushinteger(l, LLBC_CFG_THREAD_MAX_THREAD_NUM);
    lua_setfield(l, 1, "CFG_THREAD_MAX_THREAD_NUM");
    lua_pushinteger(l, LLBC_CFG_THREAD_DFT_STACK_SIZE);
    lua_setfield(l, 1, "CFG_THREAD_DFT_STACK_SIZE");
    lua_pushinteger(l, LLBC_CFG_THREAD_MINIMUM_STACK_SIZE);
    lua_setfield(l, 1, "CFG_THREAD_MINIMUM_STACK_SIZE");
    lua_pushinteger(l, LLBC_CFG_THREAD_MSG_BLOCK_DFT_SIZE);
    lua_setfield(l, 1, "CFG_THREAD_MSG_BLOCK_DFT_SIZE");

    // Log
    lua_pushinteger(l, LLBC_CFG_LOG_DEFAULT_LEVEL);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_LEVEL");
    lua_pushboolean(l, LLBC_CFG_LOG_DIRECT_FLUSH_TO_CONSOLE);
    lua_setfield(l, 1, "CFG_LOG_DIRECT_FLUSH_TO_CONSOLE");
    lua_pushboolean(l, LLBC_CFG_LOG_DEFAULT_ASYNC_MODE);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_ASYNC_MODE");
    lua_pushboolean(l, LLBC_CFG_LOG_DEFAULT_LOG_TO_CONSOLE);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_LOG_TO_CONSOLE");
    lua_pushstring(l, LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN");
    lua_pushboolean(l, LLBC_CFG_LOG_DEFAULT_ENABLED_COLOURFUL_OUTPUT);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_ENABLED_COLOURFUL_OUTPUT");
    lua_pushboolean(l, LLBC_CFG_LOG_DEFAULT_LOG_TO_FILE);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_LOG_TO_FILE");
    lua_pushstring(l, LLBC_CFG_LOG_DEFAULT_LOG_FILE_NAME);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_LOG_FILE_NAME");
    lua_pushboolean(l, LLBC_CFG_LOG_DEFAULT_FORCE_APP_LOG_PATH);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_FORCE_APP_LOG_PATH");
    lua_pushboolean(l, LLBC_CFG_LOG_DEFAULT_DAILY_MODE);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_DAILY_MODE");
    lua_pushinteger(l, LLBC_CFG_LOG_MAX_FILE_SIZE);
    lua_setfield(l, 1, "CFG_LOG_MAX_FILE_SIZE");
    lua_pushinteger(l, LLBC_CFG_LOG_MAX_BACKUP_INDEX);
    lua_setfield(l, 1, "CFG_LOG_MAX_BACKUP_INDEX");
    lua_pushinteger(l, LLBC_CFG_LOG_DEFAULT_LOG_FILE_BUFFER_SIZE);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_LOG_FILE_BUFFER_SIZE");
    lua_pushinteger(l, LLBC_CFG_LOG_DEFAULT_LOG_FLUSH_INTERVAL);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_LOG_FLUSH_INTERVAL");
    lua_pushinteger(l, LLBC_CFG_LOG_MAX_LOG_FLUSH_INTERVAL);
    lua_setfield(l, 1, "CFG_LOG_MAX_LOG_FLUSH_INTERVAL");
    lua_pushboolean(l, LLBC_CFG_LOG_USING_WITH_STREAM);
    lua_setfield(l, 1, "CFG_LOG_USING_WITH_STREAM");
    lua_pushboolean(l, LLBC_CFG_LOG_ROOT_LOGGER_TAKE_OVER_UNCONFIGED);
    lua_setfield(l, 1, "CFG_LOG_ROOT_LOGGER_TAKE_OVER_UNCONFIGED");

    // Timer
    lua_pushboolean(l, LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE);
    lua_setfield(l, 1, "CFG_CORE_TIMER_STRICT_SCHEDULE");

    // Push objbase module about config.
    lua_pushboolean(l, LLBC_CFG_OBJBASE_ENABLED);
    lua_setfield(l, 1, "CFG_OBJBASE_ENABLED");
    lua_pushinteger(l, LLBC_CFG_OBJBASE_DICT_DFT_BUCKET_SIZE);
    lua_setfield(l, 1, "CFG_OBJBASE_DICT_DFT_BUCKET_SIZE");
    lua_pushstring(l, LLBC_CFG_OBJBASE_DICT_KEY_HASH_ALGO);
    lua_setfield(l, 1, "CFG_OBJBASE_DICT_KEY_HASH_ALGO");

    // Push communication module about config.
    lua_pushboolean(l, LLBC_CFG_COMM_ORDER_IS_NET_ORDER);
    lua_setfield(l, 1, "CFG_COMM_ORDER_IS_NET_ORDER");
    lua_pushinteger(l, LLBC_CFG_COMM_DFT_CONN_TIMEOUT);
    lua_setfield(l, 1, "CFG_COMM_DFT_CONN_TIMEOUT");
    lua_pushinteger(l, LLBC_CFG_COMM_MAX_EVENT_COUNT);
    lua_setfield(l, 1, "CFG_COMM_MAX_EVENT_COUNT");
    lua_pushinteger(l, LLBC_CFG_EPOLL_MAX_LISTEN_FD_SIZE);
    lua_setfield(l, 1, "CFG_EPOLL_MAX_LISTEN_FD_SIZE");
    lua_pushinteger(l, LLBC_CFG_COMM_DFT_SEND_BUF_SIZE);
    lua_setfield(l, 1, "CFG_COMM_DFT_SEND_BUF_SIZE");
    lua_pushinteger(l, LLBC_CFG_COMM_DFT_RECV_BUF_SIZE);
    lua_setfield(l, 1, "CFG_COMM_DFT_RECV_BUF_SIZE");
    lua_pushinteger(l, LLBC_CFG_COMM_DFT_SERVICE_FPS);
    lua_setfield(l, 1, "CFG_COMM_DFT_SERVICE_FPS");
    lua_pushinteger(l, LLBC_CFG_COMM_MIN_SERVICE_FPS);
    lua_setfield(l, 1, "CFG_COMM_MIN_SERVICE_FPS");
    lua_pushinteger(l, LLBC_CFG_COMM_MAX_SERVICE_FPS);
    lua_setfield(l, 1, "CFG_COMM_MAX_SERVICE_FPS");
    lua_pushboolean(l, LLBC_CFG_COMM_ENABLE_SAMPLER_SUPPORT);
    lua_setfield(l, 1, "CFG_COMM_ENABLE_SAMPLER_SUPPORT");
    lua_pushinteger(l, LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT);
    lua_setfield(l, 1, "CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT");
    lua_pushboolean(l, LLBC_CFG_COMM_USE_FULL_STACK);
    lua_setfield(l, 1, "CFG_COMM_USE_FULL_STACK");
    lua_pushboolean(l, LLBC_CFG_COMM_ENABLE_STATUS_HANDLER);
    lua_setfield(l, 1, "CFG_COMM_ENABLE_STATUS_HANDLER");
    lua_pushboolean(l, LLBC_CFG_COMM_ENABLE_STATUS_DESC);
    lua_setfield(l, 1, "CFG_COMM_ENABLE_STATUS_DESC");
    lua_pushboolean(l, LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE);
    lua_setfield(l, 1, "CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE");
    lua_pushstring(l, LLBC_CFG_COMM_POLLER_MODEL);
    lua_setfield(l, 1, "CFG_COMM_POLLER_MODEL");

    return 0;
}
