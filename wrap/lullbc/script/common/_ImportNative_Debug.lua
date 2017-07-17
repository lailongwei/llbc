-- Import lullbc native lib
local _llbc = require "_lullbc_debug"

-- Define lullbc lib
local llbc = {}
llbc.DEBUG = true
llbc.RELEASE = false

-- Define lullbc lib metatable.
local llbc_mt = {}
setmetatable(llbc, llbc_mt)

-- Set lib config
_llbc.SetLibConfigToLua(llbc)