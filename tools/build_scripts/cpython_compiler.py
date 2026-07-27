# -*- coding: utf-8 -*-
"""
cpython submodule编译器
"""

from os import path as op

from com.sh import Sh
from com.cfg import cfg
from com.log import Log
from com.defs import ArchType, PlatformType


class CPythonCompiler(object):
    """cpython submodule编译器"""
    def compile(self):
        if cfg.platform == PlatformType.Windows:
            self._compile_cpython_in_win32()
        else:
            self._compile_cpython_in_non_win32()

    @staticmethod
    def _compile_cpython_in_win32():
        Log.i('Upgrade pcbuild.sln...')
        pcbuild_path = op.join(cfg.pyllbc_cpython_path, 'PCbuild')
        if op.exists(op.join(pcbuild_path, 'UpgradeLog.htm')):
            Log.i('pcbuild.sln has been upgraded, skip...')
        else:
            ret = Sh.execute('pushd "{}" && "{}" pcbuild.sln /Upgrade && popd'.format(pcbuild_path, cfg.devenv_path))
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

        if cfg.arch == ArchType.x86:
            platform = 'Win32'
        elif cfg.arch == ArchType.x86_64:
            platform = 'x64'
        elif cfg.arch == ArchType.arm64:
            platform = 'ARM64'
        elif cfg.arch == ArchType.arm:
            platform = 'ARM'
        else:
            Log.e('Unsupported arch:{}'.format(cfg.arch))
            return

        configuration = 'Debug' if cfg.is_debug else 'Release'

        Log.i('Compile cpython, platform:{}, configuration:{}...'.format(platform, configuration))
        ret = Sh.execute(
            'pushd "{}" && "{}" pcbuild.sln /Build "{}|{}" /project python && popd'.format(
                pcbuild_path, cfg.devenv_path, configuration, platform))
        if ret != 0:
            Log.e('Compile cpython failed, ret code:{}'.format(ret))

    def _compile_cpython_in_non_win32(self):
        # 当前llbc使用的python不支持非x86架构
        if cfg.arch not in (ArchType.x86, ArchType.x86_64):
            raise Exception('For now, the architecture in llbc framework embedded python '
                            'is unsupported, arch:{}'.format(ArchType.type2desc(cfg.arch)))

        # 定义cpython配置选项
        # Note：
        # - CPython 启用 --enable-shared 编译，运行时需要编辑器 llbc 框架编译的 cpython so
        #   故通过 LDFLAGS 传入 rpath, 确保运行 python exec 时能正确加载 cpython so
        #
        #  rpath指定方式:
        #   - Linux (ELF):  $ORIGIN         由 glibc ld.so 展开为 exec/so 所在目录
        #   - macOS (Mach-O): @executable_path  由 dyld 展开为可执行文件所在目录
        if cfg.platform == PlatformType.Darwin:
            # macOS: @executable_path 由 dyld 在加载时展开
            ldflags_opt = "LDFLAGS='-Wl,-rpath,@executable_path/../lib'"
        else:
            # Linux: $ORIGIN 由 ld.so 在加载时展开；此处 \$$ORIGIN 是为了穿过
            # Python 字符串 + shell 两层解析后，最终传给 configure 的仍是 $ORIGIN。
            ldflags_opt = "LDFLAGS='-Wl,-rpath,\\$$ORIGIN/../lib'"

        cpython_config_opts = [
            '--enable-shared',
            'CFLAGS="-Wno-register"',
            ldflags_opt,
            '--prefix="{}"'.format(cfg.pyllbc_cpython_publish_path)
        ]

        if cfg.is_debug:
            cpython_config_opts.append('--with-pydebug')
        # else:
        #     cpython_config_opts.append('--enable-optimizations')

        # 配置cpython
        Log.fi('Configure cpython, isdebug:{}...', cfg.is_debug)
        ret = Sh.execute('cd "{}" && ./configure {} > /dev/null'.format(cfg.pyllbc_cpython_path,
                                                                        ' '.join(cpython_config_opts)))
        if ret != 0:
            Log.fe('Configure cpython failed, ret code:{}', ret)

        # 编译cpython(使用当前机器核数的一半来进行编译)
        Log.i('Compile cpython...')
        ret = Sh.execute('cd "{}" && make -j$([[ `nproc` -gt 1 ]] && echo -n $((`nproc` / 2)) || echo -n 1)'
                         .format(cfg.pyllbc_cpython_path))
        if ret != 0:
            Log.fe('Compile cpython failed, ret code:{}', ret)

        # 发布cpython(到编译目录)
        ret = Sh.execute('cd "{}" && make install > /dev/null'.format(cfg.pyllbc_cpython_path))
        if ret != 0:
            Log.fe('Install cpython failed, ret code:{}'.format(ret))