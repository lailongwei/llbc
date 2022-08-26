local TestCase = {}

function TestCase.run(...)
    print 'util_table test:'
    -- Test set_table_readonly
    TestCase.test_settable_readonly()

    print 'Util_Table test success!'
end

function TestCase.test_settable_readonly()
    print 'Create table, and set table to readonly...'
    local testtable = {}
    llbc.set_table_readonly(testtable, 'testtable is readonly, could not change!')
    local ok, ret = pcall(function () testtable.aaaaa = 23333 end)
    if ok == true then
        error('Util_Table test failed, set_table_readonly invalid!')
    end
    print 'Test set table to readonly success!'
end

return TestCase