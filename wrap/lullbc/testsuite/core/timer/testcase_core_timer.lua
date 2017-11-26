local TestCase = {}

function TestCase.run(...)
    -- TestCase.standard_test()
    TestCase.performance_test()
end

function TestCase.standard_test()
    print('Timer test:')

    print("Set timer error handler")
    llbc.TimerScheduler.set_timer_errorhandler(function (timer, eventType, errMsg)
        print('Timer error, timer:', timer, ', event type:', eventType, ', errMsg:\n', errMsg)
    end)

    print('Create timer:')
    local timer = llbc.Timer.new(TestCase.ontimeout)
    print('timer created:', timer)

    print('Schedule timer')
    timer:schedule(1)

    print('Begin schedule timers...')
    while true do
        llbc.TimerScheduler.update()
        llbc.sleep(0.02)
    end
end

function TestCase.performance_test()
    print('Timer performance test:')

    TestCase.timeout_total_call = 0

    print("Set timer error handler")
    llbc.TimerScheduler.set_timer_errorhandler(function (timer, eventType, errMsg)
        print('Timer error, timer:', timer, ', event type:', eventType, ', errMsg:\n', errMsg)
    end)

    local test_timercount = 2000000
    local allTimers = {}
    print(string.format('Create %d timers for test:', test_timercount))
    for i = 1, test_timercount do
        local timer = llbc.Timer.new(TestCase.onperf_timeout)
        timer:schedule(math.random(5, 15))

        table.insert(allTimers, #allTimers + 1, timer)
    end

    print('Begin schedule timers...')
    while true do
        llbc.TimerScheduler.update()
        llbc.sleep(0.02)
    end
end

function TestCase.ontimeout(timer)
    print('hello, I am timer:', timer, ', I am timeout!')
    -- TestCase.call_raiseerror()
    return false
end

function TestCase.onperf_timeout(timer)
    TestCase.timeout_total_call = TestCase.timeout_total_call + 1
    if TestCase.timeout_total_call % 10000 == 0 then
        print(string.format('onperf_timeout() call times(%d) mod 10000 == 0, trigger timer: %s', TestCase.timeout_total_call, timer))
    end
    return true
end

function TestCase.call_raiseerror()
    TestCase.raiseerror()
end

function TestCase.raiseerror()
    error('Yep, this is a error message, for test timer error handler')
end

return TestCase