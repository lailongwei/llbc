# -*- coding: utf-8 -*-

"""
llbc核心库预构建脚本
"""

import re
from os import path as op

from com.cfg import cfg
from com.log import Log


def main():
    # 更新Version.cpp中的版本号
    cfg_ver = cfg.ver
    Log.i('Update llbc lib version number[{}]...'.format(cfg_ver))

    lib_ver_parts = [-1, -1, -1]
    cfg_ver_parts = [int(part) for part in cfg_ver.split('.')]
    re_expr = r'(^\s*int\s+#var_name#\s*=\s*)([0-9]+)(\s*;\s+$)'
    major_re = re.compile(re_expr.replace('#var_name#', 'LLBC_majorVersion'))
    minor_re = re.compile(re_expr.replace('#var_name#', 'LLBC_minorVersion'))
    update_no_re = re.compile(re_expr.replace('#var_name#', 'LLBC_updateNo'))
    ver_cpp_file = op.join(cfg.llbc_proj_path, 'src', 'common', 'Version.cpp')
    with open(ver_cpp_file, 'r') as f:
        lines = f.readlines()

        def _match_ver_part(line_idx, compiled_re, ver_part_idx):
            if lib_ver_parts[ver_part_idx] != -1:
                return False

            m = compiled_re.match(lines[line_idx])
            if not m:
                return False

            lib_ver_parts[ver_part_idx] = int(m.group(2))
            if lib_ver_parts[ver_part_idx] != cfg_ver_parts[ver_part_idx]:
                lines[line_idx] = m.group(1) + str(cfg_ver_parts[ver_part_idx]) + m.group(3)
            return True

        for idx in range(len(lines)):
            if not _match_ver_part(idx, major_re, 0):
                if not _match_ver_part(idx, minor_re, 1):
                    _match_ver_part(idx, update_no_re, 2)

    if -1 in lib_ver_parts:
        Log.fe('Parse version cpp file failed, file:{}', ver_cpp_file)
        return
    elif lib_ver_parts == cfg_ver_parts:
        Log.i('The version number is the same, no need to update')
        return

    lib_ver = '.'.join(str(ver_part) for ver_part in lib_ver_parts)
    Log.i('The llbc lib version number is different[lib ver:{}, cfg ver:{}], execute update'.format(
        lib_ver, cfg_ver))

    with open(ver_cpp_file, 'w') as f:
        f.writelines(lines)

    Log.i('Done!')


if __name__ == '__main__':
    main()
