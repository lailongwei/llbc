# -*- coding: utf-8 -*-
"""
cpython submodule编译器
"""

from os import path as op

from com.cfg import cfg
from com.defs import ArchType, PlatformType
from com.log import Log
from com.sh import Sh


class CPythonCompiler(object):
    """cpython submodule编译器"""
    def compile(self, arch, is_debug, output_dir):
        if cfg.platform == PlatformType.Windows:
            self._compile_cpython_in_win32(arch, is_debug)
        else:
            self._compile_cpython_in_non_win32(arch, is_debug)

    @staticmethod
    def _compile_cpython_in_win32(arch, is_debug):
        Log.i('Upgrade pcbuild.sln...')
        pcbuild_path = op.join(cfg.pyllbc_cpython_path, 'PCbuild')
        if op.exists(op.join(pcbuild_path, 'UpgradeLog.htm')):
            Log.i('pcbuild.sln has been upgraded, skip...')
        else:
            ret = Sh.execute('pushd "{}" && devenv.exe pcbuild.sln /Upgrade && popd'.format(pcbuild_path))
            if ret != 0:
                Log.e('Upgrade pcbuild.sln failed, ret code:{}'.format(ret))

        Log.i('Fix cpython compile error...')
        time_mod_fpath = op.join(cfg.pyllbc_cpython_path, 'Modules/timemodule.c')
        with open(time_mod_fpath, 'r') as f:
            lines = f.readlines()
        for idx in range(len(lines)):
            line = lines[idx]
            line = line.replace(', timezone', ', _timezone')
            line = line.replace(', daylight', ', _daylight')
            line = line.replace(', tzname', ', _tzname')
            lines[idx] = line
        with open(time_mod_fpath, 'w') as f:
            f.writelines(lines)

        posix_mod_fpath = op.join(cfg.pyllbc_cpython_path, 'Modules/posixmodule.c')
        with open(posix_mod_fpath, 'r') as f:
            lines = f.readlines()
        if lines[577].find('const int i1') != -1:
            lines[577:609] = ['    return _get_osfhandle(fd) == INVALID_HANDLE_VALUE ? 0 : 1;\n']
            with open(posix_mod_fpath, 'w') as f:
                f.writelines(lines)

        if arch == ArchType.x86:
            platform = 'Win32'
        elif arch == ArchType.x64:
            platform = 'x64'
        else:
            Log.e('Unsupported arch:{}'.format(arch))
            return

        configuration = 'Debug' if is_debug else 'Release'

        Log.i('Compile cpython, platform:{}, configuration:{}...'.format(platform, configuration))
        ret = Sh.execute(
            'pushd "{}" && msbuild pcbuild.sln \
    /t:python /p:Platform={} /p:Configuration={} /p:ToolsVersion=Latest && popd'.format(
                pcbuild_path, platform, configuration))
        if ret != 0:
            Log.e('Compile cpython failed, ret code:{}'.format(ret))

    def _compile_cpython_in_non_win32(self, arch, is_debug):
        pass