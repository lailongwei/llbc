-- Import lullbc native lib.
local _llbc = require "_lullbc"

-- Define lullbc lib.
local llbc = {}
llbc.DEBUG = false
llbc.RELEASE = true

-- Set lib config.
_llbc.SetLibConfigToLua(llbc)