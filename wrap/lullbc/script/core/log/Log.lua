local Log = llbc.newclass('llbc.Log')

local type = type
local getinfo = debug.getinfo
local LogMsg = _llbc.LogMsg

-- Log levels enumeration.
Log.DEBUG = 0
Log.INFO = 1
Log.WARN = 2
Log.ERROR = 3
Log.FATAL = 4

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

    local file, line
    if Log.logFileInfo then
        local defaultFileInfo = Log.defaultFileInfo
        if defaultFileInfo then
            file, line = defaultFileInfo()
        else
            local di = getinfo(3, 'Sl')
            file, line = di.source, di.currentline
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

    LogMsg(level, logger, tag, file, line, ...)

    if level >= Log.FATAL then
        error(...)
    end
end

local output = Log.output

-- Log DEBUG level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.d(...)
    output(0, nil, nil, ...)
end

-- Log DEBUG level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.d2(logger, ...)
    output(0, logger, nil, ...)
end

-- Log DEBUG level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.d3(logger, tag, ...)
    output(0, logger, tag, ...)
end

-- Log INFO level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.i(...)
    output(1, nil, nil, ...)
end

-- Log INFO level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.i2(logger, ...)
    output(1, logger, nil, ...)
end

-- Log INFO level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.i3(logger, tag, ...)
    output(1, logger, tag, ...)
end

-- Log WARN level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.w(...)
    output(2, nil, nil, ...)
end

-- Log WARN level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.w2(logger, ...)
    output(2, logger, nil, ...)
end

-- Log WARN level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ... -              log message.
-- @returns - no return.
function Log.w3(logger, tag, ...)
    output(2, logger, tag, ...)
end

-- Log ERROR level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.e(...)
    output(3, nil, nil, ...)
end

-- Log ERROR level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.e2(logger, ...)
    output(3, logger, nil, ...)
end

-- Log ERROR level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.e3(logger, tag, ...)
    output(3, logger, tag, ...)
end

-- Log FATAL level message to root logger.
-- @param ... - log message.
-- @returns - no return.
function Log.f(...)
    output(4, nil, nil, ...)
end

-- Log FATAL level message to specific logger.
-- @param[optional] logger - logger name, default is root logger.
-- @param ...              - log message.
-- @returns - no return.
function Log.f2(logger, ...)
    output(4, logger, nil, ...)
end

-- Log FATAL level message to specific logger, and append tag.
-- @param[optional] logger - logger name, default is root logger.
-- @param[optional] tag    - log tag, default is nil.
-- @param ...              - log message.
-- @returns - no return.
function Log.f3(logger, tag, ...)
    output(4, logger, tag, ...)
end

-- Set Log table to llbc
llbc.Log = Log
