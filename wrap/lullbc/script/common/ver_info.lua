-- Get llbc version information.
-- @param[optional] verbose - indicate return verbose library version information or not, default is false.
-- @returns - version information string.
function llbc.get_versioninfo(verbose)
    return _llbc.GetVersionInfo(verbose)
end

-- Implement __tostring
function llbc_mt.__tostring(t)
    return 'llbc lua wrapped library(lullbc): ' .. t.get_versioninfo(false)
end
