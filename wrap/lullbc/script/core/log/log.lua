local Log = llbc.newclass('llbc.Log')

local type = type
local getinfo = debug.getinfo
local LogMsg = _llbc.LogMsg

-- Log levels enumeration.
Log.TRACE = 0
Log.DEBUG = 1
Log.INFO = 2
Log.WARN = 3
Log.ERROR = 4
Log.FATAL = 5
-- Local log levels(for optimize performance).
local TRACE = Log.TRACE
local DEBUG = Log.DEBUG
local INFO = Log.INFO
local WARN = Log.WARN
local ERROR = Log.ERROR
local FATAL = Log.FATAL

-- log_file_info: to control output message included file&line information or not, default is false.
Log.log_file_info = false
-- default_file_info: use to fill <file> <line> param on output message, !!!must be function!!!, default is nil,
Log.default_file_info = nil
-- default_log_tag: use to fill <tag> param on output message, can !!!set to string or function!!!, default is nil.
Log.default_log_tag = nil

-- constructor, do not call.
function Log.ctor()
    error('now allow to instantiate Log instance')
end

-- Initialize Log component.
-- @param[required] cfgFile     - log init config file.
-- @param[optional] logFileinfo - log file info or not, default is false.
-- @returns - no return.
function Log.init(cfgFile, log_file_info)
    _llbc.InitLog(cfgFile)
    if log_file_info then
        Log.log_file_info = true
    end
end

-- Uninitialize Log component.
Log.uninit = _llbc.UnInitLog

-- Check Log init or log.
-- @returns boolean - init flag.
function Log.isinit()
    return _llbc.IsLogInit()
end

local _logger_levels = {}
local _root_logger_name = "root"

-- Internal function, use to check given level log message in specific logger can output or not.
-- @param[in] level  - the log level.
-- @Param[in] logger - the logger name.
-- @return - return true if can output, otherwise return false.
local function _can_output(level, logger)
    local logLevel = _logger_levels[logger or _root_logger_name]
    if not logLevel then
        if not Log.isinit() then
            return true
        end

        logLevel = _llbc.GetLogLevel(logger)
        _logger_levels[logger or _root_logger_name] = logLevel
    end

    return level >= logLevel
end

-- Output message to logger.
-- @param level  - logger level.
-- @param logger - logger name, default is root logger.
-- @param tag    - log tag, default is nil.
-- @param ...    - log message.
-- @returns - no return.
function Log.output(level, logger, tag, ...)
    if not _can_output(level, logger) then
        return
    end

    local file, line, func_name
    if Log.log_file_info then
        local default_file_info = Log.default_file_info
        if default_file_info then
            file, line = default_file_info()
        else
            local di = getinfo(3, 'Sln')
            file, line, func_name = di.source, di.currentline, di.name
        end
    end

    if not tag then
        local default_log_tag = Log.defaultLogTag
        if default_log_tag then
            if type(default_log_tag) == 'function' then
                tag = default_log_tag()
            else
                tag = default_log_tag
            end
        end
    end

    LogMsg(level, logger, tag, file, line, func_name, ...)

    if level >= FATAL then
        error(tostring(...)) -- Only output the first param
    end
end

local output = Log.output

-- Log TRACE level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.t(...)
    output(TRACE, nil, nil, ...)
end

-- Log TRACE level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.t2(logger, ...)
    output(TRACE, logger, nil, ...)
end

-- Log TRACE level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.t3(logger, tag, ...)
    output(TRACE, logger, tag, ...)
end

-- Log DEBUG level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.d(...)
    output(DEBUG, nil, nil, ...)
end

-- Log DEBUG level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.d2(logger, ...)
    output(DEBUG, logger, nil, ...)
end

-- Log DEBUG level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.d3(logger, tag, ...)
    output(DEBUG, logger, tag, ...)
end

-- Log INFO level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.i(...)
    output(INFO, nil, nil, ...)
end

-- Log INFO level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.i2(logger, ...)
    output(INFO, logger, nil, ...)
end

-- Log INFO level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.i3(logger, tag, ...)
    output(INFO, logger, tag, ...)
end

-- Log WARN level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.w(...)
    output(WARN, nil, nil, ...)
end

-- Log WARN level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.w2(logger, ...)
    output(WARN, logger, nil, ...)
end

-- Log WARN level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ... -              log message.
-- @returns - no return.
function Log.w3(logger, tag, ...)
    output(WARN, logger, tag, ...)
end

-- Log ERROR level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.e(...)
    output(ERROR, nil, nil, ...)
end

-- Log ERROR level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.e2(logger, ...)
    output(ERROR, logger, nil, ...)
end

-- Log ERROR level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.e3(logger, tag, ...)
    output(ERROR, logger, tag, ...)
end

-- Log FATAL level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.f(...)
    output(FATAL, nil, nil, ...)
end

-- Log FATAL level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.f2(logger, ...)
    output(FATAL, logger, nil, ...)
end

-- Log FATAL level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.f3(logger, tag, ...)
    output(FATAL, logger, tag, ...)
end

-- Set Log table to llbc
llbc.Log = Log
