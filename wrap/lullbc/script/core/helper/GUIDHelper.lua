--[[
@file   GUIDHelper.lua
@brief  The GUID about logic implementation.
--]]

local GUID = {}

-- Generate GUID string
-- :returns: the GUID string
GUID.generate = _llbc.GUID_Generate

llbc.GUID = GUID
