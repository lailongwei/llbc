local CPUTime = llbc.newclass('llbc.CPUTime')

-- Get cpu time.
-- @return number - the cpu time(cpu timestamp counter).
CPUTime.get_cputime = _llbc.Util_GetCPUTime

-- Convert cpu time to UTC time.
-- @param[in] cputime - the cpu time.
-- @return number - the UTC time, in milli-seconds.
CPUTime.to_utc_time = _llbc.Util_CPUTimeToUTCTime

llbc.CPUTime = CPUTime