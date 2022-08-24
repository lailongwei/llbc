local Threading = llbc.newclass('llbc.Threading')

-- Sleep milli-seconds.
-- @param[optional] seconds - sleep seconds, if not set, causes the thread to relinquish the remainder of its time.
-- @returns - no return.
Threading.sleep = _llbc.Threading_Sleep

-- Get current thread Id.
-- @return integer - the current thread Id.
Threading.get_current_threadid = _llbc.Threading_GetCurrentThreadId

-- Get current process Id.
-- @return integer - the current process Id.
Threading.get_current_processid = _llbc.Threading_GetCurrentProcessId

llbc.Threading = Threading
