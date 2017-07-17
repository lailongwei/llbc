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

-- Implement __tostring
function llbc_mt.__tostring(t)
    return 'llbc lua wrapped library(lullbc): ' .. t.GetVersionInfo(false)
end