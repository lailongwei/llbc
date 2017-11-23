--[[
@file   guid_helper.lua
@brief  The GUID about logic implementation.
--]]

local GUID = llbc.newclass('llbc.GUID')

-- constructor, do not call.
function GUID.ctor()
    error('now allow to instantiate GUID instance')
end

-- Generate GUID string
-- @returns - the GUID string
function GUID.generate()
    return _llbc.GUID_Generate()
end

llbc.GUID = GUID
