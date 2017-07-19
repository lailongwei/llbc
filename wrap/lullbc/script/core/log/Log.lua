--[[
@file   Log.lua
@brief  The log logic implementation.
--]]

local Log = {}

-- Log levels enumeration.
Log.DEBUG = 0
Log.INFO = 1
Log.WARN = 2
Log.ERROR = 3
Log.FATAL = 4

-- Define logFileInfo to control output message included file&line information or not, default is false.
Log.logFileInfo = false

-- Initialize Log component.
-- :param[required] cfgFile:     log init config file.
-- :param[optional] logFileinfo: log file info or not, default is false.
-- :returns: no return.
function Log.Init(cfgFile, logFileInfo)
    _llbc.InitLog(cfgFile)
    if logFileInfo then
        Log.logFileInfo = true
    end
end

-- Log DEBUG level message to root logger.
-- :param ...: log message.
-- :returns: no return.
function Log.d(...)
    Log.Output(0, nil, nil, ...)
end

-- Log DEBUG level message to specific logger.
-- :param[optional] logger: logger name, default is root logger.
-- :param ...:              log message.
-- :returns: no return.
function Log.d2(logger, ...)
    Log.Output(0, logger, nil, ...)
end

-- Log DEBUG level message to specific logger, and append tag.
-- :param[optional] logger: logger name, default is root logger.
-- :param[optional] tag:    log tag, default is nil.
-- :param ...:              log message.
-- :returns: no return.
function Log.d3(logger, tag, ...)
    Log.Output(0, logger, tag, ...)
end

-- Log INFO level message to root logger.
-- :param ...: log message.
-- :returns: no return.
function Log.i(...)
    Log.Output(1, nil, nil, ...)
end

-- Log INFO level message to specific logger.
-- :param[optional] logger: logger name, default is root logger.
-- :param ...:              log message.
-- :returns: no return.
function Log.i2(logger, ...)
    Log.Output(1, logger, nil, ...)
end

-- Log INFO level message to specific logger, and append tag.
-- :param[optional] logger: logger name, default is root logger.
-- :param[optional] tag:    log tag, default is nil.
-- :param ...:              log message.
-- :returns: no return.
function Log.i3(logger, tag, ...)
    Log.Output(1, logger, tag, ...)
end

-- Log WARN level message to root logger.
-- :param ...: log message.
-- :returns: no return.
function Log.w(...)
    Log.Output(2, nil, nil, ...)
end

-- Log WARN level message to specific logger.
-- :param[optional] logger: logger name, default is root logger.
-- :param ...:              log message.
-- :returns: no return.
function Log.w2(logger, ...)
    Log.Output(2, logger, nil, ...)
end

-- Log WARN level message to specific logger.
-- :param[optional] logger: logger name, default is root logger.
-- :param[optional] tag:    log tag, default is nil.
-- :param ...:              log message.
-- :returns: no return.
function Log.w3(logger, tag, ...)
    Log.Output(2, logger, tag, ...)
end

-- Log ERROR level message to root logger.
-- :param ...: log message.
-- :returns: no return.
function Log.e(...)
    Log.Output(3, nil, nil, ...)
end

-- Log ERROR level message to specific logger.
-- :param[optional] logger: logger name, default is root logger.
-- :param ...:              log message.
-- :returns: no return.
function Log.e2(logger, ...)
    Log.Output(3, logger, nil, ...)
end

-- Log ERROR level message to specific logger, and append tag.
-- :param[optional] logger: logger name, default is root logger.
-- :param[optional] tag:    log tag, default is nil.
-- :param ...:              log message.
-- :returns: no return.
function Log.e3(logger, tag, ...)
    Log.Output(3, logger, tag, ...)
end

-- Log FATAL level message to root logger.
-- :param ...: log message.
-- :returns: no return.
function Log.f(...)
    Log.Output(4, nil, nil, ...)
end

-- Log FATAL level message to specific logger.
-- :param[optional] logger: logger name, default is root logger.
-- :param ...:              log message.
-- :returns: no return.
function Log.f2(logger, ...)
    Log.Output(4, logger, nil, ...)
end

-- Log FATAL level message to specific logger, and append tag.
-- :param[optional] logger: logger name, default is root logger.
-- :param[optional] tag:    log tag, default is nil.
-- :param ...:              log message.
-- :returns: no return.
function Log.f3(logger, tag, ...)
    Log.Output(4, logger, tag, ...)
end

-- Output message to logger.
-- :param level:  logger level.
-- :param logger: logger name, default is root logger.
-- :param tag:    log tag, default is nil.
-- :param ...:    log message.
-- :returns: no return.
function Log.Output(level, logger, tag, ...)
    msg = table.concat({...}, ' ')
    local file, line
    if Log.logFileInfo then
        local di = debug.getinfo(3, 'Sl')
        file, line = di.source, di.currentline
    end

    _llbc.LogMsg(level, msg, logger, tag, file, line)
end

-- Set Log table to llbc
llbc.Log = Log