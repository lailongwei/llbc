# -*- coding: utf-8 -*-
"""
 lullbc编译后执行脚本
"""
import sys
from os import path as op

import shutil

from c import Cfg


def main():
    # 复制llbc.lua到目标目录
    target_path = sys.argv[3]
    is_debug = sys.argv[2].upper() == 'DEBUG' or False
    src = op.join(Cfg.getscriptpath(), 'llbc.lua')
    if not is_debug:
        target = op.join(target_path, 'llbc.lua')
    else:
        target = op.join(target_path, 'llbc_debug.lua')

    print 'Copy [{0}] to [{1}]...'.format(src, target)
    shutil.move(src, target)

    print 'Done!'

if __name__ == '__main__':
    main()
