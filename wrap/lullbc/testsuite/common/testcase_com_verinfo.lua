local TestCase = {}

function TestCase.run(...)
    print 'VersionInfo test:'
    print 'Test get_versioninfo(verbose=false):'
    local brief_verinfo = llbc.get_versioninfo()
    print(brief_verinfo)

    print 'Test get_versioninfo(verbose=true):'
    verbose_verinfo = llbc.get_versioninfo(true)
    print(verbose_verinfo)

    if string.len(brief_verinfo) == 0 or 
            string.len(verbose_verinfo) <= string.len(brief_verinfo) then
        error('VersionInfo test failed!')
    end

    print 'VersionInfo test success!'
end

return TestCase

