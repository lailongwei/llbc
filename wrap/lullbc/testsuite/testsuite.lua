--[[
    @file   Main.lua
    @brief  lullbc testsuite entry file.
            if you want to run the testsuite, please lua executable/library files and lullbc library files to this file directory.
--]]

-- Import lullbc library(set to _G), default test use release version
-- library, if you want to use debug version library to run testsuite, 
-- !!!!!!!!please changed to `llbc = require 'llbc_debug'`!!!!!!!!
llbc = require 'llbc'

-- Import all common module testcases.
local TestCase_Com_VerInfo = require 'common.testcase_com_verinfo'
local TestCase_Com_MonkeyPatch = require 'common.testcase_com_monkey_patch'
local TestCase_Com_ObjectOriented = require 'common.testcase_com_object_oriented'
-- Import all core module testcases.
local TestCase_Core_Log = require 'core.log.testcase_core_log'
local TestCase_Core_Timer = require 'core.timer.testcase_core_timer'
local TestCase_Core_Thread = require 'core.thread.testcase_core_thread'
local TestCase_Core_Util_GUID= require 'core.utils.testcase_core_util_guid'
local TestCase_Core_Utils_Util_Func = require 'core.utils.testcase_core_util_func'
local TestCase_Core_Utils_Util_Table = require 'core.utils.testcase_core_util_table'
local TestCase_Core_Utils_Util_String = require 'core.utils.testcase_core_util_string'
-- Import all communication module testcases.
-- ... ...

local function Main(...)
    -- Startup llbc.
    llbc.startup()

    -- All common module testcases.
    TestCase_Com_VerInfo.run(...)
    -- TestCase_Com_MonkeyPatch.run(...)
    -- TestCase_Com_ObjectOriented.run(...)

    -- All core module testcases...
    -- TestCase_Core_Log.run(...)
    -- TestCase_Core_Timer.run(...)
    -- TestCase_Core_Thread.run()
    -- TestCase_Core_Util_GUID.run(...)
    -- TestCase_Core_Util_Func.run(...)
    -- TestCase_Core_Util_Table.run(...)
    -- TestCase_Core_Util_String.run(...)

    -- All communication module testcases...
    -- ... ...

    -- Cleanup llbc.
    llbc.cleanup()
end

Main(...)