--[[
@file    llbc.lua
@author  Longwei Lai<lailongwei@126.com>
@version 1.0
@date    ----/--/--

@brief   llbc lua wrap library(lullbc)
--]]

-- ******** SCRIPT: common/_ImportNative_Debug.lua ********
-- Import lullbc native lib
local _llbc = require "_lullbc_debug"

-- Define lullbc lib
local llbc = {}
llbc.DEBUG = true
llbc.RELEASE = false

-- Set lib config
_llbc.SetLibConfigToLua(llbc)

-- ******** SCRIPT: common/VerInfo.lua ********
--[[
@file   VerInfo.lua
@brief  Implementation some version about APIs.
--]]

-- Get llbc version information.
-- :param verbose: indicate return verbose library version information or not, default is false.
-- :returns: version information string.
function llbc.GetVersionInfo(verbose)
    return _llbc.GetVersionInfo(verbose)
end


-- ******** SCRIPT: core/log/Log.lua ********
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
-- :param ...:              log message.
-- :param[optional] logger: logger name, default is root logger.
-- :returns: no return.
function Log.d2(logger, ...)
    Log.Output(0, logger, nil, ...)
end

-- Log INFO level message to root logger.
-- :param ...: log message.
-- :returns: no return.
function Log.i(...)
    Log.Output(1, nil, nil, ...)
end

-- Log INFO level message to specific logger.
-- :param ...:              log message.
-- :param[optional] logger: logger name, default is root logger.
-- :returns: no return.
function Log.i2(logger, ...)
    Log.Output(1, logger, nil, ...)
end

-- Log WARN level message to root logger.
-- :param ...: log message.
-- :returns: no return.
function Log.w(...)
    Log.Output(2, nil, nil, ...)
end

-- Log WARN level message to specific logger.
-- :param ...:              log message.
-- :param[optional] logger: logger name, default is root logger.
-- :returns: no return.
function Log.w2(logger, ...)
    Log.Output(2, logger, nil, ...)
end

-- Log ERROR level message to root logger.
-- :param ...: log message.
-- :returns: no return.
function Log.e(...)
    Log.Output(3, nil, nil, ...)
end

-- Log ERROR level message to specific logger.
-- :param ...:              log message.
-- :param[optional] logger: logger name, default is root logger.
-- :returns: no return.
function Log.e2(logger, ...)
    Log.Output(3, logger, nil, ...)
end

-- Log FATAL level message to root logger.
-- :param ...: log message.
-- :returns: no return.
function Log.f(...)
    Log.Output(4, nil, nil, ...)
end

-- Log FATAL level message to specific logger.
-- :param ...:              log message.
-- :param[optional] logger: logger name, default is root logger.
-- :returns: no return.
function Log.f2(logger, ...)
    Log.Output(4, logger, nil, ...)
end

-- Output message to logger.
-- :param level:  logger level.
-- :param logger: logger name, default is root logger.
-- :param tag:    log tag, default is nil.
-- :param ...:    log message.
-- :returns: no return.
function Log.Output(level, logger, tag, ...)
    msg = table.concat({...}, '\t')
    if Log.logFileinfo then
        local di = debug.getinfo(2, 'Sl')
        local file, line = di.source, di.currentline
    end

    _llbc.LogMsg(level, msg, logger, tag, file, line)
end

-- Set Log table to llbc
llbc.Log = Log

-- ******** SCRIPT: LullbcTop.lua ********
--[[
@file   LullbcTop.lua
@brief  lullbc library top level script file, implement like startup/cleanup funcations.
--]]

-- Startup lullbc library.
-- :param initWinSock: initialize WinSock or not, default is true, only available in Windows platform.
-- :returns: no return, if error occurred, will raise error.
function llbc.Startup(initWinSock)
    _llbc.Startup(initWinSock)
end

-- Cleanup lullbc library.
-- :returns: no return.
function llbc.Cleanup()
    _llbc.Cleanup()
end


-- return library
return llbc
