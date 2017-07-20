--[[
@file   MonkeyPatch.lua
@brief  monkey patch functions implementation.
--]]

-- Monkey patch current lua engine.
-- 1. patch table:
--   1.1 patch table.concat, after patched, table.concat() can concat any implemented __tostring's type objects.
--
-- :returns: no return.
function llbc.MonkeyPatch()
    _llbc.MonkeyPatch(-1)
end
