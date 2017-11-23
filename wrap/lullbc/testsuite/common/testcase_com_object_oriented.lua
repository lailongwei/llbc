local TestCase = {}

function TestCase.run()
    -- Create class: Foo
    print('Try to create class \'Foo\'')
    local Foo = llbc.newclass('Foo')
    
    function Foo:ctor(...)
        print('Foo constructor call!')
    end
    
    function Foo:say()
        print('Foo:say(): ', self)
    end
    
    function Foo:sleep()
        print('Foo:sleep(): ', self)
    end
    
    -- Create class: Goo, inherited from 'Foo'
    print('Try to create class \'Goo\', inherit from \'Foo\'')
    local Goo = llbc.newclass('Goo', Foo)
    
    function Goo:ctor(...)
        Foo.ctor(self, ...)
        print('Goo constructor call!')
    end
    
    function Goo:say()
        print('Goo:say():', self)
    end
    
    -- Try repeat create class: GooToo, inherited from 'Foo'
    print('Try create GooToo class, name is \'Goo\'')
    local succ, GooToo = pcall(llbc.newclass, 'Goo', Foo)
    if succ then
        error('Test failed, please check object oriented about code')
    else
        print('Yep, llbc library not allow user to create same name classes in the same lua vm')
        print(string.format('Create \'GooToo\' return message: %s', GooToo))
    end

    print('ObjectOriented test:')
    print('Foo class string repr:', Foo)
    print('Goo class(inherited from Foo) string repr: ', Goo)

    local FooToo = llbc.getclass('Foo')
    print('Call llbc.getclass(\'Foo\') return: ', FooToo)
    local GooToo = llbc.getclass('Goo')
    print('Call llbc.getclass(\'Goo\') return: ', GooToo)

    print('Create Foo objects: foo1, foo2:')
    local foo1 = Foo.new()
    local foo2 = Foo.new()
    print('foo1: ', foo1)
    print('foo2: ', foo2)

    print('Call two Foo objects say() method:')
    foo1:say()
    foo2:say()

    print('Call two Foo objects sleep() method:')
    foo1:sleep()
    foo2:sleep()

    print('Create Goo object:')
    local goo = Goo.new()
    print('Goo object created:', goo)

    print('Call goo object say() method:')
    goo:say()
    print('Call goo object sleep() method:')
    goo:sleep()

    print('foo1 is kind of Foo?', llbc.isinstance(foo1, Foo))
    print('foo1 is kind of Goo?', llbc.isinstance(foo1, Goo))
    print('goo is kind of Foo?', llbc.isinstance(goo, Foo))
    print('goo is kind of Goo?', llbc.isinstance(goo, Goo))
    print('foo1 is kind of (Foo, Goo)?', llbc.isinstance(foo1, {Foo, Goo}))
    print('goo is kind of (Foo, Goo)?', llbc.isinstance(goo, {Foo, Goo}))

    print('true is kind of boolean?', llbc.isinstance(true, 'boolean'))
    print('3 is kind of number?', llbc.isinstance(3, 'number'))
    print('3 is kind of boolean?', llbc.isinstance(3, 'boolean'))
    print('3 is kind of (boolean, number)?', llbc.isinstance(3, {'boolean', 'number'}))
    print('{} is kind of table?', llbc.isinstance({}, 'table'))
    print('{} is kind of (table)?', llbc.isinstance({}, {'table'}))
    print('{} is kind of (table, boolean)?', llbc.isinstance({}, {'table', 'boolean'}))
    print('nil is kind of nil?', llbc.isinstance(nil, 'nil'))
    print('nil is kind of (nil)?', llbc.isinstance(nil, {'nil'}))

    print('objecr oriented test success!')
end

return TestCase
