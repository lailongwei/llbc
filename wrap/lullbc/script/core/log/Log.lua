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

-- logFileInfo: to control output message included file&line information or not, default is false.
Log.logFileInfo = false
-- defaultFileInfo: use to fill <file> <line> param on output message, must be function, default is nil,
Log.defaultFileInfo = nil
-- defaultLogTag: use to fill <tag> param on output message, can set to string or function, default is nil.
Log.defaultLogTag = nil

-- constructor, do not call.
function Log.ctor()
    error('now allow to instantiate Log instance')
end

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

-- Check Log init or log.
-- @returns boolean - init flag.
function Log.isinit()
    return _llbc.IsLogInit()
end

local ROOT_LOGGER = "root"
local _logsLevel = {}

local function _canOutput(level, logger)
    local logLevel = _logsLevel[logger or ROOT_LOGGER]
    if not logLevel then
        if not Log.isinit() then
            return true
        end

        logLevel = _llbc.GetLogLevel(logger)
        _logsLevel[logger or ROOT_LOGGER] = logLevel
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
    if not _canOutput(level, logger) then
        return
    end

    local file, line, func_name
    if Log.logFileInfo then
        local defaultFileInfo = Log.defaultFileInfo
        if defaultFileInfo then
            file, line = defaultFileInfo()
        else
            local di = getinfo(3, 'Sln')
            file, line, func_name = di.source, di.currentline, di.name
        end
    end

    if not tag then
        local defaultLogTag = Log.defaultLogTag
        if defaultLogTag then
            if type(defaultLogTag) == 'function' then
                tag = defaultLogTag()
            else
                tag = defaultLogTag
            end
        end
    end

    LogMsg(level, logger, tag, file, line, func_name, ...)

    if level >= Log.FATAL then
        error(tostring(...)) -- Only output the first param
    end
end

local output = Log.output

-- Log TRACE level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.t(...)
    output(Log.TRACE, nil, nil, ...)
end

-- Log TRACE level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.t2(logger, ...)
    output(Log.TRACE, logger, nil, ...)
end

-- Log TRACE level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.t3(logger, tag, ...)
    output(Log.TRACE, logger, tag, ...)
end

-- Log DEBUG level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.d(...)
    output(Log.Debug, nil, nil, ...)
end

-- Log DEBUG level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.d2(logger, ...)
    output(Log.DEBUG, logger, nil, ...)
end

-- Log DEBUG level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.d3(logger, tag, ...)
    output(Log.Debug, logger, tag, ...)
end

-- Log INFO level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.i(...)
    output(Log.INFO, nil, nil, ...)
end

-- Log INFO level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.i2(logger, ...)
    output(Log.INFO, logger, nil, ...)
end

-- Log INFO level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.i3(logger, tag, ...)
    output(Log.INFO, logger, tag, ...)
end

-- Log WARN level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.w(...)
    output(Log.WARN, nil, nil, ...)
end

-- Log WARN level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.w2(logger, ...)
    output(Log.WARN, logger, nil, ...)
end

-- Log WARN level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ... -              log message.
-- @returns - no return.
function Log.w3(logger, tag, ...)
    output(Log.WARN, logger, tag, ...)
end

-- Log ERROR level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.e(...)
    output(Log.ERROR, nil, nil, ...)
end

-- Log ERROR level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.e2(logger, ...)
    output(Log.ERROR, logger, nil, ...)
end

-- Log ERROR level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.e3(logger, tag, ...)
    output(Log.ERROR, logger, tag, ...)
end

-- Log FATAL level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.f(...)
    output(Log.FATAL, nil, nil, ...)
end

-- Log FATAL level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.f2(logger, ...)
    output(Log.FATAL, logger, nil, ...)
end

-- Log FATAL level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.f3(logger, tag, ...)
    output(Log.FATAL, logger, tag, ...)
end

-- Set Log table to llbc
llbc.Log = Log
