# -*- coding: utf-8 -*-
"""
脚本工具相关types def
"""


class PlatformType(object):
    """平台类型"""
    Windows = 1
    Linux = 2
    Darwin = 3
    Unknown = 4

    @classmethod
    def type2desc(cls, plat_type):
        """平台类型转描述"""
        if plat_type == cls.Windows:
            return 'windows'
        elif plat_type == cls.Linux:
            return 'linux'
        elif plat_type == cls.Darwin:
            return 'darwin'
        else:
            return 'unknown'

    @classmethod
    def desc2type(cls, plat_desc):
        """平台描述转类型"""
        if plat_desc == 'windows' or plat_desc.startswith('win32'):
            return cls.Windows
        elif plat_desc.startswith('linux'):
            return cls.Linux
        elif plat_desc == 'macosx' or plat_desc == 'darwin':
            return cls.Darwin
        else:
            return cls.Unknown


class LangType(object):
    """语言类型"""
    python = 'python'
    lua = 'lua'
    cplusplus = 'cplusplus'
    csharp = 'csharp'
    unknown = 'unknown'


class ProjType(object):
    """项目类型"""
    llbc = 'llbc'
    testsuite = 'testsuite'
    pyllbc = 'pyllbc'
    lullbc = 'lullbc'
    csllbc = 'csllbc'
    unknown = 'unknown'


class ArchType(object):
    """架构类型"""
    x86 = 1
    x86_64 = 2
    ARM = 3
    ARM64 = 4
    Unknown = 5

    @classmethod
    def desc2type(cls, desc):
        """架构描述转换成类型"""
        desc = desc.strip().lower()
        if desc == 'x86':
            return cls.x86
        elif desc == 'x64' or desc == 'x86_64':
            return cls.x86_64
        elif desc == 'arm':
            return cls.ARM
        elif desc == 'arm64':
            return cls.ARM64
        else:
            return cls.Unknown

    @classmethod
    def type2desc(cls, arch_type):
        """架构类型转架构描述"""
        if arch_type == cls.x86:
            return 'x86'
        elif arch_type == cls.x86_64:
            return 'x86_64'
        elif arch_type == cls.ARM:
            return 'ARM'
        elif arch_type == cls.ARM64:
            return 'ARM64'
        else:
            return 'unknown'

    @classmethod
    def is_valid(cls, arch_type):
        """确认架构类型是否有效"""
        return arch_type == cls.x86 or \
            arch_type == cls.x86_64 or \
            arch_type == cls.ARM or \
            arch_type == cls.ARM64

    @classmethod
    def is_32bit_arch(cls, arch_type):
        """确认是否是32位架构"""
        return arch_type in (cls.x86, cls.ARM)

    @classmethod
    def is_64bit_arch(cls, arch_type):
        """确认是否是64位架构"""
        return arch_type in (cls.x86_64, cls.ARM)

