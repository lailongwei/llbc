local TestCase = {}

function TestCase.run()
    print('thread test:')
    TestCase.test_sleep()
end

function TestCase.test_sleep()
    print('sleep test:')
    
    local sleep = llbc.sleep
    print('sleep 2 seconds...')
    sleep(2)
    print('done!')

    print('sleep 0.5 seconds...')
    sleep(0.5)
    print('done!')

    print('sleep 0.01 seconds...')
    sleep(0.001)
    print('done!')

    print('sleep 0 seconds...')
    sleep(0)
    print('done!')
end

return TestCase