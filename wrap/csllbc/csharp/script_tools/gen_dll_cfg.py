# -*- coding: utf-8 -*-
"""
FILE    gen_dll_cfg.py
AUTHOR  Longwei Lai<lailongwei@126.com>
DATE    2016/09/25
VERSION 1.0
BRIEF   Auto generate csharp dll config file(need in mono environment).
"""

import sys
from os import path as op

_CS_LIB_NAME = 'csllbc'
_CS_NATIVE_LIB_NAME = 'libcsllbc_native'

def main():
    pf = sys.platform
    if pf == 'win32':
        return

    print 'Create csharp dll dllmap config file...'

    if pf == 'darwin':
        targetName = _CS_NATIVE_LIB_NAME + '.dylib'
    elif pf.startswith('linux'):
        targetName = _CS_NATIVE_LIB_NAME + '.so'
    else:
        targetName = _CS_NATIVE_LIB_NAME + '.dll'

    outputFile = op.join(sys.argv[1], '{}.dll.config'.format(_CS_LIB_NAME))

    with open(outputFile, 'w') as f:
        f.write('<!-- Map llbc csharp wrap native library name at {} platform -->\n'.format(pf))
        f.write('\n')
        f.write('<configuration>\n')
        f.write('  <dllmap dll="{}.dll" target="{}"/>\n'.format(_CS_NATIVE_LIB_NAME, targetName))
        f.write('</configuration>\n')
    
if __name__ == '__main__':
    main()
