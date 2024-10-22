local TestCase = {}

function TestCase.run(...)
    print('guid test:')

    local gen_count = 100
    print('Generate 100 GUIDs...')
    for i = 1,gen_count do
        print(tostring(i) .. ': ' .. llbc.GUID.generate())
    end

    print('GUID test success!')
end

return TestCase