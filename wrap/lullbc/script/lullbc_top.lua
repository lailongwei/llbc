-- Startup lullbc library.
-- @param[optional] initWinSock - initialize WinSock or not, default is true, only available in Windows platform.
-- @returns - no return, if error occurred, will raise error.
function llbc.startup(initWinSock)
    _llbc.Startup(initWinSock)
end

-- Cleanup lullbc library.
-- @returns - no return.
function llbc.cleanup()
    _llbc.Cleanup()
end

-- Final, set llbc library table to readonly.
llbc.set_table_readonly(llbc)
