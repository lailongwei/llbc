--[[
@file   log.lua
@brief  The log logic implementation.
--]]

local Log = {}

-- Log levels enumeration.
Log.DEBUG = 0
Log.INFO = 1
Log.WARN = 2
Log.ERROR = 3
Log.FATAL = 4

-- logFileInfo: to control output message included file&line information or not, default is false.
Log.logFileInfo = false
-- defaultLogTag: use to fill <tag> param on output message, can set to string or function, default is nil.
Log.defaultLogTag = nil

-- Initialize Log component.
-- @param[required] cfgFile     - log init config file.
-- @param[optional] logFileinfo - log file info or not, default is false.
-- @returns - no return.
function Log.init(cfgFile, logFileInfo)
    _llbc.InitLog(cfgFile)
    if logFileInfo then
        Log.logFileInfo = true
    end
end

-- Log DEBUG level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.d(...)
    Log.output(0, nil, nil, ...)
end

-- Log DEBUG level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.d2(logger, ...)
    Log.output(0, logger, nil, ...)
end

-- Log DEBUG level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.d3(logger, tag, ...)
    Log.output(0, logger, tag, ...)
end

-- Log INFO level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.i(...)
    Log.output(1, nil, nil, ...)
end

-- Log INFO level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.i2(logger, ...)
    Log.output(1, logger, nil, ...)
end

-- Log INFO level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.i3(logger, tag, ...)
    Log.output(1, logger, tag, ...)
end

-- Log WARN level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.w(...)
    Log.output(2, nil, nil, ...)
end

-- Log WARN level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.w2(logger, ...)
    Log.output(2, logger, nil, ...)
end

-- Log WARN level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ... -              log message.
-- @returns - no return.
function Log.w3(logger, tag, ...)
    Log.output(2, logger, tag, ...)
end

-- Log ERROR level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.e(...)
    Log.output(3, nil, nil, ...)
end

-- Log ERROR level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.e2(logger, ...)
    Log.output(3, logger, nil, ...)
end

-- Log ERROR level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.e3(logger, tag, ...)
    Log.output(3, logger, tag, ...)
end

-- Log FATAL level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.f(...)
    Log.output(4, nil, nil, ...)
end

-- Log FATAL level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.f2(logger, ...)
    Log.output(4, logger, nil, ...)
end

-- Log FATAL level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.f3(logger, tag, ...)
    Log.output(4, logger, tag, ...)
end

-- Output message to logger.
-- @param level  - logger level.
-- @param logger - logger name, default is root logger.
-- @param tag    - log tag, default is nil.
-- @param ...    - log message.
-- @returns - no return.
function Log.output(level, logger, tag, ...)
    local file, line
    if Log.logFileInfo then
        local di = debug.getinfo(3, 'Sl')
        file, line = di.source, di.currentline
    end

    if tag == nil then
        local defaultLogTag = Log.defaultLogTag
        if defaultLogTag ~= nil then
            if type(defaultLogTag) == 'function' then
                tag = defaultLogTag()
            else
                tag = defaultLogTag
            end
        end
    end

    if level >= 3 then -- For improve Log performance, explicit use Log Level value to perform if condition judge.
        local logMsg = _llbc.Util_Table_Concat({...}, ' ')
        local logMsgAppendedTb = debug.traceback(logMsg, 3)
        _llbc.LogMsg(level, logger, tag, file, line, logMsgAppendedTb)
    else
        _llbc.LogMsg(level, logger, tag, file, line, ...)
    end
end

-- Set Log table to llbc
llbc.Log = Log