# -*- coding: utf-8 -*-
"""
pyllbc库后置构建脚本, 完成以下事情:
- 复制 核心库dll/pyllbc dll 到pyllbc的测试目录(以方便测试)
"""
import os
import shutil
from os import path as op

from com.sh import Sh
from com.cfg import cfg
from com.defs import PlatformType
from com.log import Log


def _publish_cpython():
    """发布cpython"""
    # 创建发布目录
    pub_path = cfg.pyllbc_cpython_publish_path
    if not op.exists(pub_path):
        os.mkdir(pub_path)

    # 根据平台进行具体的发布操作
    if cfg.platform == PlatformType.Windows:
        _publish_cpython_win32()
    else:
        _publish_cpython_non_win32()


def _publish_cpython_win32():
    # 复制 python bin 及 dll
    Log.d('Copy python bin & dll files')
    cpython_output_path = cfg.pyllbc_cpython_output_path
    bin_and_dlls = ['python.exe', 'python.pdb',
                    'python27.dll', 'python27.exp', 'python27.lib', 'python27.pdb']
    if cfg.is_debug:
        for idx in range(len(bin_and_dlls)):
            f_pair = op.splitext(bin_and_dlls[idx])
            bin_and_dlls[idx] = f_pair[0] + '_d' + f_pair[1]
    for f in bin_and_dlls:
        shutil.copy(op.join(cpython_output_path, f), pub_path)

    # 复制Doc/Include/Lib目录
    for d in ('Doc', 'Include', 'Lib'):
        Log.d('Copy {}/...'.format(d))
        from_dir = op.join(cfg.pyllbc_cpython_path, d)
        to_dir = op.join(pub_path, d)
        shutil.rmtree(to_dir, ignore_errors=True)
        if not op.exists(to_dir):
            shutil.copytree(from_dir, to_dir)

    # 将pyconfig.h复制到Include/目录
    Log.d('Copy pyconfig.h to pub Include/...')
    shutil.copy(op.join(cfg.pyllbc_cpython_path, 'PC', 'pyconfig.h'),
                op.join(pub_path, 'Include'))

    # 新建DLLs目录并将 llbc库&pyllbc库 放入此目录
    dlls_pub_path = op.join(pub_path, 'DLLs')
    if not op.exists(dlls_pub_path):
        Log.d('Create DLLs/...')
        os.mkdir(dlls_pub_path)

    Log.d('Copy llbc dll to DLLs/...')
    shutil.copy(cfg.llbc_dll_path, dlls_pub_path)
    Log.d('Copy pyllbc dll to DLLs/...')
    shutil.copy(cfg.pyllbc_dll_path, dlls_pub_path)

    # 新建libs/Scripts目录
    for d in ('libs', 'Scripts'):
        to_dir = op.join(pub_path, d)
        if not op.exists(to_dir):
            Log.fd('Create {}/...', d)
            os.mkdir(to_dir)


def _publish_cpython_non_win32():
    site_packages_path = op.join(cfg.pyllbc_cpython_publish_path, 'lib/python2.7/site-packages')
    Sh.execute('mkdir -p "{}"'.format(site_packages_path))

    # 复制llbc dll 及 pyllbc dll到python site-packages/ 目录
    Log.d('Copy llbc corelib dll to python site-packages...')
    shutil.copy(cfg.llbc_dll_path, site_packages_path)
    Log.d('Copy pyllc dll to python site-packages...')
    shutil.copy(cfg.pyllbc_dll_path, site_packages_path)


def main():
    # 发布cpython
    Log.fi('Publish cpython[to:{}]...', cfg.pyllbc_cpython_publish_path)
    _publish_cpython()
    Log.i('Done!')


if __name__ == '__main__':
    main()
