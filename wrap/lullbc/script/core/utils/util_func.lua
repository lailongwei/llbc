local table_unpack = table.unpack or unpack

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
        local args = {table_unpack(call_args)}
        for k, v in ipairs({...}) do
            table.insert(args, #args + 1, v)
        end

        return func(table_unpack(args))
    end
end

llbc.FuncTool = FuncTool
