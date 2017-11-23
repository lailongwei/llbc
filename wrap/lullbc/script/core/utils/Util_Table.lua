--[[
@file   util_table.lua
@brief  The some table util functions define.
--]]

-- Set specific table to readonly.
-- @param[required] t         - specific table.
-- @param[optional] promptMsg - when outer logic try to write table, will raise this prompt message,
-- @returns - no return.
function llbc.set_table_readonly(t, promptMsg)
    local mt = getmetatable(t)
    if mt == nil then
        mt = {}
        setmetatable(t, mt)
    end

    function mt.__newindex(t, k, v)
        if type(promptMsg) == 'string' and
                string.len(promptMsg) > 0 then
            error(promptMsg)
        else
            error("Could not change readonly table")
        end
    end
end