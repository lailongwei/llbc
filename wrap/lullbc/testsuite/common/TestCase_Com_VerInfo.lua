local TestCase = {}

function TestCase.Run(...)
    print 'VersionInfo test:'
    print 'Test GetVersionInfo(verbose=false):'
    briefVerInfo = llbc.GetVersionInfo()
    print(briefVerInfo)

    print 'Test GetVersionInfo(verbose=true):'
    verboseVerInfo = llbc.GetVersionInfo(true)
    print(verboseVerInfo)

    if string.len(briefVerInfo) == 0 or 
            string.len(verboseVerInfo) <= string.len(briefVerInfo) then
        error('VersionInfo test failed!')
    end

    print 'VersionInfo test success!'
end

return TestCase

