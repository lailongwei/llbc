local TestCase = {}

function TestCase.run(...)
    print 'Log test:'
    -- Initialize Log component
    print 'Initialize log component...'

    local Log = llbc.Log
    Log.Init('core/log/Logger_Cfg.cfg')

    -- Test base log methods(all messages log to root logger).
    print 'Test base log methods, all messages log to root logger...'
    Log.d('Hello, this is a DEBUG level message...')  -- DEBUG level message, log to root logger.
    Log.i('Hello, this is a INFO level message...')  -- INFO level message, log to root logger.
    Log.w('Hello, this is a WARN level message...')  -- WARN level message, log to root logger.
    Log.e('Hello, this is a ERROR level message...')  -- ERROR level message, log to root logger.
    Log.f('Hello, this is a FATAL level message...')  -- FATAL level message, log to root logger.

    -- Test log to specific logger methods, all methods named Log.x2(logger, ...).
    print 'Test log to specific logger log methods, all messages log to specific logger...'
    Log.d2('test', 'Hello, I will write to test logger...') -- Write DEBUG level message  to 'test' logger.
    Log.i2('test', 'Hello, I will write to test logger...') -- Write INFO level message  to 'test' logger.
    Log.w2('test', 'Hello, I will write to test logger...') -- Write WARN level message  to 'test' logger.
    Log.e2('test', 'Hello, I will write to test logger...') -- Write ERROR level message  to 'test' logger.
    Log.f2('test', 'Hello, I will write to test logger...') -- Write FATAL level message  to 'test' logger.

    -- Test log to specific logger methods(appended tag), all methods named Log.x3(logger, tag, ...).
    -- logger name can be nil, if logger name is nil, message will log to root logger.
    print 'Test log to specific logger log methods(appended tag), all messages log to specific logger...'
    Log.d3('test', 'TestTag', 'Hello, I will write to test logger...') -- Write DEBUG level message  to 'test' logger.
    Log.i3('test', 'TestTag', 'Hello, I will write to test logger...') -- Write INFO level message  to 'test' logger.
    Log.w3('test', 'TestTag', 'Hello, I will write to test logger...') -- Write WARN level message  to 'test' logger.
    Log.e3('test', 'TestTag', 'Hello, I will write to test logger...') -- Write ERROR level message  to 'test' logger.
    Log.f3('test', 'TestTag', 'Hello, I will write to test logger...') -- Write FATAL level message  to 'test' logger.

    -- Test log messages unknown logger.
    print 'Test log message to unknown logger, root logger will take over these messages...'
    Log.d2('unknown_logger', 'Hello, I am a log to "unknown_logger" logger DEBUG message...')
    Log.i2('unknown_logger', 'Hello, I am a log to "unknown_logger" logger DEBUG message...')
    Log.w2('unknown_logger', 'Hello, I am a log to "unknown_logger" logger DEBUG message...')
    Log.d2('unknown_logger', 'Hello, I am a log to "unknown_logger" logger DEBUG message...')
    Log.w2('unknown_logger', 'Hello, I am a log to "unknown_logger" logger DEBUG message...')

    print 'Log test success!'
end

return TestCase