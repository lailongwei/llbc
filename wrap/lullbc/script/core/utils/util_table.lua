-- Set specific table to readonly.
-- @param[required] t          - specific table.
-- @param[optional] prompt_msg - when outer logic try to write table, will raise this prompt message,
-- @returns - no return.
function llbc.set_table_readonly(t, prompt_msg)
    local mt = getmetatable(t)
    if mt == nil then
        mt = {}
        setmetatable(t, mt)
    end

    function mt.__newindex(t, k, v)
        if type(prompt_msg) == 'string' and
                string.len(prompt_msg) > 0 then
            error(prompt_msg)
        else
            error("Could not change readonly table")
        end
    end
end