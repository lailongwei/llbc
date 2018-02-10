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

-- The timer scheduler encapsulation.
-- Use to schedule all timers.
local TimerScheduler = llbc.newclass('llbc.TimerScheduler')

-- Update timer scheduler.
--   - static method
-- @returns - no return.
function TimerScheduler.update()
    _llbc.TimerScheduler_Update()
end

-- Check timer scheduler enabled or not.
--   - static method
-- @returns - true if timer scheduler enabled, otherwise return false.
function TimerScheduler.is_enabled()
    return _llbc.TimerScheduler_IsEnabled()
end

-- Enable/Disable timer scheduler.
--   - static method
-- @param[required] enabled - true if want to enable timer scheduler, false if want to disable timer scheduler.
-- @returns - no return.
function TimerScheduler.set_enabled(enabled)
    return _llbc.TimerScheduler_SetEnabled(enabled)
end

-- Get timer count in this scheduler.
--   -- static method
-- @returns - the timer count.
function TimerScheduler.get_timercount()
    return _llbc.TimerScheduler_GetTimerCount()
end

-- Set timer error handler.
-- handler prototype:
--   function TimerErrorHandler(timer, ty, errorMsg, traceback)
-- @param[required] handler - the timer error handler.
-- @returns - no return.
function TimerScheduler.set_timer_errorhandler(handler)
    if type(handler) ~= "function" then
        error("Timer error handler type must be <function>")
    end

    _llbc.TimerScheduler_SetTimerErrorHandler(handler)
end

llbc.TimerScheduler = TimerScheduler
