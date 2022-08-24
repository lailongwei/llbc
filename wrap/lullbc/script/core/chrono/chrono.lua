local Chrono = llbc.newclass('llbc.Chrono')

-- Get Coordinated Universal Time, in seconds.
-- @return integer - the UTC time, in seconds.
Chrono.utc_seconds = _llbc.Chrono_UTC_Seconds

-- Get Coordinated Universal Time, in milli-seconds.
-- @return integer - the UTC time, in milli-seconds.
Chrono.utc_milliseconds = _llbc.Chrono_UTC_MilliSeconds

-- Get Coordinated Universal Time, in micro-seconds.
-- @return integer - the UTC time, in micro-seconds.
Chrono.utc_microseconds = _llbc.Chrono_UTC_MicroSeconds

llbc.Chrono = Chrono
