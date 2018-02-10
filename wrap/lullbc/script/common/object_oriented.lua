-- Store all classes(class name -> class definition)
local _classes = {}

-- Create new class
-- @param[required] classname - the class name.
-- @param[optional] super     - super class, can set to function or table.
-- @returns - the new class object.
function llbc.newclass(classname, super)
    if _classes[classname] then
        error(string.format("Cloud not repeat to create class '%s'", classname))
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
    cls.__cname = classname 

    function cls.new(...)
        local instance = setmetatable({}, cls)
        instance.__class = cls
        instance:ctor(...)
        return instance
    end

    if not rawget(cls, '__tostring') then
        rawset(cls, '__tostring', function (instance)
            return string.format('<%s object at %x>', classname, _llbc.Util_Misc_ToPointer(instance))
        end)
    end

    rawset(_classes, classname, cls)
    return cls
end

-- Get class by class name.
-- @param[required] classname - the class name.
-- @returns - the class object or nil.
function llbc.getclass(classname)
    return _classes[classname]
end

-- [Internal function] Check given object is kind of class.
-- @param[required] obj                - given object.
-- @param[required] class_or_classname - the class object or class name.
-- @returns - true if obj is kind of specified class or class name, otherwise return false.
local _iskindofclass = function (obj, class_or_classname)
    if type(obj) ~= 'table' then
        return false
    end

    local classname
    local ty = type(class_or_classname)
    if ty == 'string' then
        classname = class_or_classname
    elseif ty == 'table' then
        classname = class_or_classname.__cname
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

-- Check given object is instance of specific type or types.
-- @param[required] obj           - given object.
-- @param[required] type_or_types - the specific type or types.
-- @returns boolean - return true, it means given object is specific type or one of the types. otherwise return false.
function llbc.isinstance(obj, type_or_types)
    local objtype = type(obj)
    if objtype == 'table' and getmetatable(obj) then -- maybe is a llbc style class encapsulation.
        if type(type_or_types) == 'string' then -- type_or_types is string, call _iskindofclass() to check
            if _iskindofclass(obj, type_or_types) then
                return true
            end

            -- return false -- may be not a llbc style class, dont't need return.
        elseif type(type_or_types) == 'table' then -- type_or_types is a table, maybe this table is class list or just a class definition.
            if type_or_types.__cname then -- class definition.
                return _iskindofclass(obj, type_or_types)
            else
                for _, v in ipairs(type_or_types) do -- class list, for each to call llbc.isinstance
                    if llbc.isinstance(obj, v) then
                        return true
                    end
                end

                -- return false -- maybe not a llbc style class encapsulation, don't need return.
            end
        end
    end

    -- lua normal types check.
    if type(type_or_types) == 'string' then
        return objtype == type_or_types
    else
        for _, v in ipairs(type_or_types) do
            if objtype == v then
                return true
            end
        end

        return false
    end
end