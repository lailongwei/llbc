--[[
@file   monkey_patch.lua
@brief  monkey patch functions implementation.
--]]

-- Monkey patch current lua engine.
-- 1. (c layer patch) patch table.
--   1.1 patch table.concat, after patched, table.concat() can concat any implemented __tostring's type objects.
--   1.1 patch table.readonly, after patched, table.readonly() can make any table to become readonly.
--
-- 2. (c layer patch) patch _G, add topointer method.

-- 3. (lua layer patch) patch _G, add printf method.
-- 
-- 4. (lua layer patch) Patch _G, add some object oriented about methods.
--   4.1. patch newclass() method to _G, use to create class.
--   4.2. patch getclass() method to _G, use to get class definition by name.
--   4.3. patch isinstance() method to _G, use to determine object type(included llbc style class type and normal lua type).
--
-- :returns: no return.
function llbc.monkey_patch()
    -- patch table.
    table.concat = _llbc.Util_Table_Concat
    table.readonly = llbc.set_table_readonly

    -- patch topointer to _G.
    topointer = _llbc.Util_Misc_ToPointer

    -- patch printf to _G.
    printf = llbc.printf

    -- patch obiect oriented functions to _G.
    newclass = llbc.newclass
    getclass = llbc.getclass
    isinstance = llbc.isinstance
end
