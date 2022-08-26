local TestCase = {}

function TestCase.run()
    print('util_string test:')
    TestCase.test_hash()
end

function TestCase.test_hash()
    print('hash() test:')

    local hash = llbc.hash
    print('hash(nil):', hash(nil))
    print('hash(true):', hash(true))
    print('hash(false):', hash(false))
    print('hash(10086):', hash(10086))
    print('hash(1234.5678):', hash(1234.5678))
    print("hash('hello world'):", hash('hello world'))
    print("hash('hey judy'):", hash('hey judy'))
end

return TestCase