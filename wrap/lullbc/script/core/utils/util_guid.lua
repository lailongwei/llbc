local GUID = llbc.newclass('llbc.GUID')

-- Generate GUID string
-- @returns - the GUID string
GUID.generate = _llbc.Util_GUID_Generate

llbc.GUID = GUID
