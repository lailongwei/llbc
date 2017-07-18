local TestCase = {}

function TestCase.Run(...)
    print 'Util_Table test:'
    -- Test SetTableReadOnly.
    TestCase.Test_SetTableReadOnly()

    print 'Util_Table test success!'
end

function TestCase.Test_SetTableReadOnly()
    print 'Create table, and set table to readonly...'
    local testTable = {}
    llbc.SetTableReadOnly(testTable, 'testTable is readonly, could not change!')
    ok, ret = pcall(function () testTable.aaaaa = 23333 end)
    if ok == true then
        error('Util_Table test failed, SetTableReadOnly invalid!')
    end
    print 'Test set table to readonly success!'
end

return TestCase