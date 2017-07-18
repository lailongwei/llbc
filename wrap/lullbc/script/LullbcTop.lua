--[[
@file   LullbcTop.lua
@brief  lullbc library top level script file, implement like startup/cleanup funcations.
--]]

-- Startup lullbc library.
-- :param[optional] initWinSock: initialize WinSock or not, default is true, only available in Windows platform.
-- :returns: no return, if error occurred, will raise error.
function llbc.Startup(initWinSock)
    _llbc.Startup(initWinSock)
end

-- Cleanup lullbc library.
-- :returns: no return.
function llbc.Cleanup()
    _llbc.Cleanup()
end

-- Final, set llbc library table to readonly.
llbc.SetTableReadOnly(llbc)
