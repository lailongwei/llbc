# -*- coding: utf-8 -*-
"""
脚本工具相关types def
"""


class PlatformType(object):
    """平台类型"""
    Windows = 1
    Linux = 2
    Macosx = 3
    Unknown = 4

    @classmethod
    def type2desc(cls, plat_type):
        """平台类型转描述"""
        if plat_type == cls.Windows:
            return 'windows'
        elif plat_type == cls.Linux:
            return 'linux'
        elif plat_type == cls.Macosx:
            return 'macosx'
        else:
            return 'unknown'

    @classmethod
    def desc2type(cls, plat_desc):
        """平台描述转类型"""
        if plat_desc == 'windows' or plat_desc.startswith('win32'):
            return cls.Windows
        elif plat_desc.startswith('linux'):
            return cls.Linux
        elif plat_desc == 'macosx':
            return cls.Macosx
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
    x64 = 2
    Unknown = 3

    @classmethod
    def desc2type(cls, desc):
        """架构描述转换成类型"""
        desc = desc.strip().lower()
        if desc == 'x86':
            return cls.x86
        elif desc == 'x64' or desc == 'x86_64':
            return cls.x64
        else:
            return cls.Unknown

    @classmethod
    def type2desc(cls, arch_type):
        """架构类型转架构描述"""
        if arch_type == cls.x86:
            return 'x86'
        elif arch_type == cls.x64:
            return 'x64'
        else:
            return 'unknown'

    @classmethod
    def is_valid(cls, arch_type):
        """确认架构类型是否有效"""
        return arch_type == cls.x86 or \
            arch_type == cls.x64

    @classmethod
    def is_32bit_arch(cls, arch_type):
        """确认是否是32位架构"""
        arch_desc = cls.type2desc(arch_type)
        return arch_desc.endswith('32') or arch_desc.endswith('86')

    @classmethod
    def is_64bit_arch(cls, arch_type):
        """确认是否是64位架构"""
        arch_desc = cls.type2desc(arch_type)
        return arch_desc.endswith('64')

