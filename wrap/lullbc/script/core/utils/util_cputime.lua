-- Get cpu time.
-- @return number - the cpu time(cpu timestamp counter).
llbc.cputime = _llbc.CPUTime

-- Convert cpu time to universal time.
-- @param[in] cputime - the cpu time.
-- @return number - the universal time, in micro-seconds.
llbc.cputime_to_universal_time = _llbc.CPUTimeToUniversalTime