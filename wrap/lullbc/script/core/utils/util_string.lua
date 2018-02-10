-- Hash object.
-- @param[required] obj - the will hash lua object.
-- @returns number - the object hash value.
function llbc.hash(obj)
    local objType = type(obj)
    if objType == 'boolean' then
        return obj and 1 or 0
    elseif objType == 'nil' then
        return 0
    else
        return _llbc.Util_String_HashString(obj)
    end
end
