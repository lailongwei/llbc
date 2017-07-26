--[[
    @file   Main.lua
    @brief  lullbc testsuite entry file.
            if you want to run the testsuite, please lua executable/library files and lullbc library files to this file directory.
--]]

-- Import lullbc library(set to _G), default test use release version library, if you
-- want to use debug version library to run testsuite, please changed to `llbc = require 'llbc_debug'`
llbc = require 'llbc'

-- Import all common module testcases.
local TestCase_Com_VerInfo = require 'common.TestCase_Com_VerInfo'
local TestCase_Com_ObjectOriented = require 'common.TestCase_Com_ObjectOriented'
-- Import all core module testcases.
local TestCase_Core_Log = require 'core.log.TestCase_Core_Log'
local TestCase_Core_Helper_GUID= require 'core.helper.TestCase_Core_Helper_GUID'
local TestCase_Core_Utils_Util_Table = require 'core.utils.TestCase_Core_Utils_Util_Table'
-- Import all communication module testcases.
-- ... ...

local function Main(...)
    -- Startup llbc.
    llbc.Startup()

    -- All common module testcases.
    TestCase_Com_VerInfo.Run(...)
    -- TestCase_Com_ObjectOriented.Run(...)

    -- All core module testcases...
    -- TestCase_Core_Log.Run(...)
    -- TestCase_Core_Helper_GUID.Run(...)
    -- TestCase_Core_Utils_Util_Table.Run(...)

    -- All communication module testcases...
    -- ... ...

    -- Cleanup llbc.
    llbc.Cleanup()
end

Main(...)