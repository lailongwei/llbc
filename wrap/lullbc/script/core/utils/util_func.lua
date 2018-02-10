-- The MIT License(MIT)

-- Copyright(c) 2013 lailongwei<lailongwei@126.com>
-- 
-- Permission is hereby granted, free of charge, to any person obtaining a copy of
-- this software and associated documentation files(the "Software"), to deal in
-- the Software without restriction, including without limitation the rights to
-- use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
-- the Software, and to permit persons to whom the Software is furnished to do so,
-- subject to the following conditions :
-- 
-- The above copyright notice and this permission notice shall be included in all
-- copies or substantial portions of the Software.
-- 
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
-- FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
-- COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
-- IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
-- CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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
