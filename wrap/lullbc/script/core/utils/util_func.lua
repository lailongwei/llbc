--[[
@file   util_func.lua
@brief  The function util functions encapsulation.
--]]

local FuncTool = llbc.newclass('llbc.FuncTool')

-- constructor, do not call.
function FuncTool:ctor()
    error('now allow to instantiate FuncTool instance')
end

-- Return a new partial function, function first N parameters already assigned.
-- @param[required] func - the will partial function.
-- @param[optional] ...  - will assign arguments, must be greater than or equal to 1.
-- @returns function - the partial function.
function FuncTool.partial(func, ...)
    local call_args = {...}
    if #call_args < 1 then
        error('partial arguments count could not be 0')
    end

    return function (...)
        for _, v in pairs({...}) do
            table.insert(call_args, #call_args + 1, v)
        end

        return func(table.unpack(call_args))
    end
end

llbc.FuncTool = FuncTool
