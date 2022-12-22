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
    lua_pushstring(l, LLBC_CFG_LOG_ROOT_LOGGER_NAME);
    lua_setfield(l, 1, "LLBC_CFG_LOG_ROOT_LOGGER_NAME");
    lua_pushstring(l, LLBC_CFG_LOG_DEFAULT_NOT_CONFIG_OPTION_USE);
    lua_setfield(l, 1, "LLBC_CFG_LOG_DEFAULT_NOT_CONFIG_OPTION_USE");
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
    lua_pushboolean(l, LLBC_CFG_LOG_DEFAULT_COLOURFUL_OUTPUT);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_COLOURFUL_OUTPUT");
    lua_pushboolean(l, LLBC_CFG_LOG_DEFAULT_LOG_TO_FILE);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_LOG_FILE_NAME");
    lua_pushboolean(l, LLBC_CFG_LOG_DEFAULT_FORCE_APP_LOG_PATH);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_FORCE_APP_LOG_PATH");
    lua_pushinteger(l, LLBC_CFG_LOG_DEFAULT_FILE_ROLLING_MODE);
    lua_setfield(l, 1, "CFG_LOG_DEFAULT_FILE_ROLLING_MODE");
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
    lua_pushboolean(l, LLBC_CFG_LOG_LAZY_CREATE_LOG_FILE);
    lua_setfield(l, 1, "CFG_LOG_LAZY_CREATE_LOG_FILE");

    // Timer
    lua_pushboolean(l, LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE);
    lua_setfield(l, 1, "CFG_CORE_TIMER_STRICT_SCHEDULE");

    // Push objbase module about config.
    lua_pushinteger(l, LLBC_CFG_OBJBASE_DICT_DFT_BUCKET_SIZE);
    lua_setfield(l, 1, "CFG_OBJBASE_DICT_DFT_BUCKET_SIZE");
    lua_pushstring(l, LLBC_ConcatStr("", LLBC_CFG_OBJBASE_DICT_KEY_HASH_ALGO));
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
    lua_pushinteger(l, LLBC_CFG_COMM_DFT_SOCK_SEND_BUF_SIZE);
    lua_setfield(l, 1, "CFG_COMM_DFT_SOCK_SEND_BUF_SIZE");
    lua_pushinteger(l, LLBC_CFG_COMM_DFT_SOCK_RECV_BUF_SIZE);
    lua_setfield(l, 1, "CFG_COMM_DFT_SOCK_RECV_BUF_SIZE");
    lua_pushinteger(l, LLBC_CFG_COMM_DFT_SESSION_SEND_BUF_SIZE);
    lua_setfield(l, 1, "CFG_COMM_DFT_SESSION_SEND_BUF_SIZE");
    lua_pushinteger(l, LLBC_CFG_COMM_DFT_SESSION_RECV_BUF_SIZE);
    lua_setfield(l, 1, "CFG_COMM_DFT_SESSION_RECV_BUF_SIZE");
    lua_pushinteger(l, LLBC_CFG_COMM_SESSION_RECV_BUF_USE_OBJ_POOL);
    lua_setfield(l, 1, "CFG_COMM_SESSION_RECV_BUF_USE_OBJ_POOL");
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
