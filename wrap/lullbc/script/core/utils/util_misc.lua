-- Format output message.
-- @param[required] fmt - the message format, see string.format() function.
-- @param[optional] ... - format arguments.
-- @returns - no return.
function llbc.printf(fmt, ...)
    print(string.format(fmt, ...))
end

-- Get lua object c level pointer value.
-- @param[required] obj - the lua object.
-- @returns number - the pointer value.
llbc.topointer = _llbc.Util_Misc_ToPointer
