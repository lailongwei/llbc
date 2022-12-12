local TestCase = {}
local Log = llbc.Log

function TestCase.run(...)
    print 'Log test:'

    print('Call Log.isinit():', Log.isinit())
    print('Before log init, output some message...')
    Log.d('Not init debug message')
    Log.d2('test', 'Not init debug message')
    Log.d3('test', 'TestTag', 'Not init debug message')

    -- Initialize Log component.
    print 'Initialize log component...'
    Log.init('LogTestCfg.cfg', false)
    print('Call Log.isinit():', Log.isinit())

    -- Base test.
    TestCase.base_test()
    -- Performance test.
    TestCase.perf_test()
end

function TestCase.base_test()
    Log.i('Base test:')

    -- Test base log methods(all messages log to root logger).
    print 'Test base log methods, all messages log to root logger...'
    Log.d('Hello, this is a DEBUG level message...')  -- DEBUG level message, log to root logger.
    Log.i('Hello, this is a INFO level message...')  -- INFO level message, log to root logger.
    Log.w('Hello, this is a WARN level message...')  -- WARN level message, log to root logger.
    Log.e('Hello, this is a ERROR level message...')  -- ERROR level message, log to root logger.
    -- Log.f('Hello, this is a FATAL level message...')  -- FATAL level message, log to root logger.

    -- Test log to specific logger methods, all methods named Log.x2(logger, ...).
    print 'Test log to specific logger log methods, all messages log to specific logger...'
    Log.d2('test', 'Hello, I will write to test logger...') -- Write DEBUG level message  to 'test' logger.
    Log.i2('test', 'Hello, I will write to test logger...') -- Write INFO level message  to 'test' logger.
    Log.w2('test', 'Hello, I will write to test logger...') -- Write WARN level message  to 'test' logger.
    Log.e2('test', 'Hello, I will write to test logger...') -- Write ERROR level message  to 'test' logger.
    -- Log.f2('test', 'Hello, I will write to test logger...') -- Write FATAL level message  to 'test' logger.

    -- Test log to specific logger methods(appended tag), all methods named Log.x3(logger, tag, ...).
    -- logger name can be nil, if logger name is nil, message will log to root logger.
    print 'Test log to specific logger log methods(appended tag), all messages log to specific logger...'
    Log.d3('test', 'TestTag', 'Hello, I will write to test logger...') -- Write DEBUG level message  to 'test' logger.
    Log.i3('test', 'TestTag', 'Hello, I will write to test logger...') -- Write INFO level message  to 'test' logger.
    Log.w3('test', 'TestTag', 'Hello, I will write to test logger...') -- Write WARN level message  to 'test' logger.
    Log.e3('test', 'TestTag', 'Hello, I will write to test logger...') -- Write ERROR level message  to 'test' logger.
    -- Log.f3('test', 'TestTag', 'Hello, I will write to test logger...') -- Write FATAL level message  to 'test' logger.

    -- Test log messages unknown logger.
    print 'Test log message to unknown logger, root logger will take over these messages...'
    Log.d2('unknown_logger', 'Hello, I am a log to "unknown_logger" logger DEBUG message...')
    Log.i2('unknown_logger', 'Hello, I am a log to "unknown_logger" logger DEBUG message...')
    Log.w2('unknown_logger', 'Hello, I am a log to "unknown_logger" logger DEBUG message...')
    Log.d2('unknown_logger', 'Hello, I am a log to "unknown_logger" logger DEBUG message...')
    Log.w2('unknown_logger', 'Hello, I am a log to "unknown_logger" logger DEBUG message...')

    Log.i('Base test finish!\n')
end

function TestCase.perf_test(logger)
    Log.i('Begin perf test:')

    local testTimes = 1000000
    local begTestTime = llbc.Chrono.utc_microseconds()
    for i = 1, testTimes do
        if logger == nil then
            -- Log.t2('perftest', string.format('Test debug log, times:%d', i))
            -- Log.d2('perftest', string.format('Test debug log, times:%d', i))
            -- Log.i2('perftest', string.format('Test info log, times:%d', i))
            Log.t2('perftest', 'Test trace log, times')
            Log.d2('perftest', 'Test debug log, times')
            Log.i2('perftest', 'Test info log, times')
        else
        end
    end

    local costTime = llbc.Chrono.utc_microseconds() - begTestTime
    Log.i(string.format('Perf test finish, log times: %d, cost time:%s us, per-time log cost: %.3f us',
          testTimes * 3,
          costTime,
          costTime / (testTimes * 3)))
end

return TestCase