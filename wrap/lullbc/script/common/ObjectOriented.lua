--[[
@file   ObjectOriented.lua
@brief  monkey patch functions implementation.
--]]

-- Store all classes(class name -> class definition)
local _classes = {}

-- Create new class
-- :param[required] className: the class name.
-- :param[optional] super:     super class, can set to function or table.
-- :returns: the new class object.
function llbc.newclass(className, super)
    if _classes[className] then
        error(string.format("Cloud not repeat to create class '%s'", className))
    end

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
        rawset(cls, '__tostring', function (instance)
            return string.format('<%s object at %x>', className, _llbc.MonkeyPatchImpl_ToPointer(instance))
        end)
    end

    rawset(_classes, className, cls)
    return cls
end

-- Get class by class name.
-- :param[required] className: the class name.
-- :returns: the class object or nil.
function llbc.getclass(className)
    return _classes[className]
end

-- Check given object is kind of class.
-- :param[required] obj:              given object.
-- :param[required] classOrClassName: the class object or class name.
-- :returns: true if obj is kind of specified class or class name, otherwise return false.
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