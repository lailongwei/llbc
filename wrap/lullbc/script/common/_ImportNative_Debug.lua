-- Import lullbc native lib
local _llbc = require "_lullbc_debug"

-- Define lullbc lib
local llbc = {}
llbc.DEBUG = true
llbc.RELEASE = false

-- Set lib config
_llbc.SetLibConfigToLua(llbc)