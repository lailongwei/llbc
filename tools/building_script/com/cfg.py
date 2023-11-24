# -*- coding: utf-8 -*-
"""
llbc项目构建脚本脚本配置
要求调用参数:
- sys.argv[1] - 构建架构: 如x86/x64/...
- sys.argv[2] - 构建配置: debug/release
- sys.argv[3] - premake action参数: vs2022/gmake/...
"""

import os
import re
import sys
from os import path as op

from com.defs import LangType, ArchType, PlatformType
from com.log import Log

# 库版本(majorVer.minorVer.updateNo)
LIB_VER = '1.1.1'
# 库作者描述
LIB_AUTHOR = 'Longwei Lai'
# 库作者邮箱
LIB_AUTHOR_EMAIL = '964855959@qq.com'
# 文件系统编码
SYS_ENC = sys.getfilesystemencoding()


class _Cfg(object):
    """llbc构架构建配置"""
    # region __init__
    def __init__(self):
        self._license = None
        self._arch = ArchType.desc2type(sys.argv[1])
        assert(ArchType.is_valid(self._arch))
        self._is_debug = sys.argv[2].strip().lower() == 'debug'
        self._premake_action = sys.argv[3].strip()
    # endregion

    # region 平台/构建信息
    @property
    def platform(self):
        """获取平台"""
        return PlatformType.desc2type(sys.platform)

    @property
    def arch(self):
        """构架"""
        return self._arch

    @property
    def is_debug(self):
        """是否构建debug版本"""
        return self._is_debug
    # endregion

    # region 版本/license/作者信息
    @property
    def ver(self):
        """库版本"""
        return os.getenv('LLBC_LIB_VER', LIB_VER)

    @property
    def license(self):
        """license内容"""
        if not self._license:
            with open(op.join(self.sln_path, 'LICENSE'), 'r') as f:
                self._license = f.readlines()

        return self._license

    @property
    def author(self):
        """库作者"""
        return LIB_AUTHOR

    @property
    def author_email(self):
        """库作者email"""
        return LIB_AUTHOR_EMAIL

    @property
    def author_and_email(self):
        """库作者+email"""
        return self.author + '<' + self.author_email + '>'
    # endregion

    # region 目录/后缀相关
    @property
    def building_script_path(self):
        """构建脚本路径"""
        return op.dirname(op.dirname(op.abspath(__file__)))

    @property
    def sln_path(self):
        """解决方案路径"""
        return op.dirname(op.dirname(self.building_script_path))

    @property
    def output_path(self):
        """输出目录"""
        base_output_path = op.join(self.sln_path, 'output', self._premake_action)

        output_dir_name = '{}{}'.format('debug' if self.is_debug else 'release',
                                        '32' if ArchType.is_32bit_arch(self.arch) else '64')

        return op.join(base_output_path, output_dir_name)

    @property
    def exe_suffix(self):
        """可执行文件后缀"""
        return '.exe' if self.platform == PlatformType.Windows else ''

    @property
    def dll_suffix(self):
        """dll后缀"""
        pf = self.platform
        if pf == PlatformType.Windows:
            return '.dll'
        elif pf == PlatformType.Macosx:
            return '.dylib'
        else:
            return '.so'
    # endregion

    # region 核心库(llbc)相关
    @property
    def llbc_proj_path(self):
        """核心库路径"""
        return op.join(self.sln_path, 'llbc')

    @property
    def llbc_dll_path(self):
        """核心库dll路径"""
        return op.join(self.output_path,
                       'libllbc' + ('_debug' if self.is_debug else '') + self.dll_suffix)

    # endregion

    # region 核心库测试项目(testsuite)相关
    @property
    def testsuite_proj_path(self):
        """核心库测试项目路径"""
        return op.join(self.sln_path, 'testsuite')
    # endregion

    # region 包装库相关
    @property
    def wrap_projs_path(self):
        """所有包装库路径"""
        return op.join(self.sln_path, 'wrap')

    # region python包装库(pyllbc)相关
    @property
    def pyllbc_proj_path(self):
        """pyllbc项目路径"""
        return op.join(self.wrap_projs_path, 'pyllbc')

    @property
    def pyllbc_cpython_path(self):
        """cpython submodule路径"""
        return op.join(self.pyllbc_proj_path, 'cpython')

    @property
    def pyllbc_cpython_output_path(self):
        """cpython submodule输出路径"""
        if sys.platform.startswith('win32'):
            pcbuild_path = op.join(self.pyllbc_cpython_path, 'PCbuild')
            return op.join(pcbuild_path, 'amd64') \
                if ArchType.is_64bit_arch(cfg.arch) else pcbuild_path
        else:
            return self.pyllbc_cpython_path

    @property
    def pyllbc_cpython_ver(self):
        """获取cpython submodule版本"""
        cpython_path = self.pyllbc_cpython_path
        cpython_ver_match_re = re.compile(r' *#define\s+PY_VERSION\s+"(.*)"\s*$')
        ver_file = op.join(cpython_path, 'Include/patchlevel.h')
        with open(ver_file, 'r') as f:
            for line in f.readlines():
                m = cpython_ver_match_re.match(line.rstrip())
                if m:
                    return m.group(1)

        Log.e('Unable to determine the cpython version')
        return None

    @property
    def pyllbc_cpython_publish_path(self):
        """cpython submodule发布目录"""
        ver_parts = self.pyllbc_cpython_ver.split('.')
        return op.join(self.output_path, 'python' + ver_parts[0] + ver_parts[1])

    @property
    def pyllbc_dll_path(self):
        """pyllbc dll路径"""
        return op.join(self.output_path, 'llbc' +
                       ('_debug' if self.is_debug else '') + cfg.dll_suffix)
    # endregion

    # region lua包装库(lullbc)相关
    @property
    def lullbc_proj_path(self):
        """lullbc项目路径"""
        return op.join(self.wrap_projs_path, 'lullbc')

    @property
    def lullbc_lua_path(self):
        """lua submodule路径"""
        return op.join(self.lullbc_proj_path, 'lua')

    @property
    def lullbc_lua_ver(self):
        """lua submodule版本"""
        major_ver = None
        minor_ver = None
        major_match_re = re.compile(r'\s*#define\s+LUA_VERSION_MAJOR\s+"([0-9]+)"\s*')
        minor_match_re = re.compile(r'\s*#define\s+LUA_VERSION_MINOR\s+"([0-9]+)"\s*')
        with open(op.join(self.lullbc_lua_path, 'lua.h'), 'r') as f:
            for line in f.readlines():
                if major_ver is None:
                    m = major_match_re.match(line)
                    if m:
                        major_ver = m.group(1)
                elif minor_ver is None:
                    m = minor_match_re.match(line)
                    if m:
                        minor_ver = m.group(1)

        if major_ver is None or minor_ver is None:
            Log.e('Unable to determine the lua version')
            return None

        return major_ver + '.' + minor_ver

    @property
    def lullbc_lua_publish_path(self):
        """lua submodule发布目录"""
        lua_ver_parts = self.lullbc_lua_ver.split('.')
        return op.join(self.output_path, 'lua' + lua_ver_parts[0] + lua_ver_parts[1])

    # endregion

    # region c#包装库(csllbc)相关
    def csllbc_proj_path(self):
        """csllbc项目路径"""
    # endregion
    # endregion

    # region 辅助方法
    def gen_file_license_head(self, lang_type):
        """生成文件的license头"""
        if lang_type == LangType.python:
            line_prefix = '# '
        elif lang_type == LangType.lua:
            line_prefix = '-- '
        elif lang_type == LangType.cplusplus or \
                lang_type == LangType.csharp:
            line_prefix = '// '
        else:
            line_prefix = ''

        license_cnt = list(self.license)
        for i in range(len(license_cnt)):
            if i == 1:
                continue
            license_cnt[i] = line_prefix + license_cnt[i]

        return ''.join(license_cnt)
    # endregion


# 定义唯一Cfg实例
cfg = _Cfg()
