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
