-- Sleep specific seconds
-- @param[optional] seconds - sleep seconds, if not set, causes the thread to relinquish the remainder of its time.
-- @returns - no return.
function llbc.sleep(seconds)
    if type(seconds) == 'nil' then
        seconds = 0
    end

    _llbc.Thread_Sleep(seconds)
end
