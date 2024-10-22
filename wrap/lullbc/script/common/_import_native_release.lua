-- Import lullbc native lib.
local _llbc = require "_lullbc"

-- Define lullbc lib.
local llbc = {}
llbc.DEBUG = false
llbc.RELEASE = true

-- Define lullbc lib metatable.
local llbc_mt = {}
setmetatable(llbc, llbc_mt)

-- Set lib config.
llbc.config = {}
_llbc.SetLibConfigToLua(llbc.config)