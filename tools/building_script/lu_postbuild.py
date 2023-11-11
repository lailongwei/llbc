# -*- coding: utf-8 -*-
"""
 lullbc编译后执行脚本
"""
import os
from os import path as op

import shutil

from com.cfg import cfg
from com.defs import PlatformType
from com.log import Log


def _publish_lua():
    """发布lua submodule到output目录"""
    # 创建发布目录
    pub_path = cfg.lullbc_lua_publish_path
    Log.fd('Create lua publish path:{}...', pub_path)
    if not op.exists(pub_path):
        os.mkdir(pub_path)

    # 定义将copy文件标准化内部函数
    def _normalize_files(files):
        if not cfg.is_debug:
            return files

        nml_files = []
        for idx in range(len(files)):
            f_pair = op.splitext(files[idx])
            nml_files.append(f_pair[0] + '_debug' + f_pair[1])
        return nml_files

    # 复制lua exe & dll
    will_pub_files = ['lua' + cfg.exe_suffix, 'liblua' + cfg.dll_suffix]
    if cfg.platform == PlatformType.Windows:
        will_pub_files.extend(['lua.pdb', 'liblua.lib', 'liblua.pdb'])
    Log.d('Copy lua exe & dll to publish path...')
    for f in _normalize_files(will_pub_files):
        shutil.copy(op.join(cfg.output_path, f), pub_path)

    # 复制llbc & lullbc到发布目录
    Log.d('Copy llbc & pyllbc files to publish path...')
    for f in _normalize_files(['libllbc' + cfg.dll_suffix,
                               '_lullbc' + cfg.dll_suffix,
                               'llbc.lua']):
        shutil.copy(op.join(cfg.output_path, f), pub_path)


def main():
    # 复制llbc.lua到目标目录
    script_path = op.join(cfg.lullbc_proj_path, 'script')
    src = op.join(script_path, 'llbc.lua')
    target = op.join(cfg.output_path, 'llbc.lua' if not cfg.is_debug else 'llbc_debug.lua')
    Log.i('Copy [{0}] to [{1}]...'.format(src, target))
    shutil.copy(src, target)

    # 发布lua
    Log.fi('Publish lua[to:{}]...', cfg.lullbc_lua_publish_path)
    _publish_lua()


if __name__ == '__main__':
    main()
