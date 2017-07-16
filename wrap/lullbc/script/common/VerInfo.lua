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
