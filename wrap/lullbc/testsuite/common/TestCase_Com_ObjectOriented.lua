local TestCase = {}

-- Create class: Foo
local Foo = llbc.newclass('Foo')

function Foo:ctor(...)
    print('Foo constructor call!')
end

function Foo:Say()
    print('Foo:Say(): ', self)
end

function Foo:Sleep()
    print('Foo:Sleep(): ', self)
end

-- Create class: Goo, inherited from 'Foo'
local Goo = llbc.newclass('Goo', Foo)

function Goo:ctor(...)
    Foo.ctor(self, ...)
    print('Goo constructor call!')
end

function Goo:Say()
    print('Goo:Say():', self)
end

function TestCase.Run()
    print('ObjectOriented test:')
    print('Foo class string repr:', Foo)
    print('Goo class(inherited from Foo) string repr: ', Goo)

    print('Create Foo objects: foo1, foo2:')
    local foo1 = Foo.new()
    local foo2 = Foo.new()
    print('foo1: ', foo1)
    print('foo2: ', foo2)

    print('Call two Foo objects Say() method:')
    foo1:Say()
    foo2:Say()

    print('Call two Foo objects Sleep() method:')
    foo1:Sleep()
    foo2:Sleep()

    print('Create Goo object:')
    local goo = Goo.new()
    print('Goo object created:', goo)

    print('Call goo object Say() method:')
    goo:Say()
    print('Call goo object Sleep() method:')
    goo:Sleep()

    print('foo1 is kind of Foo?', llbc.iskindof(foo1, Foo))
    print('foo1 is kind of Goo?', llbc.iskindof(foo1, Goo))
    print('goo is kind of Foo?', llbc.iskindof(goo, Foo))
    print('goo is kind of Goo?', llbc.iskindof(goo, Goo))


    print('ObjectOriented test success!')
end

return TestCase