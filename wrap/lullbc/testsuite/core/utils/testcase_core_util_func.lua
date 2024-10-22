local TestCase = {}

function TestCase.run()
    print('util_func test:')
    TestCase.test_partial()

    print('util_func test success!')
end

function TestCase.test_partial()
    print('partial test:')

    print('define foo function: foo(a, b, c)')
    local foo = function(a, b, c)
        print('foo called, arguments:', a, b, c)
    end

    print('call foo(1, 2, 3)...')
    foo(1, 2, 3)

    -- test partial one argument.
    print('do partial a = 3, after partial, function name: goo')
    local goo = llbc.FuncTool.partial(foo, 3)
    print('call goo(4, 5)...')
    goo(4, 5)

    -- test partial two arguments.
    print('do partial a = 10, b = 20, after partial, function name: goo_two')
    local goo_two = llbc.FuncTool.partial(foo, 10, 20)
    print('call goo_two(30)...')
    goo_two(30)

    print('partial test success!')
end

return TestCase
