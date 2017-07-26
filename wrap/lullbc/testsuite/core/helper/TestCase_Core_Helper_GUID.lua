local TestCase = {}

function TestCase.Run(...)
    print('Core/Helper/GUIDHelper test:')

    local genCount = 100
    print('Generate 100 GUIDs...')
    for i = 1,genCount do
        print(tostring(i) .. ': ' .. llbc.GUID.generate())
    end

    print('GUID test success!')
end

return TestCase