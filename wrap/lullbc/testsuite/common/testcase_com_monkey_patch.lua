local TestCase = {}

function TestCase.run()
    print('monkey patch test:')

    print('before monkey patch:')
    TestCase.print_patchinfo()
    print('')

    print('do monkey patch..')
    llbc.monkey_patch()
    
    print('after monkey patch:')
    TestCase.print_patchinfo()

    print('call all patched functions')
    print("  table.concat({'a', 'b', 3, {}}, ', ') return:", 
        table.concat({'a', 'b', 3, {}}, ', '))
    print('  topointer(1) return: ', topointer(1))
    print('  topointer({}) return: ', topointer({}))
    print("  printf('%s %d', 'hello', 3) return:")
    printf('    %s %d', 'hello', 3)
    printf('  newclass: %s', newclass)
    printf('  getclass: %s', getclass)
    printf('  isinstance: %s', isinstance)

    print('monkey patch test success!')
end

function TestCase.print_patchinfo()
    print(string.format('  table.concat: %s', table.concat))
    print(string.format('  table.readonly: %s', table.readonly))

    print(string.format('  hash: %s', hash))
    print(string.format('  topointer: %s', topointer))
    print(string.format('  printf: %s', printf))
    print(string.format('  GUID: %s', GUID))
    print(string.format('  FuncTool: %s', FuncTool))

    print(string.format('  newclass: %s', newclass))
    print(string.format('  getclass: %s', getclass))
    print(string.format('  isinstance: %s', isinstance))

    print(string.format('  Log: %s', Log))

    print(string.format('  Timer: %s', Timer))
    print(string.format('  TimerScheduler: %s', TimerScheduler))
end

return TestCase
