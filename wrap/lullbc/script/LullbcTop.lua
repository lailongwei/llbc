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

-- Final, implement __newindex method to prevent llbc library table set operation.
function llbc_mt.__newindex(t, k, v)
    error('Could not change llbc library!')
end