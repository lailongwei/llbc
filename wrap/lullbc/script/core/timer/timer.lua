-- The MIT License(MIT)

-- Copyright(c) 2013 lailongwei<lailongwei@126.com>
-- 
-- Permission is hereby granted, free of charge, to any person obtaining a copy of
-- this software and associated documentation files(the "Software"), to deal in
-- the Software without restriction, including without limitation the rights to
-- use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
-- the Software, and to permit persons to whom the Software is furnished to do so,
-- subject to the following conditions :
-- 
-- The above copyright notice and this permission notice shall be included in all
-- copies or substantial portions of the Software.
-- 
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
-- FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
-- COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
-- IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
-- CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

local Timer = llbc.newclass('llbc.Timer')

-- Timer constructor.
-- @param[required] - timeout_func: the timeout function.
function Timer:ctor(timeout_func)
    if type(timeout_func) ~= 'function' then
        error(string.format('<timeout_func> must be a function type, got %s', type(timeout_func)))
    end

    self._cobj = _llbc.Timer_New(self, timeout_func)
end

-- Get timer Id.
-- @returns - the timer Id.
function Timer:get_timerid()
    return _llbc.Timer_GetTimerId(self._cobj)
end

-- Schedule timer.
-- @param[required] dueTime - due time, in seconds.
-- @param[optional] period  - period time, in seconds, if not provide, use due time argument.
-- @returns - no return.
function Timer:schedule(duetime, period)
    if type(period) ~= 'number' then
        period = duetime 
    end

    _llbc.Timer_Schedule(self._cobj, duetime, period)
end

-- Get due time.
-- @returns - the due time, in seconds.
function Timer:get_duetime()
    return _llbc.Timer_GetDueTime(self._cobj)
end

-- Get period time.
-- @returns - the period time, in seconds.
function Timer:get_period()
    return _llbc.Timer_GetPeriod(self._cobj)
end

-- Check timer is scheduling.
-- @returns - return true if timer is scheduling, otherwise return false.
function Timer:is_scheduling()
    return _llbc.Timer_IsScheduling(self._cobj)
end

-- override __tostring method.
function Timer:__tostring()
    return string.format(
        '<llbc.timer object, timerId: %d, scheduling: %s>', 
        self:get_timerid(), self:is_scheduling());
end

llbc.Timer = Timer
