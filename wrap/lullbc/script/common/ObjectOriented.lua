--[[
@file   ObjectOriented.lua
@brief  monkey patch functions implementation.
--]]

-- Create new class
-- :param[required] className: the class name.
-- :param[optional] super:     super class, can set to function or table.
function llbc.newclass(className, super)
    local cls, clsmt
    if super then
        clsmt = {__index = super}
        cls = setmetatable({}, clsmt)
        cls.__super = super
    else
        clsmt = {}
        cls = {ctor = function (...) end}
        setmetatable(cls, clsmt)
    end

    clsmt.__tostring = function (c) return string.format("<class '%s'>", c.__cname) end

    cls.__index = cls
    cls.__cname = className

    function cls.new(...)
        local instance = setmetatable({}, cls)
        instance.__class = cls
        instance:ctor(...)
        return instance
    end

    if not rawget(cls, '__tostring') then
        cls.__tostring = function (instance)
            return string.format('<%s object at %x>', className, _llbc.MonkeyPatchImpl_ToPointer(instance))
        end
    end

    return cls
end

-- Check given object is kind of class.
-- :param[required] obj: given object.
-- :param[
function llbc.iskindof(obj, classOrClassName)
    if type(obj) ~= 'table' then
        return false
    end

    local classname
    local ty = type(classOrClassName)
    if ty == 'string' then
        classname = classOrClassName
    elseif ty == 'table' then
        classname = classOrClassName.__cname
    end

    if type(classname) ~= 'string' then
        return false
    end

    local mt = getmetatable(obj)
    while mt do
        if mt.__cname == classname then
            return true
        end

        mt = mt.__super
    end

    return false
end